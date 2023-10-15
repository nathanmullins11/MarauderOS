#include "memory.h"
#include "processes.h"
#include <sys_req.h>
#include <context_switch.h>
#include <string.h>
#include <pcb.h>
#include <comhand.h>

/* global PCB pointer for currently running process */
struct pcb* global_current_process = NULL;

/* global PCB pointer for first context loaded with first system call */
struct context* first_context_ptr =  NULL;

/* global/static context pointer representing context from first time sys_call() is entered */
struct context* global_context_ptr = NULL;

struct context* sys_call(struct context* context_ptr)
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
        if(global_ready_queue->front != NULL)
        {
            struct pcb* current_process = global_current_process;

            // remove first from queue and store in temp variable
            struct pcb* next_process = global_ready_queue->front->pcb;
            pcb_remove(next_process);
            
            // increment global current process
            global_current_process = next_process;

           // add the current PCB back to the ready queue
            if(current_process != NULL)
            {
                // update stack pointer
                current_process->process_ptr->stack_ptr = context_ptr;
                // insert the currently running process back into appropriate queue so next process can run
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
        // delete currently running pcb by freeing memory
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
            return (struct context*)temp_pcb->process_ptr->stack_ptr;
        }

        // if ready not suspended queue is empty i.e last process running issues an exit requests and no processes are left in ready queue
        return first_context_ptr;
    }
    
    context_ptr->EAX = -1;
    return context_ptr;

}

