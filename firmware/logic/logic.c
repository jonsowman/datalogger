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
volatile bool sampling_complete = false;

// Next empty sample slot in SRAM
volatile uint32_t sampleptr;

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
	
	sampling_complete = false;
	sampleptr = 0;
	
	// Set up interrupts and leave the hardware to it...
	beginSampling(config);
	return true;
}

/**
 * Configure the timer or the external interrupt pin depending
 * on how the config is set.
 */
void beginSampling(uint8_t config)
{
	if(config & MODE_ASYNC)
	{
		startTimer();
	}
	else if(config & MODE_SYNC)
	{
		startExtInterrupt(config);
	}
}

/**
 * Return true once sampling is complete.
 */
bool samplingComplete(void)
{
	if(sampling_complete)
		return true;
	else
		return false;
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
 * Set up an interrupt to run at the samplerate for async mode,
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
	T0CONbits.PSA = 0;
	T0CONbits.T0PS2 = 0;
	T0CONbits.T0PS1  = 0;
	T0CONbits.T0PS0 = 1;

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
	
	return;
}

/**
 * Set up the external interrupt for sync mode, firing on the required
 * edges.
 */
void startExtInterrupt(uint8_t config)
{
	// Configure the edge for interrupt
	if(config & SYNC_EDGE_RISE)
		INTCON2bits.INTEDG0 = 1;
	else if(config & SYNC_EDGE_FALL)
		INTCON2bits.INTEDG0 = 0;
		
	// Clear the flag and enable the interrupt
	RCONbits.IPEN = 1;
	INTCONbits.GIEL  = 1;
	INTCONbits.GIEH = 1;
	INTCONbits.INT0IF = 0;
	INTCONbits.INT0IE = 1;
}  

// Interrupt stuff here
#pragma interrupt high_isr
void high_isr(void)
{
	if(INTCONbits.TMR0IF || INTCONbits.INT0IF)
	{
		LATB = LATB ^ 0x02;
		if(sampleptr < samplenumber) // check against max sample num
		{
			writeRAM(sampleptr);
			sampleptr++;
			if(INTCONbits.TMR0IF)
			{
				TMR0L = TIMER_PRELOAD;
				INTCONbits.TMR0IF = 0;
			}
			else if(INTCONbits.INT0IF)
			{
				INTCONbits.INT0IF = 0;
			}
		}
		else // Done sampling, stop interrupting
		{
			INTCONbits.INT0IE = 0;
			INTCONbits.TMR0IE = 0;
			T0CONbits.TMR0ON = 0;
			sampling_complete = true;
		}
	}
}
#pragma code

#pragma interruptlow low_isr
void low_isr(void)
{
}
#pragma code
