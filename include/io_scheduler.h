/**
 * @file io_scheduler.h
 * @brief functions for IO scheduler
 */

 #include <context_switch.h>

int validate_io_request(struct context* context_ptr);


void io_scheduler(struct context* context_ptr);

