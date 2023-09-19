#include "mpx/device.h"
#include "mpx/serial.h"
#include <comhand.h>
#include <stdint.h>
#include <stdlib.h>
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

	// secondary welcome message
	char picture_msg[] =
	"         __  __                                 _                ____    _____\n"
	"	|  |/  |                               | |              / __ |  / ____|\n"
	"	| |  / |  __ _  _ __   __ _  _   _   __| |  ___  _ __  | |  | || (___  \n"
	"	| |||| | / _` || '__| / _` || | | | / _` | / _ ||  __| | |  | | |___ | \n"
	"	| |  | || (_| || |   | (_| || |_| || (_| ||  __/| |    | |__| | ____) |\n"
	"	|_|  |_| |__,_||_|    |__,_| |__,_| |_,_| |___| |_|     |____/ |_____/ \n"
    "                                                                   \n"
    "                                                                   \n";

	sys_req(WRITE, COM1, picture_msg, sizeof(picture_msg));
	sys_req(WRITE, COM1, msg, sizeof(msg));

	// error messages
	char error_msg_inc_param[] = "ERR: Invalid parameter | use `help` command\n";
	char error_msg_no_param[] = "ERR: command needs parameter | run `help` command\n";
	char error_msg_empty_param[] = "ERR: empty parameter | run `help` command\n";
	char error_msg_inc_flag[] = "ERR: incorrect flag | run `help` command\n";
	char error_msg_no_flag[] = "ERR: no flag provided | run `help` command\n";
			
	// pointer to store command from user input
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
				/* VERSION */
				if ( strcmp(command, "version") == 0 ) {
					// check if there are any invalid params
					char *param = strtok(NULL, " ");
					if (param) {
						// invalid parameter
						sys_req(WRITE, COM1, error_msg_inc_param, strlen(error_msg_inc_param));
					} else {
						// run the version command
                		version();
					}
				}

				/* SHUTDOWN */
				else if ( strcmp(command, "shutdown") == 0 ) {
					// check if there are any invalid params
					char *param = strtok(NULL, " ");
					if (param) {
						// invalid parameter
						sys_req(WRITE, COM1, error_msg_inc_param, strlen(error_msg_inc_param));
					} else {
						// ask user if they want to shutdown
						char msg[] = "Are you sure you want to shutdown? (y/n)\n";
						sys_req(WRITE, COM1, msg, strlen(msg));

						// get user input
						char choice[100] = {0};
						int size_choice = sys_req(READ, COM1, choice, sizeof(choice));
						if (choice[size_choice] == '\0') {
							// check if yes
							if ( strcmp(choice, "y") == 0 ) {
								// return to kmain()
								return;
							} else if ( strcmp(choice, "n") == 0 ) {
								// do nothing, return to normal operation
							} else {
								// invalid input
								char error_msg[] = "ERR: Invalid input\n";
								sys_req(WRITE, COM1, error_msg, strlen(error_msg));
							}
						}
					}
				} 

				/* HELP */
				else if ( strcmp(command, "help") == 0 ) {
					// get parameters from the buffer
                	char *param = strtok(NULL, " ");

					// pass parameter to help command
					if (param) {
                    	help(param);
					} else {
						help(" ");
					}
				} 

				/* GETDATE */
				else if ( strcmp(command, "getdate") == 0 ) {
					// check if there are any invalid params
					char *param = strtok(NULL, " ");
					if (param) {
						// invalid parameter
						sys_req(WRITE, COM1, error_msg_inc_param, strlen(error_msg_inc_param));
					} else {
						// run getdate command
						get_date();
					}
				} 

				/* SETDATE */
				else if ( strcmp(command, "setdate") == 0 ) {
					// run the set date command
					char *flag = strtok(NULL, " ");

					// check if there is a parameter
					if (flag) {
						// start flag check
						if (flag[0] == '-') {
							if ( strcmp(flag, "-d") == 0 ) {
								// get param from flag and pass into function
								char *param = strtok(NULL, " ");
								if (param) {
									// extrapolate the day, month, and year
									char *month_str = strtok(param, "-");
									char *day_str = strtok(NULL, "-");
									char *year_str = strtok(NULL, "-");

									// convert to integer
									int day = atoi(day_str);
									int month = atoi(month_str);
									uint8_t year = atoi(year_str);

									// pass into set date function
									set_date(day, month, year);
								} else {
									// no value to pass into functon
									sys_req(WRITE, COM1, error_msg_empty_param, strlen(error_msg_empty_param));
								}
							} else {
								// incorrect flag error
								sys_req(WRITE, COM1, error_msg_inc_flag, strlen(error_msg_inc_flag));
							}
						} else {
							sys_req(WRITE, COM1, error_msg_no_flag, strlen(error_msg_no_flag));
						}
					} else {
						// no param, display error code 
						sys_req(WRITE, COM1, error_msg_no_param, strlen(error_msg_no_param));
					}
				} 

				/* GETTIME */
				else if ( strcmp(command, "gettime") == 0 ) {
					// check if there are any invalid params
					char *param = strtok(NULL, " ");
					if (param) {
						// invalid parameter
						sys_req(WRITE, COM1, error_msg_inc_param, strlen(error_msg_inc_param));
					} else {
						// run the get time command
						get_time();
					}
				} 

				/* SETTIME */
				else if ( strcmp(command, "settime") == 0 ) {
					// run the set time command
					char *flag = strtok(NULL, " ");

					// pass command into settime function 
					if (flag) {
						// start flag check
						if (flag[0] == '-') {
							if ( strcmp(flag, "-t") == 0 ) {
								// get param from flag and pass into function
								char *param = strtok(NULL, " ");
								if (param) { 
									set_time(param); 
								} else {
									// no value to pass into functon
									sys_req(WRITE, COM1, error_msg_empty_param, strlen(error_msg_empty_param));
								}
							} else {
								// incorrect flag error
								sys_req(WRITE, COM1, error_msg_inc_flag, strlen(error_msg_inc_flag));
							}
						} else {
							sys_req(WRITE, COM1, error_msg_no_flag, strlen(error_msg_no_flag));
						}
					} else {
						// no param, print error
						sys_req(WRITE, COM1, error_msg_no_param, strlen(error_msg_no_param));
					}
				} 

				/* Error */
				else {
					// command not recognized
					char error_msg_invalid_cmd[] = "ERR: Invalid Command\n";
					sys_req(WRITE, COM1, error_msg_invalid_cmd, strlen(error_msg_invalid_cmd));
				}
			}

		}
    }
}
