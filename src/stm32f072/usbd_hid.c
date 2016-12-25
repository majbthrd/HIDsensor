/*
    USB HID for STM32F072 microcontroller

    Copyright (C) 2015,2016 Peter Lawrence

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

/* Includes ------------------------------------------------------------------*/
#include "usbd_hid.h"
#include "usbd_desc.h"
#include "usbd_ctlreq.h"
#include "HidSensorSpec.h"

static uint8_t  USBD_HID_Init (USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t  USBD_HID_DeInit (USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t  USBD_HID_Setup (USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);
static uint8_t  USBD_HID_DataIn (USBD_HandleTypeDef *pdev, uint8_t epnum);
static void     USBD_HID_PMAConfig(PCD_HandleTypeDef *hpcd, uint32_t *pma_address);

const USBD_CompClassTypeDef USBD_HID = 
{
  .Init                  = USBD_HID_Init,
  .DeInit                = USBD_HID_DeInit,
  .Setup                 = USBD_HID_Setup,
  .EP0_TxSent            = NULL,
  .EP0_RxReady           = NULL,
  .DataIn                = USBD_HID_DataIn,
  .DataOut               = NULL,
  .SOF                   = NULL,
  .PMAConfig             = USBD_HID_PMAConfig,
};

/* NOTE: manually ensure that the size of this report equals the value given in the descriptor in usbd_desc.c */

__ALIGN_BEGIN static const uint8_t HID_Sensor_ReportDesc[184]  __ALIGN_END =
{
  HID_USAGE_PAGE_SENSOR,
  HID_USAGE_SENSOR_TYPE_ENVIRONMENTAL_TEMPERATURE,
  HID_COLLECTION(Physical),

  //feature reports (xmit/receive)
  HID_USAGE_PAGE_SENSOR,

  HID_USAGE_SENSOR_PROPERTY_REPORT_INTERVAL,
  HID_LOGICAL_MIN_8(0),
  HID_LOGICAL_MAX_32(0xFF,0xFF,0xFF,0xFF),
  HID_REPORT_SIZE(32),
  HID_REPORT_COUNT(1),
  HID_UNIT_EXPONENT(0),
  HID_FEATURE(Data_Var_Abs),

  HID_USAGE_SENSOR_DATA(HID_USAGE_SENSOR_DATA_ENVIRONMENTAL_TEMPERATURE,HID_USAGE_SENSOR_DATA_MOD_MAX),
  HID_LOGICAL_MIN_16(0x01,0x80), // LOGICAL_MINIMUM (-32767)
  HID_LOGICAL_MAX_16(0xFF,0x7F), // LOGICAL_MAXIMUM (32767)
  HID_REPORT_SIZE(16),
  HID_REPORT_COUNT(1),
  HID_UNIT_EXPONENT(0x0E), // scale default unit “Celsius” to provide 2 digits past the decimal point
  HID_FEATURE(Data_Var_Abs),

  HID_USAGE_SENSOR_DATA(HID_USAGE_SENSOR_DATA_ENVIRONMENTAL_TEMPERATURE,HID_USAGE_SENSOR_DATA_MOD_MIN),
  HID_LOGICAL_MIN_16(0x01,0x80), // LOGICAL_MINIMUM (-32767)
  HID_LOGICAL_MAX_16(0xFF,0x7F), // LOGICAL_MAXIMUM (32767)
  HID_REPORT_SIZE(16),
  HID_REPORT_COUNT(1),
  HID_UNIT_EXPONENT(0x0E), // scale default unit “Celsius” to provide 2 digits past the decimal point
  HID_FEATURE(Data_Var_Abs),

  //input reports (transmit)
  HID_USAGE_PAGE_SENSOR,

  HID_USAGE_SENSOR_STATE,
  HID_LOGICAL_MIN_8(0),
  HID_LOGICAL_MAX_8(6),
  HID_REPORT_SIZE(8),
  HID_REPORT_COUNT(1),
  HID_COLLECTION(Logical),
  HID_USAGE_SENSOR_STATE_UNKNOWN,
  HID_USAGE_SENSOR_STATE_READY,
  HID_USAGE_SENSOR_STATE_NOT_AVAILABLE,
  HID_USAGE_SENSOR_STATE_NO_DATA,
  HID_USAGE_SENSOR_STATE_INITIALIZING,
  HID_USAGE_SENSOR_STATE_ACCESS_DENIED,
  HID_USAGE_SENSOR_STATE_ERROR,
  HID_INPUT(Data_Arr_Abs),
  HID_END_COLLECTION,

  HID_USAGE_SENSOR_EVENT,
  HID_LOGICAL_MIN_8(0),
  HID_LOGICAL_MAX_8(16),
  HID_REPORT_SIZE(8),
  HID_REPORT_COUNT(1),
  HID_COLLECTION(Logical),
  HID_USAGE_SENSOR_EVENT_UNKNOWN,
  HID_USAGE_SENSOR_EVENT_STATE_CHANGED,
  HID_USAGE_SENSOR_EVENT_PROPERTY_CHANGED,
  HID_USAGE_SENSOR_EVENT_DATA_UPDATED,
  HID_USAGE_SENSOR_EVENT_POLL_RESPONSE,
  HID_USAGE_SENSOR_EVENT_CHANGE_SENSITIVITY,
  HID_USAGE_SENSOR_EVENT_MAX_REACHED,
  HID_USAGE_SENSOR_EVENT_MIN_REACHED,
  HID_USAGE_SENSOR_EVENT_HIGH_THRESHOLD_CROSS_UPWARD,
  HID_USAGE_SENSOR_EVENT_HIGH_THRESHOLD_CROSS_DOWNWARD,
  HID_USAGE_SENSOR_EVENT_LOW_THRESHOLD_CROSS_UPWARD,
  HID_USAGE_SENSOR_EVENT_LOW_THRESHOLD_CROSS_DOWNWARD,
  HID_USAGE_SENSOR_EVENT_ZERO_THRESHOLD_CROSS_UPWARD,
  HID_USAGE_SENSOR_EVENT_ZERO_THRESHOLD_CROSS_DOWNWARD,
  HID_USAGE_SENSOR_EVENT_PERIOD_EXCEEDED,
  HID_USAGE_SENSOR_EVENT_FREQUENCY_EXCEEDED,
  HID_USAGE_SENSOR_EVENT_COMPLEX_TRIGGER,
  HID_INPUT(Data_Arr_Abs),
  HID_END_COLLECTION,

  HID_USAGE_SENSOR_DATA_ENVIRONMENTAL_TEMPERATURE,
  HID_LOGICAL_MIN_16(0x01,0x80), // LOGICAL_MINIMUM (-32767)
  HID_LOGICAL_MAX_16(0xFF,0x7F), // LOGICAL_MAXIMUM (32767)
  HID_REPORT_SIZE(16),
  HID_REPORT_COUNT(1),
  HID_UNIT_EXPONENT(0x0E), // scale default unit “Celsius” to provide 2 digits past the decimal point
  HID_INPUT(Data_Var_Abs),
  HID_END_COLLECTION
}; 

/* store Feature Reports as const arrays that can be fetched as needed in response to a GET_REPORT */

static const uint8_t temp_sensor_feature_report[] =
{
  HID_SENSOR_VALUE_SIZE_32(100), /* report interval (ms) */
  HID_SENSOR_VALUE_SIZE_16(150 * 100), /* maximum sensor value (150C) */
  HID_SENSOR_VALUE_SIZE_16(-50 * 100), /* minimum sensor value (-50C) */
};

static const struct
{
  uint16_t itf_num;
  uint16_t wValue;
  const uint8_t *report;
  uint16_t length;
} available_reports[] =
{
  {
    .itf_num = 0,
    .wValue = 0x0300, /* Feature 0 */
    .report = temp_sensor_feature_report,
    .length = sizeof(temp_sensor_feature_report),
  },
};

/* HID report, endpoint numbers, and interface number for each HID instance */
static const struct
{
  const uint8_t *ReportDesc;
  unsigned ReportDesc_Length;
  uint8_t data_in_ep, itf_num;
} parameters[NUM_OF_STANDARDHID] = 
{
#if (NUM_OF_STANDARDHID > 0)
  {
    .ReportDesc = HID_Sensor_ReportDesc,
    .ReportDesc_Length = sizeof(HID_Sensor_ReportDesc),
    .data_in_ep = 0x81,
    .itf_num = 0,
  },
#endif
#if (NUM_OF_STANDARDHID > 1)
  #error "add more entries or use multiple Input Report IDs on the same endpoint"
#endif
};

static USBD_HID_HandleTypeDef context[NUM_OF_STANDARDHID];

static uint8_t  USBD_HID_Init (USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
  USBD_HID_HandleTypeDef *hhid = context;
  unsigned index;

  for (index = 0; index < NUM_OF_STANDARDHID; index++,hhid++)
  {
    /* Open HID EPs */
    USBD_LL_OpenEP(pdev, parameters[index].data_in_ep, USBD_EP_TYPE_INTR, HID_EP_SIZE);  

    hhid->state = HID_IDLE;
  }

  return USBD_OK;
}

static uint8_t  USBD_HID_DeInit (USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
  unsigned index;

  for (index = 0; index < NUM_OF_STANDARDHID; index++)
  {
    /* Close HID EPs */
    USBD_LL_CloseEP(pdev, parameters[index].data_in_ep);
  }

  return USBD_OK;
}

static uint8_t  USBD_HID_Setup (USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
  uint16_t len = 0;
  const uint8_t  *pbuf = NULL;
  USBD_HID_HandleTypeDef *hhid = context;
  unsigned index, idx;

  for (index = 0; index < NUM_OF_STANDARDHID; index++,hhid++)
  {
    if (parameters[index].itf_num != req->wIndex)
      continue;

    switch (req->bmRequest & USB_REQ_TYPE_MASK)
    {
    case USB_REQ_TYPE_CLASS :  
      switch (req->bRequest)
      {
      case HID_REQ_SET_PROTOCOL:
        hhid->Protocol = (uint8_t)(req->wValue);
        break;
      
      case HID_REQ_GET_PROTOCOL:
        USBD_CtlSendData (pdev, (uint8_t *)&hhid->Protocol, 1);    
        break;
      
      case HID_REQ_SET_IDLE:
        hhid->IdleState = (uint8_t)(req->wValue >> 8);
        break;
      
      case HID_REQ_GET_IDLE:
        USBD_CtlSendData (pdev, (uint8_t *)&hhid->IdleState, 1);        
        break;      
      
      case HID_REQ_GET_REPORT:
        for (idx = 0; idx < (sizeof(available_reports) / sizeof(*available_reports)); idx++)
        {
          if ( (parameters[index].itf_num == available_reports[idx].itf_num) && (available_reports[idx].wValue == req->wValue) )
          {
            len = MIN(available_reports[idx].length, req->wLength);
            pbuf = available_reports[idx].report;
            break;
          }
        }

        USBD_CtlSendData (pdev, (uint8_t *)pbuf, len);

        break;

      case HID_REQ_SET_REPORT:
        break;

      default:
        USBD_CtlError (pdev, req);
        return USBD_FAIL; 
      }
      break;
    
    case USB_REQ_TYPE_STANDARD:
      switch (req->bRequest)
      {
      case USB_REQ_GET_DESCRIPTOR: 
        if( req->wValue >> 8 == HID_REPORT_DESC)
        {
          len = MIN(parameters[index].ReportDesc_Length, req->wLength);
          pbuf = parameters[index].ReportDesc;
        }
        else if( req->wValue >> 8 == HID_DESCRIPTOR_TYPE)
        {
          len = MIN(USBD_CfgFSHIDDesc[index].len, req->wLength);
          pbuf = USBD_CfgFSHIDDesc[index].pnt;
        }
      
        USBD_CtlSendData (pdev, (uint8_t *)pbuf, len);
      
        break;
      
      case USB_REQ_GET_INTERFACE :
        USBD_CtlSendData (pdev, (uint8_t *)&hhid->AltSetting, 1);
        break;
      
      case USB_REQ_SET_INTERFACE :
        hhid->AltSetting = (uint8_t)(req->wValue);
        break;
      }
    }
  }

  return USBD_OK;
}

static uint8_t  USBD_HID_DataIn (USBD_HandleTypeDef *pdev, uint8_t epnum)
{
  USBD_HID_HandleTypeDef *hhid = context;
  unsigned index;

  for (index = 0; index < NUM_OF_STANDARDHID; index++,hhid++)
  {
    if (parameters[index].data_in_ep != (epnum | 0x80))
      continue;

    context[index].state = HID_IDLE;
    break;
  }

  return USBD_OK;
}

static void USBD_HID_PMAConfig(PCD_HandleTypeDef *hpcd, uint32_t *pma_address)
{
  unsigned index;

  for (index = 0; index < NUM_OF_STANDARDHID; index++)
  {
    HAL_PCDEx_PMAConfig(hpcd, parameters[index].data_in_ep, PCD_SNG_BUF, *pma_address);
    *pma_address += HID_EP_SIZE;
  }
}

uint8_t USBD_HID_SendReport(USBD_HandleTypeDef *pdev, unsigned index, uint8_t *report, uint16_t len)
{
  if (index >= NUM_OF_STANDARDHID)
    return USBD_FAIL;

  if (pdev->dev_state != USBD_STATE_CONFIGURED )
    return USBD_FAIL;

  if(context[index].state != HID_IDLE)
    return USBD_FAIL;

  if (USBD_OK == USBD_LL_Transmit (pdev, parameters[index].data_in_ep, report, len))
  {
    context[index].state = HID_BUSY;
    return USBD_OK;
  }

  return USBD_FAIL;
}
