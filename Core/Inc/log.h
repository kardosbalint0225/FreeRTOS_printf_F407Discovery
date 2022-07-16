/*
 * log.h
 *
 *  Created on: 2022. jul. 8.
 *      Author: Balint
 */

#ifndef INC_LOG_H_
#define INC_LOG_H_

#include <stdarg.h>

void log_init(void);
void log_deinit(void);
int  log_info(const char * s, ...);
int  log_warning(const char * s, ...);
int  log_error(const char * s, ...);

#endif /* INC_LOG_H_ */


