/**
  ******************************************************************************
  * @file           : usb_host.h
  * @brief          : Header for usb_host.c file.
  ******************************************************************************
  *
  *
  ******************************************************************************
  */
#ifndef __USB_HOST__H__
#define __USB_HOST__H__

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"

/** Status of the application. */
typedef enum {
  APPLICATION_IDLE = 0,
  APPLICATION_START,
  APPLICATION_READY,
  APPLICATION_DISCONNECT
}ApplicationTypeDef;

/** @brief USB Host initialization function. */
void MX_USB_HOST_Init(void);

#ifdef __cplusplus
}
#endif

#endif /* __USB_HOST__H__ */



