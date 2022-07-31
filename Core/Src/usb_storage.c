/*
 * usb_storage.c
 *
 *  Created on: 2022. jul. 29.
 *      Author: Balint
 */
#include "usb_storage.h"
#include <stdint.h>

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"

#include "usb_host.h"

static uint32_t is_mounted = 0;

void usb_storage_mount_task(void *params)
{
	(void)params;

	for ( ;; )
	{

	}
}
