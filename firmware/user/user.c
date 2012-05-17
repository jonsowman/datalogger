/** I N C L U D E S **********************************************************/
#include <p18cxxx.h>
#include <usart.h>
#include <timers.h>
#include "system\typedefs.h"

#include "system\usb\usb.h"

#include "io_cfg.h"             // I/O pin mapping
#include "user\user.h"
#include "logic\logic.h"

/** V A R I A B L E S ********************************************************/
#pragma udata

byte counter;
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

/** P R I V A T E  P R O T O T Y P E S ***************************************/

void BlinkUSBStatus(void);
void ServiceRequests(void);
void ResetTempLog(void);

//UCAM
byte ReadPOT(void);
void Blink(byte);
void nullSampler(void);

/** D E C L A R A T I O N S **************************************************/
#pragma code

void ProcessIO(void)
{   
    //BlinkUSBStatus();

    // User Application USB tasks
    if((usb_device_state < CONFIGURED_STATE)||(UCONbits.SUSPND==1)) return;
    
    //UCAM
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
        counter = 0;
        switch(dataPacket.CMD)
        {
            case READ_VERSION:
                //dataPacket._byte[1] is len
                dataPacket._byte[2] = MINOR_VERSION;
                dataPacket._byte[3] = MAJOR_VERSION;
                counter=0x04;
                break;

            case ID_BOARD:
                counter = 0x01;
                if(dataPacket.ID == 0)
                {
                    mLED_3_Off();mLED_4_Off();
                }
                else if(dataPacket.ID == 1)
                {
                    mLED_3_Off();mLED_4_On();
                }
                else if(dataPacket.ID == 2)
                {
                    mLED_3_On();mLED_4_Off();
                }
                else if(dataPacket.ID == 3)
                {
                    mLED_3_On();mLED_4_On();
                }
                else
                    counter = 0x00;
                break;

            case UPDATE_LED:
                // LED1 & LED2 were used as USB event indicators.
                if(dataPacket.led_num == 1)
                {
                    mLED_1 = dataPacket.led_status;
                    counter = 0x01;
                }
                if(dataPacket.led_num == 2)
                {
                    mLED_2 = dataPacket.led_status;
                    counter = 0x01;
                }
                if(dataPacket.led_num == 3)
                {
                    mLED_3 = dataPacket.led_status;
                    counter = 0x01;
                }
                else if(dataPacket.led_num == 4)
                {
                    mLED_4 = dataPacket.led_status;
                    counter = 0x01;
                }
                break;

            //UCAM
            case BLINK_LED_COMMAND: //[0xEE, Onstate]
            	//Return the sum of the numbers, note no overflow protection present, to keep simplicity.
                //Blink(dataPacket._byte[1]);
				nullSampler();
                counter=0x02; //sends back same command
                break;

           	//UCAM
           	case GET_ADC_COMMAND: //[0xED. 8-bit data]
                //dataPacket._byte[1] = ReadPOT();
				dataPacket._byte[1] = 0x2A;
                counter=0x02; //returns[0xED, command]
                break;
 
            case RESET:
                Reset();
                break;
                
            default:
                break;
        }
        if(counter != 0)
        {
            if(!mUSBGenTxIsBusy())
                USBGenWrite((byte*)&dataPacket,counter);
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
    mInitAllLEDs();
	TRISAbits.TRISA0=1;
	ADCON0=0x01;
	ADCON2=0x3C;
    ADCON2bits.ADFM = 1;   // ADC result right justified
	TRISD = 0x18;
	// Set RD0 and RD1 as output
	TRISD &= ~(1 << TRISDbits.TRISD0 | 1 << TRISDbits.TRISD1); 
	ThermSendReset();
	initAcquisition(0x0001);
}//end UserInit


/** EOF user.c ***************************************************************/
