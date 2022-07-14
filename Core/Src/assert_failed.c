/*
 * assert_failed.c
 *
 *  Created on: 2022. jul. 13.
 *      Author: Balint
 */
#include "stm32f4xx_hal.h"

#ifdef  USE_FULL_ASSERT

#include "printf.h"
#include <stdarg.h>
#include <stdint.h>

static uint8_t uart_tx_buffer[256];

/**
  * @brief  Performs the printing of the assert_failed message
  * @param  format: string containing information about the assertion
  * @param  ...:    va list, (but most likely the file and line integers
  * 				passed here)
  * @retval None
  */
static void assert_failed_printf(const char * format, ...)
{
	__disable_irq();

	GPIO_InitTypeDef   hgpio = {0};
	UART_HandleTypeDef huart = {0};

	__HAL_RCC_DMA1_CLK_DISABLE();
	__HAL_RCC_DMA1_FORCE_RESET();
	__HAL_RCC_DMA1_RELEASE_RESET();
	__HAL_RCC_DMA2_CLK_DISABLE();
	__HAL_RCC_DMA2_FORCE_RESET();
	__HAL_RCC_DMA2_RELEASE_RESET();

	__HAL_RCC_USART2_CLK_ENABLE();
	__HAL_RCC_USART2_FORCE_RESET();
	__HAL_RCC_USART2_RELEASE_RESET();

	huart.Instance          = USART2;
	HAL_UART_DeInit(&huart);

	huart.Init.BaudRate     = 115200;
	huart.Init.WordLength   = UART_WORDLENGTH_8B;
	huart.Init.StopBits     = UART_STOPBITS_1;
	huart.Init.Parity       = UART_PARITY_NONE;
	huart.Init.Mode         = UART_MODE_TX_RX;
	huart.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
	huart.Init.OverSampling = UART_OVERSAMPLING_16;
	HAL_UART_Init(&huart);

	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOA_FORCE_RESET();
	__HAL_RCC_GPIOA_RELEASE_RESET();
	HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2 | GPIO_PIN_3);

	hgpio.Pin               = GPIO_PIN_2 | GPIO_PIN_3; /*PA2 ---> USART2_TX, PA3 ---> USART2_RX */
	hgpio.Mode              = GPIO_MODE_AF_PP;
	hgpio.Pull              = GPIO_NOPULL;
	hgpio.Speed             = GPIO_SPEED_FREQ_VERY_HIGH;
	hgpio.Alternate         = GPIO_AF7_USART2;
	HAL_GPIO_Init(GPIOA, &hgpio);

	va_list va;
	va_start(va, format);
	uint32_t len = (uint32_t)vsnprintf((char *)uart_tx_buffer, sizeof(uart_tx_buffer), format, va);
	va_end(va);

	for (uint32_t i = 0; i < len; i++) {
		HAL_UART_Transmit(&huart, &uart_tx_buffer[i], sizeof(uint8_t), 0xFFFF);
	}
}

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
	/* User can add his own implementation to report the file name and line number,
	 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	assert_failed_printf("Assert failed in file: %s on line: %d !\r\n", file, line);

	while (1)
	{
	}
}

#endif /* USE_FULL_ASSERT */



