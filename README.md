USB HID Sensor implementation for PIC16F1454 and STM32F072
==========================================================

This project contains open-source firmware for an example USB HID Sensor implementation.  Functionally equivalent source code is provided for two inexpensive USB microcontrollers:

[Microchip PIC16F1454 / PIC16F1455 / PIC16F1459](http://www.microchip.com/wwwproducts/en/PIC16F1454)

[ST Micro STM32F042 / STM32F072](http://www.st.com/en/microcontrollers/stm32f0x2.html)

## What It Is

If you have arrived at this from a search engine, like me you probably encountered countless inapplicable search results of people doing Sensors that use the USB HID Protocol, rather than the desired search result of an implementation of the USB HID Sensor protocol [HUTRR39b](http://www.usb.org/developers/hidpage/HUTRR39b.pdf).

This project provides example code to demonstrate an implementation of the HID Sensor protocol; you must modify the code yourself to add your particular sensor's functionality.

## Introduction to HID Sensor

Ostensibly, the appeal of the HID Sensor protocol is it that provides a driver-less mechanism to connect sensors to a PC.  There is a very rich descriptive metadata defined in the HID Sensor protocol that tells PC software what type of sensor it is, what values it outputs, what units these values are in, and how often it sends updates.

This is also the Achilles Heel of the HID Sensor protocol; this descriptive metadata is so flexible that the OS driver writers may not have anticipated or tested the configuration of your particular sensor.

To my knowledge, the HID Sensor protocol is presently only supported in Windows 8 and Windows 10.  (There are some bleeding edge Linux drivers under development.)

To make things more convoluted for the would-be developer, there are subtle differences in behavior between Windows 8 and Windows 10 with maddeningly cryptic or misleading error messages.  For example, someone in Redmond crafted the misleading Windows 10 error message "Failed to get HID identification string from device".  Only by trial and error did I discover this error happens when the HID Sensor device does not provide USB manufacturer and serial number strings.  These strings are not mandated by the HID Sensor protocol nor by the USB specification documents.  Furthermore, they have absolutely nothing to do with "HID", despite the "HID identification string" error message.  Windows 8, in contrast to Windows 10, does not give this error message.

Similarly frustrating is that some of the example HID report descriptors in the HID Sensor protocol document will not work in Windows 8.  It just gives up with a Feature Report error; one would have hoped that, at a minimum, these examples at least would have been tested.  Thankfully, Windows 10 seems more adept at decoding these.

So, you must expect trial and error testing to see what will work.  An implementation may be 100% compliant to the HID Sensor protocol but yet not work in Windows.

A further frustration for the would-be developer is that Microsoft has seemingly abandoned its Sensor Diagnostic Tool, the slightly clunky but indispensable software provided by the Windows WDK and needed by HID Sensor developers to check the incoming sensor data.  Windows 10 seems to have broken this software, and Microsoft is either unwilling or unable to fix it.  Instead, their website indicates that the tool is now deprecated and to use the SensorInfo app.  Alas, this new software is positively useless.

## Customizing

Like the USB HID specification on which the HID Sensors are based, the HID Report Descriptor for HID Sensors describes what the Input, Output, and Feature Reports will be.  Of primary interest are Input and Feature Reports.

The Feature Report is retrieved from the USB device (via EP0) and provides much of the descriptive metadata (such as ranges, units, etc).

The Input Report is sent from the USB device and each message provides the current sensor data.

Read the HID Sensor protocol standard thoroughly to learn the specifics.  The HID Report Descriptors in this firmware follow the conventions established in the standards document.

The firmware for each of the USB microcontrollers has its own README.md file to describe the structure and operation of the firmware.

Customize the HID Report Descriptor to suit your sensor, and then craft the Feature and Input Reports to *exactly* match, byte-for-byte, what has been described in the HID Report Descriptor.

