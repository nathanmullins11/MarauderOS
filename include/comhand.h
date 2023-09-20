/**
 * @file comhand.h
 * @brief command handler function 
 */

/**
 * @brief handles the user input and executes commands based on the input
 * if user input is not recognized, then prints error message
 * 
 */
void comhand(void);

/**
 * @brief prints a string out to the COM1 port using sys_req()
 * 
 * @param out the char* to print out
 */
void print(char *out);
