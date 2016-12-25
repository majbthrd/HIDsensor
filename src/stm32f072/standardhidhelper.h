#ifndef __STANDARDHID_HELPER_H
#define __STANDARDHID_HELPER_H

#include <stdint.h>
#include "usbhelper.h"

/* macro to help generate Standard HID (e.g. keyboard and mouse) USB descriptors */

#define STANDARDHID_DESCRIPTOR(HID_INTF, DATAIN_EP, HID_REPORT_DESC_SIZE, HID_PROTOCOL) \
    { \
      { \
        /*Interface Descriptor */ \
        sizeof(struct interface_descriptor),             /* bLength: Interface Descriptor size */ \
        USB_DESC_TYPE_INTERFACE,                         /* bDescriptorType: Interface */ \
        HID_INTF,                                        /* bInterfaceNumber: Number of Interface */ \
        0x00,                                            /* bAlternateSetting: Alternate setting */ \
        0x01,                                            /* bNumEndpoints */ \
        0x03,                                            /* bInterfaceClass: HID */ \
        0x00,                                            /* bInterfaceSubClass: 1=BOOT, 0=no boot */ \
        HID_PROTOCOL,                                    /* bInterfaceProtocol: 0=none, 1=keyboard, 2=mouse */ \
        0x00,                                            /* iInterface (string index) */ \
      }, \
 \
      { \
        sizeof(struct hid_functional_descriptor),      /* bLength */ \
        HID_DESCRIPTOR_TYPE,                           /* bDescriptorType */ \
        USB_UINT16(0x0111),                            /* bcdHID */ \
        0x00,                                          /* bCountryCode */ \
        0x01,                                          /* bNumDescriptors */ \
        HID_REPORT_DESC,                               /* bDescriptorType */ \
        USB_UINT16(HID_REPORT_DESC_SIZE),              /* wItemLength */ \
      }, \
 \
      { \
        sizeof(struct endpoint_descriptor),            /* bLength: Endpoint Descriptor size */ \
        USB_DESC_TYPE_ENDPOINT,                        /* bDescriptorType: Endpoint */ \
        DATAIN_EP,                                     /* bEndpointAddress */ \
        0x03,                                          /* bmAttributes: Interrupt */ \
        USB_UINT16(HID_EP_SIZE),                       /* wMaxPacketSize */ \
        HID_POLLING_INTERVAL,                          /* bInterval */ \
      }, \
    },

struct standardhid_interface
{
  struct interface_descriptor             ctl_interface;
  struct hid_functional_descriptor        hid_func;
  struct endpoint_descriptor              ep_in;
};

#define HIDMOUSE_DESCRIPTOR(HID_INTF, DATAIN_EP, HID_REPORT_DESC_SIZE)    STANDARDHID_DESCRIPTOR(HID_INTF, DATAIN_EP, HID_REPORT_DESC_SIZE, 2)
#define HIDKEYBOARD_DESCRIPTOR(HID_INTF, DATAIN_EP, HID_REPORT_DESC_SIZE) STANDARDHID_DESCRIPTOR(HID_INTF, DATAIN_EP, HID_REPORT_DESC_SIZE, 1)
#define HIDNONE_DESCRIPTOR(HID_INTF, DATAIN_EP, HID_REPORT_DESC_SIZE) STANDARDHID_DESCRIPTOR(HID_INTF, DATAIN_EP, HID_REPORT_DESC_SIZE, 0)

#endif /* __STANDARDHID_HELPER_H */
