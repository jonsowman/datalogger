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
#define A16		LATBbits.LATB2

#define A15		LATDbits.LATD7
#define A14		LATDbits.LATD6
#define A13		LATDbits.LATD5
#define A12		LATDbits.LATD4
#define A11		LATDbits.LATD3
#define A10		LATDbits.LATD2
#define A9		LATDbits.LATD1
#define A8		LATDbits.LATD0

#define A7		LATBbits.LATB7
#define A6		LATBbits.LATB6
#define A5		LATBbits.LATB5
#define A4		LATBbits.LATB4
#define A3		LATBbits.LATB3

#define A2		LATAbits.LATA5
#define A1		LATAbits.LATA4
#define A0		LATAbits.LATA3

/********* SRAM Control Lines ********/
#define CE		LATCbits.LATC1
#define OE		LATCbits.LATC0
#define	WE		LATCbits.LATC2
#define	CE2		LATCbits.LATC6

/****Shift Regiser Control Lines *****/
#define SR_CLK		LATEbits.LATE0
#define SR_CLK_EN	LATEbits.LATE1
#define SR_PLOAD	LATEbits.LATE2
#define SR_SEROUT	LATAbits.LATA2

/**** Other *****/
#define BUFFER_EN	LATCbits.LATC7


#endif //IO_CFG_H
