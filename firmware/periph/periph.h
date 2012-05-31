/**
 * SB3 Datalogger Project
 * David Turner & Jon Sowman
 * May 2012
 */
#ifndef __PERIPH_H__
#define __PERIPH_H__

#include "system\typedefs.h"

// SRAM Control Methods
void setRAMAddress(uint32_t address);
void writeRAM(uint32_t address);
uint8_t readRAM(uint32_t address);

// Buffer control
void disableBuffer(void);
void enableBuffer(void);

// Shift register routines
uint8_t getSRByte(void);

#endif /* __PERIPH_H__ */
