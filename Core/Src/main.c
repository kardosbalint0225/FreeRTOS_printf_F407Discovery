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

extern uint32_t uwTickPrio;

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
//		echo_back('>');
//		vTaskDelay(pdMS_TO_TICKS(100));
//		echo_back('e');
//		vTaskDelay(pdMS_TO_TICKS(100));
//		echo_back('c');
//		vTaskDelay(pdMS_TO_TICKS(100));
//		echo_back('h');
//		vTaskDelay(pdMS_TO_TICKS(100));
//		echo_back('o');
//		vTaskDelay(pdMS_TO_TICKS(100));
//		echo_back('\r');
//		vTaskDelay(pdMS_TO_TICKS(100));
//		echo_back('\n');
//		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

void task_e(void *params)
{
	for ( ;; )
	{
		vTaskDelay(pdMS_TO_TICKS(60000));
		assert_param(0);
	}
}


/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
	uwTickPrio = TICK_INT_PRIORITY; 	/**< bugfix: stm32f4xx_hal.c initializes this value to 16 */
	 	 	 	 	 	 	 	 	 	/**< 	 assert_param will fail */
	HAL_Init();
	SystemClock_Config();
	SEGGER_SYSVIEW_Conf();

	GPIO_Init();

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

	xTaskCreate(
				task_e,
				"task_e",
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



