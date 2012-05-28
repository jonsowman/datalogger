#ifndef IO_CFG_H
#define IO_CFG_H

/** I N C L U D E S *************************************************/
#include "autofiles\usbcfg.h"

/** T R I S *********************************************************/
#define INPUT_PIN           1
#define OUTPUT_PIN          0

/** U S B ***********************************************************/
#define tris_usb_bus_sense  TRISAbits.TRISA1    // Input
#define usb_bus_sense       1
#define temp_sense			PORTDbits.RD7

#define tris_self_power     TRISAbits.TRISA2    // Input

#if defined(USE_SELF_POWER_SENSE_IO)
#define self_power          PORTAbits.RA2
#else
#define self_power          1
#endif

/** Switches ********************************************************/
#define sw2                 PORTBbits.RB4

/** P O T ***********************************************************/
#define mInitPOT()          TRISAbits.TRISA0=1;ADCON0=0x01;ADCON2=0x3C;

/** L E D ***********************************************************/
#define mInitAllLEDs()      LATD &= 0xF0; TRISD &= 0xF0;

#define mLED_1              LATDbits.LATD0
#define mLED_2              LATDbits.LATD1
#define mLED_3              LATDbits.LATD2
#define mLED_4              LATDbits.LATD3

#define mLED_1_On()         mLED_1 = 1;
#define mLED_2_On()         mLED_2 = 1;
#define mLED_3_On()         mLED_3 = 1;
#define mLED_4_On()         mLED_4 = 1;

#define mLED_1_Off()        mLED_1 = 0;
#define mLED_2_Off()        mLED_2 = 0;
#define mLED_3_Off()        mLED_3 = 0;
#define mLED_4_Off()        mLED_4 = 0;

#define mLED_1_Toggle()     mLED_1 = !mLED_1;
#define mLED_2_Toggle()     mLED_2 = !mLED_2;
#define mLED_3_Toggle()     mLED_3 = !mLED_3;
#define mLED_4_Toggle()     mLED_4 = !mLED_4;

/***** Actual logic analyser pins ****************************/
/******** SRAM Address Lines *******/
#define LATADDR16		LATBbits.LATB2

#define LATADDR15		LATDbits.LATD7
#define LATADDR14		LATDbits.LATD6
#define LATADDR13		LATDbits.LATD5
#define LATADDR12		LATDbits.LATD4
#define LATADDR11		LATDbits.LATD3
#define LATADDR10		LATDbits.LATD2
#define LATADDR9		LATDbits.LATD1
#define LATADDR8		LATDbits.LATD0

#define LATADDR7		LATBbits.LATB7
#define LATADDR6		LATBbits.LATB6
#define LATADDR5		LATBbits.LATB5
#define LATADDR4		LATBbits.LATB4
#define LATADDR3		LATBbits.LATB3

#define LATADDR2		LATAbits.LATA5
#define LATADDR1		LATAbits.LATA4
#define LATADDR0		LATAbits.LATA3

#define TRISADDR16		TRISBbits.TRISB2

#define TRISADDR15		TRISDbits.TRISD7
#define TRISADDR14		TRISDbits.TRISD6
#define TRISADDR13		TRISDbits.TRISD5
#define TRISADDR12		TRISDbits.TRISD4
#define TRISADDR11		TRISDbits.TRISD3
#define TRISADDR10		TRISDbits.TRISD2
#define TRISADDR9		TRISDbits.TRISD1
#define TRISADDR8		TRISDbits.TRISD0

#define TRISADDR7		TRISBbits.TRISB7
#define TRISADDR6		TRISBbits.TRISB6
#define TRISADDR5		TRISBbits.TRISB5
#define TRISADDR4		TRISBbits.TRISB4
#define TRISADDR3		TRISBbits.TRISB3

#define TRISADDR2		TRISAbits.TRISA5
#define TRISADDR1		TRISAbits.TRISA4
#define TRISADDR0		TRISAbits.TRISA3

/********* SRAM Control Lines ********/
#define LATCE			LATCbits.LATC1
#define LATOE			LATCbits.LATC0
#define	LATWE			LATCbits.LATC2
#define	LATCE2			LATCbits.LATC6

#define TRISCE			TRISCbits.TRISC1
#define TRISOE			TRISCbits.TRISC0
#define	TRISWE			TRISCbits.TRISC2
#define	TRISCE2			TRISCbits.TRISC6

/****Shift Regiser Control Lines *****/
#define LATSR_CLK		LATEbits.LATE0
#define LATSR_CLK_EN	LATEbits.LATE1
#define LATSR_PLOAD		LATEbits.LATE2

#define PORTSR_SEROUT	PORTAbits.PORTA2

#define TRISSR_CLK		TRISEbits.TRISE0
#define TRISSR_CLK_EN	TRISEbits.TRISE1
#define TRISSR_PLOAD	TRISEbits.TRISE2
#define TRISSR_SEROUT	TRISAbits.TRISA2

/**** Other *****/
#define LATBUFFER_EN	LATCbits.LATC7

#define PORTDATA0		PORTBbits.PORTB0

#define TRISBUFFER_EN	TRISCbits.TRISC7
#define TRISDATA0		TRISBbits.TRISB0

#define TRISLEDA		TRISAbits.TRISA0
#define TRISLEDB		TRISAbits.TRISA1
#define LATLEDA			LATAbits.LATA0
#define LATLEDB			LATAbits.LATA1

#endif //IO_CFG_H
