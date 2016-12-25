example HID Sensor using PIC16F1454 microcontroller
===================================================

## What It Does

The firmware functions as a Temperature HID Sensor.  It reports dummy values in progression from -50C to 125C, and then repeats.

## Requirements

[XC8](http://www.microchip.com/mplab/compilers) and make tools are needed to compile this code.

## Sanity Checklist If Customizing

The HID Interface numbers in the USB descriptor in usb\_descriptors.c must be continguous and start from zero.

Customize the HID Report descriptor (in usbd\_descriptors.c) to suit your sensor.  Refer to the protocol document [HUTRR39b](http://www.usb.org/developers/hidpage/HUTRR39b.pdf) for details.

An understanding of USB descriptors is important when modifying usb\_descriptors.c.  This data conveys the configuration of the device (including endpoint, etc.) to the host PC.

Be *certain* that the Feature and Input Reports *exactly* match, byte-for-byte, what is described in the HID Report.

The Makefile uses compiler options --rom and --codeoffset to work with this [open-source PIC16F1454 bootloader](https://github.com/majbthrd/PIC16F1-USB-DFU-Bootloader/).  If, however, you are a masochist who wants the pain and suffering of using a Microchip PICkit3 to reprogram the chip each and every time you change your code, then alter these compiler options.

