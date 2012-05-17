/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/* Copyright (c) National Instruments 2012. All Rights Reserved.          */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  PANEL2                          1
#define  PANEL2_quit_button              2       /* callback function: quit_button_hit */
#define  PANEL2_DISPLAYTAB               3
#define  PANEL2_RADIOBUTTON              4
#define  PANEL2_RADIOBUTTON_2            5
#define  PANEL2_NUMERIC                  6
#define  PANEL2_RADIOBUTTON_3            7
#define  PANEL2_RADIOBUTTON_4            8
#define  PANEL2_RADIOBUTTON_5            9
#define  PANEL2_TEXTMSG                  10
#define  PANEL2_DECORATION               11
#define  PANEL2_DECORATION_2             12
#define  PANEL2_TEXTMSG_2                13


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */ 

int  CVICALLBACK quit_button_hit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
