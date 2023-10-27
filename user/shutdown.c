#include "mpx/device.h"
#include "sys_req.h"
#include <shutdown.h>
#include <pcb.h>

void shutdown(void) {
    /* Clear the ready queue */
    // get head B-)
    struct node* cur_node = global_ready_queue->front;
    struct node* next_node = NULL;

    // goodbye message
    char msg_bye[] = 
    "\t\x1b[34m   _____                 _ _                \n"
    "\t  / ____|               | | |               \n"
    "\t | |  __  ___   ___   __| | |__  _   _  ___ \n"
    "\t | | |_ |/ _ \\ / _ \\ / _` | '_ \\| | | |/ _ \\\n"
    "\t | |__| | (_) | (_) | (_| | |_) | |_| |  __/\n"
    "\t  \\_____|\\___/ \\___/ \\__,_|_.__/ \\__, |\\___|\n"
    "\t                                  __/ |     \n"
    "\t                                 |___/      \x1b[0m\n";

    // if null then ready queue empty
    while( cur_node != NULL ) {
        // if not then iterate each node and remove from queue
        next_node = cur_node->next;

        pcb_remove(cur_node->pcb);

        cur_node = next_node;
    }

    sys_req(WRITE, COM1, msg_bye, strlen(msg_bye));

    /* Call sys_req(exit) */
    sys_req(EXIT);
}
