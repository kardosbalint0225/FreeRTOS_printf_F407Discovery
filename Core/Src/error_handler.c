/*
 * error_handler.c
 *
 *  Created on: 2022. jul. 10.
 *      Author: Balint
 */
#include "error_handler.h"
#include "stm32f4xx_hal.h"
#include "gpio.h"


/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
	/* User can add his own implementation to report the HAL error return state */
	HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin, GPIO_PIN_SET);
	while (1)
	{
	}
}




