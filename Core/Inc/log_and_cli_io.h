/*
 * log_and_cli_io.h
 *
 *  Created on: 2022. jul. 14.
 *      Author: Balint
 */

#ifndef INC_LOG_AND_CLI_IO_H_
#define INC_LOG_AND_CLI_IO_H_

#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>

void log_and_cli_io_init(void);
void log_and_cli_io_deinit(void);
int  log_(const char * format, const char * type, va_list va);
uint32_t cli_io_read(uint8_t *ch);
void cli_io_write(const char * s, uint16_t size);


#endif /* INC_LOG_AND_CLI_IO_H_ */
