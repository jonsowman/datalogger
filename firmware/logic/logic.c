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
#pragma code high_vector=0x808
void interrupt_at_high_vector(void)
{
	_asm GOTO high_isr _endasm
}
#pragma code

#pragma code low_vector=0x818
void interrupt_at_low_vector(void)
{
	_asm GOTO low_isr _endasm
}
#pragma code

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
