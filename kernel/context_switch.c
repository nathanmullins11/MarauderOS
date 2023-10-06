#include "memory.h"
#include "processes.h"
#include <sys_req.h>
#include <context_switch.h>
#include <string.h>
#include <pcb.h>

/* PROBLEM - I THINK:
    When proc1 is run it stays in the idle state until after exiting. right now we have it set up so that if the context ptr EAX is idle,
    it will grab the next process to run. However, proc1 has not finished, which for some reason causes a page fault. Somehow we need to it to
    check if proc1 is still running, and if so, not try to grab the next process. I think this can be done by keeping a global variable to store the previous
    running processes context, then comparing it with the context next to run.

    ## UPDATE ##
    RUN debugger and print out the stack ptr using this: p *(struct context*)global_ready_queue->front->next->pcb->process_ptr->stack_ptr
    FOR some reason the values are not passing correctly to the context switch which makes no sense..
*/

// global PCB pointer for currently running process
struct pcb* global_current_process = NULL;
// gloabal temp pointer to hold front pcb in queue to run next
// struct pcb* temp_pcb = NULL;

struct context* first_context_ptr =  NULL;

// global/static context pointer representing context from first time sys_call() is entered
struct context* global_context_ptr = NULL;

struct context* global_prev_context = NULL;

struct pcb* global_prev_process = NULL;

struct context* sys_call(struct context* context_ptr) // context passed in is comhand context
{

    if(global_context_ptr != context_ptr)
    {
        global_context_ptr = context_ptr;
    }

    // if operation code is IDLE
    if(global_context_ptr->EAX == IDLE)
    {
        if(first_context_ptr == NULL)
        {
            first_context_ptr = context_ptr;
        }
        // check for PCBs in ready not suspended queue
        if(global_ready_queue->front != NULL && (global_prev_context->EIP != global_context_ptr->EIP))
        {
            if(global_prev_process != NULL && global_current_process != NULL)
            {
                if(strcmp(global_current_process->name_arr, global_prev_process->name_arr) == 0)
                {
                    global_prev_context = context_ptr;
                    return context_ptr;
                }
            }
            struct pcb* current_process = global_current_process;
            // remove first from queue and store in temp variable
            struct pcb* next_process = global_ready_queue->front->pcb;
            pcb_remove(next_process);
            
            // increment globabl current process
            global_current_process = next_process;

           // add the current PCB back to the ready queue
            if(current_process != NULL)
            {
                current_process->process_ptr->stack_ptr = context_ptr;
                pcb_insert(current_process);
            }

            // return pointer to stack, which contains context of process to be run next
            global_prev_context = (struct context*)next_process->process_ptr->stack_ptr;
            global_prev_process = next_process;
            return (struct context*)next_process->process_ptr->stack_ptr;
        }

        // if ready not suspended queue is empty, continue with current process
        global_prev_context = context_ptr;
        return context_ptr;
    } 
    else if (global_context_ptr->EAX == EXIT) 
    {
        // delete currently running pcb
        if(global_current_process != NULL)
        {
            pcb_free(global_current_process);
        }

         // check for PCBs in ready not suspended queue
        if(global_ready_queue->front != NULL)
        {
            // remove first from queue and store in temp variable
            struct pcb* temp_pcb = global_ready_queue->front->pcb;
            pcb_remove(temp_pcb);

            // save context of current PCB by updating stack pointer
            global_current_process = temp_pcb;

            // return pointer to stack, which contains context of process to be run next
             global_prev_context = (struct context*)temp_pcb->process_ptr->stack_ptr;
            return global_prev_context;
        }

        // if ready not suspended queue is empty
        global_prev_context = first_context_ptr;
        return first_context_ptr;
    }
    
    context_ptr->EAX = -1;
    global_prev_context = context_ptr;
    return context_ptr;

}

