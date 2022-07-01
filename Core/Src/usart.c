/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
  ******************************************************************************
  *
  *
  ******************************************************************************
  */
#include "usart.h"
#include "main.h"
#include "stm32f4xx_hal.h"

UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart2_tx;

static void UART2_MspInit(UART_HandleTypeDef* uartHandle);
static void UART2_MspDeInit(UART_HandleTypeDef* uartHandle);

void UART2_Init(void)
{
	huart2.Instance          = USART2;
	huart2.Init.BaudRate     = 115200;
	huart2.Init.WordLength   = UART_WORDLENGTH_8B;
	huart2.Init.StopBits     = UART_STOPBITS_1;
	huart2.Init.Parity       = UART_PARITY_NONE;
	huart2.Init.Mode         = UART_MODE_TX_RX;
	huart2.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
	huart2.Init.OverSampling = UART_OVERSAMPLING_16;

	HAL_UART_RegisterCallback(&huart2, HAL_UART_MSPINIT_CB_ID, UART2_MspInit);
	HAL_UART_RegisterCallback(&huart2, HAL_UART_MSPDEINIT_CB_ID, UART2_MspDeInit);

	if (HAL_UART_Init(&huart2) != HAL_OK) {
		Error_Handler();
	}
}

static void UART2_MspInit(UART_HandleTypeDef* uartHandle)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	/* USART2 clock enable */
	__HAL_RCC_USART2_CLK_ENABLE();

	__HAL_RCC_GPIOA_CLK_ENABLE();
	/**USART2 GPIO Configuration
	PA2     ------> USART2_TX
	PA3     ------> USART2_RX
	*/
	GPIO_InitStruct.Pin       = GPIO_PIN_2|GPIO_PIN_3;
	GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull      = GPIO_NOPULL;
	GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/* USART2 DMA Init */
	/* USART2_TX Init */
	hdma_usart2_tx.Instance                 = DMA1_Stream6;
	hdma_usart2_tx.Init.Channel             = DMA_CHANNEL_4;
	hdma_usart2_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
	hdma_usart2_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
	hdma_usart2_tx.Init.MemInc              = DMA_MINC_ENABLE;
	hdma_usart2_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	hdma_usart2_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
	hdma_usart2_tx.Init.Mode                = DMA_NORMAL;
	hdma_usart2_tx.Init.Priority            = DMA_PRIORITY_LOW;
	hdma_usart2_tx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;

	if (HAL_DMA_Init(&hdma_usart2_tx) != HAL_OK) {
		Error_Handler();
	}

	__HAL_LINKDMA(uartHandle,hdmatx,hdma_usart2_tx);

	/* USART2 interrupt Init */
	HAL_NVIC_SetPriority(USART2_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(USART2_IRQn);
}

static void UART2_MspDeInit(UART_HandleTypeDef* uartHandle)
{
	/* Peripheral clock disable */
	__HAL_RCC_USART2_CLK_DISABLE();

	/**USART2 GPIO Configuration
	PA2     ------> USART2_TX
	PA3     ------> USART2_RX
	*/
	HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2|GPIO_PIN_3);

	/* USART2 DMA DeInit */
	HAL_DMA_DeInit(uartHandle->hdmatx);

	/* USART2 interrupt Deinit */
	HAL_NVIC_DisableIRQ(USART2_IRQn);
}


