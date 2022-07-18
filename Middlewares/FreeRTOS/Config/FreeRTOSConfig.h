#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * These parameters and more are described within the 'configuration' section of the
 * FreeRTOS API documentation available on the FreeRTOS.org web site.
 *
 * See http://www.freertos.org/a00110.html
 *----------------------------------------------------------*/

#include <stdint.h>
#include "stm32f4xx_hal.h"

extern uint32_t SystemCoreClock;

#define configENABLE_FPU                              ( 0 )
#define configENABLE_MPU                              ( 0 )

#define configUSE_PREEMPTION                          ( 1 )                 
#define configUSE_PORT_OPTIMISED_TASK_SELECTION       ( 1 )                 
#define configUSE_TICKLESS_IDLE                       ( 0 )                 
#define configCPU_CLOCK_HZ                            ( SystemCoreClock )   
//#define configSYSTICK_CLOCK_HZ                        configCPU_CLOCK_HZ
#define configTICK_RATE_HZ                            ( (TickType_t)1000 )  
#define configMAX_PRIORITIES                          ( 7 )                 
#define configMINIMAL_STACK_SIZE                      ( (uint16_t)128 )     
#define configMAX_TASK_NAME_LEN                       ( 16 )                
#define configUSE_16_BIT_TICKS                        ( 0 )
#define configIDLE_SHOULD_YIELD                       ( 1 )
#define configUSE_TASK_NOTIFICATIONS                  ( 1 )
#define configTASK_NOTIFICATION_ARRAY_ENTRIES         ( 1 )
#define configUSE_MUTEXES                             ( 1 )
#define configUSE_RECURSIVE_MUTEXES                   ( 1 )
#define configUSE_COUNTING_SEMAPHORES                 ( 1 )
#define configUSE_ALTERNATIVE_API                     ( 0 ) /* Deprecated! */
#define configQUEUE_REGISTRY_SIZE                     ( 8 )
#define configUSE_QUEUE_SETS                          ( 0 )	/**< Build fails. SystemView related issue */
#define configUSE_TIME_SLICING                        ( 1 )
#define configUSE_NEWLIB_REENTRANT                    ( 0 )
#define configENABLE_BACKWARD_COMPATIBILITY           ( 0 )
#define configNUM_THREAD_LOCAL_STORAGE_POINTERS       ( 5 )
#define configUSE_APPLICATION_TASK_TAG                ( 1 )
#define configSTACK_DEPTH_TYPE                        uint16_t
#define configMESSAGE_BUFFER_LENGTH_TYPE              size_t

/* Memory allocation related definitions. */
#define configSUPPORT_STATIC_ALLOCATION               ( 1 )
#define configSUPPORT_DYNAMIC_ALLOCATION              ( 1 )
#define configTOTAL_HEAP_SIZE                         ( (size_t)15360 )
#define configAPPLICATION_ALLOCATED_HEAP              ( 0 )
#define configSTACK_ALLOCATION_FROM_SEPARATE_HEAP     ( 0 )

/* Hook function related definitions. */
#define configUSE_IDLE_HOOK                           ( 0 )
#define configUSE_TICK_HOOK                           ( 0 )
#define configCHECK_FOR_STACK_OVERFLOW                ( 2 )
#define configUSE_MALLOC_FAILED_HOOK                  ( 1 )
#define configUSE_DAEMON_TASK_STARTUP_HOOK            ( 1 )

/* Run time and task stats gathering related definitions. */
#define configGENERATE_RUN_TIME_STATS                 ( 1 )
#define configUSE_TRACE_FACILITY                      ( 1 )
#define configUSE_STATS_FORMATTING_FUNCTIONS          ( 1 )

/* Co-routine related definitions. */
#define configUSE_CO_ROUTINES                         ( 0 )
#define configMAX_CO_ROUTINE_PRIORITIES               ( 1 )

/* Software timer related definitions. */
#define configUSE_TIMERS                              ( 1 )
#define configTIMER_TASK_PRIORITY                     ( 3 )
#define configTIMER_QUEUE_LENGTH                      ( 10 )
#define configTIMER_TASK_STACK_DEPTH                  ( configMINIMAL_STACK_SIZE * 2)


/* Interrupt nesting behaviour configuration. */

/* Cortex-M specific definitions. */
#ifdef __NVIC_PRIO_BITS
    /* __NVIC_PRIO_BITS will be specified when CMSIS is being used. */
    #define configPRIO_BITS                           __NVIC_PRIO_BITS
#else
    #define configPRIO_BITS                           ( 4 )
#endif

/* The lowest interrupt priority that can be used in a call to a "set priority"
 * function. */
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY       ( 15 )

/* The highest interrupt priority that can be used by any interrupt service
 * routine that makes calls to interrupt safe FreeRTOS API functions.  
 * DO NOT CALL INTERRUPT SAFE FREERTOS API FUNCTIONS FROM ANY INTERRUPT THAT HAS A HIGHER
 * PRIORITY THAN THIS! (higher priorities are lower numeric values. */
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY  ( 5 )

/* Interrupt priorities used by the kernel port layer itself.  These are generic
 * to all Cortex-M ports, and do not rely on any particular library functions. */
#define configKERNEL_INTERRUPT_PRIORITY 		      ( configLIBRARY_LOWEST_INTERRUPT_PRIORITY      << (8 - configPRIO_BITS) )

/* !!!! configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to zero !!!!
 * See http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html. */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY 	      ( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )

/* configMAX_API_CALL_INTERRUPT_PRIORITY is a new name for configMAX_SYSCALL_INTERRUPT_PRIORITY 
 * that is used by newer ports only. The two are equivalent. */
//#define configMAX_API_CALL_INTERRUPT_PRIORITY         [dependent on processor and application]

/* Definitions that map the FreeRTOS port interrupt handlers to their CMSIS
 * standard names. */
#define vPortSVCHandler                               SVC_Handler
#define xPortPendSVHandler                            PendSV_Handler
#define xPortSysTickHandler                           SysTick_Handler

/* Define to trap errors during development. */
#define configASSERT( x )                         	  assert_param( ( x ) )

/* FreeRTOS MPU specific definitions. */
#define configINCLUDE_APPLICATION_DEFINED_PRIVILEGED_FUNCTIONS ( 0 )
#define configTOTAL_MPU_REGIONS                       ( 8 )      /* Default value. */
#define configTEX_S_C_B_FLASH                         ( 0x07UL ) /* Default value. */
#define configTEX_S_C_B_SRAM                          ( 0x07UL ) /* Default value. */
#define configENFORCE_SYSTEM_CALLS_FROM_KERNEL_ONLY   ( 1 )
#define configALLOW_UNPRIVILEGED_CRITICAL_SECTIONS    ( 0 )

/* ARMv8-M secure side port related definitions. */
#define secureconfigMAX_SECURE_CONTEXTS               ( 5 )

/* Optional functions - most linkers will remove unused functions anyway. */
#define INCLUDE_vTaskPrioritySet                      ( 1 )
#define INCLUDE_uxTaskPriorityGet                     ( 1 )
#define INCLUDE_vTaskDelete                           ( 1 )
#define INCLUDE_vTaskSuspend                          ( 1 )
#define INCLUDE_xResumeFromISR                        ( 1 )
#define INCLUDE_vTaskDelayUntil                       ( 1 )
#define INCLUDE_vTaskDelay                            ( 1 )
#define INCLUDE_xTaskGetSchedulerState                ( 1 )
#define INCLUDE_xTaskGetCurrentTaskHandle             ( 1 )
#define INCLUDE_uxTaskGetStackHighWaterMark           ( 1 )
#define INCLUDE_xTaskGetIdleTaskHandle                ( 1 )
#define INCLUDE_eTaskGetState                         ( 1 )
#define INCLUDE_xEventGroupSetBitFromISR              ( 1 )
#define INCLUDE_xTimerPendFunctionCall                ( 1 )
#define INCLUDE_xTaskAbortDelay                       ( 1 )
#define INCLUDE_xTaskGetHandle                        ( 1 )
#define INCLUDE_xTaskResumeFromISR                    ( 1 )
#define INCLUDE_vTaskCleanUpResources                 ( 1 )

/**< FreeRTOS + CLI */
#define configCOMMAND_INT_MAX_OUTPUT_SIZE 			  ( 2048 )

/**< Run-time statistics */
extern void vConfigureTimerForRunTimeStats( void );
extern volatile uint32_t runtime_stats_timer;

#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS() 	  vConfigureTimerForRunTimeStats()
#define portGET_RUN_TIME_COUNTER_VALUE()         	  ( runtime_stats_timer )

/**< SystemView related defines */
#include "SEGGER_SYSVIEW_FreeRTOS.h"
#define INCLUDE_pxTaskGetStackStart                   ( 1 )

#endif /* FREERTOS_CONFIG_H */




