#line 1 "usbmmap.c"
#line 1 "usbmmap.c"

#line 35 "usbmmap.c"
 


#line 149 "usbmmap.c"
 
 
 



 
#pragma udata
byte usb_device_state;          
USB_DEVICE_STATUS usb_stat;     
byte usb_active_cfg;            
byte usb_alt_intf[MAX_NUM_INT]; 
                                

 
#pragma udata usbram4=0x400     


#line 171 "usbmmap.c"
 

#line 174 "usbmmap.c"
volatile far BDT ep0Bo;         
volatile far BDT ep0Bi;         
#line 177 "usbmmap.c"

#line 179 "usbmmap.c"
#line 182 "usbmmap.c"

#line 184 "usbmmap.c"
#line 187 "usbmmap.c"

#line 189 "usbmmap.c"
#line 192 "usbmmap.c"

#line 194 "usbmmap.c"
#line 197 "usbmmap.c"

#line 199 "usbmmap.c"
#line 202 "usbmmap.c"

#line 204 "usbmmap.c"
#line 207 "usbmmap.c"

#line 209 "usbmmap.c"
#line 212 "usbmmap.c"

#line 214 "usbmmap.c"
#line 217 "usbmmap.c"

#line 219 "usbmmap.c"
#line 222 "usbmmap.c"

#line 224 "usbmmap.c"
#line 227 "usbmmap.c"

#line 229 "usbmmap.c"
#line 232 "usbmmap.c"

#line 234 "usbmmap.c"
#line 237 "usbmmap.c"

#line 239 "usbmmap.c"
#line 242 "usbmmap.c"

#line 244 "usbmmap.c"
#line 247 "usbmmap.c"

#line 249 "usbmmap.c"
#line 252 "usbmmap.c"


#line 267 "usbmmap.c"
 
volatile far CTRL_TRF_SETUP SetupPkt;
volatile far CTRL_TRF_DATA CtrlTrfData;


#line 275 "usbmmap.c"
 
volatile far byte usbgen_out[USBGEN_EP_SIZE];
volatile far byte usbgen_in[USBGEN_EP_SIZE];

#pragma udata

 
