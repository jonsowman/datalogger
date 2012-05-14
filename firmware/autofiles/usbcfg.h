#ifndef USBCFG_H
#define USBCFG_H

/** D E F I N I T I O N S *******************************************/
#define EP0_BUFF_SIZE           8   // 8, 16, 32, or 64
#define MAX_NUM_INT             1   // For tracking Alternate Setting

/* Parameter definitions are defined in usbdrv.h */
#define MODE_PP                 _PPBM0
#define UCFG_VAL                _PUEN|_TRINT|_FS|MODE_PP

//mod by Tim - commented out next lines
//#define USE_SELF_POWER_SENSE_IO
//#define USE_USB_BUS_SENSE_IO
//end mod by Tim

/** D E V I C E  C L A S S  U S A G E *******************************/

#define USB_USE_GEN

/*
 * MUID = Microchip USB Class ID
 * Used to identify which of the USB classes owns the current
 * session of control transfer over EP0
 */
#define MUID_NULL               0
#define MUID_USB9               1

/** E N D P O I N T S  A L L O C A T I O N **************************/
/*
 * See usbmmap.c for an explanation of how the endpoint allocation works
 */

/* PICDEM FS USB Demo (using generic usb class template) */
#define USBGEN_INTF_ID          0x00
#define USBGEN_UEP              UEP1
#define USBGEN_BD_OUT           ep1Bo
#define USBGEN_BD_IN            ep1Bi
#define USBGEN_EP_SIZE          64

#define MAX_EP_NUMBER           2           // UEP2

#endif //USBCFG_H
