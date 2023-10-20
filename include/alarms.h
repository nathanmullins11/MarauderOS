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

