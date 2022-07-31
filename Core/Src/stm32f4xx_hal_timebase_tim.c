/**
  ******************************************************************************
  * @file    stm32f4xx_hal_timebase_TIM.c
  * @brief   HAL time base based on the hardware TIM.
  ******************************************************************************
  *
  *
  ******************************************************************************
  */
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_tim.h"

TIM_HandleTypeDef htim7;

void TIM7_PeriodElapsedCallback(TIM_HandleTypeDef *htim);


/**
  * @brief  This function configures the TIM7 as a time base source.
  *         The time source is configured  to have 1ms time base with a dedicated
  *         Tick interrupt priority.
  * @note   This function is called  automatically at the beginning of program after
  *         reset by HAL_Init() or at any time when clock is configured, by HAL_RCC_ClockConfig().
  * @param  TickPriority: Tick interrupt priority.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
{
	RCC_ClkInitTypeDef    clkconfig;
	uint32_t              uwTimclock       = 0U;
	uint32_t              uwPrescalerValue = 0U;
	uint32_t              pFLatency;
	uint32_t			  uwAPB1Prescaler  = 0U;

	/*Configure the TIM7 IRQ priority */
	HAL_NVIC_SetPriority(TIM7_IRQn, TickPriority ,0);
	uwTickPrio = TickPriority;

	/* Enable the TIM7 global Interrupt */
	HAL_NVIC_EnableIRQ(TIM7_IRQn);

	/* Enable TIM7 clock */
	__HAL_RCC_TIM7_CLK_ENABLE();

	/* Get clock configuration */
	HAL_RCC_GetClockConfig(&clkconfig, &pFLatency);

	/* Get APB1 prescaler */
	uwAPB1Prescaler = clkconfig.APB1CLKDivider;

	/* Compute TIM7 clock */
	if (uwAPB1Prescaler == RCC_HCLK_DIV1) {
		uwTimclock = HAL_RCC_GetPCLK1Freq();
	} else {
		uwTimclock = 2UL * HAL_RCC_GetPCLK1Freq();
	}

	/* Compute the prescaler value to have TIM7 counter clock equal to 1MHz */
	uwPrescalerValue = (uint32_t) ((uwTimclock / 1000000U) - 1U);

	/* Initialize TIM7 */
	htim7.Instance = TIM7;

	/* Initialize TIMx peripheral as follow:
	+ Period = [(TIM7CLK/1000) - 1]. to have a (1/1000) s time base.
	+ Prescaler = (uwTimclock/1000000 - 1) to have a 1MHz counter clock.
	+ ClockDivision = 0
	+ Counter direction = Up
	*/
	htim7.Init.Period = (1000000U / 1000U) - 1U;
	htim7.Init.Prescaler = uwPrescalerValue;
	htim7.Init.ClockDivision = 0;
	htim7.Init.CounterMode = TIM_COUNTERMODE_UP;

	HAL_StatusTypeDef ret;
	ret = HAL_TIM_Base_Init(&htim7);
	assert_param(HAL_OK == ret);
	
	ret = HAL_TIM_RegisterCallback(&htim7, HAL_TIM_PERIOD_ELAPSED_CB_ID, TIM7_PeriodElapsedCallback);
	assert_param(HAL_OK == ret);

	/* Start the TIM time Base generation in interrupt mode */
	ret = HAL_TIM_Base_Start_IT(&htim7);
	assert_param(HAL_OK == ret);
	
	/* Return function status */
	return ret;
}

/**
  * @brief  Suspend Tick increment.
  * @note   Disable the tick increment by disabling TIM7 update interrupt.
  * @param  None
  * @retval None
  */
void HAL_SuspendTick(void)
{
	/* Disable TIM7 update Interrupt */
	__HAL_TIM_DISABLE_IT(&htim7, TIM_IT_UPDATE);
}

/**
  * @brief  Resume Tick increment.
  * @note   Enable the tick increment by Enabling TIM7 update interrupt.
  * @param  None
  * @retval None
  */
void HAL_ResumeTick(void)
{
	/* Enable TIM7 Update interrupt */
	__HAL_TIM_ENABLE_IT(&htim7, TIM_IT_UPDATE);
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM7 interrupt took place, inside
  * 		HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * 		a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void TIM7_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	HAL_IncTick();
}

//void HAL_Delay(uint32_t Delay)
//{
//	TIM7->CNT  = 0x0;
//	TIM7->CR1 |= 0x1;
//
//	uint32_t tickcount = 0;
//
//	while (tickcount < Delay) {
//		if ((uint32_t)999 == (uint32_t)(TIM7->CNT)) {
//			tickcount = tickcount + 1;
//			TIM7->CNT  = 0x0;
//		}
//	}
//
//	TIM7->CR1 &= ~(0x1);
//}

