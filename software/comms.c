//#include <ansi_c.h>
#include <cvirte.h>		
#include <userint.h>
 
#include <stdio.h>
#include <windows.h> 
#include "mpusbapi.h"


#include "comms.h"


#define READ_VERSION    0
#define MSG    8

//------USB Example--------//

void USBDLL(void);
DWORD SendReceivePacket(BYTE *SendData, DWORD SendLength, BYTE *ReceiveData,
                    DWORD *ReceiveLength, UINT SendDelay, UINT ReceiveDelay);

 
DWORD (*MPUSBGetDLLVersion)(void);
DWORD (*MPUSBGetDeviceCount)(PCHAR pVID_PID);
HANDLE (*MPUSBOpen)(DWORD instance,         // Input
                 PCHAR pVID_PID,            // Input
                 PCHAR pEP,                 // Input
                 DWORD dwDir,               // Input
                 DWORD dwReserved);         // Input <Future Use>

DWORD (*MPUSBRead)(HANDLE handle,           // Input
                PVOID pData,                // Output
                DWORD dwLen,                // Input
                PDWORD pLength,             // Output
                DWORD dwMilliseconds);      // Input

DWORD (*MPUSBWrite)(HANDLE handle,          // Input
                 PVOID pData,               // Input
                 DWORD dwLen,               // Input
                 PDWORD pLength,            // Output
                 DWORD dwMilliseconds);     // Input

DWORD (*MPUSBReadInt)(HANDLE handle,        // Input
                   PVOID pData,             // Output
                   DWORD dwLen,             // Input
                   PDWORD pLength,          // Output
                   DWORD dwMilliseconds);   // Input

BOOL (*MPUSBClose)(HANDLE handle);  


HANDLE myOutPipe;
HANDLE myInPipe;
BOOL fFreeResult, fRunTimeLinkSuccess = FALSE; 
HINSTANCE hinstLib; 
//---------------------------------

char vid_pid[]= "vid_04d8&pid_000c"; 
char out_pipe[]= "\\MCHP_EP1";


BYTE send_buf[64],receive_buf[64];



int init_usb()
{
   WORD Version=0; 
   DWORD HackReceiveLength=4;
   
   send_buf[0] = 0x00;
   receive_buf[0] = 0x00;
    
 
    // Get a handle to the DLL module.
 
     myOutPipe = myInPipe = INVALID_HANDLE_VALUE; 
     
     hinstLib = LoadLibrary("mpusbapi.dll"); 


	  if (hinstLib != NULL) 
     { 
        //MPUSBGetDLLVersion = (DWORD)GetProcAddress(hinstLib, "DWORD _MPUSBGetDLLVersion"); 
 		 MPUSBGetDLLVersion=(DWORD(*)(void))GetProcAddress(hinstLib,"_MPUSBGetDLLVersion");
		 
         MPUSBGetDeviceCount=(DWORD(*)(PCHAR))GetProcAddress(hinstLib,"_MPUSBGetDeviceCount");
                    
         MPUSBOpen=(HANDLE(*)(DWORD,PCHAR,PCHAR,DWORD,DWORD))GetProcAddress(hinstLib,"_MPUSBOpen");
                    
        MPUSBWrite=(DWORD(*)(HANDLE,PVOID,DWORD,PDWORD,DWORD))GetProcAddress(hinstLib,"_MPUSBWrite");
                    
        MPUSBRead=(DWORD(*)(HANDLE,PVOID,DWORD,PDWORD,DWORD))GetProcAddress(hinstLib,"_MPUSBRead");
                    
        MPUSBReadInt=(DWORD(*)(HANDLE,PVOID,DWORD,PDWORD,DWORD))GetProcAddress(hinstLib,"_MPUSBReadInt");
                    
        MPUSBClose=(BOOL(*)(HANDLE))GetProcAddress(hinstLib,"_MPUSBClose");
                
         
        // If the function address is valid, call the function.
 
        
        Version=MPUSBGetDeviceCount(vid_pid) ;  
            
           
        myOutPipe = MPUSBOpen(0,vid_pid,out_pipe,MP_WRITE,0);
        myInPipe = MPUSBOpen(0,vid_pid,out_pipe,MP_READ,0);
            
       if(myOutPipe == INVALID_HANDLE_VALUE )
	   {


		   if(debug) printf("Invalid output handle value from init_usb\n");
		   // These outputs are considered debug, as they will be printed if
		   // the analyser is not connected, which is a rather normal case.

		   
          return USB_ERROR;
	   }
	   
       if(myInPipe == INVALID_HANDLE_VALUE)
	   {
		   if(debug) printf("Invalid input handle value from init_usb\n");

          return USB_ERROR;
	   }
   
       //UCAM HACK NOT SURE WHY
   	   send_buf[0]=0xEE;
       SendReceivePacket(send_buf, 1, receive_buf,&HackReceiveLength,1000,1000);          
          
      return SUCCESS; 
     }
            
	  printf("Failed to load msusbapi.dll...\n"); // This shouldn't happen in any sensible case, and so
	  // is printed even if debug is disabled.
     return USB_ERROR;  

}

int close_usb()
{
	  if(myOutPipe != INVALID_HANDLE_VALUE)
	  {
	  	MPUSBClose(myOutPipe);
	  	myOutPipe = INVALID_HANDLE_VALUE;
	  }
	  
	  if(myInPipe != INVALID_HANDLE_VALUE)
	  {
		MPUSBClose(myInPipe);
	  	myInPipe = INVALID_HANDLE_VALUE;
	  }
	  
      if(FreeLibrary(hinstLib) != 0)
		  return SUCCESS;
	  else
		  return USB_ERROR;
}

DWORD SendReceivePacket(BYTE *SendData, DWORD SendLength, BYTE *ReceiveData,
                    DWORD *ReceiveLength, UINT SendDelay, UINT ReceiveDelay)
{
    DWORD SentDataLength;
    DWORD ExpectedReceiveLength = *ReceiveLength;

    if(myOutPipe != INVALID_HANDLE_VALUE && myInPipe != INVALID_HANDLE_VALUE)
    {
		if(debug && SendData[0] != 0xdd) printf("About to send command 0x%x\n", SendData[0]);
		// Squelch ping/pongs
		
        if(MPUSBWrite(myOutPipe,SendData,SendLength,&SentDataLength,SendDelay))
        {
            if(MPUSBRead(myInPipe,ReceiveData, ExpectedReceiveLength,
                        ReceiveLength,ReceiveDelay))
            {
				// Read went ok!
				
				if(debug && ReceiveData[0] != 0xdd) printf("Received command 0x%x\n", ReceiveData[0]);
				// Squelch ping/pongs
				
				// We have some commands which can validly return unexpected lengths:
				// "The UCAM hack" - who knows what's going on, but it doesn't really matter
				// Poll response - length is either 0x03 or 0x07 depending on state - length is checked in poll_state
				// The data request/response thingy.
				// Error responses
				
				// For all other commands length should be as expected - so do length check here.
				
				if( (ReceiveData[0] != 0xEE) && (ReceiveData[0] != CMD_ERROR_RS) && (ReceiveData[0] != CMD_POLL_RS) &&
					(ReceiveData[0] != CMD_GETDATA_RS) && (*ReceiveLength != ExpectedReceiveLength) )
				{
					// We have an unexpected receive length on a packet for which this is *NOT* ok
					if(debug)
					{
						printf("SendReceivePacket failed: Incorrect receive length, cmd type %x\n", ReceiveData[0]);
						printf("Expected length %d, actual length %d\n", ExpectedReceiveLength, *ReceiveLength);
					}

                    return USB_ERROR;
				}
				
                return SUCCESS;
                
				
            }
			else
			{

				if(debug) printf("SendReceivePacket failed - error from MPUSBRead\n");

				return USB_ERROR;
			}
        }
		else
		{

			if(debug) printf("SendReceivePacket failed - error from MPUSBWrite\n");

			return USB_ERROR;
		}
        
          
    }


	if(debug) printf("SendReceivePacket failed - Input or Output pipe handle invalid\n");

    return USB_ERROR;  // Operation Failed
}

//-------------------------------

int read_debug_byte (int *value)
{
	DWORD RecvLength = LEN_DEBUG_RS; //set expected receive length;
	
     send_buf[0] = CMD_DEBUG_RQ;      // Command
   
	
    if (SendReceivePacket(send_buf,LEN_DEBUG_RQ,receive_buf,&RecvLength,1000,1000) == SUCCESS)
    {
        if ((RecvLength == LEN_DEBUG_RS) && (receive_buf[0] == CMD_DEBUG_RS))
        {
            *value = receive_buf[2] & 0xFF; // & ensures we only get one byte
			return SUCCESS;
        }
		else
		{

			if(debug) printf("Debug byte received is incorrect length or incorrect command type\n");

			return USB_ERROR;
		}
		
    }
    else
    {

        if(debug) printf("Failed to retrieve debug byte - error from SendReceivePacket\n");

		return USB_ERROR;
	}
}

int send_config_message(bool async, bool sync, bool rising, bool falling, bool both,
	unsigned long rate, unsigned long samplenumber)
{
	DWORD RecvLength = LEN_CONFIG_RS; // Expected recv len
	
	unsigned long *lptr;
	char *cptr;
	
	
	if( (!async && !sync) || (async && sync) )
	{
		printf("Error in send_config_message - one and only one of async or sync must be high!\n");
		return USB_ERROR;
	}
	
	
	if(sync)
	{
		if( (!rising && !falling && !both) || (rising && falling) || (falling && both) || (rising && both) )
		{
			printf("Error in send_config_message - one and only one of rising or falling or both must be high if synch!\n");
			return USB_ERROR;
		}
	}
	

	if(debug)
	{
		printf("config: async %d sync %d rise %d fall %d both %d\n", async, sync, rising, falling, both);
		printf("config: rate %d number %d\n", rate, samplenumber);
	}

	
	send_buf[0] = CMD_CONFIG_RQ; // config command code
	send_buf[1] = LEN_CONFIG_RQ; // fixed length for config
	send_buf[2] = (async & 1) | ((sync & 1) << 1) | ((rising & 1) << 2) | ((falling & 1) << 3) | ((both & 1) << 4) | (1 << 7);
	
	*( (unsigned int *)( send_buf+3 ) ) = rate; // Have to be careful
	// VC's runtime stuff sometimes detects when you do "creative" things
	// with pointers and complains.
	
	*( (unsigned int *)( send_buf+7 ) ) = samplenumber;

	
	
    if (SendReceivePacket(send_buf, LEN_CONFIG_RQ, receive_buf,&RecvLength,1000,1000) == SUCCESS)
    {
        if( (RecvLength != LEN_CONFIG_RS) || ( (receive_buf[0] != CMD_CONFIG_RS) && (receive_buf[0] != CMD_ERROR_RS) ) ) 
        {
			if(debug) printf("Config response incorrect length or incorrect command code!!\n");

			return USB_ERROR;
       	}
		
		
		
		if( ( (receive_buf[0]==CMD_CONFIG_RS) && (receive_buf[2] == CONFIG_FAIL) ) || (receive_buf[0]==CMD_ERROR_RS) )
		{
			if(debug) printf("Config response says we failed - command returned is 0x%x, payload 0x%x\n",
						  receive_buf[0], receive_buf[2]);

			return CONFIG_ERROR;
		}
		
		// Correct length, claims to have succeeded, USB happy
		return SUCCESS;
    }
    else
    {
        if(debug) printf("USB Operation Failed\r\n");

		return USB_ERROR;
	}
}

int send_arm_request(void)
{
	DWORD RecvLength = LEN_ARM_RS; // Expected recv len == 0x03
	
	if(debug) printf("Attempting to arm...\n");
	
	send_buf[0] = CMD_ARM_RQ;
	send_buf[1] = LEN_ARM_RQ;
	
    if (SendReceivePacket(send_buf, LEN_ARM_RQ, receive_buf,&RecvLength,1000,1000) == SUCCESS)
    {
        if( (RecvLength != LEN_ARM_RS) || (receive_buf[0] != CMD_ARM_RS) ) 
        {
			if(debug) printf("Arm response incorrect length or incorrect command code!\n");

			return USB_ERROR;
       	}
		
		if( receive_buf[2] != ARM_SUCCESS )
		{
			if(debug) printf("Analyser refused to arm....\n");

			return ARM_ERROR;
		}
		
		// Looks like it's armed ok!
		return SUCCESS;
    }
    else
    {
        if(debug) printf("USB Operation Failed\r\n");

		return USB_ERROR;
	}
	
	
}

int poll_state(unsigned int *sampleptr, unsigned int *state)
{
	DWORD RecvLength = LEN_POLL_RS; // NB variable!
	send_buf[0] = CMD_POLL_RQ;
	send_buf[1] = LEN_POLL_RQ;
	
	if (SendReceivePacket(send_buf, LEN_POLL_RQ, receive_buf,&RecvLength,1000,1000) == SUCCESS)
	{
		// Error/sanity checks:
		if(receive_buf[0] != CMD_POLL_RS)
		{
			if(debug) printf("Invalid command code received from poll request\n");
			return USB_ERROR;
		}
		
		if( (receive_buf[2] == STATE_PROG) && (RecvLength != 0x07) )
		{
			if(debug) printf("In_progress poll response packet is invalid length\n");
			return USB_ERROR;
		}
		
		if( (receive_buf[2] != STATE_PROG) && (RecvLength != 0x03) )
		{
			if(debug) printf("Poll response packet (Not in_progress state) is invalid length\n");
			return USB_ERROR;
		}
		
		// Ok, so everything is valid/happy at least.
		
		
		// If in_progress, set sampleptr:
		// Yoink 4 bytes starting at send_buf[3] and stick them in *sampleptr.
		// Assumes both PIC and PC are running with LSB in lowest address.
		if(receive_buf[2] == STATE_PROG)
			*sampleptr = *( (unsigned int *)( receive_buf+3 ) );

		// Otherwise, we really don't care about the state of *sampleptr
			
		// Return state:
		*state = receive_buf[2];
			
		// All good
		return SUCCESS;
		
		
	}
	else
	{
		if(debug) printf("USB error when trying to poll\n");
		return USB_ERROR;
	}
	
	
	return 0;
}

int getdata(char *datastore, char **datastoreptr)
{
	unsigned int i;
	
	DWORD RecvLength = LEN_GETDATA_RS;
	send_buf[0] = CMD_GETDATA_RQ;
	send_buf[1] = LEN_GETDATA_RQ;
	
	if (SendReceivePacket(send_buf, LEN_GETDATA_RQ, receive_buf,&RecvLength,1000,1000) != SUCCESS)
	{
		if(debug) printf("USB error in getdata\n");
		return USB_ERROR;
	}
	
	if( (receive_buf[0] != CMD_GETDATA_RS) && (receive_buf[0] != CMD_ERROR_RS) )
	{
		if(debug) printf("Unexpected command returned from getdata request: 0x%x\n", receive_buf[0]);
		return USB_ERROR;
	}
	
	// Check for EOF or other errors:
	if(receive_buf[0] == CMD_ERROR_RS)
	{
		// Check payload:
		if(receive_buf[2] == ERROR_PAYLOAD_DATA_UNAVAILABLE)
			return GETDATA_EOF; // Finished receiving data!
		
		// else:
		if(debug) printf("Getdata returned error packet from analyser, payload 0x%x\n", receive_buf[2]);
			return GETDATA_ERROR; // Not an EOF - is a real error
		
	}
	
	
	// Check we won't overflow the datastore:
	// (Sub 2 from recvlength to remove cmd&len bytes)
	if( *datastoreptr + (RecvLength-2) > datastore + DATASTORE_SIZE)
	{
		printf("Analyser tried to overflow our datastore!\n");
		return USB_ERROR;
	}
	
	// Right, copy data:
	memcpy(*datastoreptr, receive_buf+2, RecvLength - 2);
	
	// Invert data
	for(i=0; i<RecvLength-2; i++)
		(*datastoreptr)[i] = ~((*datastoreptr)[i]);
	
	*datastoreptr += RecvLength - 2;
	
	return GETDATA_SUCCESS;
	
}

int do_ping()
{
	DWORD RecvLength = LEN_PING_RS;
	send_buf[0] = CMD_PING_RQ;
	send_buf[1] = LEN_PING_RQ;
	
	if (SendReceivePacket(send_buf, LEN_PING_RQ, receive_buf,&RecvLength,1000,1000) == SUCCESS)
	{
		if(receive_buf[0] == CMD_PING_RS)
			return SUCCESS;
		else
		{
			close_usb();
			return USB_ERROR;
		}
	}
	else
	{
		close_usb();
		return USB_ERROR;
	}
}

/*
int set_led (int value)
{
    send_buf[0] = 0xEE;      // Command
    send_buf[1] = value;     //the value
    
    RecvLength = 2; //set expected receive length 
    if (SendReceivePacket(send_buf, 2, receive_buf,&RecvLength,1000,1000) == 1)
    {
        if ((RecvLength == 2) && (receive_buf[0] == 0xEE))
        {
        }					 // !?
		return SUCCESS;
    }
    else
    {    
        printf("USB Operation Failed\r\n");
		return USB_ERROR;
	}
 
}			 */
