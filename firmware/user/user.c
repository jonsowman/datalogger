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

void ServiceRequests(void)
{
    byte index;

	int i = 0;
	int q = 0;
	int temp = 0;
    
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

            case UPDATE_LED:
                // LED1 & LED2 are USB event indicators
                if(dataPacket.led_num == 1)
                {
                    mLED_1 = dataPacket.led_status;
                    *usblen = 1;
                }
                if(dataPacket.led_num == 2)
                {
                    mLED_2 = dataPacket.led_status;
                    *usblen = 1;
                }
                if(dataPacket.led_num == 3)
                {
                    mLED_3 = dataPacket.led_status;
                    *usblen = 1;
                }
                else if(dataPacket.led_num == 4)
                {
                    mLED_4 = dataPacket.led_status;
                    *usblen = 1;
                }
                break;
                
            case LOGIC_SET_SRATE:
            	// Rate is 16 bit, MSB first
            	uint16_t rate = *usbptr | *(usbptr + 1);
            	setSampleRate(rate);
            	// Return CMD along with 1 for success [CMD, 0x03, 0x01]
            	*usbptr = 0x01;
            	*usblen = 3;
            	break;
            	
            case LOGIC_GET_SRATE:
            	uint16_t rate = getSampleRate();
            	*usbptr = (rate >> 8) & 0xFF;
            	*usbptr = rate & 0xFF;
            	// Returned is like [CMD, 0x04, MSB, LSB]
            	*usblen = 4;
            	break;

            // The following two commands break the the command/response
            // protocol defined for the Logic Analyser, in order that they be
            // back compatible with the provided example PC interface.
            // (They are missing length field).
            case BLINK_LED_COMMAND: //[0xEE, Onstate]
				nullSampler();
                *usblen = 2; //sends back same command
                break;

           	case GET_ADC_COMMAND: //[0xED. 8-bit data]
                *usbptr = ReadPOT();
                *usblen = 2; //returns[0xED, command]
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
		// If we've put data into the send buffer, then transmit
        if(*usblen != 0)
        {
            if(!mUSBGenTxIsBusy())
                USBGenWrite((byte*)&dataPacket,*usblen);
        }
    }
return;
}

void BlinkUSBStatus(void)
{
    static word led_count=0;
    
    if(led_count == 0)led_count = 10000U;
    led_count--;


    #define mLED_Both_Off()         {mLED_1_Off();mLED_2_Off();}
    #define mLED_Both_On()          {mLED_1_On();mLED_2_On();}
    #define mLED_Only_1_On()        {mLED_1_On();mLED_2_Off();}
    #define mLED_Only_2_On()        {mLED_1_Off();mLED_2_On();}


    if(UCONbits.SUSPND == 1) //power conservation mode
    {
        if(led_count==0)
        {
            mLED_1_Toggle();
            mLED_2 = mLED_1;        // Both blink at the same time
        }//end if
    }
    else
    {
        if(usb_device_state == DETACHED_STATE)
        {
            mLED_Both_Off();
            
        }
        else if(usb_device_state == ATTACHED_STATE)
        {
            mLED_Both_On();
        }
        else if(usb_device_state == POWERED_STATE)
        {
            mLED_Only_1_On();
        }
        else if(usb_device_state == DEFAULT_STATE)
        {
            mLED_Only_2_On();
        }
        else if(usb_device_state == ADDRESS_STATE)
        {
            if(led_count == 0)
            {
                mLED_1_Toggle();
                mLED_2_Off();
            }
        }
        else if(usb_device_state == CONFIGURED_STATE)
        {
            if(led_count==0)
            {
                mLED_1_Toggle();
                mLED_2 = !mLED_1;       // Alternate blink                
            }
        }
    }

}

//UCAM
//code section will blink LEDs 5 times
// For some reason this writes the ENTIRE PORTD
void Blink(byte onState)
{
	LATD = onState;
	return;
}

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

void CheckButtons(void)
{
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

void ThermWrite(int command)
{
	int i;
	//Set RD7 as OUTPUT
	
	for (i=0;i<=15;i++)
	{
		if (command & 1)
		{
			TRISD = 0x18;
			LATDbits.LATD6 = 0;
			CallDelay(1);
			LATDbits.LATD6 = 1;
			//TRISD = 0xD8;
			CallDelay(10);
		}
		else
		{
			TRISD = 0x18;
			LATDbits.LATD6 = 0;
			CallDelay(11);
			//CodeSectionAccessed(0x4);
		}
		command >>=1;
	}
	return;
}

int ThermReadTemp(void)
{
	int i;
	int finalTemp;
	int temp[7];
	for (i=0;i<=7;i++)
	{
		TRISD = 0x18;
		LATDbits.LATD7 = 0;
		TRISD = 0x98;
		temp[i] = temp_sense;
		CallDelay(8);
		if (temp[i]){
			finalTemp += 1 << i;
		}
	}
	//CodeSectionAccessed(0x4);
	return finalTemp;
}

void ThermSendReset(void)
{
	TRISD = 0x18; //Set all but RD3, RD4 as output
	LATDbits.LATD7 = 0;  //Pull Data line low
	CallDelay(83);//CallDelay gives 6-8us delay per unit, min. 480us required
	//After 30us, thermometer returns 112us (60-240us) presence pulse (line low)
	TRISD = 0x98;	//Set RD7 as INPUT
	CallDelay(2);
	return;
}//end ThermSendReset

int ReadTemp(void)
{
	int deg;
	ThermSendReset();
	ThermWrite(0xAA);//Request temp command
	//ThermWrite(0xF);//Request temp command
	deg = ThermReadTemp();
	return deg;
}//end ReadTemp

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
	TRISDbits.TRISD0 = 0;
	TRISDbits.TRISD1 = 0; 
}


/** EOF user.c ***************************************************************/
