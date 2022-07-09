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

#include "log.h"

#include "SEGGER_SYSVIEW.h"

void SystemClock_Config(void);


void task_a(void *params)
{
	for ( ;; )
	{
		log_info("Test log with arguments: %d, %s\r\n", 1234, "info");
		vTaskDelay(pdMS_TO_TICKS(2000));
	}
}

void task_b(void *params)
{
	for ( ;; )
	{
		log_warning("Test log with arguments: %d, %s\r\n", 5678, "warning");
		vTaskDelay(pdMS_TO_TICKS(5000));
	}
}

void task_c(void *params)
{
	for ( ;; )
	{
		log_error("Test log with arguments: %d, %s\r\n", 123456789, "error");
		vTaskDelay(pdMS_TO_TICKS(10000));
	}
}

void task_d(void *params)
{
	for ( ;; )
	{
		echo_back('>');
		vTaskDelay(pdMS_TO_TICKS(100));
		echo_back('e');
		vTaskDelay(pdMS_TO_TICKS(100));
		echo_back('c');
		vTaskDelay(pdMS_TO_TICKS(100));
		echo_back('h');
		vTaskDelay(pdMS_TO_TICKS(100));
		echo_back('o');
		vTaskDelay(pdMS_TO_TICKS(100));
		echo_back('\r');
		vTaskDelay(pdMS_TO_TICKS(100));
		echo_back('\n');
		vTaskDelay(pdMS_TO_TICKS(1000));
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

	GPIO_Init();
	DMA_Init();

	log_init();

	xTaskCreate(
				task_a,
				"task_a",
				( configMINIMAL_STACK_SIZE * 3 ),
				NULL,
				tskIDLE_PRIORITY,
				NULL
				);

	xTaskCreate(
				task_b,
				"task_b",
				( configMINIMAL_STACK_SIZE * 3 ),
				NULL,
				tskIDLE_PRIORITY,
				NULL
				);

	xTaskCreate(
				task_c,
				"task_c",
				( configMINIMAL_STACK_SIZE * 3 ),
				NULL,
				tskIDLE_PRIORITY,
				NULL
				);

	xTaskCreate(
				task_d,
				"task_d",
				( configMINIMAL_STACK_SIZE * 3 ),
				NULL,
				tskIDLE_PRIORITY,
				NULL
				);

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
