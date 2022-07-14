/**
  ******************************************************************************
  * @file    gpio.h
  * @brief   This file contains all the function prototypes for
  *          the gpio.c file
  ******************************************************************************
  *
  *
  ******************************************************************************
  */
#ifndef __GPIO_H__
#define __GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif

#define LD4_Pin 				GPIO_PIN_12
#define LD4_GPIO_Port 			GPIOD
#define LD3_Pin 				GPIO_PIN_13
#define LD3_GPIO_Port 			GPIOD
#define LD5_Pin 				GPIO_PIN_14
#define LD5_GPIO_Port 			GPIOD
#define LD6_Pin 				GPIO_PIN_15
#define LD6_GPIO_Port 			GPIOD

void GPIO_Init(void);

#ifdef __cplusplus
}
#endif
#endif /*__ GPIO_H__ */

