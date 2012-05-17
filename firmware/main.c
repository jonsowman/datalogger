/** I N C L U D E S **********************************************************/
#include <p18cxxx.h>
#include "system\typedefs.h"                        // Required
#include "system\usb\usb.h"                         // Required
#include "io_cfg.h"                                 // Required

#include "system\usb\usb_compile_time_validation.h" // Optional
#include "user\user.h"                              // Modifiable

/** V A R I A B L E S ********************************************************/
#pragma udata

/** P R I V A T E  P R O T O T Y P E S ***************************************/
static void InitializeSystem(void);
void USBTasks(void);
void low_isr(void);
void high_isr(void);

/** V E C T O R  R E M A P P I N G *******************************************/

extern void _startup (void);        // See c018i.c in your C18 compiler dir
#pragma code _RESET_INTERRUPT_VECTOR = 0x000800
void _reset (void)
{
    _asm goto _startup _endasm
}
#pragma code

#pragma code _HIGH_INTERRUPT_VECTOR = 0x000008
void _high_ISR (void)
{
    _asm goto high_isr _endasm
}

#pragma code _LOW_INTERRUPT_VECTOR = 0x000018
void _low_ISR (void)
{
    _asm goto low_isr _endasm
}

#pragma interrupt high_isr
void high_isr(void)
{
	LATDbits.LATD1 = 1; // Turn RD1 on
	if(INTCONbits.TMR0IE && INTCONbits.TMR0IF)
	{
		TMR0L = 0x00;
		INTCONbits.TMR0IF = 0;
	}
}

#pragma interruptlow low_isr
void low_isr(void)
{
	LATDbits.LATD1 = 1; // Turn RD1 on
	if(INTCONbits.TMR0IE && INTCONbits.TMR0IF)
	{
		TMR0L = 0x00;
		INTCONbits.TMR0IF = 0;
	}
}
#pragma code

// End interrupt handling

#pragma code
void main(void)
{
    InitializeSystem();
    while(1)
    {
        USBTasks();         // USB Tasks
		//UCAM
        ProcessIO();        // See user\user.c & .h
		UserTasks();
    }
}

/* InitializeSystem is a centralize initialization routine.
 * All required USB initialization routines are called from
 * here.
 */
static void InitializeSystem(void)
{

    #if defined(USE_USB_BUS_SENSE_IO)
    tris_usb_bus_sense = INPUT_PIN; // See io_cfg.h
    #endif
    
    #if defined(USE_SELF_POWER_SENSE_IO)
    tris_self_power = INPUT_PIN;
    #endif
    
    mInitializeUSBDriver();         // See usbdrv.h
    
    UserInit();                     // See user.c & .h

}

/*
 * Call this after InitializeSystem()
 */
void USBTasks(void)
{
    // Servicing Hardware
    USBCheckBusStatus();          // Must use polling method
    if(UCFGbits.UTEYE!=1)
	{
        USBDriverService();     // Interrupt or polling method
	}
}
