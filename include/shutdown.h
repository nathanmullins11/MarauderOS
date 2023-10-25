/**
 * @file shutdown.h
 * @brief handles shutdown of system
 */

/**
 * @brief shutdown's MarauderOS system
 * Clears the ready queue and then calls sys_req(exit)
 */
void shutdown(void);
