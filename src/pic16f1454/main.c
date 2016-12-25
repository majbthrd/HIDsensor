/*
    example HID Sensor using PIC16F1454 microcontroller

    Copyright (C) 2016 Peter Lawrence

    based on top of M-Stack USB driver stack by Alan Ott, Signal 11 Software

    Permission is hereby granted, free of charge, to any person obtaining a 
    copy of this software and associated documentation files (the "Software"), 
    to deal in the Software without restriction, including without limitation 
    the rights to use, copy, modify, merge, publish, distribute, sublicense, 
    and/or sell copies of the Software, and to permit persons to whom the 
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in 
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
    DEALINGS IN THE SOFTWARE.
*/

#include "usb.h"
#include <xc.h>
#include <string.h>
#include "usb_config.h"
#include "usb_ch9.h"
#include "usb_hid.h"
#include "HidSensorSpec.h"

/* 
since this is a downloaded app, configuration words (e.g. __CONFIG or #pragma config) are not relevant
*/

static uint8_t tick_counter;

int main(void)
{
	uint8_t *TxDataBuffer;
	int16_t temperature = -5000;

	usb_init();

	TxDataBuffer = usb_get_in_buffer(1);

	for (;;)
	{
		usb_service();

		/* if USB isn't configured, there is no point in proceeding further */
		if (!usb_is_configured())
			continue;

		/* proceed further only when the IN endpoint is ready */
		if (usb_in_endpoint_halted(1) || usb_in_endpoint_busy(1))
			continue;

		/* wait for prescribed interval (in ticks) before sending new sensor value */
		if (tick_counter < 100)
			continue;

		TxDataBuffer[0] = 0x02; /* Ready */
		TxDataBuffer[1] = 0x03; /* Data Updated */
		TxDataBuffer[2] = (temperature >> 0); /* sensor value */
		TxDataBuffer[3] = (temperature >> 8);

		/* send a reading to the PC */
		usb_send_in_buffer(1, 4);

		/* pick next value; generate temperatures from -50.0C to 125.0C in increments of 1.0C */
		if (temperature > 12500)
			temperature = -5000;
		else
			temperature += 100;

		/* reset counter */
		tick_counter = 0;
	}
}

int8_t app_unknown_setup_request_callback(const struct setup_packet *setup)
{
	return process_hid_setup_request(setup);
}

static const uint8_t temp_sensor_feature_report[] =
{
	HID_SENSOR_VALUE_SIZE_32(100), /* report interval (ms) */
	HID_SENSOR_VALUE_SIZE_16(150 * 100), /* maximum sensor value (150C) */
	HID_SENSOR_VALUE_SIZE_16(-50 * 100), /* minimum sensor value (-50C) */
};

int16_t app_get_report_callback(uint8_t interface, uint8_t report_type, uint8_t report_id, const void **report, usb_ep0_data_stage_callback *callback, void **context)
{
	*report = temp_sensor_feature_report;
	return sizeof(temp_sensor_feature_report);
}

void app_start_of_frame_callback(void)
{
	tick_counter++;
}

int8_t app_set_report_callback(uint8_t interface, uint8_t report_type, uint8_t report_id)
{
	usb_send_data_stage(NULL, 0, NULL, NULL);
	return 0;
}

int8_t app_set_idle_callback(uint8_t interface, uint8_t report_id, uint8_t idle_rate)
{
	usb_send_data_stage(NULL, 0, NULL, NULL);
	return 0;
}
