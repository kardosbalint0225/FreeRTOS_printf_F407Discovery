/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  *
  *
  ******************************************************************************
  */
#include "stm32f4xx_hal.h"
#include "gpio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"

#include "log.h"
#include "cli.h"
#include "FreeRTOS_CLI.h"
#include "SEGGER_SYSVIEW.h"
#include "fatfs.h"
#include "usb_host.h"

void SystemClock_Config(void);

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
	HAL_Init();
	SystemClock_Config();
	SEGGER_SYSVIEW_Conf();

	GPIO_Init();

	MX_FATFS_Init();

	log_init();
	cli_init(FreeRTOS_CLIGetOutputBuffer(), FreeRTOS_CLIProcessCommand);

	vTaskStartScheduler();

	while (1)
	{
	}
}



