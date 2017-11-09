#ifndef __USBD_DESC_H
#define __USBD_DESC_H

/* Includes ------------------------------------------------------------------*/
#include "usbd_def.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
extern const USBD_DescriptorsTypeDef USBD_Desc;
extern const uint8_t *const USBD_CfgFSDesc_pnt;
extern const uint16_t USBD_CfgFSDesc_len;

struct USBD_CfgFSHIDDesc_struct
{
  const uint8_t *pnt;
  const uint16_t len;
};

extern const struct USBD_CfgFSHIDDesc_struct *USBD_CfgFSHIDDesc;

#endif /* __USBD_DESC_H */
