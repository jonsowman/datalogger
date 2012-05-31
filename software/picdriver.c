#include <ansi_c.h>
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
 		 MPUSBGetDLLVersion=(DWORD(*)(void))\
                    GetProcAddress(hinstLib,"_MPUSBGetDLLVersion");
         MPUSBGetDeviceCount=(DWORD(*)(PCHAR))\
                    GetProcAddress(hinstLib,"_MPUSBGetDeviceCount");
                    
         MPUSBOpen=(HANDLE(*)(DWORD,PCHAR,PCHAR,DWORD,DWORD))\
                    GetProcAddress(hinstLib,"_MPUSBOpen");
                    
        MPUSBWrite=(DWORD(*)(HANDLE,PVOID,DWORD,PDWORD,DWORD))\
                    GetProcAddress(hinstLib,"_MPUSBWrite");
                    
        MPUSBRead=(DWORD(*)(HANDLE,PVOID,DWORD,PDWORD,DWORD))\
                    GetProcAddress(hinstLib,"_MPUSBRead");
                    
        MPUSBReadInt=(DWORD(*)(HANDLE,PVOID,DWORD,PDWORD,DWORD))\
                    GetProcAddress(hinstLib,"_MPUSBReadInt");
                    
        MPUSBClose=(BOOL(*)(HANDLE))GetProcAddress(hinstLib,"_MPUSBClose");
                
         
        // If the function address is valid, call the function.
 
        
        Version=MPUSBGetDeviceCount(vid_pid) ;  
            
           
        myOutPipe = MPUSBOpen(0,vid_pid,out_pipe,MP_WRITE,0);
        myInPipe = MPUSBOpen(0,vid_pid,out_pipe,MP_READ,0);
            
       if(myOutPipe == INVALID_HANDLE_VALUE )
          return USB_ERROR;
       if(myInPipe == INVALID_HANDLE_VALUE)
          return USB_ERROR;
   
       //UCAM HACK NOT SURE WHY
   	   send_buf[0]=0xEE;
       SendReceivePacket(send_buf, 1, receive_buf,&RecvLength,1000,1000);          
          
      return USB_NO_ERROR; 
     }
            
     return USB_ERROR;  

}

int close_usb()
{
      fFreeResult = FreeLibrary(hinstLib);   
	  return fFreeResult;
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
                if(*ReceiveLength == ExpectedReceiveLength)
                {
                    return USB_NO_ERROR;   // Success!
                }
                else if(*ReceiveLength < ExpectedReceiveLength)
                {
                    return USB_ERROR;   // Partially failed, incorrect receive length
                }//end if else
            }
             
  
        }
        
          
    }//end if

    return USB_ERROR;  // Operation Failed
}

//-------------------------------

int read_debug_byte (int *value)
{
     send_buf[0] = 0xED;      // Command
    
    RecvLength = 3; //set expected receive length 
	
    if (SendReceivePacket(send_buf,1,receive_buf,&RecvLength,1000,1000) == 1)
    {
        if ((RecvLength == 3) && (receive_buf[0] == 0xED))
        {
            *value = receive_buf[2] & 0xFF;
        }
		return USB_NO_ERROR;
    }
    else
    {
        printf("Failed to retrieve debug byte\r\n");
		return USB_ERROR;
	}
}

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
 
}
