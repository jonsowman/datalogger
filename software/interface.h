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

#define  IFACEPANEL                      1       /* callback function: IFACEPANEL_hit */
#define  IFACEPANEL_QUITBUTTON           2       /* callback function: QUITBUTTON_hit */
#define  IFACEPANEL_DISPLAYTAB           3       /* callback function: DISPLAYTAB_hit */
#define  IFACEPANEL_TEXTMSG_3            4
#define  IFACEPANEL_TEXTMSG_2            5
#define  IFACEPANEL_TEXTMSG              6
#define  IFACEPANEL_SYNCASYNCTAB         7       /* callback function: SYNCASYNCTAB_hit */
#define  IFACEPANEL_CAPTUREBUTTON        8       /* callback function: CAPTUREBUTTON_hit */
#define  IFACEPANEL_STATUSBOX            9
#define  IFACEPANEL_DEBUGBUTTON          10      /* callback function: DEBUGBUTTON_hit */
#define  IFACEPANEL_DEBUGBYTE            11
#define  IFACEPANEL_RECONNECTBUTTON      12      /* callback function: RECONNECTBUTTON_hit */
#define  IFACEPANEL_SAMPLENUMBER         13

     /* tab page panel controls */
#define  LISTPANEL_TEXTBOX               2

     /* tab page panel controls */
#define  TABPANEL_SAMPLEFREQ             2
#define  TABPANEL_SAMPLEFREQ_HZ          3
#define  TABPANEL_SAMPLEFREQ_KHZ         4

     /* tab page panel controls */
#define  TABPANEL_2_EDGE                 2       /* callback function: EDGE_hit */

     /* tab page panel controls */
#define  TIMPANEL_DIGGRAPH               2


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */ 

int  CVICALLBACK CAPTUREBUTTON_hit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK DEBUGBUTTON_hit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK DISPLAYTAB_hit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK EDGE_hit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK IFACEPANEL_hit(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK QUITBUTTON_hit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK RECONNECTBUTTON_hit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK SYNCASYNCTAB_hit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
