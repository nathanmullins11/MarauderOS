/**
 * @file time.h
 * @brief time function, includes set time, set date, get time, get date
 */

void time(void);

void get_date(void);

void get_time(void);

void set_time(const char *command);

const char* extract_time(const char* command);

int strncmp(const char* str1, const char* str2, size_t n);
