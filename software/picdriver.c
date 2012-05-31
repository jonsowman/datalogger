//#include <ansi_c.h>
#include <cvirte.h>		
#include <userint.h>
 
#include <stdio.h>
#include <windows.h> 
#include "mpusbapi.h"


#include "picdriver.h"


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

char SendData[]="why?";
DWORD SendLength=4;
DWORD SentDataLength=0;
DWORD SendDelay=1000;
char ReceiveData[4];
DWORD ExpectedReceiveLength=4;
DWORD ReceiveLength=0;
DWORD ReceiveDelay=5000;
BYTE send_buf[64],receive_buf[64];
DWORD RecvLength=4;



//--------------driver implementation from example code------------

int init_usb()
{
   WORD Version=0; 
    
 
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
#ifdef DEBUG
    	   printf("Invalid output handle value from init_usb\n");
		   // These outputs are considered debug, as they will be printed if
		   // the analyser is not connected, which is a rather normal case.
#endif
          return USB_ERROR;
	   }
	   
       if(myInPipe == INVALID_HANDLE_VALUE)
	   {
#ifdef DEBUG
		   printf("Invalid input handle value from init_usb\n");
#endif
          return USB_ERROR;
	   }
   
       //UCAM HACK NOT SURE WHY
   	   send_buf[0]=0xEE;
       SendReceivePacket(send_buf, 1, receive_buf,&RecvLength,1000,1000);          
          
      return USB_NO_ERROR; 
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
		  return USB_NO_ERROR;
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
        if(MPUSBWrite(myOutPipe,SendData,SendLength,&SentDataLength,SendDelay))
        {

            if(MPUSBRead(myInPipe,ReceiveData, ExpectedReceiveLength,
                        ReceiveLength,ReceiveDelay))
            {
                if((*ReceiveLength == ExpectedReceiveLength) || (ReceiveData[0] == 0xee))
					// 0xEE is some magic command in the usb init - seems to return 
					// incorrect length when logic analyser is reset without being
					// unplugged.  Return len shouldn't matter anywho.
                    return USB_NO_ERROR;   // Success!
                
				else// if(*ReceiveLength < ExpectedReceiveLength)
                {
#ifdef DEBUG
					printf("SendReceivePacket failed: Incorrect receive length, cmd type %x\n", ReceiveData[0]);
					printf("Expected length %d, actual length %d\n", ExpectedReceiveLength, *ReceiveLength);
#endif
                    return USB_ERROR;   // Partially failed, incorrect receive length
                }
            }
			else
			{
#ifdef DEBUG
				printf("SendReceivePacket failed - error from MPUSBRead\n");
#endif
				return USB_ERROR;
			}
        }
		else
		{
#ifdef DEBUG
			printf("SendReceivePacket failed - error from MPUSBWrite\n");
#endif
			return USB_ERROR;
		}
        
          
    }

#ifdef DEBUG
	printf("SendReceivePacket failed - Input or Output pipe handle invalid\n");
#endif
    return USB_ERROR;  // Operation Failed
}

//-------------------------------

int read_debug_byte (int *value)
{
     send_buf[0] = 0xED;      // Command
    
    RecvLength = 3; //set expected receive length 
	
    if (SendReceivePacket(send_buf,1,receive_buf,&RecvLength,1000,1000) == USB_NO_ERROR)
    {
        if ((RecvLength == 3) && (receive_buf[0] == 0xED))
        {
            *value = receive_buf[2] & 0xFF;
			return USB_NO_ERROR;
        }
		else
		{
#ifdef DEBUG
			printf("Debug byte received is incorrect length or incorrect command type\n");
#endif
			return USB_ERROR;
		}
		
    }
    else
    {
#ifdef DEBUG
        printf("Failed to retrieve debug byte - error from SendReceivePacket\n");
#endif
		return USB_ERROR;
	}
}

int send_config_message(bool async, bool sync, bool rising, bool falling, bool both,
	unsigned long rate, unsigned long samplenumber)
{
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
	
#ifdef DEBUG
	printf("config: async %d sync %d rise %d fall %d both %d\n", async, sync, rising, falling, both);
	printf("config: rate %d number %d\n", rate, samplenumber);
#endif
	
	send_buf[0] = 0x42; // config command code
	send_buf[1] = 11; // fixed length for config
	send_buf[2] = (async & 1) | ((sync & 1) << 1) | ((rising & 1) << 2) | ((falling & 1) << 3) | ((both & 1) << 4) | (1 << 7);
	
	
	return USB_NO_ERROR;
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
		return USB_NO_ERROR;
    }
    else
    {    
        printf("USB Operation Failed\r\n");
		return USB_ERROR;
	}
 
}			 */
