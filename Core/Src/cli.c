/*
 *
 */

#include "cli.h"
#include "log_and_cli_io.h"
#include "stm32f4xx_hal.h"

#include <string.h>
#include <stdio.h>
#include <stdbool.h>

/* Dimensions the buffer into which input characters are placed. */
#define cmdMAX_INPUT_SIZE					50

/* DEL acts as a backspace. */
#define cmdASCII_DEL						( 0x7F )

extern void register_commands( void );
static void cli_task(void *pvParameters);
static bool is_end_of_line(char ch);
static void process_command();
static void process_input(char *received_char);

static const char * const welcome_message = "\r\n\r\nFreeRTOS command server.\r\nType Help to view a list of registered commands.\r\n\r\n>";
static const char * const end_message     = "\r\n[Press ENTER to execute the previous command again]\r\n>";
static const char * const new_line        = "\r\n";
static char cli_input_buffer[ cmdMAX_INPUT_SIZE ];
static char *cli_output_buffer = NULL;
static char last_input_string[ cmdMAX_INPUT_SIZE ];
static uint8_t input_index = 0;

#define CLI_TASK_STACKSIZE			1024
static StackType_t  cli_task_stack[CLI_TASK_STACKSIZE];
static StaticTask_t cli_task_tcb;
static TaskHandle_t cli_task_handle = NULL;

static cli_callback_t commandline_interpreter = NULL;

/**
  * @brief  Initializes the Commandline Interface
  * @param	output_buffer points to a global buffer where command
  * 		output string can be stored
  * @param  cli_callback function pointer points to the command line
  * 		interpreter function
  * @retval None
  * @note   This function will create the CLI task that implements
  * 		command line processing
  */
void cli_init(char *output_buffer, cli_callback_t cli_callback)
{
	register_commands();

	cli_output_buffer = output_buffer;
	assert_param(NULL != cli_output_buffer);

	commandline_interpreter = cli_callback;
	assert_param(NULL != cli_callback);

	cli_task_handle = xTaskCreateStatic(
						cli_task,			/* The task that implements the command console. */
						"CLI",				/* Text name assigned to the task.  This is just to assist debugging.  The kernel does not use this name itself. */
						CLI_TASK_STACKSIZE,	/* The size of the stack allocated to the task. */
						NULL,				/* The parameter is not used, so NULL is passed. */
						tskIDLE_PRIORITY,	/* The priority allocated to the task. */
						cli_task_stack,
						&cli_task_tcb);

	assert_param(NULL != cli_task_handle);
}

/**
  * @brief  Deinitializes the Commandline Interface
  * @param	None
  * @retval None
  * @note   This function will delete the CLI task that implements
  * 		command line processing
  */
void cli_deinit(void)
{
	vTaskDelete(cli_task_handle);
	cli_output_buffer 		= NULL;
	commandline_interpreter = NULL;
}

/**
  * @brief  The task that implements command line processing
  * @param	params optionally points to data which can be passed 
  * 		on task creation
  * @retval None
  */
static void cli_task(void * params)
{
	( void ) params;
	char received_char;

	/* Send the welcome message. */
	cli_io_write( welcome_message, strlen( welcome_message ) );

	for( ;; )
	{
		if( pdTRUE == (BaseType_t)cli_io_read((uint8_t *)&received_char)) {
			/* Echo the character back. */
			cli_io_write( &received_char, sizeof( received_char ) );

			/* Was it the end of the line? */
			if (true == is_end_of_line(received_char)) {
				process_command();				
			} else {
				process_input(&received_char);
			}
		}
	}
}

/**
  * @brief  Checks whether a character is an end of line character or not
  * @param	ch the character to be checked
  * @retval true if ch is \n or \r, false otherwise
  */
static bool is_end_of_line(char ch)
{
	bool retv;

	if (ch == '\n' || ch == '\r') {
		retv = true;
	} else {
		retv = false;
	}

	return retv;
}

/**
  * @brief  Processes the characters received on the CLI I/O
  * @param	received_char points to the received character
  * @retval None
  * @note   This function should only be called from the CLI task since it
  * 		uses multiple CLI related global variables and therefore it is 
  * 		not reentrant
  */
static void process_input(char *received_char)
{
	if ( *received_char == '\r' ) {
		
		/* Ignore the character. */

	} else if ( ( *received_char == '\b' ) || ( *received_char == cmdASCII_DEL ) ) {
		
		/* Backspace was pressed.  Erase the last character in the string - if any. */
		
		if ( input_index > 0 ) {
			input_index = input_index - 1;
			cli_input_buffer[ input_index ] = '\0';
		}

	} else {
		
		/* A character was entered.  Add it to the string entered so far.  
		When a \n is entered the complete string will be passed to the command interpreter. */

		if ( ( *received_char >= ' ' ) && ( *received_char <= '~' ) && ( input_index < cmdMAX_INPUT_SIZE ) ) {

			cli_input_buffer[ input_index ] = *received_char;
			input_index = input_index + 1;
		}
	}
}

/**
  * @brief  Processes the command string
  * @param	None
  * @retval None
  * @note   This function will pass the received command string to the
  * 		command line interpreter
  * @note   This function should only be called from the CLI task since it
  * 		uses multiple CLI related global variables and therefore it is 
  * 		not reentrant
  */
static void process_command(void)
{
	/* Just to space the output from the input. */
	cli_io_write( new_line, strlen( new_line ) );

	/* See if the command is empty, indicating that the last command is	to be executed again. */
	if( input_index == 0 ) {					
		strcpy( cli_input_buffer, last_input_string ); /* Copy the last command back into the input string. */
	}

	/* Pass the received command to the command interpreter.  The command interpreter is called repeatedly until it returns pdFALSE
	(indicating there is no more output) as it might generate more than	one string. */

	portBASE_TYPE xReturned;
	
	do {
		/* Get the next output string from the command interpreter. */
		xReturned = commandline_interpreter( cli_input_buffer, cli_output_buffer, configCOMMAND_INT_MAX_OUTPUT_SIZE );
	
		/* Write the generated string to the UART. */
		cli_io_write( cli_output_buffer, strlen( cli_output_buffer ) );
	
	} while( xReturned != pdFALSE );

	/* All the strings generated by the input command have been sent. Clear the input string ready to receive the next command.
	 * Remember	the command that was just processed first in case it is to be processed again. */
	
	strcpy( last_input_string, cli_input_buffer );
	input_index = 0;
	memset( cli_input_buffer, 0x00, cmdMAX_INPUT_SIZE );

	cli_io_write( end_message, strlen( end_message ) );
}



