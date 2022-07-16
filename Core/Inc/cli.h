#ifndef CLI_IO_H
#define CLI_IO_H

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

typedef BaseType_t ( *cli_callback_t )( const char * const cli_input, char * cli_output, size_t cli_output_size );

/*
 * Create the task that implements a command console using the USB virtual com
 * port driver for input and output.
 */
void cli_init( char *cli_output_buffer, cli_callback_t cli_callback );

#endif /* CLI_IO_H */



