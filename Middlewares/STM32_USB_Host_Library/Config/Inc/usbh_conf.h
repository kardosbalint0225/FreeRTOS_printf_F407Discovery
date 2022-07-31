/**
  ******************************************************************************
  * @file           : Target/usbh_conf.h
  * @brief          : Header for usbh_conf.c file.
  ******************************************************************************
  *
  *
  ******************************************************************************
  */
#ifndef __USBH_CONF__H__
#define __USBH_CONF__H__
#ifdef __cplusplus
 extern "C" {
#endif

//#include <stdio.h>    /**< !!! */
//#include <stdlib.h>   /**< !!! */
#include <string.h>
#include "main.h"

#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"

/*----------   -----------*/
#define USBH_MAX_NUM_ENDPOINTS           2U

/*----------   -----------*/
#define USBH_MAX_NUM_INTERFACES          2U

/*----------   -----------*/
#define USBH_MAX_NUM_CONFIGURATION       1U

/*----------   -----------*/
#define USBH_KEEP_CFG_DESCRIPTOR         1U

/*----------   -----------*/
#define USBH_MAX_NUM_SUPPORTED_CLASS     1U

/*----------   -----------*/
#define USBH_MAX_SIZE_CONFIGURATION      256U

/*----------   -----------*/
#define USBH_MAX_DATA_BUFFER             512U

/*----------   -----------*/
#define USBH_DEBUG_LEVEL                 0U

/*----------   -----------*/
#define USBH_USE_OS                      1U

/**< ************************************** */
#define USBH_USE_FREERTOS                1U
#define USBH_USE_FREERTOS_DYNAMIC_ALLOC  1U
#define USBH_USE_FREERTOS_STATIC_ALLOC   0U

/****************************************/
/* #define for FS and HS identification */
#define HOST_HS 		                 0U
#define HOST_FS 		                 1U

#if (USBH_USE_OS == 1)
  #if (USBH_USE_FREERTOS == 1U)
    #include "FreeRTOS.h"
    #include "task.h"
    #include "timers.h"
    #include "queue.h"
    #include "semphr.h"
    #include "event_groups.h"
    #define USBH_PROCESS_PRIO          4
    #define USBH_PROCESS_STACK_SIZE    ((uint16_t)128)
  #else
    #include "cmsis_os.h"
    #define USBH_PROCESS_PRIO          osPriorityNormal
    #define USBH_PROCESS_STACK_SIZE    ((uint16_t)128)
  #endif
#endif /* (USBH_USE_OS == 1) */

/* Memory management macros */
/** Alias for memory allocation. */
#define USBH_malloc         pvPortMalloc /**< !!! */

/** Alias for memory release. */
#define USBH_free           vPortFree    /**< !!! */

/** Alias for memory set. */
#define USBH_memset         memset

/** Alias for memory copy. */
#define USBH_memcpy         memcpy

/* DEBUG macros */

#if (USBH_DEBUG_LEVEL > 0U)
#define  USBH_UsrLog(...)   do { \
                            printf(__VA_ARGS__); \
                            printf("\n"); \
} while (0)
#else
#define USBH_UsrLog(...) do {} while (0)
#endif

#if (USBH_DEBUG_LEVEL > 1U)

#define  USBH_ErrLog(...) do { \
                            printf("ERROR: ") ; \
                            printf(__VA_ARGS__); \
                            printf("\n"); \
} while (0)
#else
#define USBH_ErrLog(...) do {} while (0)
#endif

#if (USBH_DEBUG_LEVEL > 2U)
#define  USBH_DbgLog(...)   do { \
                            printf("DEBUG : ") ; \
                            printf(__VA_ARGS__); \
                            printf("\n"); \
} while (0)
#else
#define USBH_DbgLog(...) do {} while (0)
#endif


#ifdef __cplusplus
}
#endif

#endif /* __USBH_CONF__H__ */



