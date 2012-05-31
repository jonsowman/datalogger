#include "radioGroup.h"
#include <ansi_c.h>
#include <cvirte.h>		
#include <userint.h>

static int tabpanel2;

//#include "pictest.h"
#include "picdriver.h"  
#include "interface.h"
#include <stdio.h>



//static int panelHandle;
bool AnalyserConnected;

void StatusMessage(int panel, int statusbox, char *message)
{
	int count;
	InsertListItem(panel, statusbox, -1, message, 0);
	GetNumListItems(panel,statusbox,&count);
	SetCtrlIndex (panel, statusbox, count-1);
}

int main (int argc, char *argv[])
{
	int panelHandle;
	
	if (InitCVIRTE (0, argv, 0) == 0)
		return -1;	/* out of memory */
	if ((panelHandle = LoadPanel (0, "interface.uir", IFACEPANEL)) < 0)
		return -1;
	GetPanelHandleFromTabPage (panelHandle, IFACEPANEL_SYNCASYNCTAB, 1, &tabpanel2);
    Radio_ConvertFromTree (tabpanel2, TABPANEL_2_EDGE);
	DisplayPanel (panelHandle);
	
	if(init_usb() == USB_NO_ERROR)
	{
		AnalyserConnected=true;
		StatusMessage(panelHandle, IFACEPANEL_STATUSBOX, "Connected to logic analyser!");
	}
	else
	{
		AnalyserConnected=false;
		StatusMessage(panelHandle, IFACEPANEL_STATUSBOX, "Failed to connect to logic analyser...");
	}
	
	RunUserInterface ();
	
	StatusMessage(panelHandle, IFACEPANEL_STATUSBOX, "Done for now!");
	
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
	char statusmessage[64];
	
	switch (event)
	{
		case EVENT_COMMIT: // Button clicked...
			
			if(read_debug_byte (&received) == USB_NO_ERROR) // Blocks til value returned
			{
				sprintf(displaystring, "%x", received);
			
				SetCtrlVal(panel,IFACEPANEL_DEBUGBYTE, displaystring); // Display as decimal (currently)
				sprintf(statusmessage, "Retrieved debug byte: %s", displaystring);
				StatusMessage(panel, IFACEPANEL_STATUSBOX, statusmessage);
			}
			else
			{
				SetCtrlVal(panel,IFACEPANEL_DEBUGBYTE, "error");
				StatusMessage(panel, IFACEPANEL_STATUSBOX, "Error retrieving debug byte...");
			}
			
			break;
		

	}
	return 0;
}

int CVICALLBACK CAPTUREBUTTON_hit (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int async, sync, rising, falling, both;
	unsigned long rate, samplenumber;
	int asyncsynctab;
	int edge;

	
	
	switch (event)
	{
		case EVENT_COMMIT:
			// Generate config byte, send it
			// check whether config was valid
			// Rest is TODO.
			
			
			
			GetActiveTabPage(panel, IFACEPANEL_SYNCASYNCTAB, &asyncsynctab);
			if(asyncsynctab==0) // async
			{
				async=1;
				sync=rising=falling=both=0;
				
				// TODO:
				rate=0;
				
				//GetCtrlVal(panel, s
			}
			else // sync
			{
				sync=1;
				async=rate=0;
			
				Radio_GetMarkedOption(panel, TABPANEL_2_EDGE, &edge);
				printf("%d", edge);
				switch(edge)
				{
					case 0:
						rising = true;
						falling = both = false;
						break;
						
					case 1:
						falling = true;
						rising = both = false;
						break;
						
					case 2:
						both = true;
						rising = falling = false;
						break;
			}
			
			GetCtrlVal(panel, IFACEPANEL_SAMPLENUMBER, &samplenumber);
			
				
				
			
			send_config_message(async, sync, rising, falling, both, rate, samplenumber);
			

		}			
		
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
				StatusMessage(panel, IFACEPANEL_STATUSBOX, "Disconnected from logic analyser.");
			}
			else
				StatusMessage(panel, IFACEPANEL_STATUSBOX, "Failed to disconnect from logic analyser...");
				
				
				
			if(init_usb() == USB_NO_ERROR)
			{
				AnalyserConnected=true;
				StatusMessage(panel, IFACEPANEL_STATUSBOX, "Connected to logic analyser!");
			}
			else
			{
				StatusMessage(panel, IFACEPANEL_STATUSBOX, "Failed to connect to logic analyser...");
				AnalyserConnected=false;
				close_usb();
			}

			break;
	}
	return 0;
}
