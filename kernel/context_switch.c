#include "memory.h"
#include "processes.h"
#include <sys_req.h>
#include <context_switch.h>
#include <string.h>
#include <pcb.h>

// global PCB pointer for currently running process
struct pcb* global_current_process = NULL;
// gloabal temp pointer to hold front pcb in queue to run next
struct pcb* temp_pcb = NULL;

struct context* first_context_ptr =  NULL;

// global/static context pointer representing context from first time sys_call() is entered
struct context* global_context_ptr = NULL;

struct context* sys_call(struct context* context_ptr) // context passed in is comhand context
{
    if(first_context_ptr == NULL)
    {
        first_context_ptr = context_ptr;
    }
    
    if(global_context_ptr != context_ptr)
    {
        global_context_ptr = context_ptr;
    }

    global_context_ptr->EAX = 1;

    // if operation code is IDLE
    if(global_context_ptr->EAX == IDLE)
    {
        // check for PCBs in ready not suspended queue
        if(global_ready_queue->front != NULL)
        {
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
            return (struct context*)next_process->process_ptr->stack_ptr;
        }

        // if ready not suspended queue is empty, continue with current process
        return context_ptr;
    } 
    else if (global_context_ptr->EAX == EXIT) 
    {
        // delete currently running pcb
        if(global_current_process != NULL)
        {
            delete_pcb(global_current_process->name_arr);
        }

         // check for PCBs in ready not suspended queue
        if(global_ready_queue->front != NULL)
        {
            // remove first from queue and store in temp variable
            temp_pcb = global_ready_queue->front->pcb;
            pcb_remove(temp_pcb);

            // save context of current PCB by updating stack pointer
            context_ptr->EAX = IDLE;
            global_current_process = temp_pcb;

            // return pointer to stack, which contains context of process to be run next
            return temp_pcb->process_ptr->stack_ptr;
        }

        // if ready not suspended queue is empty
        return context_ptr;
    }
    
    struct context* test = global_ready_queue->front->pcb->process_ptr->stack_ptr;
    context_ptr->EAX = -1;
    return test;

}

