/**
 * @file time.h
 * @brief time function, includes set time, set date, get time, get date
 */


#include <stdint.h>

/**
 @brief Obtains the date from the clock registers and displays it to the user
*/
void get_date(void);

/* */
void get_time(void);

/**
 * @brief Set the time object
 * 
 * @param command 
 */
void set_time(const char *command);


/**
 * @brief Checks if the user inputted time is a valid time
 * 
 * @param input the user inputted time
 * @return returns 0 if time is not valid
 */
int isValidTimeFormat(const char* input);

/**
 * @brief Takes the users input and writes the proper date to the clock
 * 
 * @param day   The day the user wants to set the date to
 * @param month The month the user wants to set the date to
 * @param year The year the user wants to set the date to
 */
void set_date(uint8_t day, uint8_t month, uint8_t year);

/**
 * @brief Takes a hexidecimal value and converts it to its equivalent decimal value
 * 
 * @param hex The hexidecimal value that needs to be converted
 * @return int returns the decimal value from the hexidecimal value
 */
int hexToDec(uint8_t hex);

/**
 * @brief takes a decimal value and converts it to a hexidecimal value
 * 
 * @param decimal the decimal value that needs to be converted
 * @return uint8_t returns the hexidecimal value from the decimal
 */
uint8_t decToHex(int decimal);

/**
 * @brief Checks if the user inputted time is a valid time
 * 
 * @param input the user inputted time
 * @return returns 0 if time is not valid
 */
int isValidTimeFormat(const char* input);
