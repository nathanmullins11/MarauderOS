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
    "\n\x1b[34m  _______   ______     ______    _______  .______   ____    ____  _______\n" 
    " /  _____| /  __  \\   /  __  \\  |       \\ |   _  \\  \\   \\  /   / |   ____|\n"
    "|  |  __  |  |  |  | |  |  |  | |  .--.  ||  |_)  |  \\   \\/   /  |  |__   \n"
    "|  | |_ | |  |  |  | |  |  |  | |  |  |  ||   _  <    \\_    _/   |   __|  \n"
    "|  |__| | |  `--'  | |  `--'  | |  '--'  ||  |_)  |     |  |     |  |____ \n"
    " \\______|  \\______/   \\______/  |_______/ |______/      |__|     |_______|\x1b[0m\n\n";

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
