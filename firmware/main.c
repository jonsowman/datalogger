// Includes
#include <p18cxxx.h>
#include <timers.h>
#include "system\typedefs.h"                        // Required
#include "system\usb\usb.h"                         // Required
#include "io_cfg.h"                                 // Required

#include "system\usb\usb_compile_time_validation.h" // Optional
#include "user\user.h"                              // Modifiable

// Variables
#pragma udata

// Private prototypes
static void InitializeSystem(void);
void USBTasks(void);
void low_isr(void);
void high_isr(void);

// Vector remapping
extern void _startup (void);        // See c018i.c in your C18 compiler dir
#pragma code _RESET_INTERRUPT_VECTOR = 0x000800
void _reset (void)
{
	_asm GOTO _startup _endasm
}

#pragma code high_vector=0x08
void interrupt_at_high_vector(void)
{
	_asm GOTO high_isr _endasm
}

#pragma code low_vector=0x18
void interrupt_at_low_vector(void)
{
	_asm GOTO low_isr _endasm
}

#pragma interrupt high_isr
void high_isr(void)
{
	LATDbits.LATD0 = 1; // Turn RD0 on
}

#pragma interruptlow low_isr
void low_isr(void)
{
	LATDbits.LATD0 = 1; // Turn RD0 on
}

// End interrupt handling

#pragma code
void main(void)
{
    InitializeSystem();

	OpenTimer0(TIMER_INT_ON & T0_SOURCE_INT & T0_8BIT
		& T0_PS_1_1);
	INTCONbits.GIE = 1;

	LATDbits.LATD0 = 0; // Check LED off

	while(1);

    while(1)
    {
        USBTasks();         // USB Tasks
        ProcessIO();        // See user\user.c & .h
		UserTasks();
    }
}

/**
 * InitializeSystem is a centralized initialization routine.
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
