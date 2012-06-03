#define LOGICANALYSER_C
// Used to correctly wangle extern variables


#include "radioGroup.h"
#include <ansi_c.h>
#include <cvirte.h>		
#include <userint.h>
#include <math.h>
#include <time.h>

static int TABPANEL; // These two are async/sync tabs
static int TABPANEL_2; // labwindows doesn't give us
					// constants for pages in tabs

static int DISPLAYTABPANEL1;
static int DISPLAYTABPANEL2;

#include "comms.h"  
#include "interface.h"
#include <stdio.h>

// Couple of static state variables:
BOOL capture_begun;
BOOL retrieval_begun;
unsigned int GetDatasPerTick;


char *datastore=NULL;
char *datastoreptr=NULL;
unsigned int datalength=0;


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

void UpdateSliders(int panel) // Call this any time you update datalength or move the range slider
{
	unsigned int position, range;
	
	// Set max range to 20 or datalength, whichever is smaller, unless datalength=0 in which case set max range to 1
	// (Doesn't like min = max = 0)
	if(datalength>20)
		SetCtrlAttribute(panel, IFACEPANEL_RANGESLIDER, ATTR_MAX_VALUE, 20);
	else
		if(datalength == 0)
			SetCtrlAttribute(panel, IFACEPANEL_RANGESLIDER, ATTR_MAX_VALUE, 1);
		else
			SetCtrlAttribute(panel, IFACEPANEL_RANGESLIDER, ATTR_MAX_VALUE, datalength);
		
	GetCtrlVal(panel, IFACEPANEL_RANGESLIDER, &range);
	
	
	// Now setup the position max:
	if(datalength == 0)
		SetCtrlAttribute(panel, IFACEPANEL_POSITIONSLIDER, ATTR_MAX_VALUE, 1);
	else
		SetCtrlAttribute(panel, IFACEPANEL_POSITIONSLIDER, ATTR_MAX_VALUE, datalength-range);
}

int main (int argc, char *argv[])
{
	int panelHandle;
	
	if (InitCVIRTE (0, argv, 0) == 0)
		return -1;	/* out of memory */
	if ((panelHandle = LoadPanel (0, "interface.uir", IFACEPANEL)) < 0)
		return -1;
	
	// Create datastore:
	datastore = malloc(sizeof(char) * DATASTORE_SIZE);
	if(datastore == NULL)
	{
		printf("Out of memory!\n");
		return -1;
	}
	
	
	
	GetPanelHandleFromTabPage (panelHandle, IFACEPANEL_SYNCASYNCTAB, 0, &TABPANEL);
	GetPanelHandleFromTabPage (panelHandle, IFACEPANEL_SYNCASYNCTAB, 1, &TABPANEL_2);
	
	GetPanelHandleFromTabPage (panelHandle, IFACEPANEL_DISPLAYTAB, 0, &DISPLAYTABPANEL1);
	GetPanelHandleFromTabPage (panelHandle, IFACEPANEL_DISPLAYTAB, 1, &DISPLAYTABPANEL2);
	
	
	
    Radio_ConvertFromTree (TABPANEL, TABPANEL_RATEMULTIPLIER);
	Radio_ConvertFromTree (TABPANEL_2, TABPANEL_2_EDGE);
	
	
	DisplayPanel (panelHandle);
	
	GetCtrlVal(panelHandle, IFACEPANEL_DEBUGCHECKBOX, &debug);
	
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
	
	datalength=0;
	UpdateSliders(panelHandle);
	
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
	
	// Clear out old data:
	datalength = 0;
	UpdateSliders(panel);
	
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
	
	SetCtrlAttribute(panel, IFACEPANEL_CAPTUREPROGRESS, ATTR_MAX_VALUE, samplenumber);
	// Set sample counter widget max to samplenumber
	
	SetCtrlVal(panel, IFACEPANEL_CAPTUREPROGRESS, 0);
	// reset sample counter widget
		
	// Let's get this capture started:
	
	// We send an ARM request:
	if(send_arm_request() != SUCCESS)
	{
		// Failed to arm :(
		StatusMessage(panel, IFACEPANEL_STATUSBOX, "Failed to arm analyser");
		return 0;
	}
	
	// Now we are armed we hand off to the timer to poll and retrieve data.
	StatusMessage(panel, IFACEPANEL_STATUSBOX, "Analyser armed");

	
	capture_begun = false; // state used later - set once samples have been taken
	retrieval_begun = false; // Set once capture finished
	datastoreptr=datastore;
	// Start the retrieve timer:
	
	SetCtrlAttribute(panel, IFACEPANEL_RETRIEVETIMER, ATTR_INTERVAL, 0.1); // 10Hz for capture stage
	SetCtrlAttribute(panel, IFACEPANEL_RETRIEVETIMER, ATTR_ENABLED, 1); // Start the timer
		
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

int CVICALLBACK DEBUGCHECKBOX_hit (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	if(event != EVENT_COMMIT)
		return 0; // Not a click!

	GetCtrlVal(panel, IFACEPANEL_DEBUGCHECKBOX, &debug);
	
	return 0;
}

int CVICALLBACK RETRIEVETIMER_hit (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	unsigned int numsamples, max, sampleptr, state, i;

	if(event != EVENT_TIMER_TICK)
		return 0; // not a tick!
	
	
	if(!retrieval_begun) // Still in the capture phase - poll!
	{
		if(poll_state(&sampleptr, &state) != SUCCESS)
		{
			if(debug) printf("poll_state returned non-success\n");
			return 0;
		}
	
		switch(state) // Sampleptr is only set if STATE_PROG.
		{
			case STATE_START:
				// Just wait for the next tick and hope the state is something more helpful
				return 0;
			
			case STATE_WAIT:
				// Waiting for capture to begin
				StatusMessage(panel, IFACEPANEL_STATUSBOX, "Waiting for trigger/clock");
				return 0;
			
			case STATE_PROG:
				// Capture in progress!
			
				if(!capture_begun)
				{
					// Output status message only once!
					StatusMessage(panel, IFACEPANEL_STATUSBOX, "Capture begun");
					capture_begun=true;
				}
			
				SetCtrlVal(panel, IFACEPANEL_CAPTUREPROGRESS, sampleptr);
				return 0;
			
			case STATE_FIN:
				// Capture finished!
				StatusMessage(panel, IFACEPANEL_STATUSBOX, "Capture finished");
			
				retrieval_begun = true;
				
				// Need to calculate GetDatasPerTick and TickRate
				GetCtrlVal(panel, IFACEPANEL_CAPTUREPROGRESS, &numsamples);
				if(numsamples < 100 * 64)
				{
					GetDatasPerTick = 1;
					SetCtrlAttribute(panel, IFACEPANEL_RETRIEVETIMER, ATTR_INTERVAL, 0.002); // Max tick rate, i.e. 500Hz
				}
				else
				{
					GetDatasPerTick = ceil((double)numsamples / 64.0 / 100.0);
					if(GetDatasPerTick > 50) GetDatasPerTick=50; // Cap to limit latency at ~50-100ms
					if(GetDatasPerTick<1) GetDatasPerTick = 1; // Ensure at least 1!
					SetCtrlAttribute(panel, IFACEPANEL_RETRIEVETIMER, ATTR_INTERVAL, (double)GetDatasPerTick / 500.0);
						// tickrate = 500Hz / GetDatasPerTick assuming about 2ms per GetData
				}
				
				return 0;
			
			default:
				// This really shouldn't happen - invalid state
				printf("Invalid state received from poll: 0x%x", state);
				return 0;
		}
	}
	
	// So now capture has finished and retrieval begun
	// Do multiple getdatas per tick:
	for(i=0; i<GetDatasPerTick; i++)
	{
		switch(getdata(datastore, &datastoreptr))
		{
			case GETDATA_SUCCESS:
				// We have nabbed some more data into datastore
				// Break out of the switch so we can get some more
				break;

			case GETDATA_EOF:
				// We have finished getting data
				StatusMessage(panel, IFACEPANEL_STATUSBOX, "Finished data retrieval");
				
				// For now just stop the timer.
				SetCtrlAttribute(panel, IFACEPANEL_RETRIEVETIMER, ATTR_ENABLED, 0);
				
				// Mark data length:
				datalength = datastoreptr-datastore;
				UpdateSliders(panel);
				
				return 0;
			
			default: // i.e. GETDATA_ERROR
				// Any other return is some sort of error...
				printf("Unexpected error from getdata\n");
				// TODO: Abort capture?
				// For now just stop the timer.
				SetCtrlAttribute(panel, IFACEPANEL_RETRIEVETIMER, ATTR_ENABLED, 0);
				return 0;
		
		}
	} // Only keeps looping while data was got successfully.
	
	// Only update the UI once per tick
	// Update the tank.  We make it count back down as we retrieve:
	GetCtrlAttribute(panel, IFACEPANEL_CAPTUREPROGRESS, ATTR_MAX_VALUE, &numsamples);
	SetCtrlVal(panel, IFACEPANEL_CAPTUREPROGRESS, numsamples-(datastoreptr-datastore));
	
	
	return 0;
}

int CVICALLBACK GENERATELISTINGBUTTON_hit (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	unsigned int i, j;
	char buf[8];
	int CHenable[8]; // Waste of space, but 32 bytes makes the code so much nicer and neater
	
	if(event != EVENT_COMMIT)
		return 0; // Not a click
	
	if(datalength>1000)
	{
		printf("I'm sorry Dave, I can't let you do that (datalen>1000)\n");
		return 0;
	}
	
	// Fill CHenable:
	// Note reverse order - Because left to right, CH7 is first.
	GetCtrlVal(panel, IFACEPANEL_CH7_CHECKBOX, CHenable);   GetCtrlVal(panel, IFACEPANEL_CH6_CHECKBOX, CHenable+1);
	GetCtrlVal(panel, IFACEPANEL_CH5_CHECKBOX, CHenable+2); GetCtrlVal(panel, IFACEPANEL_CH4_CHECKBOX, CHenable+3);
	GetCtrlVal(panel, IFACEPANEL_CH3_CHECKBOX, CHenable+4); GetCtrlVal(panel, IFACEPANEL_CH2_CHECKBOX, CHenable+5);
	GetCtrlVal(panel, IFACEPANEL_CH1_CHECKBOX, CHenable+6); GetCtrlVal(panel, IFACEPANEL_CH0_CHECKBOX, CHenable+7);
	
	// Do headers:
	// Reset textbox:
	ResetTextBox(DISPLAYTABPANEL2, LISTPANEL_DATALISTING, "");
	
	for(j=0; j<8; j++)
		if(CHenable[j])
		{
			sprintf(buf, "CH%d ", 7-j);
			SetCtrlVal(DISPLAYTABPANEL2, LISTPANEL_DATALISTING, buf);  // append buf - CH headers
		}
		
	SetCtrlVal(DISPLAYTABPANEL2, LISTPANEL_DATALISTING, "\n");  // append newline

	// Do data:
	for(i=0; i<datalength; i++)
	{
		for(j=0; j<8; j++)
			if(CHenable[j])
			{
				sprintf(buf, "%d   ", (datastore[i]>>(7-j)) & 1 ); // Again, lazyily aligning with space padding
									// This is a way of retrieving a single bit - shift, then filter out the LSbit with the &
				SetCtrlVal(DISPLAYTABPANEL2, LISTPANEL_DATALISTING, buf);  // append CH values, spaces to align lazily
			}
		
		SetCtrlVal(DISPLAYTABPANEL2, LISTPANEL_DATALISTING, "\n"); // finally newline
	}
	
	return 0;
}

int CVICALLBACK DUMMYDATABUTTON_hit (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	unsigned int i;
	
	if(event != EVENT_COMMIT)
		return 0; // Not a click
	
	GetCtrlVal(panel, IFACEPANEL_SAMPLENUMBER, &datalength);
	
	for(i=0; i<datalength; i++)
		datastore[i]=0x42;
	
	if(debug) printf("Data Dummied - length %d\n", datalength);
	
	UpdateSliders(panel);
	
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
			QuitUserInterface (0);

			break;
	}
	return 0;
}

int CVICALLBACK ALLCHBUTTON_hit (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	if(event != EVENT_COMMIT) return 0; // not a click
	
	SetCtrlVal(panel, IFACEPANEL_CH7_CHECKBOX, 1); SetCtrlVal(panel, IFACEPANEL_CH6_CHECKBOX, 1);
	SetCtrlVal(panel, IFACEPANEL_CH5_CHECKBOX, 1); SetCtrlVal(panel, IFACEPANEL_CH4_CHECKBOX, 1);
	SetCtrlVal(panel, IFACEPANEL_CH3_CHECKBOX, 1); SetCtrlVal(panel, IFACEPANEL_CH2_CHECKBOX, 1);
	SetCtrlVal(panel, IFACEPANEL_CH1_CHECKBOX, 1); SetCtrlVal(panel, IFACEPANEL_CH0_CHECKBOX, 1);
	
	return 0;
}

int CVICALLBACK NONECHBUTTON_hit (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	if(event != EVENT_COMMIT) return 0; // not a click
	
	SetCtrlVal(panel, IFACEPANEL_CH7_CHECKBOX, 0); SetCtrlVal(panel, IFACEPANEL_CH6_CHECKBOX, 0);
	SetCtrlVal(panel, IFACEPANEL_CH5_CHECKBOX, 0); SetCtrlVal(panel, IFACEPANEL_CH4_CHECKBOX, 0);
	SetCtrlVal(panel, IFACEPANEL_CH3_CHECKBOX, 0); SetCtrlVal(panel, IFACEPANEL_CH2_CHECKBOX, 0);
	SetCtrlVal(panel, IFACEPANEL_CH1_CHECKBOX, 0); SetCtrlVal(panel, IFACEPANEL_CH0_CHECKBOX, 0);
	
	return 0;
}

int CVICALLBACK RANGESLIDER_hit (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	if(event != EVENT_COMMIT)
		return 0; // not a click
	
	UpdateSliders(panel);
	
	return 0;
}
