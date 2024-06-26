#include "sys_req.h"
#include <pcb.h>
#include <string.h>
#include <memory.h>
#include <comhand.h>

/* initialize queues in this file */
struct queue* global_ready_queue;
struct queue* global_suspended_ready_queue;
struct queue* global_blocked_queue;
struct queue* global_suspended_blocked_queue;


struct pcb* pcb_find(const char* process)
{
    /* set queues for each type from parameters */
    struct node* current_ready = global_ready_queue->front;
    struct node* current_suspended_ready = global_suspended_ready_queue->front;
    struct node* current_blocked = global_blocked_queue->front;
    struct node* current_suspended_blocked = global_suspended_blocked_queue->front;

    /* search for process in ready queue*/
    while(current_ready != NULL)
    {
        // if find then return pcb pointer
        if((strcmp(current_ready->pcb->name_arr, process) == 0))
        {
            return current_ready->pcb;
        }

        // iterate
        current_ready = current_ready->next;
    }

    /* search for process in suspended ready queue*/
    while(current_suspended_ready != NULL)
    {
        // if find then return pcb pointer
        if((strcmp(current_suspended_ready->pcb->name_arr, process) == 0))
        {
            return current_suspended_ready->pcb;
        }

        // iterate
        current_suspended_ready = current_suspended_ready->next;
    }

    /* search for process in blocked queue*/
    while(current_blocked != NULL)
    {
        // if find then return pcb pointer
        if((strcmp(current_blocked->pcb->name_arr, process) == 0))
        {
            return current_blocked->pcb;
        }

        // iterate
        current_blocked = current_blocked->next;
    }

    /* search for process in suspended blocked queue*/
    while(current_suspended_blocked != NULL)
    {
        // if found then return pcb pointer
        if((strcmp(current_suspended_blocked->pcb->name_arr, process) == 0))
        {
            return current_suspended_blocked->pcb;
        }

        // iterate
        current_suspended_blocked = current_suspended_blocked->next;
    }

    // if process not found in any queue, return NULL
    return NULL;
}

void pcb_insert(struct pcb* process) {
    // if the process is RUNNING, then do not insert
    if (process->process_ptr->pcb_state == RUNNING) {
        return;
    }

    // check state
    if (process->process_ptr->pcb_state == READY_NOT_SUSPENDED) {
        // not suspended & ready -> enqueue in global_ready_queue
        enqueue_pri("ready", process);
    } else if (process->process_ptr->pcb_state == BLOCKED_NOT_SUSPENDED) {
        // not suspended & blocked -> enqueue in global_blocked_queue
        enqueue_reg("blocked", process);
    } else if (process->process_ptr->pcb_state == READY_SUSPENDED) {
        // suspended & ready -> enqueue in global_suspended_ready_queue
        enqueue_pri("suspended ready", process);
    } else if (process->process_ptr->pcb_state == BLOCKED_SUSPENDED) {
        // suspended & blocked -> enqueue in global_suspended_blocked_queue
        enqueue_reg("suspended blocked", process);
    }
    
}

int pcb_free(struct pcb *process) {
    // free memory associated with pcb
    return sys_free_mem(process);
}

struct pcb* pcb_allocate(void)
{
    // allocate pcb struct and members using sys_alloc_mem
    struct pcb* new_pcb = (struct pcb*)sys_alloc_mem(sizeof(struct pcb));
    new_pcb->process_ptr = (struct process*)sys_alloc_mem(sizeof(struct process));
    new_pcb->name_arr = (char*)sys_alloc_mem(16);
    new_pcb->process_ptr->pcb_stack = (char*)sys_alloc_mem(PCB_STACK_SIZE);
    //new_pcb->process_ptr->pcb_stack[0] = 0;
    new_pcb->process_ptr->stack_ptr = new_pcb->process_ptr->pcb_stack + PCB_STACK_SIZE - sizeof(struct context*);

    // error message
    char *err = "ERR: Cannot allocate memory for pcb\n";

    /* check if memory was dynamically allocated correctly */
    // check pcb pointer
    if (new_pcb == NULL)
    {
        sys_req(WRITE, COM1, err, strlen(err));
        return NULL;
    }

    // check process pointer
    if(new_pcb->process_ptr == NULL)
    {
        sys_req(WRITE, COM1, err, strlen(err));
        return NULL;
    }

    // check name
    if(new_pcb->name_arr == NULL)
    {
        sys_req(WRITE, COM1, err, strlen(err));
        return NULL;
    }

    if(new_pcb->process_ptr->pcb_stack == NULL)
    {
        sys_req(WRITE, COM1, err, strlen(err));
        return NULL;
    }

    return new_pcb;
}


struct pcb* pcb_setup(const char *process_name , int class, int priority)
{
    // check parameters for validity
    if (process_name == NULL || (class != 0 && class != 1) || (priority < 0 || priority > 9)) 
    {
        return NULL;
    }

    // allocate memory for new PCB
    struct pcb* new_pcb = pcb_allocate();
    if (new_pcb == NULL) 
    {
        // error with allocation of memory, free and return
        pcb_free(new_pcb); 
        return NULL;
    }

    /* Initialize PCB with provided data */
    memcpy((char*)new_pcb->name_arr, process_name, strlen(process_name));
    new_pcb->process_ptr->pcb_class = class;
    new_pcb->process_ptr->pcb_priority = priority;

    // init with initial state
    new_pcb->process_ptr->pcb_state = READY_NOT_SUSPENDED;

    return new_pcb;
}

int pcb_remove(struct pcb *process)
{
    // returns 1 if error, 0 if no error
    int error = 1;
    int noError = 0;

    // if the process is running, then do not insert
    if (process->process_ptr->pcb_state == RUNNING) {
        return error;
    }

    // check the state
    if (process->process_ptr->pcb_state == READY_NOT_SUSPENDED) {
        // not suspended & ready -> enqueue in global_ready_queue
        dequeue("ready", process);
        return noError;
    } else if (process->process_ptr->pcb_state == BLOCKED_NOT_SUSPENDED) {
        // not suspended & blocked -> enqueue in global_blocked_queue
        dequeue("blocked", process);
        return noError;
    } else if (process->process_ptr->pcb_state == READY_SUSPENDED) {
        // suspended & ready -> enqueue in global_suspended_ready_queue
        dequeue("suspended ready", process);
        return noError;
    } else if (process->process_ptr->pcb_state == BLOCKED_SUSPENDED) {
        // suspended & blocked -> enqueue in global_suspended_blocked_queue
        dequeue("suspended blocked", process);
        return noError;
    }

    return error;
}
