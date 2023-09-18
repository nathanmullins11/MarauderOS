#include <pcb.h>
#include <string.h>

/* initialize queues in this file*/
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
        if((strcmp(current_ready->pcb->process_ptr->process_name, process) == 0))
        {
            return current_ready->pcb;
        }

        current_ready = current_ready->next;
    }

    /* search for process in suspended ready queue*/
    while(current_suspended_ready != NULL)
    {
        if((strcmp(current_suspended_ready->pcb->process_ptr->process_name, process) == 0))
        {
            return current_suspended_ready->pcb;
        }

        current_suspended_ready = current_suspended_ready->next;
    }

    /* search for process in blocked queue*/
    while(current_blocked != NULL)
    {
        if((strcmp(current_blocked->pcb->process_ptr->process_name, process) == 0))
        {
            return current_blocked->pcb;
        }

        current_blocked = current_blocked->next;
    }

    /* search for process in suspended blocked queue*/
    while(current_suspended_blocked != NULL)
    {
        if((strcmp(current_suspended_blocked->pcb->process_ptr->process_name, process) == 0))
        {
            return current_suspended_blocked->pcb;
        }

        current_suspended_blocked = current_suspended_blocked->next;
    }

    // if process not found in any queue, return NULL
    return NULL;
}

void pcb_insert(struct pcb* process) {
    // if the process is running, then do not insert
    if ( (strcmp(process->process_ptr->pcb_state.execution_state, "running")) == 0 ) {
        return;
    }

    // check if the process dispatching state is suspended or not suspended
    if ( strcmp(process->process_ptr->pcb_state.dispatching_state, "not suspended") == 0 ) {
        // now check the execution state
        if ( strcmp(process->process_ptr->pcb_state.execution_state, "ready") == 0 ) {
            // not suspended & ready -> enqueue in global_ready_queue
            enqueue(&global_ready_queue->rear, process);
        } else if ( strcmp(process->process_ptr->pcb_state.dispatching_state, "blocked") == 0 ) {
            // not suspended & blocked -> enqueue in global_blocked_queue
            enqueue(&global_blocked_queue->rear, process);
        }
    } else if ( strcmp(process->process_ptr->pcb_state.dispatching_state, "suspended") == 0 ) {
        // now check the execution state
        if ( strcmp(process->process_ptr->pcb_state.execution_state, "ready") == 0 ) {
            // suspended & ready -> enqueue in global_suspended_ready_queue
            enqueue(&global_suspended_ready_queue->rear, process);
        } else if ( strcmp(process->process_ptr->pcb_state.dispatching_state, "blocked") == 0 ) {
            // suspended & blocked -> enqueue in global_suspended_blocked_queue
            enqueue(&global_suspended_blocked_queue->rear, process);
        }
    }
}
