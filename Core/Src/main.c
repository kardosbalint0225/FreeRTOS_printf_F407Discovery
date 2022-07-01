/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  *
  *
  ******************************************************************************
  */


#include "main.h"
#include "dma.h"
#include "usart.h"
#include "gpio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"

#include "printf.h"

#include "SEGGER_SYSVIEW.h"

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

void SystemClock_Config(void);


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


void task_a(void *params)
{
	for ( ;; )
	{
		uart_tx_data_t data = {
			.pbuf = NULL,
			.size = 0,
		};

		if ( pdPASS == xQueueReceive(uart_tx_available_queue, &data.pbuf, portMAX_DELAY) ) {
			data.size = (uint8_t)snprintf((char *)data.pbuf, 128, "I am Task A. -> %d.)\r\n", 1);
			xQueueSend(uart_tx_ready_queue, &data, portMAX_DELAY);
		}
	}
}

void task_b(void *params)
{
	for ( ;; )
	{
		uart_tx_data_t data = {
			.pbuf = NULL,
			.size = 0,
		};

		if ( pdPASS == xQueueReceive(uart_tx_available_queue, &data.pbuf, portMAX_DELAY) ) {
			data.size = (uint8_t)snprintf((char *)data.pbuf, 128, "I am Task B. -> %d.)\r\n", 2);
			xQueueSend(uart_tx_ready_queue, &data, portMAX_DELAY);
		}
	}
}

void task_c(void *params)
{
	for ( ;; )
	{
		uart_tx_data_t data = {
			.pbuf = NULL,
			.size = 0,
		};

		if ( pdPASS == xQueueReceive(uart_tx_available_queue, &data.pbuf, portMAX_DELAY) ) {
			data.size = (uint8_t)snprintf((char *)data.pbuf, 128, "I am Task C. -> %d.)\r\n", 3);
			xQueueSend(uart_tx_ready_queue, &data, portMAX_DELAY);
		}
	}
}

void task_d(void *params)
{
	for ( ;; )
	{
		uart_tx_data_t data = {
			.pbuf = NULL,
			.size = 0,
		};

		if ( pdPASS == xQueueReceive(uart_tx_available_queue, &data.pbuf, portMAX_DELAY) ) {
			data.size = (uint8_t)snprintf((char *)data.pbuf, 128, "I am Task D. -> %d.)\r\n", 4);
			xQueueSend(uart_tx_ready_queue, &data, portMAX_DELAY);
		}
	}
}

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  HAL_Init();
  SystemClock_Config();
  SEGGER_SYSVIEW_Conf();

  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART2_UART_Init();

  uart_tx_available_queue  = xQueueCreate(8, sizeof(uint8_t *));
  uart_tx_ready_queue = xQueueCreate(8, sizeof(uart_tx_data_t));


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
/*
  xTaskCreate(
		  	  task_a,
  			  "Task A",
  			  ( configMINIMAL_STACK_SIZE * 3 ),
  			  NULL,
			  tskIDLE_PRIORITY,
  			  NULL
  		  	  );

  xTaskCreate(
  		  	  task_b,
  			  "Task B",
  			  ( configMINIMAL_STACK_SIZE * 3 ),
  			  NULL,
  			  tskIDLE_PRIORITY,
  			  NULL
  		  	  );

  xTaskCreate(
			  task_c,
			  "Task C",
			  ( configMINIMAL_STACK_SIZE * 3 ),
			  NULL,
			  tskIDLE_PRIORITY,
			  NULL
			  );

  xTaskCreate(
			  task_d,
			  "Task D",
			  ( configMINIMAL_STACK_SIZE * 3 ),
			  NULL,
			  tskIDLE_PRIORITY,
			  NULL
			  );
*/
  vTaskStartScheduler();



  while (1)
  {
  }
}


/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
