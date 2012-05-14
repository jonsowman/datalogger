/*********************************************************************
 * Descriptor specific type definitions are defined in:
 * system\usb\usbdefs\usbdefs_std_dsc.h
 ********************************************************************/

#ifndef USBDSC_H
#define USBDSC_H

/** I N C L U D E S *************************************************/
#include "system\typedefs.h"
#include "autofiles\usbcfg.h"

#include "system\usb\usb.h"

/** D E F I N I T I O N S *******************************************/

#define CFG01 rom struct                            \
{   USB_CFG_DSC             cd01;                   \
    USB_INTF_DSC            i00a00;                 \
    USB_EP_DSC              ep01o_i00a00;           \
    USB_EP_DSC              ep01i_i00a00;           \
} cfg01

/*/mod by Tim
#define CFG02 rom struct                            \
{   
//config descriptor
	USB_CFG_DSC             cd02;                   \ 
//interface descriptor 		i<i/f#>a<alternate i/f#>
    USB_INTF_DSC            i00a00;                 \
//endpoint descriptors		ep<ep#><direction>_<i/f>
    USB_EP_DSC              ep01o_i00a00;           \
    USB_EP_DSC              ep01i_i00a00;           \
} cfg02
//end mod by Tim
*/
/** E X T E R N S ***************************************************/
extern rom USB_DEV_DSC device_dsc;
extern CFG01;
extern rom const unsigned char *rom USB_CD_Ptr[];
extern rom const unsigned char *rom USB_SD_Ptr[];

#endif //USBDSC_H
