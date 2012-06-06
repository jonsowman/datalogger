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
#include "periph\periph.h"

// Variables
#pragma udata

byte trf_state;

// Write pointer from the logic subsystem
extern volatile uint32_t writeptr;

DATA_PACKET dataPacket;

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
	uint32_t rate;
    if(USBGenRead((byte*)&dataPacket,sizeof(dataPacket)))
    {   
	    // Pointer to the data packet
	    uint8_t* usbptr = dataPacket._byte;
	    uint8_t* usbcmd = usbptr;
	    uint8_t* usblen = usbptr + 1;
	    usbptr += 2; // Points to beginning of payload
		
		// When writing data to the buffer, usbptr must always
		// point to the next free byte in the buffer.
		
		// Switch on the command byte of the USB packet we got from the PC
        switch(dataPacket.CMD)
        {
            case READ_VERSION:
                *usbptr++ = MINOR_VERSION;
                *usbptr++ = MAJOR_VERSION;
                break;
            	
            case LOGIC_GET_SRATE:
            	rate = getSampleRate();
            	*usbptr++ = (rate >> 24) & 0xFF;
            	*usbptr++ = (rate >> 16) & 0xFF;
            	*usbptr++ = (rate >> 8) & 0xFF;
            	*usbptr++ = rate & 0xFF;
            	// Returned is like [CMD, 0x04, MSB, {}, {}, LSB]
            	break;
            
            case LOGIC_CONFIG:
				// Firstly configure the analyser options
            	if(!logicConfig(*usbptr))
				{
					*usbcmd = LOGIC_ERROR;
					*usbptr++ = ERROR_INVALID_CONFIG;
					break;
				}
				// Now the sample rate
				if(!setSampleRate((uint32_t*)(usbptr+1)))
				{
					*usbcmd = LOGIC_ERROR;
					*usbptr++ = ERROR_INVALID_SAMPLE_RATE;
					break;
				}
				// And finally the number of samples
				if(!setSampleNumber((uint32_t*)(usbptr+5)))
				{
					*usbcmd = LOGIC_ERROR;
					*usbptr++ = ERROR_INVALID_SAMPLE_NUMBER;
					break;
				}
            	// Return 3 bytes, payload is '1' for success [CMD, 0x03, 0x01]
            	*usbptr++ = 0x01;
            	break;
            	
            case LOGIC_ARM: // return [CMD, LEN, RESULT]
            	logicStart();
            	*usbptr++ = 0x01;
            	break;
            	
            case LOGIC_POLL: // return [POLL, LEN, STATE]
            	*usbptr++ = getLogicState();
            	if(getLogicState() == LOGIC_INPROGRESS)
            	{
	            	*(uint32_t*)usbptr = writeptr;
	            	usbptr += 4;
	            }
            	break;
            	
            case LOGIC_DATA:
            	if(getLogicState() == LOGIC_END)
            	{
            		usbptr = fillUSBBuffer(usbptr);
            		break;
            	} else if(getLogicState() == LOGIC_END_DATA)
	            {
	            	*usbcmd = LOGIC_ERROR;
	            	*usbptr++ = ERROR_END_OF_DATA;
            		break;
            	} else
            	{
	            	*usbcmd = LOGIC_ERROR;
	            	*usbptr++ = ERROR_DATA_UNAVAILABLE;
	            	break;
	            }
            	
            // The following command breaks the the command/response
            // protocol defined for the Logic Analyser, in order that they be
            // back compatible with the provided example PC interface.
            // (They are missing length field).
            case BLINK_LED_COMMAND: // [0xEE, Onstate]
				LATDbits.LATD1 = *++usbcmd;
                break;
            	
           	case GET_ADC_COMMAND: // [0xED. 8-bit data]
                *usbptr++ = _calcPrescaler(1000UL);
                break;
                
            case LOGIC_RESET:
            	logicReset();
            	*usbptr++ = 0x01;
            	break;
                
           	case PING:
           		break;
 
            case RESET:
                Reset();
                break;
                
            default:
            	// We didn't understand the command the PC sent, so error
            	*usbcmd = LOGIC_ERROR;
            	*usbptr++ = ERROR_CMD_NOT_FOUND;
                break;
        }

		// Calculate the length of the data in the transmit buffer
		*usblen = usbptr - usbcmd;

		// If we've put data into the send buffer, then transmit
        if(*usblen != 0 && !mUSBGenTxIsBusy())
        	USBGenWrite((byte*)&dataPacket,*usblen);
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
	// Set RD0 and RD1 as output
	ADCON1bits.PCFG0 = 1;
	ADCON1bits.PCFG1 = 1;
	ADCON1bits.PCFG2 = 1;
	ADCON1bits.PCFG3 = 1;
	TRISBbits.TRISB0 = 1;
	// RD0 and RD1 are outputs
	TRISDbits.TRISD0 = 0;
	TRISDbits.TRISD1 = 0;

	// Set up LEDs as outputs
	TRISLEDA = 0;
	TRISLEDB = 0;

	// SRAM Control lines as outputs
	TRISCE = 0;
	TRISCE2 = 0;
	TRISOE = 0;
	TRISWE = 0;
	
	// Shift reg control lines as outputs
	TRISSR_CLK = 0;
	TRISSR_CLK_EN = 0;
	TRISSR_PLOAD = 0;
	TRISSR_SEROUT = 1;
	
	// Tri-state buffer control line
	TRISBUFFER_EN = 0;
	
	// Channel 0 (D0) as input
	TRISDATA0 = 1;

	// Set up ADDR lines as outputs
	TRISADDR0 = 1;
	TRISADDR1 = 1;
	TRISADDR2 = 1;
	TRISADDR3 = 1;
	TRISADDR4 = 1;
	TRISADDR5 = 1;
	TRISADDR6 = 1;
	TRISADDR7 = 1;
	TRISADDR8 = 1;
	TRISADDR9 = 1;
	TRISADDR10 = 1;
	TRISADDR11 = 1;
	TRISADDR12 = 1;
	TRISADDR13 = 1;
	TRISADDR14 = 1;
	TRISADDR15 = 1;
	TRISADDR16 = 1;
	
	// Put the SRAM into standby
	LATCE = 1;
	LATCE2 = 0;
	
	// Put the shift register into standby
	LATSR_PLOAD = 1;
	LATSR_CLK_EN = 1;
	LATSR_CLK = 0;
	
	LATLEDA = 0;
	LATLEDB = 0;
	
	// RB1 is hw addressing MR
	TRISBbits.TRISB1 = 0;
	LATBbits.LATB1 = 0; // MR active high
	LATAbits.LATA0 = 1; // clock on falling
}

/** EOF user.c ***************************************************************/
