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

#define  IFACEPANEL                      1
#define  IFACEPANEL_QUITBUTTON           2       /* callback function: QUITBUTTON_hit */
#define  IFACEPANEL_DISPLAYTAB           3
#define  IFACEPANEL_SYNCASYNCTAB         4
#define  IFACEPANEL_CAPTUREBUTTON        5       /* callback function: CAPTUREBUTTON_hit */
#define  IFACEPANEL_STATUSBOX            6
#define  IFACEPANEL_DEBUGBUTTON          7       /* callback function: DEBUGBUTTON_hit */
#define  IFACEPANEL_DEBUGBYTE            8
#define  IFACEPANEL_RECONNECTBUTTON      9       /* callback function: RECONNECTBUTTON_hit */
#define  IFACEPANEL_SAMPLENUMBER         10
#define  IFACEPANEL_CONNECTEDLED         11
#define  IFACEPANEL_DEBUGCHECKBOX        12      /* callback function: DEBUGCHECKBOX_hit */
#define  IFACEPANEL_PINGTIMER            13      /* callback function: PINGTIMER_hit */
#define  IFACEPANEL_CAPTUREPROGRESS      14
#define  IFACEPANEL_RETRIEVETIMER        15      /* callback function: RETRIEVETIMER_hit */

     /* tab page panel controls */
#define  LISTPANEL_TEXTBOX               2

     /* tab page panel controls */
#define  TABPANEL_SAMPLEFREQ             2
#define  TABPANEL_RATEMULTIPLIER         3

     /* tab page panel controls */
#define  TABPANEL_2_EDGE                 2

     /* tab page panel controls */
#define  TIMPANEL_DIGGRAPH               2


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */ 

int  CVICALLBACK CAPTUREBUTTON_hit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK DEBUGBUTTON_hit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK DEBUGCHECKBOX_hit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK PINGTIMER_hit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK QUITBUTTON_hit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK RECONNECTBUTTON_hit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK RETRIEVETIMER_hit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
