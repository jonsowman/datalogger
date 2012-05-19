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

#pragma code highVector=0x08
void _high_ISR (void)
{
	while(1)
		LATDbits.LATD1=1;



//    _asm goto high_isr _endasm
}

#pragma code lowVector=0x18
void _low_ISR (void)
{
	while(1)
		LATDbits.LATD1=1;



//    _asm goto low_isr _endasm
}

#pragma interruptlow high_isr
void high_isr(void)
{
	while(1)
		LATDbits.LATD1 = 1; // Turn RD1 on



	if(INTCONbits.TMR0IF)
	{
		TMR0L = 0x00;
		INTCONbits.TMR0IF = 0;
	}
}

#pragma interruptlow low_isr
void low_isr(void)
{
	while(1)
		LATDbits.LATD1=1;





	LATDbits.LATD1 = 1; // Turn RD1 on
	if(INTCONbits.TMR0IF)
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

	// Enable interrupt priority
	RCONbits.IPEN = 1;

	// Stop timer
	T0CONbits.TMR0ON = 0;

	// Set to 8 bit mode
	T0CONbits.T08BIT = 1;

	// Clock on instruction clock cycles
	T0CONbits.T0CS = 0;
	
	// 256 prescaler
	T0CONbits.PSA = 0;
	T0CONbits.T0PS2 = 1;
	T0CONbits.T0PS1  = 1;
	T0CONbits.T0PS0 = 1; // Prescaled to 256:1 -
							// one clock per 256 cycles
			
	// Enable TIMER0 OVF interrupt, periph interrupt
	// and global interrupts
	INTCONbits.GIEL  = 1;
	INTCONbits.GIEH = 1;

	INTCONbits.TMR0IF = 0; // Clear overflow flag
	INTCONbits.TMR0IE = 1; // Enable T0 interrupt.
	
	// Set TIMER0 OVF to high priority
	INTCON2bits.TMR0IP = 1;

	// Finally enable the timer
	T0CONbits.TMR0ON = 1;

	TMR0L = 1; // Clear the timer *to 1*
	LATDbits.LATD1=0; // Check LED off.

	while(1)
	{


		//if(INTCONbits.TMR0IF == 1) // If overflow occured
		//	while(1)
		//		LATDbits.LATD1 = 1;


	}
// Don't get here.
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
