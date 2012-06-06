/**
 * SB3 Datalogger Project
 * David Turner & Jon Sowman
 * May 2012
 */
#include <p18cxxx.h>
#include <timers.h>
#include <delays.h>
#include "system\typedefs.h"
#include "system\usb\usb.h"
#include "io_cfg.h"
#include "periph\periph.h"

/**
 * Write the given SRAM byte address to the IO pins.
 */
void setRAMAddress(uint32_t address)
{
	// Write the address to the line
	// This is horrifically hacky due to C18 being a 
	// steaming pile of poo, and PICs being rubbish. 

	// ADDR[0-2] lie in bits [3-5] of PORTA
	LATA = ((address & 0x07) << 3) | (LATA & 0xC7);

	// ADDR[3-7] lie in bits [3-7] of PORTB
	LATB = (address & 0xF8) | (LATB & 0x07);

	// ADDR[8-15] lie bytewise on PORTD
	LATD = address >> 8;

	// Finally, ADDR16 is on PORTB[2]
	LATADDR16 = (address >> 16);
	
	return;
}

/**
 * Clock the data on the address bus into RAM using the
 * CE#/CE2 controlled write cycle (see datasheet).
 */
void writeRAM(uint32_t address)
{
	LATB ^= 0x02;
	// Do this in hw
	//setRAMAddress(address);
	LATAbits.LATA0 = 0;
	Delay1TCY();
	LATAbits.LATA1 = 1;
	Delay1TCY();
	
	// WE must drop with or before CE#/CE2 for outputs
	// to remain Hi-Z after write

	// Start RAM write by dropping CE# and WE# and
	// raising CE2.
	LATC = 0x40 | (LATC & 0xB9);
	
	Delay1TCY(); // This is about 50ns (want 45)
	
	// End write by dropping CE2 and raising CE# and WE#
	LATC = 0x06 | (LATC & 0xB9);
	
	// Disable the buffer
	//disableBuffer();
}

/**
 * Read a byte from the RAM at the given address.
 */
uint8_t readRAM(uint32_t address)
{
	uint8_t data;
	
	setRAMAddress(address);
	
	// WE# must remain high during a read cycle
	LATWE = 1;
	LATCE = 0;
	LATCE2 = 1;
	LATOE = 0;
	
	// Wait 30ns for the DOUT to become valid
	Delay1TCY();
	
	// Disable the buffer so the RAM can take control of
	// the data bus.
	disableBuffer();
	
	data = getSRByte();
	
	LATOE = 1;
	LATCE2 = 0;
	LATCE = 1;
	
	// Wait 20ns for the outputs to go Hi-Z
	Delay1TCY();
	
	// Reenable the buffer to give control back to the
	// input channels
	enableBuffer();
	
	return data;
}

// Buffer control stuff
void disableBuffer(void)
{
	LATBUFFER_EN = 1;
	return;
}

void enableBuffer(void)
{
	LATBUFFER_EN = 0;
	return;
}

/**
 * Get a byte from the parallel data bus (MSB first).
 */
uint8_t getSRByte(void)
{
	uint8_t data = 0;
	int8_t i;
	
	// Dump parallel data into the SR
	LATSR_PLOAD = 0;
	
	// Wait 5us
	Delay10TCYx(6); // (1/12) * 10 * 6 (us)
	LATSR_PLOAD = 1;
	LATSR_CLK_EN = 0;
	
	// Clock the byte in, MSB first
	for(i = 7; i >= 0; i--)
	{
		data |= PORTSR_SEROUT << i;
		LATSR_CLK = 1;
		Delay10TCYx(6);
		LATSR_CLK = 0;
	}
	
	// Disable the clock
	LATSR_CLK_EN = 1;
	return data;
}
