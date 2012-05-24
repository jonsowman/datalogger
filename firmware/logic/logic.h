/**
 * SB3 Datalogger Project
 * David Turner & Jon Sowman
 * May 2012
 */
#ifndef __LOGIC_H__
#define __LOGIC_H__

#include "system\typedefs.h"

#define TIMER_PRELOAD			250

void logicConfig(uint8_t options);
void logicStart(void);
void setSampleRate(uint16_t samplerate);
uint16_t getSampleRate(void);
void startTimer();

// ISR prototypes
void low_isr(void);
void high_isr(void);

#endif /* __LOGIC_H__ */
