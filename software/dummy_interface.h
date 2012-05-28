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
#define  PANEL2_TEXTMSG_3                4
#define  PANEL2_TEXTMSG_2                5
#define  PANEL2_TEXTMSG                  6
#define  PANEL2_TAB                      7
#define  PANEL2_COMMANDBUTTON            8
#define  PANEL2_LISTBOX                  9

     /* tab page panel controls */
#define  LISTPANEL_TEXTBOX               2

     /* tab page panel controls */
#define  TABPANEL_NUMERIC                2
#define  TABPANEL_RADIOBUTTON_3          3
#define  TABPANEL_RADIOBUTTON_4          4

     /* tab page panel controls */
#define  TABPANEL_2_TEXTMSG_2            2
#define  TABPANEL_2_RADIOBUTTON          3
#define  TABPANEL_2_RADIOBUTTON_2        4

     /* tab page panel controls */
#define  TIMPANEL_DIGGRAPH               2


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */ 

int  CVICALLBACK quit_button_hit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
