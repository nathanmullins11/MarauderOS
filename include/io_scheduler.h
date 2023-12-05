/**
 * @file io_scheduler.h
 * @brief functions for IO scheduler
 */

#include <context_switch.h>

/**
 * @brief check to ensure the io request is valid based on context
 * either READ or WRITE & device ID valid & valid buffer
 * @param context_ptr pointer to context to validate
 * @return int 0 if valid, otherwise invalid
 */
int validate_io_request(struct context* context_ptr);

/**
 * @brief checks if process is available, process request if so
 * 
 * @param context_ptr pointer to context to schedule for i/o processing
 */
void io_scheduler(struct context* context_ptr);
