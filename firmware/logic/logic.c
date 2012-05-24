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
/*
sync/mode
clock edge for sync
sample rate
total no of samples
*/

/**
 * Configure the logic analyser, takes one byte bitfield
 * containing the configuration options. See documentation
 * for details.
 */
uint8_t logicConfig(uint8_t options)
{
	if((options & MODE_ASYNC) && (options & MODE_SYNC))
		return 0;
	if((options & SYNC_EDGE_RISE) && (options && SYNC_EDGE_FALL)
			&& (options && SYNC_EDGE_BOTH))
		return 0;
	config = options;
	return 1;
}

/**
 * If in async mode, start logging now and filling the SRAM
 * buffer. If in sync mode, arm the analyser and start logging
 * on the clock trigger.
 */
void logicStart(void)
{
}

/**
 * Set the sample rate for the analyser in kHz . We always sample 
 * at the same rate, but oversample in the slower modes.
 */
uint8_t setSampleRate(uint32_t rate)
{
	if(rate <= MAX_SAMPLE_RATE)
	{
		samplerate = rate;
		return 1;
	} else {
		samplerate = 0;
		return 0;
	}
}

/**
 * Get the currently set sample rate in kHz.
 */
uint32_t getSampleRate(void)
{
}

/**
 * Set the number of samples, up to the allowable amount, returning
 * 1 if set successfully, 0 if not.
 */
uint8_t setSampleNumber(uint32_t count)
{
	if(count <= MAX_SAMPLE_NUM)
	{
		samplenumber = count;
		return 1;
	} else {
		samplenumber = 0;
		return 0;
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
