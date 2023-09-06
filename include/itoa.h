/**
 * @file itoa.h
 * @brief integer to string function
 */

/**
 * @brief converts an integer to a string
 * 
 * @param value integer to be converted
 * @param destination place to store string conversion
 * @param base numerical base used to represent the value as a string
 * @return char* to the destination location
 */
char* itoa(int value, char* destination, int base);

