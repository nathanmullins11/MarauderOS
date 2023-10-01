#include <sys_req.h>
#include <context_switch.h>
#include <string.h>
#include <pcb.h>

// global PCB pointer for currently running process
struct pcb* global_current_process = NULL;

// gloabal temp pointer to hold front pcb in queue to run next
struct pcb* temp_pcb = NULL;

// global/static context pointer representing context from first time sys_call() is entered
struct context* gloabal_context_ptr = NULL;

struct context* sys_call(struct context* context_ptr)
{
    if(gloabal_context_ptr == NULL)
    {
        gloabal_context_ptr = context_ptr;
    }
    
    // if operation code is IDLE
    if(gloabal_context_ptr->EAX == IDLE)
    {
        // check for PCBs in ready not suspended queue
        if(global_ready_queue->front != NULL)
        {
            // remove first from queue and store in temp variable
            temp_pcb = global_ready_queue->front->pcb;
            pcb_remove(temp_pcb);

            // save context of current PCB by updating stack pointer

            // add the current PCB back to the ready queue
            pcb_insert(global_current_process);

            // return pointer to stack, which contains context of process to be run next)
            return temp_pcb->process_ptr->stack_ptr;
        }

        // if ready not suspended queue is empty, continue with current process
        return context_ptr;
    } 
    else if (gloabal_context_ptr->EAX == EXIT) 
    {
        // delete currently running pcb
        delete_pcb(global_current_process->name_arr);

         // check for PCBs in ready not suspended queue
        if(global_ready_queue->front != NULL)
        {
            // remove first from queue and store in temp variable
            temp_pcb = global_ready_queue->front->pcb;
            pcb_remove(temp_pcb);

            // save context of current PCB by updating stack pointer

            // add the current PCB back to the ready queue
            pcb_insert(global_current_process);

            // return pointer to stack, which contains context of process to be run next)
            return temp_pcb->process_ptr->stack_ptr;
        }

        // if ready not suspended queue is empty
        return context_ptr;
    }
    
    return NULL;

}

