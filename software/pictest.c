#include <cvirte.h>		
#include <userint.h>
//#include "pictest.h"
#include "picdriver.h"  
#include "dummy_interface.h"
#include <stdio.h>

#define true 1
#define false 0

static int panelHandle;

int main (int argc, char *argv[])
{
	if (InitCVIRTE (0, argv, 0) == 0)
		return -1;	/* out of memory */
	if ((panelHandle = LoadPanel (0, "dummy_interface.uir", PANEL2)) < 0)
		return -1;
	DisplayPanel (panelHandle);
	init_usb();
	RunUserInterface ();
	close_usb();
	DiscardPanel (panelHandle);
	return 0;
}



int CVICALLBACK quit_button_hit (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
    if (event!=EVENT_COMMIT) return 0;  
    QuitUserInterface (0);
    return 0;
}
				 
int CVICALLBACK debug_hit (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int received=0;
	char displaystring[33];
	
	switch (event)
	{
		case EVENT_COMMIT: // Button clicked...
			
			if(read_analog_input (&received) == USB_NO_ERROR) // Blocks til value returned
			{
				sprintf(displaystring, "%x");
			
				SetCtrlVal(panel,PANEL2_DEBUGBYTE, displaystring); // Display as decimal (currently)
			}
			else
				SetCtrlVal(panel,PANEL2_DEBUGBYTE, "error");
			
			break;
		

	}
	return 0;
}
