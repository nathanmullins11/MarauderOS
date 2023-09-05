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
			
	char *command;

	// loop forever until shutdown
    for ( ;; ) 
    {
		// create buffer to hold user input and read using READ op-code
    	char buf[100] = {0};
        int size_buffer = sys_req(READ, COM1, buf, sizeof(buf));

		// check if the buffer is ended with a null terminator before evaluating content
		if (buf[size_buffer] == '\0') {

			// get the user's command from input
			command = strtok(buf, " ");

			// if there is a command, start comparing
			if (command) {
				if ( strcmp(command, "version") == 0 ) {
					// run the version command
                	version();
				} else if ( strcmp(command, "shutdown") == 0 ) {
					// return to kmain() to shutdown OS
                	return;
				} else if ( strcmp(command, "help") == 0 ) {
					// get parameters from the buffer
                	char *param = strtok(NULL, " ");

					// pass parameter to help command
					if (param) {
                    	help(param);
					} else {
						help(" ");
					}
				} else if ( strcmp(command, "getdate") == 0 ) {
					// run the get date command
					get_date();
				} else if ( strcmp(command, "setdate") == 0 ) {
					// run the set date command

				} else if ( strcmp(command, "settime") == 0 ) {
					// run the set time command
					
				} else if ( strcmp(command, "gettime") == 0 ) {
					// run the get time command
					
				} else {
					// command not recognized
					char error_msg[] = "ERR: Invalid Command\n";
					sys_req(WRITE, COM1, error_msg, strlen(error_msg));
				}
			}

		}
    }
}
