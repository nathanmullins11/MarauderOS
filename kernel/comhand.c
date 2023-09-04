#include "mpx/device.h"
#include "mpx/serial.h"
#include <comhand.h>
#include <sys_req.h>
#include <itoa.h>
#include <string.h>
#include <version.h>
#include <time.h>
#include <help.h>
#include <mpx/io.h>

void comhand(void)
{
	// print welcoming message
	char msg[] = "Welcome to MarauderOS | Use 'help' command to see list of commands\n";
	sys_req(WRITE, COM1, msg, sizeof(msg));

	// loop forever until shutdown
    for ( ;; ) 
    {
		// create buffer to hold user input and read using READ op-code
    	char buf[100] = {0};
        int size_buffer = sys_req(READ, COM1, buf, sizeof(buf));

		// check if the buffer is ended with a null terminator before evaluating content
		if (buf[size_buffer] == '\0') {
			if ( strcmp("version", buf) == 0 ) {
				// execute verion command
				version();
			} else if ( strcmp("shutdown", buf) == 0 ) {
				// execute shutdown (return to kmain.c)
				return;
			} else if ( strcmp("getdate", buf) == 0 ) {
				get_date();
			} else if ( strcmp("help", buf) <= 0 ) {
				// execute help command
				// get the argument from the buffer
				char argument[95]; 
				memset(argument, 0, sizeof(argument)); 
				memcpy(argument, buf + 5, size_buffer - 5);
				
				// pass argument to help command
				help(argument);
			} else {
				// command not recognized 
				char error_msg[] = "ERR: Invalid Command\n";
				sys_req(WRITE, COM1, error_msg, strlen(error_msg));
			}

		}
    }
}
