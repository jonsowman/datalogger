#include "radioGroup.h"
#include <ansi_c.h>
#include <cvirte.h>		
#include <userint.h>
#include <math.h>
#include <time.h>

static int TABPANEL; // These two are async/sync tabs
static int TABPANEL_2; // labwindows doesn't give us
					// constants for pages in tabs

#include "comms.h"  
#include "interface.h"
#include <stdio.h>



//static int panelHandle;

void StatusMessage(int panel, int statusbox, char *message)
{
	struct tm *timeinfo;
	time_t now = time(NULL);
	char buf[128];
	int count;
	
	timeinfo = localtime(&now);
	
	sprintf(buf,"%d:%02d:%02d - ", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
	
	strncat(buf,message,128-sizeof("00:00:00 - "));
	
	
	InsertListItem(panel, statusbox, -1, buf, 0);
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
	
	if(init_usb() == SUCCESS)
	{
		StatusMessage(panelHandle, IFACEPANEL_STATUSBOX, "Connected to logic analyser");
		SetCtrlVal(panelHandle, IFACEPANEL_CONNECTEDLED, 1);
	}
	else
	{
		StatusMessage(panelHandle, IFACEPANEL_STATUSBOX, "Failed to connect to logic analyser");
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
			
			if(read_debug_byte (&received) == SUCCESS) // Blocks til value returned
			{
				sprintf(displaystring, "%x", received);
			
				SetCtrlVal(panel,IFACEPANEL_DEBUGBYTE, displaystring); // Display as decimal (currently)
				sprintf(statusmessage, "Retrieved debug byte: %s", displaystring);
				StatusMessage(panel, IFACEPANEL_STATUSBOX, statusmessage);
			}
			else
			{
				SetCtrlVal(panel,IFACEPANEL_DEBUGBYTE, "error");
				StatusMessage(panel, IFACEPANEL_STATUSBOX, "Error retrieving debug byte");
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
	int result;

	if(event != EVENT_COMMIT)
		return 0; // not a click! mouseover etc.
	
	
	
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
		
	result = send_config_message(async, sync, rising, falling, both, rate, samplenumber);
	
	if(result == USB_ERROR)
	{
		StatusMessage(panel, IFACEPANEL_STATUSBOX, "Failed to configure analyser - connection fault");
		SetCtrlVal(panel, IFACEPANEL_CONNECTEDLED, 0);
		return 0;
	}
	
	if(result == CONFIG_ERROR)
	{
		StatusMessage(panel,IFACEPANEL_STATUSBOX, "Failed to configure analyser - configuration invalid");
		return 0;
	}
	
	StatusMessage(panel, IFACEPANEL_STATUSBOX, "Successfully configured analyser");
		
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
			if(close_usb() == SUCCESS)
			{
				StatusMessage(panel, IFACEPANEL_STATUSBOX, "Disconnected from logic analyser");
				SetCtrlVal(panel, IFACEPANEL_CONNECTEDLED, 0);
			}
			else
				StatusMessage(panel, IFACEPANEL_STATUSBOX, "Failed to disconnect from logic analyser");
				
				
				
			if(init_usb() == SUCCESS)
			{
				StatusMessage(panel, IFACEPANEL_STATUSBOX, "Connected to logic analyser");
				SetCtrlVal(panel, IFACEPANEL_CONNECTEDLED, 1);
			}
			else
			{
				StatusMessage(panel, IFACEPANEL_STATUSBOX, "Failed to connect to logic analyser");
				SetCtrlVal(panel, IFACEPANEL_CONNECTEDLED, 0);
				close_usb();
			}

			break;
	}
	return 0;
}

int CVICALLBACK PINGTIMER_hit (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int AnalyserConnected;
	
	if(event != EVENT_TIMER_TICK)
		return 0; // not a tick!
	
	GetCtrlVal(panel, IFACEPANEL_CONNECTEDLED, &AnalyserConnected);
	if(AnalyserConnected != 1)
		return 0;
	
	if(do_ping() != SUCCESS)
	{
		StatusMessage(panel, IFACEPANEL_STATUSBOX, "Logic analyser stopped responding - disconnecting");
		// Say disconnecting - in reality do_ping() has already called the usb_disconnect, since that's in comm.c
		
		SetCtrlVal(panel, IFACEPANEL_CONNECTEDLED, 0);
	}
	
	return 0;
}
