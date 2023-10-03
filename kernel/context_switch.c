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
            struct context* re = next_process->process_ptr->stack_ptr;
            re->CS = 0x08;
            re->DS = 0x10;
            re->ES = 0x10;
            re->FS = 0x10;
            re->GS = 0x10;
            re->SS = 0x10;

            // EPB set to bottom of stack
           re->EBP = (int)&next_process->process_ptr->pcb_stack[PCB_STACK_SIZE];
            // ESP set to top of stack
           re->ESP = (int)&next_process->process_ptr->pcb_stack[0];
            // EIP point to function proc1
            re->EIP = (int)proc1;
            /* all other registers */
            re->EAX = 0;
            re->EBX = 0;
            re->ECX = 0;
            re->EDX = 0;
            re->ESI = 0;
            re->EDI = 0;
            re->EAX = 0;
           // return (struct context*)next_process->process_ptr->stack_ptr;
           return re;
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
    
    context_ptr->EAX = -1;
    return NULL;

}

