/**
 * SB3 Datalogger Project
 * David Turner & Jon Sowman
 * May 2012
 */
 
 // Includes from PATH
#include <p18cxxx.h>
#include <usart.h>
#include <timers.h>

// Includes from local repo
#include "system\typedefs.h"
#include "system\usb\usb.h"
#include "io_cfg.h"             // I/O pin mapping
#include "user\user.h"
#include "logic\logic.h"

// Variables
#pragma udata

byte trf_state;

DATA_PACKET dataPacket;

byte pTemp;                     // Pointer to current logging position, will
                                // loop to zero once the max index is reached
byte valid_temp;                // Keeps count of the valid data points
word temp_data[30];             // 30 points of data

// Timer0 - 1 second interval setup.
// Fosc/4 = 12MHz
// Use /256 prescalar, this brings counter freq down to 46,875 Hz
// Timer0 should = 65536 - 46875 = 18661 or 0x48E5
#define TIMER0L_VAL         0xE5
#define TIMER0H_VAL         0x48

// Private prototypes

void BlinkUSBStatus(void);
void ServiceRequests(void);
void ResetTempLog(void);

// Added by CUED
byte ReadPOT(void);
void Blink(byte);
void nullSampler(void);

#pragma code

void ProcessIO(void)
{   
    //BlinkUSBStatus();

    // User Application USB tasks
    if((usb_device_state < CONFIGURED_STATE)||(UCONbits.SUSPND==1)) return;
    
    // UCAM
    ServiceRequests();
}

/**
 * Check for incoming data in the USB buffer. If found, process it and 
 * fill the USB buffer with data to be returned to the USB host.
 */
void ServiceRequests(void)
{
	uint16_t rate;
    if(USBGenRead((byte*)&dataPacket,sizeof(dataPacket)))
    {   
	    // Pointer to the data packet
	    uint8_t* usbptr = dataPacket._byte;
	    uint8_t* usbcmd = usbptr;
	    uint8_t* usblen = usbptr + 1;
	    usbptr += 2; // Now points to beginning of payload
		
		// Switch on the command byte of the USB packet we got from the PC
        switch(dataPacket.CMD)
        {
            case READ_VERSION:
                *usbptr++ = MINOR_VERSION;
                *usbptr++ = MAJOR_VERSION;
                *usblen = 4;
                break;
                
            case LOGIC_SET_SRATE:
            	// Rate is 16 bit, MSB first
            	rate = (uint16_t)(*usbptr);
            	rate <<= 8;
            	rate |= *(usbptr + 1);
            	setSampleRate(rate);
            	// Return CMD along with 1 for success [CMD, 0x03, 0x01]
            	*usbptr = 0x01;
            	*usblen = 3;
            	break;
            	
            case LOGIC_GET_SRATE:
            	rate = getSampleRate();
            	*usbptr = (rate >> 8) & 0xFF;
            	*usbptr = rate & 0xFF;
            	// Returned is like [CMD, 0x04, MSB, LSB]
            	*usblen = 4;
            	break;
            
            case LOGIC_CONFIG:
				// Firstly configure the analyser options
            	if(!logicConfig(*usbptr++))
				{
					*usbcmd = LOGIC_ERROR;
					*usbptr = ERROR_INVALID_CONFIG;
					*usblen = 3;
					break;
				}
				// Now the sample rate
				if(!setSampleRate(usbptr))
				{
					*usbcmd = LOGIC_ERROR;
					*usbptr = ERROR_INVALID_SAMPLE_RATE;
					*usblen = 3;
					break;
				}
				// And finally the number of samples
				usbptr += 4;
				if(!setSampleNumber(usbptr))
				{
					*usbcmd = LOGIC_ERROR;
					*usbptr = ERROR_INVALID_SAMPLE_NUMBER;
					*usblen = 3;
					break;
				}
            	// Return 3 bytes, payload is '1' for success [CMD, 0x03, 0x01]
            	*usbptr = 0x01;
            	*usblen = 3;
            	break;
            	
            case LOGIC_ARM:
            	logicStart();
            	// Return 3 bytes, payload is '1' for success [CMD, 0x03, 0x01]
            	*usbptr = 0x01;
            	*usblen = 3;
            	break;

            // The following two commands break the the command/response
            // protocol defined for the Logic Analyser, in order that they be
            // back compatible with the provided example PC interface.
            // (They are missing length field).
            case BLINK_LED_COMMAND: // [0xEE, Onstate]
				LATDbits.LATD1 = *++usbcmd;
                *usblen = 2; // sends back same command
                break;

           	case GET_ADC_COMMAND: // [0xED. 8-bit data]
                *usbptr = ReadPOT();
                *usblen = 2; // returns[0xED, command]
                break;
 
            case RESET:
                Reset();
                break;
                
            default:
            	// We didn't understand the command the PC sent, so error
            	*usbcmd = LOGIC_ERROR;
            	*usbptr++ = ERROR_CMD_NOT_FOUND;
            	*usblen = 3;
                break;
        }

		// Calculate the length of the data in the transmit buffer
		// *usblen = usbptr - usbcmd;

		// If we've put data into the send buffer, then transmit
        if(*usblen != 0)
        {
            if(!mUSBGenTxIsBusy())
                USBGenWrite((byte*)&dataPacket,*usblen);
        }
    }
return;
}

// UCAM
// For some reason this writes the ENTIRE PORTD
void Blink(byte onState)
{
	LATD = onState;
	return;
}

/**
 * Return the 8 bit value of ADC channel 0.
 */
byte ReadPOT(void)
{
	byte low, high;
    ADCON0bits.GO = 1;              // Start AD conversion
    while(ADCON0bits.NOT_DONE);     // Wait for conversion
	low = ADRESL;
	high = ADRESH;
	low >>= 2;
	high <<= 6;
    return (low | high);
}

/**
 * Run lots of ADC samples and toggle an IO
 * to test sampling rate.
 */
void nullSampler(void)
{
	uint16_t i;
	for( i = 0; i < 65535; i++)
	{
		ADCON0bits.GO = 1;
    	while(ADCON0bits.NOT_DONE);
		LATD = ~(LATDbits.LATD0) & 0x01;
	}
	return;
}

//Delay of 1 gives 6-8us (this is a totally useless number)
void CallDelay(int delay)
{
	int i;
	for(i=0;i<delay;i++)
	{
	}
	return;
}

void UserTasks(void)
{
	return;
}

void UserInit(void)
{
    //mInitAllLEDs();
	//TRISAbits.TRISA0=1;
	//ADCON0=0x01;
	//ADCON2=0x3C;
    //ADCON2bits.ADFM = 1;   // ADC result right justified
	// Set RD0 and RD1 as output
	ADCON1bits.PCFG0 = 1;
	ADCON1bits.PCFG1 = 1;
	ADCON1bits.PCFG2 = 1;
	ADCON1bits.PCFG3 = 1;
	TRISBbits.TRISB0 = 1;
	// RD0 and RD1 are outputs
	TRISDbits.TRISD0 = 0;
	TRISDbits.TRISD1 = 0; 
}


/** EOF user.c ***************************************************************/
