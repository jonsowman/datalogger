#include <cvirte.h>		
#include <userint.h>
#include "pictest.h"
#include "picdriver.h"  

#define true 1
#define false 0

static int panelHandle;

int main (int argc, char *argv[])
{
	if (InitCVIRTE (0, argv, 0) == 0)
		return -1;	/* out of memory */
	if ((panelHandle = LoadPanel (0, "pictest.uir", PANEL)) < 0)
		return -1;
	DisplayPanel (panelHandle);
	init_usb();
	RunUserInterface ();
	close_usb();
	DiscardPanel (panelHandle);
	return 0;
}

int CVICALLBACK read_analog_button_hit (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{   int value;

    if (event!=EVENT_COMMIT) return 0;
    
    read_analog_input (&value);

    SetCtrlVal (panel, PANEL_analog_input_slide, value);
    
    return 0;
}

int CVICALLBACK quit_button_hit (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
    if (event!=EVENT_COMMIT) return 0;  
    QuitUserInterface (0);
    return 0;
}

int CVICALLBACK led_button_hit (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{   int button_state;

    if (event!=EVENT_COMMIT) return 0; 
    
    GetCtrlVal(panel, control, &button_state);
    
    set_led (button_state);
    
	return 0;
}
