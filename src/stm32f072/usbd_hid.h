#ifndef __USB_HID_H
#define __USB_HID_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "usbd_ioreq.h"
#include "usbd_composite.h"
#include "config.h"

#define HID_EP_SIZE                   0x40

#define HID_DESCRIPTOR_TYPE           0x21
#define HID_REPORT_DESC               0x22
#define HID_POLLING_INTERVAL          0x01

#define HID_REQ_SET_PROTOCOL          0x0B
#define HID_REQ_GET_PROTOCOL          0x03

#define HID_REQ_SET_IDLE              0x0A
#define HID_REQ_GET_IDLE              0x02

#define HID_REQ_SET_REPORT            0x09
#define HID_REQ_GET_REPORT            0x01

typedef enum
{
  HID_IDLE = 0,
  HID_BUSY,
}
HID_StateTypeDef; 

typedef struct
{
  uint32_t             Protocol;   
  uint32_t             IdleState;  
  uint32_t             AltSetting;
  uint8_t buffer[HID_EP_SIZE];
  HID_StateTypeDef     state;  
}
USBD_HID_HandleTypeDef; 

extern const USBD_CompClassTypeDef USBD_HID;

uint8_t USBD_HID_SendReport(USBD_HandleTypeDef *pdev, unsigned index, uint8_t *report, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif  /* __USB_HID_H */
