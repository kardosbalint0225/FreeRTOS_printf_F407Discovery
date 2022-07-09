/*
 * log.c
 *
 *  Created on: 2022. jul. 8.
 *      Author: Balint
 */
#include "log.h"
#include "rtc.h"
#include "printf.h"
#include "stm32f4xx_hal.h"
#include "usart.h"

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"

typedef struct {
	uint8_t *pbuf;
	uint8_t  size;
} uart_tx_data_t;

extern UART_HandleTypeDef huart2;

static SemaphoreHandle_t uart_tx_complete_semaphore = NULL;
QueueHandle_t uart_tx_available_queue;
QueueHandle_t uart_tx_ready_queue;
QueueHandle_t uart_tx_pending_queue;

uint8_t uart_tx_buffer[128*8];

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	portBASE_TYPE higher_priority_task_woken;
	xSemaphoreGiveFromISR(uart_tx_complete_semaphore, &higher_priority_task_woken);

	portYIELD_FROM_ISR(higher_priority_task_woken);
}

void uart_write_task(void *params)
{
	vSemaphoreCreateBinary(uart_tx_complete_semaphore);
	configASSERT(uart_tx_complete_semaphore);

	uart_tx_pending_queue = xQueueCreate(1, sizeof(uint8_t *));

	uart_tx_data_t uart_tx_data = {
		.pbuf = NULL,
		.size = 0,
	};

	for ( ;; )
	{
		if (pdPASS == xSemaphoreTake(uart_tx_complete_semaphore, portMAX_DELAY)) {
			uint8_t *released = NULL;

			xQueueReceive(uart_tx_pending_queue, &released, 0);
			xQueueSend(uart_tx_available_queue, &released, 0);

			if (pdPASS == xQueueReceive(uart_tx_ready_queue, &uart_tx_data, portMAX_DELAY)) {
				HAL_UART_Transmit_DMA(&huart2, uart_tx_data.pbuf, uart_tx_data.size);
				xQueueSend(uart_tx_pending_queue, &uart_tx_data.pbuf, 0);
			}
		}
	}
}

uint32_t log_init(void)
{
	RTC_Init();
	UART2_Init();

	uart_tx_available_queue  = xQueueCreate(8, sizeof(uint8_t *));
	uart_tx_ready_queue      = xQueueCreate(8, sizeof(uart_tx_data_t));

	for (uint8_t i = 0; i < 8; i++) {
		uint8_t *buffer_address = &uart_tx_buffer[i*128];
		xQueueSend(uart_tx_available_queue, &buffer_address, 0);
	}

	xTaskCreate(
				uart_write_task,
				"uart write",
				( configMINIMAL_STACK_SIZE * 3 ),
				NULL,
				tskIDLE_PRIORITY+1,
				NULL
				);
}

uint32_t log_deinit(void)
{

}

uint32_t log_info(const char * s, ...)
{
	uint8_t hours;
	uint8_t minutes;
	uint8_t seconds;
	uint8_t len = 0;
	va_list va;

	uart_tx_data_t data = {
		.pbuf = NULL,
		.size = 0,
	};

	va_start(va, s);

	if ( pdPASS == xQueueReceive(uart_tx_available_queue, &data.pbuf, portMAX_DELAY) ) {

		RTC_GetTime(&hours, &minutes, &seconds);

		len += (uint8_t)snprintf((char *)data.pbuf, 128, "[%02d:%02d:%02d] INFO: ", hours, minutes, seconds);
		data.size = len + (uint8_t)vsnprintf((char *)(data.pbuf+len), 128-len, s, va);

		xQueueSend(uart_tx_ready_queue, &data, portMAX_DELAY);
	}

	va_end(va);

	return (uint32_t)len;
}

uint32_t log_warning(const char * s, ...)
{
	uint8_t hours;
	uint8_t minutes;
	uint8_t seconds;
	uint8_t len = 0;
	va_list va;

	uart_tx_data_t data = {
		.pbuf = NULL,
		.size = 0,
	};

	va_start(va, s);

	if ( pdPASS == xQueueReceive(uart_tx_available_queue, &data.pbuf, portMAX_DELAY) ) {

		RTC_GetTime(&hours, &minutes, &seconds);

		len += (uint8_t)snprintf((char *)data.pbuf, 128, "[%02d:%02d:%02d] WARNING: ", hours, minutes, seconds);
		data.size = len + (uint8_t)vsnprintf((char *)(data.pbuf+len), 128-len, s, va);

		xQueueSend(uart_tx_ready_queue, &data, portMAX_DELAY);
	}

	va_end(va);

	return (uint32_t)len;
}

uint32_t log_error(const char * s, ...)
{
	uint8_t hours;
	uint8_t minutes;
	uint8_t seconds;
	uint8_t len = 0;
	va_list va;

	uart_tx_data_t data = {
		.pbuf = NULL,
		.size = 0,
	};

	va_start(va, s);

	if ( pdPASS == xQueueReceive(uart_tx_available_queue, &data.pbuf, portMAX_DELAY) ) {

		RTC_GetTime(&hours, &minutes, &seconds);

		len += (uint8_t)snprintf((char *)data.pbuf, 128, "[%02d:%02d:%02d] ERROR: ", hours, minutes, seconds);
		data.size = len + (uint8_t)vsnprintf((char *)(data.pbuf+len), 128-len, s, va);

		xQueueSend(uart_tx_ready_queue, &data, portMAX_DELAY);
	}

	va_end(va);

	return (uint32_t)len;
}

uint32_t echo_back(const char c)
{
	uart_tx_data_t data = {
		.pbuf = NULL,
		.size = 1,
	};

	if ( pdPASS == xQueueReceive(uart_tx_available_queue, &data.pbuf, portMAX_DELAY) ) {
		*data.pbuf = c;
		xQueueSend(uart_tx_ready_queue, &data, portMAX_DELAY);
	}

	return (uint32_t)1;
}


