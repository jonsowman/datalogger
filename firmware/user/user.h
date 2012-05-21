#ifndef PICDEM_FS_DEMO_H
#define PICDEM_FS_DEMO_H

// Includes
#include "system\typedefs.h"

/* PICDEM FS USB Demo Version */
#define MINOR_VERSION   0x00    //Demo Version 1.00
#define MAJOR_VERSION   0x01

/* Temperature Mode */
#define TEMP_REAL_TIME  0x00
#define TEMP_LOGGING    0x01

/// Structs
typedef union DATA_PACKET
{
    byte _byte[USBGEN_EP_SIZE];		//For byte access
    word _word[USBGEN_EP_SIZE/2];	//For word access
    struct
    {
        enum
        {
            READ_VERSION		= 0x00,
            ID_BOARD      	 	= 0x31,
            UPDATE_LED			= 0x32,
            //UCAM
			GET_ADC_COMMAND		= 0xED,
            BLINK_LED_COMMAND	= 0xEE,
            RESET				= 0xFF,
			// Logic analyser commands
			LOGIC_SET_SRATE		= 0x40,
			LOGIC_GET_SRATE		= 0x41,
			LOGIC_CONFIG		= 0x42,
			LOGIC_ARM			= 0x60,
			LOGIC_START			= 0x61,
			LOGIC_END			= 0x62,
			LOGIC_DATA			= 0x63,
			LOGIC_ERROR			= 0xAA
        } CMD;
        byte len;
    };
    struct
    {
        unsigned :8;
        byte ID;
    };
    struct
    {
        unsigned :8;
        byte led_num;
        byte led_status;
    };
    struct
    {
        unsigned :8;
        word word_data;
    };
} DATA_PACKET;

// Error values returned by the Logic Analyser
#define	ERROR_CMD_NOT_FOUND				0x01
#define ERROR_INVALID_SAMPLE_RATE		0x02

// Public prototypes
void UserInit(void);
void ProcessIO(void);

#endif /* PICDEM_FS_DEMO_H */
