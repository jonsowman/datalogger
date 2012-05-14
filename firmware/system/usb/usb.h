#ifndef USB_H
#define USB_H

/*
 * usb.h provides a centralize way to include all files
 * required by Microchip USB Firmware.
 *
 * The order of inclusion is important.
 * Dependency conflicts are resolved by the correct ordering.
 */

#include "autofiles\usbcfg.h"
#include "system\usb\usbdefs\usbdefs_std_dsc.h"
#include "autofiles\usbdsc.h"

#include "system\usb\usbdefs\usbdefs_ep0_buff.h"
#include "system\usb\usbmmap.h"

#include "system\usb\usbdrv\usbdrv.h"
#include "system\usb\usbctrltrf\usbctrltrf.h"
#include "system\usb\usb9\usb9.h"

#if defined(USB_USE_GEN)               // See autofiles\usbcfg.h
#include "system\usb\class\generic\usbgen.h"
#endif
          
#endif //USB_H
