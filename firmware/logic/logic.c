/**
 * SB3 Datalogger Project
 * David Turner & Jon Sowman
 * May 2012
 */
#include <p18cxxx.h>
#include <usart.h>
#include <timers.h>
#include <delays.h>
#include "system\typedefs.h"
#include "system\usb\usb.h"
#include "io_cfg.h"
#include "logic\logic.h"
#include "user\user.h"
#include "periph\periph.h"

uint32_t samplenumber;
uint32_t samplerate;
uint8_t config;

// State of the analyser (see user.h)
volatile uint8_t logic_state = LOGIC_IDLE;

// Next empty sample slot in SRAM
volatile uint32_t writeptr;

// Next unread slot in SRAM
uint32_t readptr;

// The preload value of the sample timer
uint8_t timer_preload = TIMER_PRELOAD;

/**
 * Configure the logic analyser, takes one byte bitfield
 * containing the configuration options. See documentation
 * for details.
 */
bool logicConfig(uint8_t options)
{
	if(!verifyOptions(options))
	{
		config = 0;
		return false;
	} else {
		config = options;
		return true;
	}
}

/**
 * Take a config byte and verify it.
 */
bool verifyOptions(uint8_t options)
{
	if((options & MODE_ASYNC) && (options & MODE_SYNC))
		return false;
	if((options & EDGE_RISE) && (options & EDGE_FALL)
			&& (options & EDGE_BOTH))
		return false;
	if(!options & OPTIONS_VALID)
		return false;
	return true;
}

/**
 * If in async mode, start logging now and filling the SRAM
 * buffer. If in sync mode, arm the analyser and start logging
 * on the clock trigger.
 */
bool logicStart(void)
{
	// Check that all options are valid
	if(!verifyOptions(config))
		return false;
	
	logicReset();
	logic_state = LOGIC_ARM;
	
	// Calculate and load the prescaler and preload
	_initTimer();
	
	// Set up interrupts and leave the hardware to it...
	_beginSampling(config);
	return true;
}

/**
 * Configure the timer or the external interrupt pin depending
 * on how the config is set.
 */
void _beginSampling(uint8_t config)
{
	// Async untriggered
	if(config == (MODE_ASYNC | OPTIONS_VALID))
	{
		_startTimer();
	}
	// Async triggered
	else if(config & MODE_ASYNC)
	{
	// This is a bit of a hack to get around the fact
	// that the PIC hardware can't trigger on changing
	// level, only rising or falling. Therefore we read
	// the value of the pin and set the interrupt to
	// trigger on the opposite level.
		if(config & EDGE_BOTH)
		{
			config &= ~(EDGE_BOTH);
			if(PORTDATA0) // If line is really low
				config |= EDGE_RISE;
			else
				config |= EDGE_FALL;
		}
		_startExtInterrupt(config);	
	}
	// Sync
	else if(config & MODE_SYNC)
	{
		_startExtInterrupt(config);
	}
	return;
}

/**
 * Get the current state of the logic analyser so we can
 * monitor what it's doing when asked by the PC.
 */
uint8_t getLogicState(void)
{
	return logic_state;
}

/**
 * Set the sample rate for the analyser in Hz . We always sample 
 * at the same rate, but oversample in the slower modes.
 */
bool setSampleRate(uint32_t* rate)
{
	if((config & MODE_ASYNC && *rate <= MAX_SAMPLE_RATE 
		&& *rate >= MIN_SAMPLE_RATE) || config & MODE_SYNC)
	{
		samplerate = *rate;
		return true;
	} else {
		samplerate = 0;
		return false;
	}
}

/**
 * Get the currently set sample rate in Hz.
 */
uint32_t getSampleRate(void)
{
	return samplerate;
}

/**
 * Set the number of samples, up to the allowable amount, returning
 * 1 if set successfully, 0 if not.
 */
bool setSampleNumber(uint32_t* count)
{
	if(*count <= MAX_SAMPLE_NUM)
	{
		samplenumber = *count;
		return true;
	} else {
		samplenumber = 0;
		return false;
	}
}

/**
 * Get the number of samples currently set in the logger subsystem
 */
uint32_t getSampleNumber(void)
{
	return samplenumber;
}

/**
 * Put up to 62 bytes into the USB buffer from SRAM,
 * updating read pointers as required. 'usbptr' must
 * be maintained as the next available space, and
 * 'readptr' must be maintained as the next unread byte.
 */
uint8_t* fillUSBBuffer(uint8_t* usbptr)
{
	uint8_t i;
	for(i=0; i<(USBGEN_EP_SIZE - 2); i++)
	{
		if(readptr == writeptr)
		{
			logic_state = LOGIC_END_DATA;
			break;
		}
		*usbptr++ = readRAM(readptr++);
	}
	return usbptr;
}

/**
 * Reset the analyser from whatever state it is currently
 * in, ready for another sampling run followed by
 * returning the data to the PC.
 */
void logicReset(void)
{
	LATBbits.LATB1 = 1;
	Delay10TCYx(1);
	LATBbits.LATB1 = 0;
	writeptr = 0;
	readptr = 0;
	logic_state = LOGIC_IDLE;
}

/**
 * Calculate the required prescaler for a given sample
 * rate.
 */
uint8_t _calcPrescaler(uint32_t rate)
{
	uint32_t tickrate = TIMER_PSICLK;
	// Divide down from the 1:2 (000) prescaler
	uint8_t i;
	for(i=0; i<8; i++)
	{
		if(tickrate < rate) break;
		tickrate /= 2;
	}
	return i;
}

/**
 * Calculate the timer preload value.
 */
uint8_t _calcPreload(uint32_t targetrate, uint8_t ps)
{
	uint32_t preload = targetrate - (TIMER_PSICLK >> ps);
	preload *= 256UL;
	preload /= targetrate;
	return (uint8_t)preload;
}

/**
 * Calculate and set the prescaler and preload values
 * for the current configuration.
 */
void _initTimer(void)
{
	uint8_t ps;
	
	// Turn on the prescaler and set the preload
	ps = _calcPrescaler(samplerate);
	T0CON = (ps & 0x07) | (T0CON & 0xF8);
	timer_preload = _calcPreload(samplerate, ps);
	T0CONbits.PSA = 0;
	
	// Enable interrupt priority
	RCONbits.IPEN = 1;

	// Stop timer
	T0CONbits.TMR0ON = 0;

	// Set to 8 bit mode
	T0CONbits.T08BIT = 1;

	// Clock on instruction clock cycles
	T0CONbits.T0CS = 0;	
	
	INTCONbits.GIEL  = 1;
	INTCONbits.GIEH = 1;
	
	// Set TIMER0 OVF to high priority
	INTCON2bits.TMR0IP = 1;
}

/**
 * Set up an interrupt to run at the samplerate for async mode,
 * discard data for now.
 */
void _startTimer(void)
{	
	INTCONbits.TMR0IF = 0;
	INTCONbits.TMR0IE = 1;

	// Finally enable the timer
	T0CONbits.TMR0ON = 1;
	
	// Skip the triggering
	logic_state = LOGIC_INPROGRESS;
	
	return;
}

/**
 * Set up the external interrupt for sync mode, firing on the required
 * edges.
 */
void _startExtInterrupt(uint8_t config)
{
	// Configure the edge for interrupt, these are the
	// wrong way around since the frontend filters invert
	if(config & EDGE_RISE || config & EDGE_BOTH)
		INTCON2bits.INTEDG0 = 0;
	else if(config & EDGE_FALL)
		INTCON2bits.INTEDG0 = 1;
		
	// Clear the flag and enable the interrupt
	RCONbits.IPEN = 1;
	INTCONbits.GIEL  = 1;
	INTCONbits.GIEH = 1;
	INTCONbits.INT0IF = 0;
	INTCONbits.INT0IE = 1;
	
	logic_state = LOGIC_WAITING;
}  

// Interrupt stuff here
#pragma interrupt high_isr
void high_isr(void)
{
	if(logic_state == LOGIC_INPROGRESS)
	{
		if(writeptr < samplenumber)
		{
			writeRAM(writeptr);
			writeptr++;
			if(INTCONbits.TMR0IF)
			{
				TMR0L = timer_preload;
				INTCONbits.TMR0IF = 0;
			}
			else
			{
				if(config & EDGE_BOTH) INTCON2 ^= 0x40;
				INTCONbits.INT0IF = 0;
			}
		}
		else // Done sampling, stop interrupting
		{
			INTCONbits.INT0IE = 0;
			INTCONbits.TMR0IE = 0;
			logic_state = LOGIC_END;
		}
	} else if(logic_state == LOGIC_WAITING)
	{
		if(config & MODE_ASYNC)
		{
			INTCONbits.INT0IE = 0;
			INTCONbits.INT0IF = 0;
			logic_state = LOGIC_INPROGRESS;
			// Start the async capture timer
			INTCONbits.TMR0IF = 0;
			INTCONbits.TMR0IE = 1;
			T0CONbits.TMR0ON = 1;
		}
		else if(config & MODE_SYNC)
		{
			INTCONbits.INT0IF = 0;
			logic_state = LOGIC_INPROGRESS;
		}
	}
}
#pragma code

#pragma interruptlow low_isr
void low_isr(void)
{
}
#pragma code
