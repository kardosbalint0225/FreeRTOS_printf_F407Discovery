/*
 * log.c
 *
 *  Created on: 2022. jul. 8.
 *      Author: Balint
 */
#include "log.h"
#include "log_and_cli_io.h"

void log_init(void)
{
	log_and_cli_io_init();
}

void log_deinit(void)
{
	log_and_cli_io_deinit();
}

int log_info(const char * s, ...)
{
	va_list va;
	va_start(va, s);
	int len = log_(s, "INFO", va);
	va_end(va);
	return len;
}

int log_warning(const char * s, ...)
{
	va_list va;
	va_start(va, s);
	int len = log_(s, "WARNING", va);
	va_end(va);
	return len;
}

int log_error(const char * s, ...)
{
	va_list va;
	va_start(va, s);
	int len = log_(s, "ERROR", va);
	va_end(va);
	return len;
}




