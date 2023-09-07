/**
 * @file time.h
 * @brief time function, includes set time, set date, get time, get date
 */

//void time(void);

#include <stdint.h>

/* */
void get_date(void);

/* */
void get_time(void);

/**
 * @brief Set the time object
 * 
 * @param command 
 */
void set_time(const char *command);

//int strncmp(const char* str1, const char* str2, size_t n);

/**
 * @brief 
 * 
 * @param input 
 * @return int 
 */
int isValidTimeFormat(const char* input);

/**
 * @brief Set the date object
 * 
 * @param day 
 * @param month 
 * @param year 
 */
void set_date(uint8_t day, uint8_t month, uint8_t year);

/**
 * @brief 
 * 
 * @param hex 
 * @return int 
 */
int hexToDec(uint8_t hex);

/**
 * @brief 
 * 
 * @param decimal 
 * @return uint8_t 
 */
uint8_t decToHex(int decimal);

/**
 * @brief 
 * 
 * @param input 
 * @return int 
 */
int isValidTimeFormat(const char* input);
