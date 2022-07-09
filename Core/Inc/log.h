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

uint32_t log_init(void);
uint32_t log_deinit(void);
uint32_t log_info(const char * s, ...);
uint32_t log_warning(const char * s, ...);
uint32_t log_error(const char * s, ...);
uint32_t echo_back(const char c);


#endif /* INC_LOG_H_ */


