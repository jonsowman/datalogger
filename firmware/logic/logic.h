/**
 * SB3 Datalogger Project
 * David Turner & Jon Sowman
 * May 2012
 */
#ifndef __LOGIC_H__
#define __LOGIC_H__

#include "system\typedefs.h"

#define TIMER_PRELOAD			111
#define MAX_SAMPLE_RATE			250000UL
#define MAX_SAMPLE_NUM			0x20000

#define MODE_ASYNC				0x01
#define MODE_SYNC				0x02
#define SYNC_EDGE_RISE			0x04
#define SYNC_EDGE_FALL			0x08
#define SYNC_EDGE_BOTH			0x10
#define OPTIONS_VALID			0x80

// Configure the analyser with the options 8-bit bitfield
bool logicConfig(uint8_t options);

// Check whether the supplied options byte is valid
bool verifyOptions(uint8_t options);

// Arm the analyser
bool logicStart(void);

// Getting and setting samplerate and samplenumber
bool setSampleRate(uint32_t* samplerate);
uint32_t getSampleRate(void);
bool setSampleNumber(uint32_t* count);
uint32_t getSampleNumber(void);

// Get the current state of the analyser (LOGIC_POLL)
uint8_t getLogicState(void);

// Fill the USB buffer with data from SRAM
void fillUSBBuffer(uint8_t* usbptr);

// Reset the analyser
void logicReset(void);

// Private prototypes
void _beginSampling(uint8_t config);
void _startTimer(void);
void _startExtInterrupt(uint8_t config);

// ISR prototypes
void low_isr(void);
void high_isr(void);

#endif /* __LOGIC_H__ */
