#include "memory.h"
#include <pcb.h>
#include <string.h>


char* search(struct pcb* pcb_ptr)
{
    // if the process is running, then do not insert
    if ( (strcmp(pcb_ptr->process_ptr->pcb_state.dispatching_state, "running")) == 0 ) {
        return "running";
    }

    // check if the process dispatching state is suspended or not suspended
    if ( strcmp(pcb_ptr->process_ptr->pcb_state.dispatching_state, "not suspended") == 0 ) {
        // now check the execution state
        if ( strcmp(pcb_ptr->process_ptr->pcb_state.execution_state, "ready") == 0 ) {
            // not suspended & ready -> enqueue in global_ready_queue
            return "ready";
        } else if ( strcmp(pcb_ptr->process_ptr->pcb_state.dispatching_state, "blocked") == 0 ) {
            // not suspended & blocked -> enqueue in global_blocked_queue
            return "blocked";
        }
    } else if ( strcmp(pcb_ptr->process_ptr->pcb_state.dispatching_state, "suspended") == 0 ) {
        // now check the execution state
        if ( strcmp(pcb_ptr->process_ptr->pcb_state.execution_state, "ready") == 0 ) {
            // suspended & ready -> enqueue in global_suspended_ready_queue
            return "suspended ready";
        } else if ( strcmp(pcb_ptr->process_ptr->pcb_state.dispatching_state, "blocked") == 0 ) {
            // suspended & blocked -> enqueue in global_suspended_blocked_queue
            return "suspended blocked";
        }
    }

    // if process/pcb was not found
    return NULL;
}

void enqueue(char* status, struct pcb* pcb) {
    // Find which queue pcb should be placed in
    char* pcb_status = status;

    // if status of pcb is ready
    if (strcmp(pcb_status, "ready") == 0) 
    {
        int pri = pcb->process_ptr->pcb_priority;
        // create node to insert
        struct node* new_node = create_node(pcb);

        if (new_node == NULL) {
            // Handle memory allocation failure
            return;
        }

        // if the queue is empty or the new node has the highest priority, insert it at the rear.
        if (global_ready_queue->rear == NULL || pri >= global_ready_queue->rear->pcb->process_ptr->pcb_priority) {
            new_node->next = NULL;
            new_node->prev = global_ready_queue->rear;
            if (global_ready_queue->rear != NULL) {
                global_ready_queue->rear->next = new_node;
            }
            global_ready_queue->rear = new_node;

            // If the queue was empty, set the front as well.
            if (global_ready_queue->front == NULL) {
                global_ready_queue->front = new_node;
            }
        } else {
            // Find the correct position to insert the new node
            struct node* compare = global_ready_queue->rear;
            while (compare->prev != NULL && pri < compare->prev->pcb->process_ptr->pcb_priority) {
                compare = compare->prev;
            }

            // Insert the new node
            new_node->next = compare->next;
            new_node->prev = compare;
            if (compare->next != NULL) {
                compare->next->prev = new_node;
            }
            compare->next = new_node;
        }
    }

    // if status of pcb is suspended ready
    if (strcmp(pcb_status, "suspended ready") == 0) 
    {
        int pri = pcb->process_ptr->pcb_priority;
        // create node to insert
        struct node* new_node = create_node(pcb);

        if (new_node == NULL) {
            // Handle memory allocation failure
            return;
        }

        // if the queue is empty or the new node has the highest priority, insert it at the rear.
        if (global_suspended_ready_queue->rear == NULL || pri >= global_suspended_ready_queue->rear->pcb->process_ptr->pcb_priority) {
            new_node->next = NULL;
            new_node->prev = global_suspended_ready_queue->rear;
            if (global_suspended_ready_queue->rear != NULL) {
                global_suspended_ready_queue->rear->next = new_node;
            }
            global_suspended_ready_queue->rear = new_node;

            // If the queue was empty, set the front as well.
            if (global_suspended_ready_queue->front == NULL) {
                global_suspended_ready_queue->front = new_node;
            }
        } else {
            // Find the correct position to insert the new node
            struct node* compare = global_ready_queue->rear;
            while (compare->prev != NULL && pri < compare->prev->pcb->process_ptr->pcb_priority) {
                compare = compare->prev;
            }

            // Insert the new node
            new_node->next = compare->next;
            new_node->prev = compare;
            if (compare->next != NULL) {
                compare->next->prev = new_node;
            }
            compare->next = new_node;
        }
    }

    // if status of pcb is blocked, not based on priority just FIFO so place at rear
    if (strcmp(pcb_status, "blocked") == 0)
    {
        // create node to insert
        struct node* new_node = create_node(pcb);

        if (new_node == NULL) {
            // Handle memory allocation failure
            return;
        }

        new_node->next = NULL; // the new node will be the last node, so its next pointer should be NULL.

        if (global_blocked_queue->rear == NULL) {
            // if the queue is empty, set both front and rear to the new node.
            global_blocked_queue->front = new_node;
            global_blocked_queue->rear = new_node;
        } else {
            // otherwise, update the rear node's next pointer to the new node.
            global_blocked_queue->rear->next = new_node;
            new_node->prev = global_blocked_queue->rear;
            global_blocked_queue->rear = new_node;
        }
    }


    // if status of pcb is suspended blocked, just FIFO
    if (strcmp(pcb_status, "suspended blocked") == 0)
    {
        // create node to insert
        struct node* new_node = create_node(pcb);

        if (new_node == NULL) {
            // Handle memory allocation failure
            return;
        }

        new_node->next = NULL; // the new node will be the last node, so its next pointer should be NULL.

        if (global_suspended_blocked_queue->rear == NULL) {
            // if the queue is empty, set both front and rear to the new node.
            global_suspended_blocked_queue->front = new_node;
            global_suspended_blocked_queue->rear = new_node;
        } else {
            // otherwise, update the rear node's next pointer to the new node.
            global_suspended_blocked_queue->rear->next = new_node;
            new_node->prev = global_suspended_blocked_queue->rear;
            global_suspended_blocked_queue->rear = new_node;
        }
    }
}


struct node* create_node(struct pcb* pcb) 
{
    struct node* new_node = (struct node*)sys_alloc_mem(sizeof(struct node));

    if (new_node == NULL) {
        // Handle memory allocation failure
        return NULL;
    }

    new_node->pcb = pcb;
    new_node->next = NULL;
    new_node->prev = NULL;

    return new_node;
}

void dequeue(char* status, struct node *pcb)
{
    // find which queue the pcb node belongs to
    char *pcb_status = status;

    if (strcmp(pcb_status, "ready") == 0)
    {
        // check if the node to dequeue is the front of the ready queue
        if (global_ready_queue->front == pcb)
        {
            global_ready_queue->front = pcb->next;
            if (pcb->next != NULL)
            {
                pcb->next->prev = NULL;
            }
        }
        // check if the node to dequeue is the rear of the ready queue
        else if (global_ready_queue->rear == pcb)
        {
            global_ready_queue->rear = pcb->prev;
            if (pcb->prev != NULL)
            {
                pcb->prev->next = NULL;
            }
        }
        else
        {
            // the node is somewhere in the middle of the ready queue so adjust pointers
            pcb->prev->next = pcb->next;
            pcb->next->prev = pcb->prev;
        }
    }
    else if (strcmp(pcb_status, "blocked") == 0)
    {
        // check if the node to dequeue is the front of the blocked queue
        if (global_blocked_queue->front == pcb)
        {
            global_blocked_queue->front = pcb->next;
            if (pcb->next != NULL)
            {
                pcb->next->prev = NULL;
            }
        }
        // check if the node to dequeue is the rear of the blocked queue
        else if (global_blocked_queue->rear == pcb)
        {
            global_blocked_queue->rear = pcb->prev;
            if (pcb->prev != NULL)
            {
                pcb->prev->next = NULL;
            }
        }
        else
        {
            // the node is somewhere in the middle of the blocked queue
            pcb->prev->next = pcb->next;
            pcb->next->prev = pcb->prev;
        }
    }
    else if (strcmp(pcb_status, "suspended ready") == 0)
    {
        // check if the node to dequeue is the front of the suspended ready queue
        if (global_suspended_ready_queue->front == pcb)
        {
            global_suspended_ready_queue->front = pcb->next;
            if (pcb->next != NULL)
            {
                pcb->next->prev = NULL;
            }
        }
        // check if the node to dequeue is the rear of the suspended ready queue
        else if (global_suspended_ready_queue->rear == pcb)
        {
            global_suspended_ready_queue->rear = pcb->prev;
            if (pcb->prev != NULL)
            {
                pcb->prev->next = NULL;
            }
        }
        else
        {
            // the node is somewhere in the middle of the suspended ready queue so adjust pointers
            pcb->prev->next = pcb->next;
            pcb->next->prev = pcb->prev;
        }
    }
    else if (strcmp(pcb_status, "suspended blocked") == 0)
    {
        // check if the node to dequeue is the front of the suspended blocked queue
        if (global_suspended_blocked_queue->front == pcb)
        {
            global_suspended_blocked_queue->front = pcb->next;
            if (pcb->next != NULL)
            {
                pcb->next->prev = NULL;
            }
        }
        // check if the node to dequeue is the rear of the suspended blocked queue
        else if (global_suspended_blocked_queue->rear == pcb)
        {
            global_suspended_blocked_queue->rear = pcb->prev;
            if (pcb->prev != NULL)
            {
                pcb->prev->next = NULL;
            }
        }
        else
        {
            // the node is somewhere in the middle of the suspended blocked queue so adjust pointers
            pcb->prev->next = pcb->next;
            pcb->next->prev = pcb->prev;
        }
    }
}



