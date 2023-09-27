#include "memory.h"
#include "sys_req.h"
#include <pcb.h>
#include <string.h>
#include <comhand.h>

char* search(struct pcb* pcb_ptr)
{
    // if the process is RUNNING, then do not insert
    if (pcb_ptr->process_ptr->pcb_state == RUNNING) { // RUNNING
        return "running";
    }

    // check if the process dispatching state is suspended or not suspended
    if (pcb_ptr->process_ptr->pcb_state == READY_NOT_SUSPENDED) { // READY NOT SUSPENDED
        return "ready not suspended";
    } else if (pcb_ptr->process_ptr->pcb_state == BLOCKED_NOT_SUSPENDED) { // BLOCKED NOT SUSPENDED
        return "blocked not suspended";
    } else if (pcb_ptr->process_ptr->pcb_state == READY_SUSPENDED) { // READY SUSPENDED
        return "ready suspended";
    } else if (pcb_ptr->process_ptr->pcb_state == BLOCKED_SUSPENDED) { // BLOCKED SUSPENDED
        return "blocked suspended";
    }

    // if process/pcb was not found
    return NULL;
}

void enqueue_pri(char* status, struct pcb* pcb) {
    // Find which queue pcb should be placed in
    char* pcb_status = status;

    // Create a node to insert
    struct node* new_node = create_node(pcb);

    if (new_node == NULL) {
        // Handle memory allocation failure
        print("ERR: Memory error\n");
        return;
    }

    // if status of pcb is ready
    if (strcmp(pcb_status, "ready") == 0) 
    {
        int pri = pcb->process_ptr->pcb_priority;

        // if the queue is empty or the new node has higher priority, insert it at the front.
        if (global_ready_queue->rear == NULL || pri < global_ready_queue->front->pcb->process_ptr->pcb_priority) {
            new_node->prev = NULL;
            new_node->next = global_ready_queue->front;
            if (global_ready_queue->front != NULL) {
                global_ready_queue->front->prev = new_node;
            }
            global_ready_queue->front = new_node;

            // If the queue was empty, set the rear as well.
            if (global_ready_queue->rear == NULL) {
                global_ready_queue->rear = new_node;
            }
        } else {
            // Find the correct position to insert the new node
            struct node* compare = global_ready_queue->front;
            while (compare->next != NULL && pri >= compare->next->pcb->process_ptr->pcb_priority) {
                compare = compare->next;
            }

            // Insert the new node
            new_node->prev = compare;
            new_node->next = compare->next;
            if (compare->next != NULL) {
                compare->next->prev = new_node;
            }
            compare->next = new_node;

            // Update the rear if the new node is inserted at the end
            if (new_node->next == NULL) {
                global_ready_queue->rear = new_node;
            }
        }
    }
    // if status of pcb is suspended ready
    else if (strcmp(pcb_status, "suspended ready") == 0) 
    {
        int pri = pcb->process_ptr->pcb_priority;

        // if the queue is empty or the new node has higher priority, insert it at the front.
        if (global_suspended_ready_queue->rear == NULL || pri < global_suspended_ready_queue->front->pcb->process_ptr->pcb_priority) {
            new_node->prev = NULL;
            new_node->next = global_suspended_ready_queue->front;
            if (global_suspended_ready_queue->front != NULL) {
                global_suspended_ready_queue->front->prev = new_node;
            }
            global_suspended_ready_queue->front = new_node;

            // If the queue was empty, set the rear as well.
            if (global_suspended_ready_queue->rear == NULL) {
                global_suspended_ready_queue->rear = new_node;
            }
        } else {
            // Find the correct position to insert the new node
            struct node* compare = global_suspended_ready_queue->front;
            while (compare->next != NULL && pri >= compare->next->pcb->process_ptr->pcb_priority) {
                compare = compare->next;
            }

            // Insert the new node
            new_node->prev = compare;
            new_node->next = compare->next;
            if (compare->next != NULL) {
                compare->next->prev = new_node;
            }
            compare->next = new_node;

            // Update the rear if the new node is inserted at the end
            if (new_node->next == NULL) {
                global_suspended_ready_queue->rear = new_node;
            }
        }
    }
    // if status of pcb is blocked, not based on priority just FIFO so place at rear
    else if (strcmp(pcb_status, "blocked") == 0)
    {
        // create node to insert
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
    else if (strcmp(pcb_status, "suspended blocked") == 0)
    {
        // create node to insert
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

void enqueue_reg(char *status, struct pcb *pcb)
{
    // Find which queue pcb should be placed in
    char *pcb_status = status;

    // Create a node to insert
    struct node *new_node = create_node(pcb);

    if (new_node == NULL)
    {
        // Handle memory allocation failure
        print("ERR: Memory error\n");
        return;
    }

    // Determine the queue based on the status
    if (strcmp(pcb_status, "ready") == 0)
    {
        if (global_ready_queue->rear == NULL)
        {
            // If the queue is empty, set both front and rear to the new node.
            global_ready_queue->front = new_node;
            global_ready_queue->rear = new_node;
        }
        else
        {
            // Otherwise, update the rear node's next pointer to the new node.
            global_ready_queue->rear->next = new_node;
            new_node->prev = global_ready_queue->rear;
            global_ready_queue->rear = new_node;
        }
    }
    else if (strcmp(pcb_status, "suspended ready") == 0)
    {
        if (global_suspended_ready_queue->rear == NULL)
        {
            // If the queue is empty, set both front and rear to the new node.
            global_suspended_ready_queue->front = new_node;
            global_suspended_ready_queue->rear = new_node;
        }
        else
        {
            // Otherwise, update the rear node's next pointer to the new node.
            global_suspended_ready_queue->rear->next = new_node;
            new_node->prev = global_suspended_ready_queue->rear;
            global_suspended_ready_queue->rear = new_node;
        }
    }
    else if (strcmp(pcb_status, "blocked") == 0)
    {
        if (global_blocked_queue->rear == NULL)
        {
            // If the queue is empty, set both front and rear to the new node.
            global_blocked_queue->front = new_node;
            global_blocked_queue->rear = new_node;
        }
        else
        {
            // Otherwise, update the rear node's next pointer to the new node.
            global_blocked_queue->rear->next = new_node;
            new_node->prev = global_blocked_queue->rear;
            global_blocked_queue->rear = new_node;
        }
    }
    else if (strcmp(pcb_status, "suspended blocked") == 0)
    {
        if (global_suspended_blocked_queue->rear == NULL)
        {
            // If the queue is empty, set both front and rear to the new node.
            global_suspended_blocked_queue->front = new_node;
            global_suspended_blocked_queue->rear = new_node;
        }
        else
        {
            // Otherwise, update the rear node's next pointer to the new node.
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

void dequeue(const char* status, struct pcb* pcb)
{
    // Find which queue the pcb node belongs to
    const char* pcb_status = status;

    if (strcmp(pcb_status, "ready") == 0 || strcmp(pcb_status, "suspended ready") == 0)
    {
        struct queue* target_queue = (strcmp(pcb_status, "suspended ready") == 0) ? global_suspended_ready_queue : global_ready_queue;
        struct node* current = target_queue->front;

        while (current != NULL)
        {
            if (current->pcb == pcb)
            {
                if (current == target_queue->front)
                {
                    // PCB to dequeue is at the front of the queue
                    target_queue->front = current->next;
                    if (target_queue->front == NULL)
                    {
                        // Queue is now empty, update rear pointer
                        target_queue->rear = NULL;
                    }
                    else
                    {
                        target_queue->front->prev = NULL;
                    }
                }
                else if (current == target_queue->rear)
                {
                    // PCB to dequeue is at the rear of the queue
                    target_queue->rear = current->prev;
                    target_queue->rear->next = NULL;
                }
                else
                {
                    // PCB to dequeue is somewhere in the middle of the queue
                    current->prev->next = current->next;
                    current->next->prev = current->prev;
                }

                return; // PCB found and dequeued, exit the function
            }

            current = current->next;
        }
    }
    else if (strcmp(pcb_status, "blocked") == 0 || strcmp(pcb_status, "suspended blocked") == 0)
    {
        struct queue* target_queue = (strcmp(pcb_status, "suspended blocked") == 0) ? global_suspended_blocked_queue : global_blocked_queue;
        struct node* current = target_queue->front;

        while (current != NULL)
        {
            if (current->pcb == pcb)
            {
                if (current == target_queue->front)
                {
                    // PCB to dequeue is at the front of the queue
                    target_queue->front = current->next;
                    if (target_queue->front == NULL)
                    {
                        // Queue is now empty, update rear pointer
                        target_queue->rear = NULL;
                    }
                    else
                    {
                        target_queue->front->prev = NULL;
                    }
                }
                else if (current == target_queue->rear)
                {
                    // PCB to dequeue is at the rear of the queue
                    target_queue->rear = current->prev;
                    target_queue->rear->next = NULL;
                }
                else
                {
                    // PCB to dequeue is somewhere in the middle of the queue
                    current->prev->next = current->next;
                    current->next->prev = current->prev;
                }

                return; // PCB found and dequeued, exit the function
            }

            current = current->next;
        }
    }
}



struct queue* create_queue(void) {
    // Allocate memory for the queue structure
    struct queue* new_queue = (struct queue*)sys_alloc_mem(sizeof(struct queue));

    // Initialize the front and rear pointers to NULL since the queue is empty
    if (new_queue != NULL) {
        new_queue->front = NULL;
        new_queue->rear = NULL;
    }

    return new_queue; // Return the pointer to the new queue
}



