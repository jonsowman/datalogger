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

/**
 * Configure the logic analyser, takes one byte bitfield
 * containing the configuration options. See documentation
 * for details.
 */
void logicConfig(uint8_t options)
{
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
void setSampleRate(uint16_t samplerate)
{
}

/**
 * Get the currently set sample rate in kHz.
 */
uint16_t getSampleRate(void)
{
}

/**
 * Set up an interrupt to run at the samplerate,
 * discard data for now.
 */
void startTimer(uint16_t samplerate)
{	
	// Enable interrupt priority
	RCONbits.IPEN = 1;

	// Stop timer
	T0CONbits.TMR0ON = 0;

	// Set to 8 bit mode
	T0CONbits.T08BIT = 1;

	// Clock on instruction clock cycles
	T0CONbits.T0CS = 0;
	
	// 256 prescaler
	T0CONbits.PSA = 0;
	T0CONbits.T0PS2 = 1;
	T0CONbits.T0PS1  = 1;
	T0CONbits.T0PS0 = 1;

	// Enable TIMER0 OVF interrupt, periph interrupt
	// and global interrupts
	INTCONbits.TMR0IE = 1;
	INTCONbits.TMR0IF = 0;
	INTCONbits.GIEL  = 1;
	INTCONbits.GIEH = 1;

	// Set TIMER0 OVF to high priority
	INTCON2bits.TMR0IP = 1;

	// Finally enable the timer
	T0CONbits.TMR0ON = 1;
}
