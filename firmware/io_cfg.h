#ifndef IO_CFG_H
#define IO_CFG_H

/** I N C L U D E S *************************************************/
#include "autofiles\usbcfg.h"

/** T R I S *********************************************************/
#define INPUT_PIN           1
#define OUTPUT_PIN          0

/** U S B ***********************************************************/
#define tris_usb_bus_sense  TRISAbits.TRISA1    // Input
//mod by Tim
//#if defined(USE_USB_BUS_SENSE_IO)
//#define usb_bus_sense       PORTAbits.RA1
//#else
//remove "if" condition
#define usb_bus_sense       1
//#endif
#define temp_sense			PORTDbits.RD7
//end mod by Tim

#define tris_self_power     TRISAbits.TRISA2    // Input

#if defined(USE_SELF_POWER_SENSE_IO)
#define self_power          PORTAbits.RA2
#else
#define self_power          1
#endif

//mod by Tim
/** Switches ********************************************************/
#define sw2                 PORTBbits.RB4

/** P O T ***********************************************************/
#define mInitPOT()          TRISAbits.TRISA0=1;ADCON0=0x01;ADCON2=0x3C;

//end mod by Tim

/** L E D ***********************************************************/
//mod by Tim
//Original
#define mInitAllLEDs()      LATD &= 0xF0; TRISD &= 0xF0;
//mod by Tim
//#define mInitAllLEDs()      LATD &= 0xFF; TRISD &= 0x00;
//end mod by Tim

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

#endif //IO_CFG_H
