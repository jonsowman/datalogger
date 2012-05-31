#include "radioGroup.h"
#include <ansi_c.h>
#include <cvirte.h>		
#include <userint.h>
#include <math.h>

static int TABPANEL; // These two are async/sync tabs
static int TABPANEL_2; // labwindows doesn't give us
					// constants for pages in tabs

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
	
	
	GetPanelHandleFromTabPage (panelHandle, IFACEPANEL_SYNCASYNCTAB, 0, &TABPANEL);
	GetPanelHandleFromTabPage (panelHandle, IFACEPANEL_SYNCASYNCTAB, 1, &TABPANEL_2);
	
    Radio_ConvertFromTree (TABPANEL, TABPANEL_RATEMULTIPLIER);
	Radio_ConvertFromTree (TABPANEL_2, TABPANEL_2_EDGE);
	
	
	DisplayPanel (panelHandle);
	
	if(init_usb() == USB_NO_ERROR)
	{
		AnalyserConnected=true;
		StatusMessage(panelHandle, IFACEPANEL_STATUSBOX, "Connected to logic analyser!");
		SetCtrlVal(panelHandle, IFACEPANEL_CONNECTEDLED, 1);
	}
	else
	{
		AnalyserConnected=false;
		StatusMessage(panelHandle, IFACEPANEL_STATUSBOX, "Failed to connect to logic analyser...");
		SetCtrlVal(panelHandle, IFACEPANEL_CONNECTEDLED, 0);
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
				SetCtrlVal(panel, IFACEPANEL_CONNECTEDLED, 0);
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
	double ratedouble;
	int multiplier;

	
	
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
				
				GetCtrlVal(TABPANEL, TABPANEL_SAMPLEFREQ, &ratedouble);
				
				Radio_GetMarkedOption (TABPANEL, TABPANEL_RATEMULTIPLIER, &multiplier);
				
				if(multiplier==1)
					ratedouble*=1000;
				
				rate = floor(ratedouble+0.5); // C doesn't have a round function, this is equivilent.
			}
			else // sync
			{
				sync=1;
				async=rate=0;
			
				Radio_GetMarkedOption (TABPANEL_2, TABPANEL_2_EDGE, &edge);
				
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
			

			}
			
			GetCtrlVal(panel, IFACEPANEL_SAMPLENUMBER, &samplenumber);
				
			if(send_config_message(async, sync, rising, falling, both, rate, samplenumber) != USB_NO_ERROR)
			{
				SetCtrlVal(panel, IFACEPANEL_CONNECTEDLED, 0);
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

int CVICALLBACK EDGE_hit (int panel, int control, int event,
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
				SetCtrlVal(panel, IFACEPANEL_CONNECTEDLED, 0);
			}
			else
				StatusMessage(panel, IFACEPANEL_STATUSBOX, "Failed to disconnect from logic analyser...");
				
				
				
			if(init_usb() == USB_NO_ERROR)
			{
				AnalyserConnected=true;
				StatusMessage(panel, IFACEPANEL_STATUSBOX, "Connected to logic analyser!");
				SetCtrlVal(panel, IFACEPANEL_CONNECTEDLED, 1);
			}
			else
			{
				StatusMessage(panel, IFACEPANEL_STATUSBOX, "Failed to connect to logic analyser...");
				AnalyserConnected=false;
				SetCtrlVal(panel, IFACEPANEL_CONNECTEDLED, 0);
				close_usb();
			}

			break;
	}
	return 0;
}
