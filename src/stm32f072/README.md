example USB HID Sensor for STM32F072 microcontroller
====================================================

## What It Does

The firmware functions as a Temperature HID Sensor.  It reports dummy values in progression from -50C to 125C, and then repeats.

## Requirements

[Rowley Crossworks for ARM](http://www.rowley.co.uk/arm/) is needed to compile this code.  The source code is gcc-friendly, but you must adapt the code yourself if you wish to adopt a different tool chain.

## Sanity Checklist If Customizing

config.h has a NUM\_OF\_STANDARDHID value that is used throughout the code to control the number of HID interfaces.  If contemplating multiple sensors, consider using multiple Input Report IDs for in lieu of multiple HID interfaces.

The HID Interface numbers in the USB descriptor in usbd\_desc.c must be continguous and start from zero.

One must manually ensure that the size of the HID Report descriptor (in usbd\_hid.c) equals the report size in the Configuration Descriptor in usbd\_desc.c.

Customize the HID Report descriptor (in usbd\_hid.c) to suit your sensor.  Refer to the protocol document [HUTRR39b](http://www.usb.org/developers/hidpage/HUTRR39b.pdf) for details.

An understanding of USB descriptors is important when modifying usb\_desc.c.  This data conveys the configuration of the device (including endpoint, etc.) to the host PC.

Be *certain* that the Feature and Input Reports *exactly* match, byte-for-byte, what is described in the HID Report.

USB transfers are handled via a distinct section of memory called "PMA".  Read the ST documentation on this.  At most, there is 1kBytes that must be shared across all endpoints.  Consider the usage of this PMA memory when scaling up the number of interfaces and buffer sizes.

