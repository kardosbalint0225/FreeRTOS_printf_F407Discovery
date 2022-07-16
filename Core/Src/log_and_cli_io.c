/*
 * log_and_cli_io.c
 *
 *  Created on: 2022. jul. 14.
 *      Author: Balint
 */
#include "log_and_cli_io.h"
#include "stm32f4xx_hal.h"
#include "rtc.h"
#include "printf.h"

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"

#include <string.h>

typedef struct {
	uint8_t *pbuf;
	uint8_t  size;
} uart_tx_data_t;

static void UART2_Init(void);
static void UART2_Deinit(void);
static void UART2_MspInit(UART_HandleTypeDef* huart);
static void UART2_MspDeInit(UART_HandleTypeDef* huart);
static void UART2_TxCpltCallback(UART_HandleTypeDef *huart);
static void UART2_RxCpltCallback(UART_HandleTypeDef *huart);

static SemaphoreHandle_t uart_tx_complete_semaphore_handle = NULL;
static StaticSemaphore_t uart_tx_complete_semaphore_storage;
SemaphoreHandle_t uart_rx_complete_semaphore_handle = NULL;
static StaticSemaphore_t uart_rx_complete_semaphore_storage;

#define UART_TX_AVAILABLE_QUEUE_LENGTH				8
static StaticQueue_t uart_tx_available_struct;
static uint8_t       uart_tx_available_queue_storage[UART_TX_AVAILABLE_QUEUE_LENGTH * sizeof(uint8_t *)];
static QueueHandle_t uart_tx_available_queue_handle	= NULL;

#define UART_TX_READY_QUEUE_LENGTH					8
static StaticQueue_t uart_tx_ready_struct;
static uint8_t		 uart_tx_ready_queue_storage[UART_TX_READY_QUEUE_LENGTH * sizeof(uart_tx_data_t)];
static QueueHandle_t uart_tx_ready_queue_handle		= NULL;

#define UART_TX_PENDING_QUEUE_LENGTH				1
static StaticQueue_t uart_tx_pending_struct;
static uint8_t		 uart_tx_pending_queue_storage[UART_TX_PENDING_QUEUE_LENGTH * sizeof(uint8_t *)];
static QueueHandle_t uart_tx_pending_queue_handle 	= NULL;

#define UART_WRITE_TASK_STACKSIZE					1024
static StackType_t   uart_write_task_stack[UART_WRITE_TASK_STACKSIZE];
static StaticTask_t  uart_write_task_tcb;
static TaskHandle_t  uart_write_task_handle			= NULL;

UART_HandleTypeDef huart2;
DMA_HandleTypeDef  hdma_usart2_tx;

static uint8_t uart_tx_buffer[2048*8];

static void uart_write_task(void *params)
{

	for ( ;; )
	{
		if (pdPASS == xSemaphoreTake(uart_tx_complete_semaphore_handle, portMAX_DELAY)) {
			uint8_t *released = NULL;

			xQueueReceive(uart_tx_pending_queue_handle, &released, 0);
			xQueueSend(uart_tx_available_queue_handle, &released, 0);

			uart_tx_data_t uart_tx_data = {
				.pbuf = NULL,
				.size = 0,
			};

			if (pdPASS == xQueueReceive(uart_tx_ready_queue_handle, &uart_tx_data, portMAX_DELAY)) {
				HAL_UART_Transmit_DMA(&huart2, uart_tx_data.pbuf, uart_tx_data.size);
				xQueueSend(uart_tx_pending_queue_handle, &uart_tx_data.pbuf, 0);
			}
		}
	}
}

void log_and_cli_io_init(void)
{
	RTC_Init();
	UART2_Init();

	uart_tx_complete_semaphore_handle = xSemaphoreCreateBinaryStatic(&uart_tx_complete_semaphore_storage);
	//vSemaphoreCreateBinary(uart_tx_complete_semaphore_handle);
	assert_param(NULL != uart_tx_complete_semaphore_handle);
	xSemaphoreGive(uart_tx_complete_semaphore_handle);	// ????????????????

	uart_rx_complete_semaphore_handle = xSemaphoreCreateBinaryStatic(&uart_rx_complete_semaphore_storage);
	assert_param(NULL != uart_rx_complete_semaphore_handle);
	xSemaphoreTake(uart_rx_complete_semaphore_handle, 0);

	uart_tx_pending_queue_handle    = xQueueCreateStatic(
										UART_TX_PENDING_QUEUE_LENGTH,
										sizeof(uint8_t *),
										uart_tx_pending_queue_storage,
										&uart_tx_pending_struct);
	assert_param(NULL != uart_tx_pending_queue_handle);

	uart_tx_available_queue_handle  = xQueueCreateStatic(
										UART_TX_AVAILABLE_QUEUE_LENGTH,
										sizeof(uint8_t *),
										uart_tx_available_queue_storage,
										&uart_tx_available_struct);
	assert_param(NULL != uart_tx_available_queue_handle);

	uart_tx_ready_queue_handle      = xQueueCreateStatic(
										UART_TX_READY_QUEUE_LENGTH,
										sizeof(uart_tx_data_t),
										uart_tx_ready_queue_storage,
										&uart_tx_ready_struct);
	assert_param(NULL != uart_tx_ready_queue_handle);

	for (uint8_t i = 0; i < 8; i++) {
		uint8_t *buffer_address = &uart_tx_buffer[i*2048];
		xQueueSend(uart_tx_available_queue_handle, &buffer_address, 0);
	}

//	uart_write_task_handle 			= xTaskCreateStatic(
//										uart_write_task,
//										"UART write",
//										UART_WRITE_TASK_STACKSIZE,
//										NULL,
//										tskIDLE_PRIORITY+1,
//										uart_write_task_stack,
//										&uart_write_task_tcb);
//
//	assert_param(NULL != uart_write_task_handle);

	BaseType_t ret 					= xTaskCreate(
										uart_write_task,
										"UART write",
										(configMINIMAL_STACK_SIZE * 3),
										NULL,
										tskIDLE_PRIORITY+1,
										&uart_write_task_handle
										);

	assert_param(pdPASS == ret);
}

void log_and_cli_io_deinit(void)
{
	UART2_Deinit();
	RTC_Deinit();

	vTaskDelete(uart_write_task_handle);
	vQueueDelete(uart_tx_available_queue_handle);
	vQueueDelete(uart_tx_ready_queue_handle);
	vQueueDelete(uart_tx_pending_queue_handle);
	vSemaphoreDelete(uart_tx_complete_semaphore_handle);
	vSemaphoreDelete(uart_rx_complete_semaphore_handle);
}

/**
  * @brief  Initializes the UART2 peripheral
  * @param  None
  * @retval None
  * @note	The communication is configured 115200 Baudrate 8N1 with no
  * 		flowcontrol.
  */
static void UART2_Init(void)
{
	huart2.Instance          = USART2;
	huart2.Init.BaudRate     = 115200;
	huart2.Init.WordLength   = UART_WORDLENGTH_8B;
	huart2.Init.StopBits     = UART_STOPBITS_1;
	huart2.Init.Parity       = UART_PARITY_NONE;
	huart2.Init.Mode         = UART_MODE_TX_RX;
	huart2.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
	huart2.Init.OverSampling = UART_OVERSAMPLING_16;

	HAL_StatusTypeDef ret;

	ret = HAL_UART_RegisterCallback(&huart2, HAL_UART_MSPINIT_CB_ID, UART2_MspInit);
	assert_param(HAL_OK == ret);

	ret = HAL_UART_RegisterCallback(&huart2, HAL_UART_MSPDEINIT_CB_ID, UART2_MspDeInit);
	assert_param(HAL_OK == ret);

	ret = HAL_UART_Init(&huart2);
	assert_param(HAL_OK == ret);

	ret = HAL_UART_RegisterCallback(&huart2, HAL_UART_TX_COMPLETE_CB_ID, UART2_TxCpltCallback);
	assert_param(HAL_OK == ret);

	ret = HAL_UART_RegisterCallback(&huart2, HAL_UART_RX_COMPLETE_CB_ID, UART2_RxCpltCallback);
	assert_param(HAL_OK == ret);
}

/**
  * @brief  Initializes the UART2 peripheral low-level
  * @param  uartHandle
  * @retval None
  * @note	This function is called by the HAL library
  * @note	DMA1 Stream6 is used for TX. UART TX and DMA priority
  * 		are set to 14 but possibly can be set to 15
  */
static void UART2_MspInit(UART_HandleTypeDef* huart)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	/* USART2 clock enable */
	__HAL_RCC_USART2_CLK_ENABLE();

	/* DMA controller clock enable */
	__HAL_RCC_DMA1_CLK_ENABLE();

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

	HAL_StatusTypeDef ret = HAL_DMA_Init(&hdma_usart2_tx);
	assert_param(HAL_OK == ret);

	__HAL_LINKDMA(huart,hdmatx,hdma_usart2_tx);

	/* USART2 interrupt Init */
	HAL_NVIC_SetPriority(USART2_IRQn, 14, 0);
	HAL_NVIC_EnableIRQ(USART2_IRQn);

	/* DMA interrupt init */
	/* DMA1_Stream6_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA1_Stream6_IRQn, 14, 0);
	HAL_NVIC_EnableIRQ(DMA1_Stream6_IRQn);
}

/**
  * @brief  Deinitializes the UART2 peripheral
  * @param  None
  * @retval None
  * @note	-
  *
  */
static void UART2_Deinit(void)
{
	HAL_StatusTypeDef ret;

	ret = HAL_UART_UnRegisterCallback(&huart2, HAL_UART_TX_COMPLETE_CB_ID);
	assert_param(HAL_OK == ret);

	ret = HAL_UART_DeInit(&huart2);
	assert_param(HAL_OK == ret);

	ret = HAL_UART_UnRegisterCallback(&huart2, HAL_UART_MSPINIT_CB_ID);
	assert_param(HAL_OK == ret);

	ret = HAL_UART_UnRegisterCallback(&huart2, HAL_UART_MSPDEINIT_CB_ID);
	assert_param(HAL_OK == ret);
}

/**
  * @brief  Deinitializes the UART2 peripheral low-level
  * @param  uartHandle
  * @retval None
  * @note	This function is called by the HAL library
  * @note 	It wont disable the DMA1 peripheral clock since the
  * 		DMA1 might be used by other peripherals
  * @note	It wont disable the GPIOA peripheral clock since the
  * 		GPIOA might be used by other peripherals
  */
static void UART2_MspDeInit(UART_HandleTypeDef* huart)
{
	/* Peripheral clock disable */
	__HAL_RCC_USART2_CLK_DISABLE();

	/**USART2 GPIO Configuration
	PA2     ------> USART2_TX
	PA3     ------> USART2_RX
	*/
	HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2|GPIO_PIN_3);

	/* USART2 DMA DeInit */
	HAL_DMA_DeInit(huart->hdmatx);

	/* USART2 interrupt Deinit */
	HAL_NVIC_DisableIRQ(USART2_IRQn);
	HAL_NVIC_DisableIRQ(DMA1_Stream6_IRQn);
}

/**
  * @brief  UART2 Transfer complete callback
  * @param  huart
  * @retval None
  * @note	This function is called by the HAL library
  * 		when the DMA1 is finished transferring data
  */
static void UART2_TxCpltCallback(UART_HandleTypeDef *huart)
{
	portBASE_TYPE higher_priority_task_woken;
	xSemaphoreGiveFromISR(uart_tx_complete_semaphore_handle, &higher_priority_task_woken);

	portYIELD_FROM_ISR(higher_priority_task_woken);
}

/**
  * @brief  UART2 Receive complete callback
  * @param  huart
  * @retval None
  * @note	This function is called by the HAL library
  * 		when a character is arrived.
  */
static void UART2_RxCpltCallback(UART_HandleTypeDef *huart)
{
	portBASE_TYPE higher_priority_task_woken;
	xSemaphoreGiveFromISR(uart_rx_complete_semaphore_handle, &higher_priority_task_woken);

	portYIELD_FROM_ISR(higher_priority_task_woken);
}

/**
  * @brief  Low-level log function (used by log_info, log_warning and log_error)
  * @param  format, string with optional formatspecifiers
  * @param	type, string defines the log type (INFO, WARNING, ERROR)
  * @param  va, list containing arguments defined by format
  * @retval len, total length of the log message string
  * @note	This function might cause the calling task to go to the blocked state
  * 		if there is no free space in the queue
  */
uint32_t log_(const char * format, const char * type, va_list va)
{
	uint8_t hours;
	uint8_t minutes;
	uint8_t seconds;

	uart_tx_data_t data = {
		.pbuf = NULL,
		.size = 0,
	};

	if ( pdPASS == xQueueReceive(uart_tx_available_queue_handle, &data.pbuf, portMAX_DELAY) ) {

		RTC_GetTime(&hours, &minutes, &seconds);

		int len = snprintf((char *)data.pbuf, 2048, "[%02d:%02d:%02d] %s: ", hours, minutes, seconds, type);
		assert_param(len < 2048);

		data.size = (uint8_t)len + (uint8_t)vsnprintf((char *)(data.pbuf+len), 2048-len, format, va);
		assert_param(data.size <= 2048);

		xQueueSend(uart_tx_ready_queue_handle, &data, portMAX_DELAY);
	}

	return (int)data.size;
}

void cli_io_read(uint8_t *buf, uint16_t size)
{
	HAL_StatusTypeDef ret = HAL_UART_Receive_IT(&huart2, buf, size);
	assert_param(HAL_OK == ret);
}

void cli_io_write(const char * s, uint16_t size)
{
	uart_tx_data_t data = {
		.pbuf = NULL,
		.size = 0,
	};

	if ( pdPASS == xQueueReceive(uart_tx_available_queue_handle, &data.pbuf, portMAX_DELAY) ) {

		memcpy(data.pbuf, s, size);
		data.size = size;

		xQueueSend(uart_tx_ready_queue_handle, &data, portMAX_DELAY);
	}
}

bool is_character_received(void)
{
	bool ret;

	if (xSemaphoreTake( uart_rx_complete_semaphore_handle, portMAX_DELAY ) == pdPASS ) {
		ret = true;
	} else {
		ret = false;
	}

	return ret;
}



