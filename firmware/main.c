// Includes
#include <p18cxxx.h>
#include <timers.h>
#include <delays.h>
#include "system\typedefs.h"                        // Required
#include "system\usb\usb.h"                         // Required
#include "io_cfg.h"                                 // Required

#include "system\usb\usb_compile_time_validation.h" // Optional
#include "user\user.h"                              // Modifiable
#include "logic\logic.h"
#include "periph\periph.h"

// Variables
#pragma udata

// Private prototypes
static void InitializeSystem(void);
void USBTasks(void);

// Vector remapping
extern void _startup (void);        // See c018i.c in your C18 compiler dir
#pragma code _RESET_INTERRUPT_VECTOR = 0x000800
void _reset (void)
{
	_asm GOTO _startup _endasm
}
#pragma code

// Remap the interrupt vectors (NB: Bootloader code resides between
// 0x000 and 0x7FF, interrupt vectors have moved. See linker script.
#pragma code high_vector=0x808
void interrupt_at_high_vector(void)
{
	_asm GOTO high_isr _endasm
}
#pragma code

#pragma code low_vector=0x818
void interrupt_at_low_vector(void)
{
	_asm GOTO low_isr _endasm
}
#pragma code

// Main program loop
void main(void)
{   
	uint32_t snum = 0xFFFF;
    InitializeSystem();
	enableBuffer();
	
	/*
	writeRAM(0); // write data to byte 0 on boot
	while(1) {
		uint8_t b = readRAM(0);
		if(b & 0x02) LATLEDA = 1;
		Delay10TCYx(60);
	}
	*/
	
	// Configure to async mode
	logicConfig(0x81);
	if(!setSampleNumber(&snum));
	if(!logicStart()) LATLEDB = 1;
	while(1)
	{
		if(samplingComplete())
			LATLEDA = 1;
	}
		
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
