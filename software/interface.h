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
#define  IFACEPANEL_GENERATELISTINGBUTTON 3      /* callback function: GENERATELISTINGBUTTON_hit */
#define  IFACEPANEL_DISPLAYTAB           4
#define  IFACEPANEL_SYNCASYNCTAB         5
#define  IFACEPANEL_CAPTUREBUTTON        6       /* callback function: CAPTUREBUTTON_hit */
#define  IFACEPANEL_STATUSBOX            7
#define  IFACEPANEL_DEBUGBUTTON          8       /* callback function: DEBUGBUTTON_hit */
#define  IFACEPANEL_DEBUGBYTE            9
#define  IFACEPANEL_RECONNECTBUTTON      10      /* callback function: RECONNECTBUTTON_hit */
#define  IFACEPANEL_SAMPLENUMBER         11
#define  IFACEPANEL_CONNECTEDLED         12
#define  IFACEPANEL_DEBUGCHECKBOX        13      /* callback function: DEBUGCHECKBOX_hit */
#define  IFACEPANEL_PINGTIMER            14      /* callback function: PINGTIMER_hit */
#define  IFACEPANEL_CAPTUREPROGRESS      15
#define  IFACEPANEL_RETRIEVETIMER        16      /* callback function: RETRIEVETIMER_hit */
#define  IFACEPANEL_DUMMYDATABUTTON      17      /* callback function: DUMMYDATABUTTON_hit */
#define  IFACEPANEL_CH1_CHECKBOX         18      /* callback function: CH_CHECKBOX_hit */
#define  IFACEPANEL_CH2_CHECKBOX         19      /* callback function: CH_CHECKBOX_hit */
#define  IFACEPANEL_CH3_CHECKBOX         20      /* callback function: CH_CHECKBOX_hit */
#define  IFACEPANEL_CH4_CHECKBOX         21      /* callback function: CH_CHECKBOX_hit */
#define  IFACEPANEL_CH5_CHECKBOX         22      /* callback function: CH_CHECKBOX_hit */
#define  IFACEPANEL_CH0_CHECKBOX         23      /* callback function: CH_CHECKBOX_hit */
#define  IFACEPANEL_CH6_CHECKBOX         24      /* callback function: CH_CHECKBOX_hit */
#define  IFACEPANEL_CH7_CHECKBOX         25      /* callback function: CH_CHECKBOX_hit */
#define  IFACEPANEL_ALLCHBUTTON          26      /* callback function: ALLCHBUTTON_hit */
#define  IFACEPANEL_NONECHBUTTON         27      /* callback function: NONECHBUTTON_hit */
#define  IFACEPANEL_RANGESLIDER          28      /* callback function: RANGESLIDER_hit */
#define  IFACEPANEL_POSITIONSLIDER       29      /* callback function: POSITIONSLIDER_hit */

     /* tab page panel controls */
#define  LISTPANEL_DATALISTING           2
#define  LISTPANEL_LISTINGHEADING        3

     /* tab page panel controls */
#define  TABPANEL_SAMPLEFREQ             2
#define  TABPANEL_RATEMULTIPLIER         3

     /* tab page panel controls */
#define  TABPANEL_2_EDGE                 2

     /* tab page panel controls */
#define  TIMPANEL_TIMINGDIAGRAM          2


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */ 

int  CVICALLBACK ALLCHBUTTON_hit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CAPTUREBUTTON_hit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CH_CHECKBOX_hit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK DEBUGBUTTON_hit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK DEBUGCHECKBOX_hit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK DUMMYDATABUTTON_hit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK GENERATELISTINGBUTTON_hit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK IFACEPANEL_hit(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK NONECHBUTTON_hit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK PINGTIMER_hit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK POSITIONSLIDER_hit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK QUITBUTTON_hit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK RANGESLIDER_hit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK RECONNECTBUTTON_hit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK RETRIEVETIMER_hit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
