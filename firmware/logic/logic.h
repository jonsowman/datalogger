/**
 * SB3 Datalogger Project
 * David Turner & Jon Sowman
 * May 2012
 */
#ifndef __LOGIC_H__
#define __LOGIC_H__

#include "system\typedefs.h"

#define TIMER_PRELOAD			250
#define MAX_SAMPLE_RATE			250000UL
#define MAX_SAMPLE_NUM			131072UL

#define MODE_ASYNC				0x01
#define MODE_SYNC				0x02
#define SYNC_EDGE_RISE			0x04
#define SYNC_EDGE_FALL			0x08
#define SYNC_EDGE_BOTH			0x10
#define OPTIONS_VALID			0x80

bool logicConfig(uint8_t options);
bool verifyOptions(uint8_t options);
bool logicStart(void);
bool setSampleRate(uint32_t* samplerate);
uint32_t getSampleRate(void);
bool setSampleNumber(uint32_t* count);
uint32_t getSampleNumber(void);
void startTimer();

// SRAM Control Methods
void setRAMAddress(uint32_t address);

// Buffer control
void disableBuffer(void);
void enableBuffer(void);

// Shift register routines
uint8_t SRGetByte(void);

// ISR prototypes
void low_isr(void);
void high_isr(void);

#endif /* __LOGIC_H__ */
