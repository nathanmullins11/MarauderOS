#include "sys_req.h"
#include <shutdown.h>
#include <pcb.h>

void shutdown(void) {
    /* Clear the ready queue */
    // get head B-)
    struct node* cur_node = global_ready_queue->front;
    struct node* next_node = NULL;

    // if null then ready queue empty
    while( cur_node != NULL ) {
        // if not then iterate each node and remove from queue
        next_node = cur_node->next;

        pcb_remove(cur_node->pcb);

        cur_node = next_node;
    }

    /* Call sys_req(exit) */
    sys_req(EXIT);
}
