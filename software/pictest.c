#include <cvirte.h>		
#include <userint.h>
//#include "pictest.h"
#include "picdriver.h"  
#include "interface.h"
#include <stdio.h>


//static int panelHandle;
bool AnalyserConnected;

int main (int argc, char *argv[])
{
	int panelHandle;
	
	if (InitCVIRTE (0, argv, 0) == 0)
		return -1;	/* out of memory */
	if ((panelHandle = LoadPanel (0, "interface.uir", IFACEPANEL)) < 0)
		return -1;
	DisplayPanel (panelHandle);
	
	if(init_usb() == USB_NO_ERROR)
	{
		AnalyserConnected=true;
		InsertListItem(panelHandle, IFACEPANEL_STATUSBOX, 0, "Connected to logic analyser!", 0);
	}
	else
	{
		AnalyserConnected=false;
		InsertListItem(panelHandle, IFACEPANEL_STATUSBOX, 0, "Failed to connect to logic analyser...", 0);
	}
	
	RunUserInterface ();
	
	close_usb();
	
	DiscardPanel (panelHandle);
	return 0;
}



int CVICALLBACK QUITBUTTON_hit (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
    if (event!=EVENT_COMMIT) return 0;  
    QuitUserInterface (0);
    return 0;
}
				 
int CVICALLBACK DEBUGBUTTON_hit (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int received=0;
	char displaystring[33];
	
	switch (event)
	{
		case EVENT_COMMIT: // Button clicked...
			
			if(read_debug_byte (&received) == USB_NO_ERROR) // Blocks til value returned
			{
				sprintf(displaystring, "%x", received);
			
				SetCtrlVal(panel,IFACEPANEL_DEBUGBYTE, displaystring); // Display as decimal (currently)
			}
			else
				SetCtrlVal(panel,IFACEPANEL_DEBUGBYTE, "error");
			
			break;
		

	}
	return 0;
}

int CVICALLBACK CAPTUREBUTTON_hit (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
		
			break;
	}
	return 0;
}

int CVICALLBACK IFACEPANEL_hit (int panel, int event, void *callbackData,
		int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_GOT_FOCUS:

			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_CLOSE:

			QuitUserInterface(0);
	
			break;
	}
	return 0;
}

int CVICALLBACK SYNCASYNCTAB_hit (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_ACTIVE_TAB_CHANGE:

			break;
	}
	return 0;
}

int CVICALLBACK RISINGEDGE_hit (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:

			break;
	}
	return 0;
}

int CVICALLBACK FALLINGEDGE_hit (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:

			break;
	}
	return 0;
}

int CVICALLBACK DISPLAYTAB_hit (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_ACTIVE_TAB_CHANGE:

			break;
	}
	return 0;
}

int CVICALLBACK RECONNECTBUTTON_hit (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			if(close_usb() == USB_NO_ERROR)
			{
				AnalyserConnected=false;
				InsertListItem(panel, IFACEPANEL_STATUSBOX, 0, "Disconnected from logic analyser.", 0);
			}
			else
				InsertListItem(panel, IFACEPANEL_STATUSBOX, 0, "Failed to disconnect from logic analyser...", 0);
				
				
				
			if(init_usb() == USB_NO_ERROR)
			{
				AnalyserConnected=true;
				InsertListItem(panel, IFACEPANEL_STATUSBOX, 0, "Connected to logic analyser!", 0);
			}
			else
			{
				InsertListItem(panel, IFACEPANEL_STATUSBOX, 0, "Failed to connect to logic analyser...", 0);
				AnalyserConnected=false;
				close_usb();
			}

			break;
	}
	return 0;
}
