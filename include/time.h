/**
 * @file time.h
 * @brief time function, includes set time, set date, get time, get date
 */

//void time(void);

#include <stdint.h>

void get_date(void);

void get_time(void);

void set_time(const char *command);

//int strncmp(const char* str1, const char* str2, size_t n);

int isValidTimeFormat(const char* input);

void set_date(uint8_t day, uint8_t month, uint8_t year);
