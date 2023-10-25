/**
 * @file alarms.h
 * @brief alarm functions 
 */

/**
 * @brief handles the user input and executes commands based on the input
 * if user input is not recognized, then prints error message
 * 
 */
void alarm(char* formatted_time, char* message);

/**
 * @brief message alarm will print when time is reached
 * 
 */
void print_message(void);

/**
 * @brief check for valid time to print message
 * 
 */
int check_time(void);

/**
 * @brief check if/which alarm process and running to know which message to grab
 * 
 * @return int 0 for alarm0, 1 for alarm 1, 2 for alarm2, 3 for alarm3, 4 for alarm4
 */
int check_running_process(void);

