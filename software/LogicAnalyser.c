#define LOGICANALYSER_C
// Used to correctly wangle extern variables


#include "radioGroup.h"
#include <ansi_c.h>
#include <cvirte.h>		
#include <userint.h>
#include <math.h>
#include <time.h>


static int TIMINGTAB; // Labwindows doesn't give us constants for tab panels
static int LISTINGTAB; // So do our own - these are set in main()
static int RS232TAB;

#include "comms.h"  
#include "interface.h"
#include <stdio.h>

// Couple of static state variables:
BOOL trigger_wait;
BOOL capture_begun;
BOOL retrieval_begun;
unsigned int GetDatasPerTick;


char *datastore=NULL;
char *datastoreptr=NULL;
unsigned int datalength=0;
unsigned int samplerate=1;

void StatusMessage(int panel, int statusbox, char *message);
void UpdateSliders(int panel);
void UpdateDisplay(int panel);
void UpdateConfig(int panel);

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
	unsigned int position, range, tab;
	
	GetActiveTabPage(panel, IFACEPANEL_DISPLAYTAB, &tab);
	
	// Set max range to MAX or datalength, whichever is smaller, unless datalength=0 in which case set max range to 1
	// (Doesn't like min = max = 0)
	// MAX depends on whether listing or timing diag.
	if(tab == 0) // Timing diag
	{
		if(datalength>100)
			SetCtrlAttribute(panel, IFACEPANEL_RANGESLIDER, ATTR_MAX_VALUE, 100);
		else
				SetCtrlAttribute(panel, IFACEPANEL_RANGESLIDER, ATTR_MAX_VALUE, datalength);
	}
	else
	{
		if(datalength>23)
			SetCtrlAttribute(panel, IFACEPANEL_RANGESLIDER, ATTR_MAX_VALUE, 23); // Master Max Range Option!
		else
				SetCtrlAttribute(panel, IFACEPANEL_RANGESLIDER, ATTR_MAX_VALUE, datalength);
	}
		
	GetCtrlVal(panel, IFACEPANEL_RANGESLIDER, &range);
	
	
	// Now setup the position max:
	SetCtrlAttribute(panel, IFACEPANEL_POSITIONSLIDER, ATTR_MAX_VALUE, datalength-range);
	
	UpdateDisplay(panel);
}

void UpdateDisplay(int panel)
{
	unsigned int i, j, range, position, samplenumcols;
	char buf[128]="";
	char buf2[128]="";
	int CHenable[8]; // Waste of space, but 32 bytes makes the code so much nicer and neater
	double value;
	
	unsigned short int numchannels=0;
	char *timingdata = NULL;
	char *chunkptr = NULL;
	
	
	// Fill CHenable:
	// Note reverse order - Because left to right, CH7 is first.
	GetCtrlVal(panel, IFACEPANEL_CH7_CHECKBOX, CHenable);   GetCtrlVal(panel, IFACEPANEL_CH6_CHECKBOX, CHenable+1);
	GetCtrlVal(panel, IFACEPANEL_CH5_CHECKBOX, CHenable+2); GetCtrlVal(panel, IFACEPANEL_CH4_CHECKBOX, CHenable+3);
	GetCtrlVal(panel, IFACEPANEL_CH3_CHECKBOX, CHenable+4); GetCtrlVal(panel, IFACEPANEL_CH2_CHECKBOX, CHenable+5);
	GetCtrlVal(panel, IFACEPANEL_CH1_CHECKBOX, CHenable+6); GetCtrlVal(panel, IFACEPANEL_CH0_CHECKBOX, CHenable+7);
	
	GetCtrlVal(panel, IFACEPANEL_POSITIONSLIDER, &position);
	GetCtrlVal(panel, IFACEPANEL_RANGESLIDER, &range);
	
	// Reset textbox(s):
	ResetTextBox(LISTINGTAB, LISTPANEL_DATALISTING, "");
	SetCtrlVal(LISTINGTAB, LISTPANEL_LISTINGHEADING, "");  // Note headers are in a string not textbox
	
	// Clear timing diag labels:
	SetCtrlAttribute(TIMINGTAB, TIMPANEL_TIMINGDIAGRAM, ATTR_YLABEL_VISIBLE, 0);
	
	if(datalength == 0)
		return;
	
	
	/***************** TIMING DIAGRAM *****************************/
	
	// We need to chunk up the appropriate chunk of datastore into a timing diagram friendly format
	// Which is: [CH0 Sample 0, CH1 Sample 0, CH0 Sample 1, CH1 Sample 1, ....] i.e. interleaved
	// First we work out how long we need to allocate the data chunk is, and allocate it:
	
	for(i=0; i<8; i++)
		if(CHenable[i])
			numchannels++;
		
	if(numchannels == 0)
	{
		// Empty the timing diagram:
		ClearDigitalGraph(TIMINGTAB, TIMPANEL_TIMINGDIAGRAM);
		SetCtrlAttribute(TIMINGTAB, TIMPANEL_TIMINGDIAGRAM, ATTR_YLABEL_VISIBLE, 0);
		return; // No timing diagram, listing already cleared, so we can just quit here.
	}
	
	// We have at least one channel and nonzero data:
	SetCtrlAttribute(TIMINGTAB, TIMPANEL_TIMINGDIAGRAM, ATTR_YLABEL_VISIBLE, 1);
	
	timingdata = malloc(sizeof(short int)*range*numchannels);
	if(timingdata == NULL) return; // Out of mem!?
	
	chunkptr = timingdata;
		
	for(i=0; i<range; i++)
		for(j=0; j<8; j++)
			if(CHenable[j])
			{
				*chunkptr = (datastore[i+position]>>(7-j)) & 1;
				chunkptr++;
			}
	
	PlotDigitalLines(TIMINGTAB, TIMPANEL_TIMINGDIAGRAM, timingdata, range*numchannels, VAL_CHAR, numchannels);
	
	// Correct X axis:
	SetCtrlAttribute(TIMINGTAB, TIMPANEL_TIMINGDIAGRAM, ATTR_XAXIS_OFFSET, (double)position);
	
	// Replace axis lables with CH?
	i=0;
	for(j=0; j<8; j++)
	{
		if(CHenable[7-j]) // Remember, CHenable is backwards, hence 7-j
		{
			GetAxisItem(TIMINGTAB, TIMPANEL_TIMINGDIAGRAM, VAL_LEFT_YAXIS, i, buf2, &value); // temp store auto gen value
			// Temp store label in buf2
			
			sprintf(buf2, "CH%d", j); // Generate new label
			ReplaceAxisItem(TIMINGTAB, TIMPANEL_TIMINGDIAGRAM, VAL_LEFT_YAXIS, i, buf2, value); // replace label
			i++; // count channels done
		}
	}

	
	/******************** LISTING **********************************/
	
	// In general buf2 used for sprintfs and buf used for collating
	
	// Need to work out how many digits the sample count is going to be at max
	// i.e. ceil(log10(datalen-1)) (datalen-1 is max b/c of 0 based counting)
	if(datalength>1)
		samplenumcols=ceil(log10(datalength-1));
	else
		samplenumcols=1; // Skillfully avoiding log(0).  Or log(-1) for that matter.
	
	/****** HEADERS *******/
	
	buf[0]='\0'; // Empty buf
	
	// Empty space for samplenum alignment:
	for(j=0; j<samplenumcols+3; j++) // 3 extra spaces for [] and spacer space
		strncat(buf, " ", 1);
	
	// Actual headers
	for(j=0; j<8; j++)
		if(CHenable[j])
		{
			sprintf(buf2, "CH%d ", 7-j);
			strncat(buf, buf2, 5);
			
		}
		
	SetCtrlVal(LISTINGTAB, LISTPANEL_LISTINGHEADING, buf);  // Note headers are in a string not textbox
	// So set actually sets instead of appending
		
	// Do data samples:
	for(i=0; i<range; i++)
	{
		// Print samplenum. printf can't pad to a variable const width so we have to do it manually.
		strncpy(buf, "[", 2);
		
		if(position+i == 0) // special case
			for(j=0; j<samplenumcols-1; j++) // pad with appropriate num of 0s for const width
				strncat(buf, "0", 1);
		else			
			for(j=0; j<samplenumcols-ceil(log10(position+i+1)); j++) // pad with appropriate num of 0s for const width
				strncat(buf, "0", 1);
		
		sprintf(buf2, "%d", i+position);
		strncat(buf, buf2, samplenumcols+1); // +1 for extra space
		strncat(buf, "] ", 2);
		SetCtrlVal(LISTINGTAB, LISTPANEL_DATALISTING, buf);  // start line with samplenumber (+space)
		
		for(j=0; j<8; j++)
			if(CHenable[j])
			{
				sprintf(buf2, " %d  ", (datastore[i+position]>>(7-j)) & 1 ); // Again, lazyily aligning with space padding
									// This is a way of retrieving a single bit - shift, then filter out the LSbit with the &
				
				SetCtrlVal(LISTINGTAB, LISTPANEL_DATALISTING, buf2);  // append CH values, spaces to align lazily
			}
		
		SetCtrlVal(LISTINGTAB, LISTPANEL_DATALISTING, "\n"); // finally newline
	}
	
	
	
}

void UpdateConfig(int panel)
{
	int capturemode, asynctrigger;
	
	// Disable ASYNCTRIGGER unless CAPTUREMODE=async
	// Disable CLOCKEDGE unless ASYNCTRIGGER or CAPTUREMODE=sync
	// Disable SAMPLERATE unless CAPTUREMODE=async
	
	// Retrieve CAPTUREMODE:
	Radio_GetMarkedOption(panel, IFACEPANEL_CAPTUREMODE, &capturemode);
	
	// Do ASYNCTRIGGER:
	if(capturemode == 0) // async
		SetCtrlAttribute(panel, IFACEPANEL_ASYNCTRIGGER, ATTR_DIMMED, 0);
	else
	{
		SetCtrlVal(panel, IFACEPANEL_ASYNCTRIGGER, 0);
		SetCtrlAttribute(panel, IFACEPANEL_ASYNCTRIGGER, ATTR_DIMMED, 1);
	}
	
	GetCtrlVal(panel, IFACEPANEL_ASYNCTRIGGER, &asynctrigger);
	if( (capturemode == 1) /* sync */ || asynctrigger)
		SetCtrlAttribute(panel, IFACEPANEL_CLOCKEDGE, ATTR_DIMMED, 0);
	else
		SetCtrlAttribute(panel, IFACEPANEL_CLOCKEDGE, ATTR_DIMMED, 1);
	
	if(capturemode == 0) // async
		SetCtrlAttribute(panel, IFACEPANEL_SAMPLERATE, ATTR_DIMMED, 0);
	else
		SetCtrlAttribute(panel, IFACEPANEL_SAMPLERATE, ATTR_DIMMED, 1);
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
	
	
	
	GetPanelHandleFromTabPage (panelHandle, IFACEPANEL_DISPLAYTAB, 0, &TIMINGTAB);
	GetPanelHandleFromTabPage (panelHandle, IFACEPANEL_DISPLAYTAB, 1, &LISTINGTAB);
	GetPanelHandleFromTabPage (panelHandle, IFACEPANEL_DISPLAYTAB, 2, &RS232TAB);
	
	SetCtrlAttribute(TIMINGTAB, TIMPANEL_TIMINGDIAGRAM, ATTR_DIGWAVEFORM_SHOW_STATE_LABEL, 0); // disable state labels. 
	
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
	UpdateConfig(panelHandle);
	
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
	
	free(datastore);
	
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
	int capturemode, edge, result, asynctrigger;

	if(event != EVENT_COMMIT)
		return 0; // not a click! mouseover etc.
	
	Radio_GetMarkedOption(panel, IFACEPANEL_CAPTUREMODE, &capturemode); // Retrieve async/sync
	
	if(capturemode==0) // async
	{
		async=1;
		sync=0;
		GetCtrlVal(panel, IFACEPANEL_SAMPLERATE, &rate); // retrieve rate
		
		// Check for async trigger - used later
		GetCtrlVal(panel, IFACEPANEL_ASYNCTRIGGER, &asynctrigger);
	}
	else // sync
	{
		sync=1;
		async=rate=0;
	}
	
	if( (capturemode == 1) || asynctrigger) // sync or (async with trigger)
	{
		Radio_GetMarkedOption (panel, IFACEPANEL_CLOCKEDGE, &edge);
		
		switch(edge)
		{
			case 0: // rising
				rising = true;
				falling = both = false;
				break;
				
			case 1: // falling
				falling = true;
				rising = both = false;
				break;
				
			case 2: // "change" == both
				both = true;
				rising = falling = false;
				break;
		}
	}
	else // async without trigger
		rising=falling=both=0;
	
	
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
	
	// Configured ok!
	
	StatusMessage(panel, IFACEPANEL_STATUSBOX, "Successfully configured analyser");
	
	samplerate = rate; // Set the global samplerate, used in RS232decode
	
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

	
	trigger_wait = false; // state used later - set once samples have been taken
	capture_begun = false;  // more state
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
	int CaptureInProgress;
	
	if(event != EVENT_COMMIT) return 0; // not a click
	
	// Abort capture if in progress:
	GetCtrlAttribute(panel, IFACEPANEL_RETRIEVETIMER, ATTR_ENABLED, &CaptureInProgress);
	if(CaptureInProgress)
	{
		StatusMessage(panel, IFACEPANEL_STATUSBOX, "Reconnecting: aborting capture/retrive");
		datalength=0;
		retrieval_begun=0;
		capture_begun=0;
		SetCtrlAttribute(panel, IFACEPANEL_RETRIEVETIMER, ATTR_ENABLED, 0);
		return 0;
	}
	
	
	
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
	unsigned int numsamples, max, sampleptr, state, i, AnalyserConnected;

	if(event != EVENT_TIMER_TICK)
		return 0; // not a tick!
	
	
	// If we aren't connected, we need to abort the capture/retrieve:
	GetCtrlVal(panel, IFACEPANEL_CONNECTEDLED, &AnalyserConnected);
	if(!AnalyserConnected)
	{
		StatusMessage(panel, IFACEPANEL_STATUSBOX, "Connection lost: aborting capture/retrive");
		datalength=0;
		retrieval_begun=0;
		capture_begun=0;
		SetCtrlAttribute(panel, IFACEPANEL_RETRIEVETIMER, ATTR_ENABLED, 0);
		return 0;
	}
	
	
	
	
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
				if(!trigger_wait)
				{
					StatusMessage(panel, IFACEPANEL_STATUSBOX, "Waiting for trigger/clock");
					trigger_wait=true;
				}
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


int CVICALLBACK DUMMYDATABUTTON_hit (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	unsigned int i;
	
	if(event != EVENT_COMMIT)
		return 0; // Not a click
	
	GetCtrlVal(panel, IFACEPANEL_SAMPLENUMBER, &datalength);
	
	for(i=0; i<datalength; i++)
		datastore[i]=i;
	
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
	
	UpdateDisplay(panel);
	
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
	
	UpdateDisplay(panel);
	
	return 0;
}

int CVICALLBACK RANGESLIDER_hit (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	if((event != EVENT_COMMIT) && (event != EVENT_VAL_CHANGED))
		return 0; // not a click
	
	UpdateSliders(panel);
	
	return 0;
}

int CVICALLBACK POSITIONSLIDER_hit (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	if((event != EVENT_COMMIT) && (event != EVENT_VAL_CHANGED))
		return 0;
	
	UpdateDisplay(panel);
	
	return 0;
}

int CVICALLBACK CH_CHECKBOX_hit (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	if(event == EVENT_COMMIT)
		UpdateDisplay(panel);
	return 0;
}

int CVICALLBACK CAPTUREMODE_hit (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	if(event != EVENT_COMMIT) return 0;
	
	UpdateConfig(panel);
	
	return 0;
}

int CVICALLBACK ASYNCTIRGGER_hit (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	if(event != EVENT_COMMIT) return 0;
	
	UpdateConfig(panel);
	
	return 0;
}

int CVICALLBACK DECODEBUTTON_hit (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	const int framelength=8; // exclusing start/stop bits. Usually 8 = 1 byte
	
	const int channel=0; // Default CH0
	
	unsigned int i=0, j, bitrate; 
	// We only care about ratio of samplerate to bitrate, in this case 5:1
	
	unsigned int ptr=0; // For nav inside the datastore
	unsigned int readframe; // output frame
	
	
	if(event != EVENT_COMMIT) return 0;
	
	
	GetCtrlVal(RS232TAB, RS232TAB_BITRATE, &bitrate);
	
	/******* RS232 FRAMING DECODE PROCESS **************
	In order to synchronise reliably, we need to find what is definitely a
	start bit, and must know for sure that it is not part of a data frame.
	Only sure way to do this is wait for a mark period of at least N bit
	periods, where N=framelength.  After this any SPACE bit is definitely
	a start bit.
	
	Having found a start bit, we need to synchronise in order to decide
	what samples to read to check the END MARK bit and for each data bit.
	A simple (and in theory reliable) way of doing this is to assume the
	falling edge happened precisely half way between the last MARK sample
	and first SPACE sample.  We can then use the bit rate to estimate
	the position of the centre of each data bit and the END bit.  We then
	pick the nearest sample to each of these centres.  We read the END bit -
	if it is not a SPACE, we have a framing error, and need to return to
	finding a period of at least N MARK bits.
	
	We can now read the N data bits, decode ASCII, display as hex,
	check parity, or whatever.
	
	Flow:
	10 Scan for N MARKs followed by SPACE
	20 Check for END MARK - if not GOTO 10
	30 Read Byte
	40 Scan for next SPACE start bit
	50 GOTO 20
	*****************************************************/
	
	if(debug) printf("Sample rate: %d, bit rate: %d, samples/bit: %f\n", samplerate, bitrate, (double)samplerate/bitrate);
	
	// Synchronising loop
	while( ptr < datalength - ceil( (double)samplerate/bitrate*(framelength-1) ) )
	{
		if(datastore[ptr] == MARK)
		{
			i++; // MARK count
			ptr++;
			continue;
		}
		else // space
			if(i <= framelength) // Cannot sync -- reset
			{
				i=0; // Reset MARK count
				ptr++;
				continue;
			}
				
		// We now consider ptr to be the first sample of a START bit
		
		if(debug) printf("rs232: START found @ sample %d\n", ptr);
			
		// We will assume the falling edge of the START bit was 0.5 samples before ptr
		// The middle of the start bit is at sample
		//		 ptr - 0.5 + (samplerate/bitrate)/2
		
		// Check for the end bit - the middle of the end bit should be at sample:
		//		 ptr - 0.5 + (samplerate/bitrate)*(N+1.5)
		// We need to round this to get a whole sample num - C has no round() but floor(x+0.5) is equivilent
		
		if(debug) printf("Estimated END bit @ sample %d\n", (int)((double)ptr + floor( (double)samplerate/bitrate * ((double)framelength + 1.5) )));
		printf("> framelength = %d\n", framelength);
		printf("Above before round: sample %f\n", ptr + (double)samplerate/bitrate * ((double)framelength + 1.5) - 0.5);

		if(datastore[(int)((double)ptr + floor( (double)samplerate/bitrate * ((double)framelength + 1.5) ))] != MARK)
		{
			// Incorrect END bit - framing error!
			if(debug) printf("Framing error!\n");
			// Return to original idle-scan:
			i=0;
			ptr++;
			//continue;
		}
		
		// Now we have a frame with correct START and END bits - read off the data!
		readframe=0;
		// * sample = ptr - 0.5 + (samplerate/bitrate)*(1.5+j)
		// * Again, round with ceil(x+0.5)
		// * Pick out channel using (datastore[sample] >> channel) & 1
		// * Shift into correct bit position in frame using << j (Remember LSbit first)
		// * Add to readframe using |=
		
		for(j=0; j<framelength; j++)
			readframe |= ((datastore[(int)(floor((double)ptr + ((double)samplerate/bitrate)*(1.5+j)))] >> channel) & 1) << j;
		
		// Now readframe should contain the byte!
		printf("byte decoded: 0x%x\n", readframe);
		
		// Finally, align ptr with somewhere inside the END MARK bit
		// Aiming for the middle: ptr - 0.5 + (samplerate/bitrate)*(N+1.5)
		
		ptr += floor( (samplerate/bitrate)*(framelength+1.5) );
		// i should already be > framelength, so we can just reloop
		// and go straight to looking for the first falling edge of a START bit.
		
	}
	
	return 0;
}

int CVICALLBACK DISPLAYTAB_hit (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int range, max;
	
	if(event != EVENT_ACTIVE_TAB_CHANGE) return 0;
	
	UpdateSliders(panel);
	
	// put value within range if necessary
	GetCtrlVal(panel, IFACEPANEL_RANGESLIDER, &range);
	GetCtrlAttribute(panel, IFACEPANEL_RANGESLIDER, ATTR_MAX_VALUE, &max);
	
	printf("%d\n", range);
	
	if(range>max)
		SetCtrlVal(panel, IFACEPANEL_RANGESLIDER, max);
	
	
	return 0;
}
