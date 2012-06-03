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
#define  IFACEPANEL_DUMMYDATABUTTON      16      /* callback function: DUMMYDATABUTTON_hit */
#define  IFACEPANEL_CH1_CHECKBOX         17      /* callback function: CH_CHECKBOX_hit */
#define  IFACEPANEL_CH2_CHECKBOX         18      /* callback function: CH_CHECKBOX_hit */
#define  IFACEPANEL_CH3_CHECKBOX         19      /* callback function: CH_CHECKBOX_hit */
#define  IFACEPANEL_CH4_CHECKBOX         20      /* callback function: CH_CHECKBOX_hit */
#define  IFACEPANEL_CH5_CHECKBOX         21      /* callback function: CH_CHECKBOX_hit */
#define  IFACEPANEL_CH0_CHECKBOX         22      /* callback function: CH_CHECKBOX_hit */
#define  IFACEPANEL_CH6_CHECKBOX         23      /* callback function: CH_CHECKBOX_hit */
#define  IFACEPANEL_CH7_CHECKBOX         24      /* callback function: CH_CHECKBOX_hit */
#define  IFACEPANEL_ALLCHBUTTON          25      /* callback function: ALLCHBUTTON_hit */
#define  IFACEPANEL_NONECHBUTTON         26      /* callback function: NONECHBUTTON_hit */
#define  IFACEPANEL_RANGESLIDER          27      /* callback function: RANGESLIDER_hit */
#define  IFACEPANEL_POSITIONSLIDER       28      /* callback function: POSITIONSLIDER_hit */
#define  IFACEPANEL_CAPTUREMODE          29      /* callback function: CAPTUREMODE_hit */
#define  IFACEPANEL_CLOCKEDGE            30
#define  IFACEPANEL_ASYNCTRIGGER         31      /* callback function: ASYNCTIRGGER_hit */
#define  IFACEPANEL_SAMPLERATE           32
#define  IFACEPANEL_DECORATION           33

     /* tab page panel controls */
#define  LISTPANEL_DATALISTING           2
#define  LISTPANEL_LISTINGHEADING        3

     /* tab page panel controls */
#define  TIMPANEL_TIMINGDIAGRAM          2


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */ 

int  CVICALLBACK ALLCHBUTTON_hit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK ASYNCTIRGGER_hit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CAPTUREBUTTON_hit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CAPTUREMODE_hit(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
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
