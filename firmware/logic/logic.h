/**
 * SB3 Datalogger Project
 * David Turner & Jon Sowman 2012
 */
#ifndef __LOGIC_H__
#define __LOGIC_H__

#include "system\typedefs.h"

void logicConfig(uint8_t options);
void logicStart(void);
void setSampleRate(uint16_t samplerate);
uint16_t getSampleRate(void);

void startTimer(uint16_t samplerate);

#endif /* __LOGIC_H__ */
