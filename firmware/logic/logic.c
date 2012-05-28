/**
 * SB3 Datalogger Project
 * David Turner & Jon Sowman
 * May 2012
 */
#include <p18cxxx.h>
#include <usart.h>
#include <timers.h>
#include "system\typedefs.h"
#include "system\usb\usb.h"
#include "io_cfg.h"
#include "logic\logic.h"

uint32_t samplenumber;
uint32_t samplerate;
uint8_t config;

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
	if((options & SYNC_EDGE_RISE) && (options && SYNC_EDGE_FALL)
			&& (options && SYNC_EDGE_BOTH))
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

	// Set up the timer according to the config
	return true;
}

/**
 * Set the sample rate for the analyser in Hz . We always sample 
 * at the same rate, but oversample in the slower modes.
 */
bool setSampleRate(uint32_t* rate)
{
	if(*rate <= MAX_SAMPLE_RATE)
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
 * Set up an interrupt to run at the samplerate,
 * discard data for now.
 */
void startTimer()
{	
	// Enable interrupt priority
	RCONbits.IPEN = 1;

	// Stop timer
	T0CONbits.TMR0ON = 0;

	// Set to 8 bit mode
	T0CONbits.T08BIT = 1;

	// Clock on instruction clock cycles
	T0CONbits.T0CS = 0;

	// Turn off the prescalar so we clock on instruction clk cycles
	T0CONbits.PSA = 1;
	T0CONbits.T0PS2 = 0;
	T0CONbits.T0PS1  = 0;
	T0CONbits.T0PS0 = 0;

	// Enable TIMER0 OVF interrupt, periph interrupt
	// and global interrupts
	INTCONbits.GIEL  = 1;
	INTCONbits.GIEH = 1;
	INTCONbits.TMR0IE = 1;
	INTCONbits.TMR0IF = 0;

	// Set TIMER0 OVF to high priority
	INTCON2bits.TMR0IP = 1;

	// Finally enable the timer
	T0CONbits.TMR0ON = 1;
}

/**
 * Write the given SRAM byte address to the IO pins.
 */
void setRAMAddress(uint32_t address)
{
	// Write the address to the line
	// This is horrifically hacky due to the C18 compiler being a 
	// steaming pile of poo, and PICs being rubbish. 

	// ADDR[0-2] lie in bits [3-5] of PORTA
	LATA = ((address & 0x07) << 3) | (LATA & 0xC7);

	// ADDR[3-7] lie in bits [3-7] of PORTB
	LATB = (address & 0xF8) | (LATB & 0x07);

	// ADDR[8-15] lie bytewise on PORTD
	LATD = address >> 8;

	// Finally, ADDR16 is on PORTB[2]
	LATADDR16 = (address >> 16);
}

/**
 * Clock the data on the address bus into RAM
 */
void clockRAMDataIn(void)
{
}

// Buffer control stuff
void disableBuffer(void)
{
	LATBUFFER_EN = 1;
}

void enableBuffer(void)
{
	LATBUFFER_EN = 0;
}

/**
 * Load a byte from the parallel data bus and clock it in.
 */
uint8_t SRGetByte(void)
{
	// Dump parallel data into the SR
	LATSR_PLOAD = 0;
	CallDelay(1);
	LATSR_PLOAD = 1;
	
	// Enable the clock
	LATSR_CLK_EN = 0;
	
	// Clock the byte in serially, MSB first
	uint8_t data;
	for(uint8_t i = 7; i >= 0; i--)
	{
		data |= PORTSR_SEROUT << i;
		LATSR_CLK = 1;
		CallDelay(1);
		LATSR_CLK = 0;
	}
	
	return data;
}

// Interrupt stuff here
#pragma interrupt high_isr
void high_isr(void)
{
	if(INTCONbits.TMR0IF)
	{
		LATDbits.LATD0 ^= 1; // Turn RD0 on
		TMR0L = TIMER_PRELOAD;
		INTCONbits.TMR0IF = 0;
	}
}
#pragma code

#pragma interruptlow low_isr
void low_isr(void)
{
}
#pragma code
