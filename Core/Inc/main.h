/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  *
  *
  ******************************************************************************
  */
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#define OTG_FS_PowerSwitchOn_Pin 				GPIO_PIN_0
#define OTG_FS_PowerSwitchOn_GPIO_Port 			GPIOC
#define OTG_FS_ID_Pin 							GPIO_PIN_10
#define OTG_FS_ID_GPIO_Port 					GPIOA
#define OTG_FS_DM_Pin 							GPIO_PIN_11
#define OTG_FS_DM_GPIO_Port 					GPIOA
#define OTG_FS_DP_Pin 							GPIO_PIN_12
#define OTG_FS_DP_GPIO_Port 					GPIOA
#define OTG_FS_OverCurrent_Pin 					GPIO_PIN_5
#define OTG_FS_OverCurrent_GPIO_Port 			GPIOD


#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */


