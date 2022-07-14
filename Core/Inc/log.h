/*
 * log.h
 *
 *  Created on: 2022. jul. 8.
 *      Author: Balint
 */

#ifndef INC_LOG_H_
#define INC_LOG_H_

#include <stdint.h>
#include <stdarg.h>

void log_init(void);
void log_deinit(void);
uint32_t log_info(const char * s, ...);
uint32_t log_warning(const char * s, ...);
uint32_t log_error(const char * s, ...);

#endif /* INC_LOG_H_ */


