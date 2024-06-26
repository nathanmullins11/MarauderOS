#include "memory.h"
#include "mpx/device.h"
#include "mpx/serial.h"
#include <comhand.h>
#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys_req.h>
#include <itoa.h>
#include <string.h>
#include <version.h>
#include <time.h>
#include <help.h>
#include <mpx/io.h>
#include <shutdown.h>
#include <alarms.h>
#include <interrupt_control.h>
#include <context_switch.h>

// variable used for printing > symbol
int j = 0;

void print(char *out) {
	sys_req(WRITE, COM1, out, strlen(out));
}

void comhand(void)
{
	// print welcoming message
	const char* msg = "Welcome to MarauderOS | Use 'help' command to see list of commands\n";

	// warning message
	const char* warn_msg = "\x1b[33m[WARN]\x1b[0m Please review `r6-warnings.md` in documentation (doc/r6-warnings.md) before use\n";

	// secondary welcome message
	char* picture_msg =
	"\x1b[34m         __  __                                 _                ____    _____\n"
	"	|  |/  |                               | |              / __ |  / ____|\n"
	"	| |  / |  __ _  _ __   __ _  _   _   __| |  ___  _ __  | |  | || (___  \n"
	"	| |||| | / _` || '__| / _` || | | | / _` | / _ ||  __| | |  | | |___ | \n"
	"	| |  | || (_| || |   | (_| || |_| || (_| ||  __/| |    | |__| | ____) |\n"
	"	|_|  |_| |__,_||_|    |__,_| |__,_| |_,_| |___| |_|     |____/ |_____/ \n"
    "                                                                   \n"
    "                                                                   \x1b[0m\n";

	// print logo and welcome message
	sys_req(WRITE, COM1, picture_msg, strlen(picture_msg));
	sys_req(WRITE, COM1, msg, strlen(msg));

	sys_req(WRITE, COM1, warn_msg, strlen(warn_msg));

	// error messages
	char error_msg_inc_param[] = "\x1b[31mERR: Invalid parameter | use `help` command\x1b[0m\n";
	char error_msg_no_param[] = "\x1b[31mERR: command needs parameter | run `help` command\x1b[0m\n";
	char error_msg_empty_param[] = "\x1b[31mERR: empty parameter | run `help` command\x1b[0m\n";
	char error_msg_inc_flag[] = "\x1b[31mERR: incorrect flag | run `help` command\x1b[0m\n";
	char error_msg_no_flag[] = "\x1b[31mERR: no flag provided | run `help` command\x1b[0m\n";
	char error_msg_missing_name[] = "\x1b[31mERR: missing PCB name | run `help` command\x1b[0m\n";
	char error_msg_missing_priority[] = "\x1b[31mERR: missing PCB priority | run `help` command\x1b[0m\n";
	char error_msg_unknown_suboption[] = "\x1b[31mERR: missing required flag for option | run `help` command\x1b[0m\n";
			
	// pointer to store command from user input
	char *command;

	// define pointers to store parameters for various commands
	char* time = (char*)sys_alloc_mem(100 * sizeof(char));
	// char* message = (char*)sys_alloc_mem(100 * sizeof(char));

	// loop forever until shutdown
    for ( ;; ) 
    {
		// store current dcb into temp pointer
		int dcb_index = check_cur_dcb();
		// if invalid index, return
		if(dcb_index < 0)
		{
			return;
		}
    
    struct dcb* temp_dcb = dcb_array[dcb_index];

		// print input indent 
		if (j % 2 == 0) {
			outb(COM1, '>');
			outb(COM1, ' ');
		}
		j++;
		
		// create buffer to hold user input and read using READ op-code
    	char buf[100] = {0};
		
		// read from input
        sys_req(READ, COM1, buf, strlen(buf));

		// copy contents into dcb buffer
		for(size_t i = 0; i < strlen(temp_dcb->rw_buf); i++)

		{
			buf[i] = temp_dcb->rw_buf[i];
		}

		buf[strlen(buf)] = '\0';

		// check if the buffer is ended with a null terminator before evaluating content
		if (buf[strlen(buf)] == '\0') {

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
						char msg[] = "Are you sure you want to shutdown? (y/n)\n> ";
						sys_req(WRITE, COM1, msg, strlen(msg));

						temp_dcb->ring_chars_transferred = 0;
						memset(temp_dcb->ring_buf, 0, strlen(temp_dcb->ring_buf));
						memset(temp_dcb->rw_buf, 0, strlen(temp_dcb->rw_buf));
						temp_dcb->allocation_status = 0;
						// get user input
						char choice[100] = {0};
						sys_req(READ, COM1, choice, sizeof(choice));

						for(size_t i = 0; i < strlen(temp_dcb->rw_buf); i++)
						{
							choice[i] = temp_dcb->rw_buf[i];
						}

						choice[strlen(choice)] = '\0';
						
						if (choice[strlen(choice)] == '\0') {
							// check if yes
							if ( strcmp(choice, "y") == 0 ) {
								// call shutdown method
								shutdown();
							} else if ( strcmp(choice, "n") == 0 ) {
								// do nothing, return to normal operation
								temp_dcb->ring_chars_transferred = 0;
								memset(temp_dcb->ring_buf, 0, strlen(temp_dcb->ring_buf));
								memset(temp_dcb->rw_buf, 0, strlen(temp_dcb->rw_buf));
								temp_dcb->allocation_status = 0;
								temp_dcb->rw_index = 0;
							} else {
								// invalid input
								// char error_msg[] = "\x1b[31mERR: Invalid input\x1b[0m\n";
								// sys_req(WRITE, COM1, error_msg, strlen(error_msg));
								temp_dcb->ring_chars_transferred = 0;
								memset(temp_dcb->ring_buf, 0, strlen(temp_dcb->ring_buf));
								memset(temp_dcb->rw_buf, 0, strlen(temp_dcb->rw_buf));
								temp_dcb->allocation_status = 0;
							}
						}
					}
				} 

				/* IMMEDIATE SHUTDOWN */
				else if ( strcmp(command, "shutdown!") == 0 ) {
					// check if there are any invalid params
					char *param = strtok(NULL, " ");
					if (param) {
						// invalid parameter
						sys_req(WRITE, COM1, error_msg_inc_param, strlen(error_msg_inc_param));
					} else {
						// run the version command
                		shutdown();
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
								if (param && (param[2] == '-' || param[5] == '-')) {
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
									// clear buffers
									memset(temp_dcb->ring_buf, 0, strlen(temp_dcb->ring_buf));
									memset(temp_dcb->rw_buf, 0, strlen(temp_dcb->rw_buf));
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
									// clear buffers
									memset(temp_dcb->ring_buf, 0, strlen(temp_dcb->ring_buf));
									memset(temp_dcb->rw_buf, 0, strlen(temp_dcb->rw_buf));
								} else {
									// no value to pass into functon
									sys_req(WRITE, COM1, error_msg_empty_param, strlen(error_msg_empty_param));
								}
							} else {
								// incorrect flag error
								sys_req(WRITE, COM1, error_msg_inc_flag, strlen(error_msg_inc_flag));
							}
						} else {
							// no flag
							sys_req(WRITE, COM1, error_msg_no_flag, strlen(error_msg_no_flag));
						}
					} else {
						// no param, print error
						sys_req(WRITE, COM1, error_msg_no_param, strlen(error_msg_no_param));
					}
				} 

				/* PCB */
				else if (strcmp(command, "pcb") == 0) {
					char *param = strtok(NULL, " ");

					// store parameters
					char *name = NULL;
					int pri = -1;
					int isList = -1;

					if(param) {
						if (param[0] == '-') {
							// Check if the parameter is a flag
							if (strcmp(param, "-c") == 0) {
								/* DEPRECATED IN R3 */
								char *error_msg_dep = "\x1b[31mERR: Feature no longer supported\x1b[0m\n";
								sys_req(WRITE, COM1, error_msg_dep, strlen(error_msg_dep));
							} else if (strcmp(param, "-p") == 0) {
								name = strtok(NULL, " ");
								if (!name) {
									// Missing name, print error
									sys_req(WRITE, COM1, error_msg_missing_name, strlen(error_msg_missing_name));
								} else {
									char *param2 = strtok(NULL, " ");
									if (param2 && strcmp(param2, "--pri") == 0) {
										char *pri_str = strtok(NULL, " ");
										if (!pri_str) {
											// Missing priority, print error
											sys_req(WRITE, COM1, error_msg_missing_priority, strlen(error_msg_missing_priority));
										} else {
											pri = atoi(pri_str);
										}
									} else {
										// Unknown sub-option, print error
										sys_req(WRITE, COM1, error_msg_unknown_suboption, strlen(error_msg_unknown_suboption));
									}
								}
								
							} else if (strcmp(param, "-d") == 0 || strcmp(param, "-b") == 0 ||
									strcmp(param, "-u") == 0 || strcmp(param, "-s") == 0 ||
									strcmp(param, "-r") == 0) {
								name = strtok(NULL, " ");
								if (!name) {
									// Missing name, print error
									sys_req(WRITE, COM1, error_msg_missing_name, strlen(error_msg_missing_name));
								}
							} else if ( strcmp(param, "-l") == 0 ) {
								name = strtok(NULL, " ");
								if (!name) {
									name = "all";
								}
							} else {
								// Unknown flag, print error
								sys_req(WRITE, COM1, error_msg_inc_flag, strlen(error_msg_inc_flag));

							}
						} else {
							// Invalid parameter format, print error
							sys_req(WRITE, COM1, error_msg_no_flag, strlen(error_msg_no_flag));
						}
					} else {
						sys_req(WRITE, COM1, error_msg_no_flag, strlen(error_msg_no_flag));
					}

					// check if -l flag
					if (param) {
						isList = strcmp(param, "-l");
					}
					
					// Check if at least one option was specified
					if (name || isList == 0) {
						// Call the appropriate function based on the option
						if (strcmp(param, "-c") == 0) {
							/* DEPRECATED IN R3 */
						} else if (strcmp(param, "-p") == 0) {
							// PCB priority 
							if (pri != -1) {
								// run function
								set_pcb_priority(name, pri);
							}
						} else if (strcmp(param, "-d") == 0) {
							// PCB delete
							delete_pcb(name);
						} else if (strcmp(param, "-b") == 0) {
							// PCB block
							// block_pcb(name);
							/* Deprecated in R6 */
							char *error_msg_dep = "\x1b[31mERR: Feature no longer supported\x1b[0m\n";
							sys_req(WRITE, COM1, error_msg_dep, strlen(error_msg_dep));
						} else if (strcmp(param, "-u") == 0) {
							// PCB unblock
							//unblock_pcb(name);
							/* Deprecated in R6 */
							char *error_msg_dep = "\x1b[31mERR: Feature no longer supported\x1b[0m\n";
							sys_req(WRITE, COM1, error_msg_dep, strlen(error_msg_dep));
						} else if (strcmp(param, "-s") == 0) {
							// PCB suspend
							suspend_pcb(name);
						} else if (strcmp(param, "-r") == 0) {
							// PCB resume
							resume_pcb(name);
						} else if (strcmp(param, "-l") == 0) {
							// PCB list 
							if ( strcmp(name, "ready") == 0 ) {
								// list all ready 
								show_ready();
							} else if ( strcmp(name, "blocked") == 0 ) {
								// list all blocked
								show_blocked();
							} else if ( strcmp(name, "all") == 0 ) {
								// list all processes
								show_all();
							} else {
								// list specific process
								show_pcb(name, 1);
							}
						} else {
							// Missing sub-options, print error
							sys_req(WRITE, COM1, error_msg_no_flag, strlen(error_msg_no_flag));
						}
					}
				}
				
				/* LOAD R3 */
				else if( strcmp(command, "load_r3") == 0 ) {
					// check if there are any invalid params
					char *param = strtok(NULL, " ");
					if (param) {
						// invalid parameter
						sys_req(WRITE, COM1, error_msg_inc_param, strlen(error_msg_inc_param));
					} else {
						// run yield command
						load_r3();
					}
				}

				/* ALARM */
				else if ( strcmp(command, "alarm") == 0 ) {
					// get first flag
					char *param = strtok(NULL, " ");

					// store parameters
					// check the flag
					if (param) {
						if ( strcmp(param, "-t") == 0 ) {
							time = strtok(NULL, " ");

							int time_format_checker = 1;
							if(isValidTimeFormat(time) != 1)
							{
								sys_req(WRITE, COM1, "\x1b[31mERR: Invalid time format | use 'help' command\x1b[0m\n", 46);
								time_format_checker = 0;
							}

							if (time && time_format_checker) {
								// everything valid thus far, get command message
								char msg[] = "Enter an alarm message of 100 characters or less:\n> ";
								sys_req(WRITE, COM1, msg, strlen(msg));

								temp_dcb->ring_chars_transferred = 0;
								memset(temp_dcb->ring_buf, 0, strlen(temp_dcb->ring_buf));

								// get user input
								char temp_buf[100] = {0};
								sys_req(READ, COM1, temp_buf, sizeof(temp_buf));

								for(size_t i = 0; i < strlen(temp_dcb->rw_buf); i++)
								{
									temp_buf[i] = temp_dcb->rw_buf[i];
								}

								temp_buf[strlen(temp_buf)] = '\0';

								// check if the first character is a space or empty
								// if not then parse message and pass into function
								//if (isspace((int)temp_buf[0]) != 1 || message[0] != '\0') {
								if(0) {
									char err_spaces[] = "\x1b[31mERR: Alarm name cannot be empty\x1b[0m\n";
									print(err_spaces);
								} else {
									// clear temp buffer
									if (temp_buf[0] != '0')
									{
										// remove all chars in temp buffer except first '0'
										size_t temp_len = strlen(temp_buf);
										while (temp_len > 1) {
											for (size_t i = 0; i < temp_len - 1; i++) {
												temp_buf[i] = temp_buf[i + 1];
											}
											temp_len--;
										}
										temp_buf[0] = '\0'; // Null-terminate after '0'
									}

									//if (temp_dcb->rw_buf[strlen(temp_dcb->rw_buf)] == '\0') {
										// message valid, pass into function
										alarm(time, temp_dcb->rw_buf);
									//}
								}
							} else {
								if(time_format_checker)
								{
									print(error_msg_no_param);
								}
							}
						} else {
							// incorrect flag
							print(error_msg_inc_flag);
						}
					} else {
						// no flag
						print(error_msg_no_flag);
					}
					
				}

				/* CLEAR */
				else if (strcmp(command, "clear") == 0) {
					print("\033[2J\033[1;1H");
				}

				/* Error */
				else {
					// command not recognized
					char error_msg_invalid_cmd[] = "\x1b[31mERR: Invalid Command\x1b[0m\n";
					sys_req(WRITE, COM1, error_msg_invalid_cmd, strlen(error_msg_invalid_cmd));
				}
			}

		}

		temp_dcb->ring_chars_transferred = 0;
		memset(temp_dcb->ring_buf, 0, strlen(temp_dcb->ring_buf));
		memset(temp_dcb->rw_buf, 0, strlen(temp_dcb->rw_buf));
		temp_dcb->allocation_status = 0;
		temp_dcb->rw_index = 0;

		sys_req(IDLE);
    }
}
