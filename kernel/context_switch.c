#include "memory.h"
#include "mpx/device.h"
#include "mpx/io.h"
#include "processes.h"
#include <sys_req.h>
#include <string.h>
#include <comhand.h>
#include <interrupt_control.h>
#include <io_scheduler.h>

/* global PCB pointer for currently running process */
struct pcb* global_current_process = NULL;

/* global/static context pointer representing context from first time sys_call() is entered */
struct context* global_context_ptr = NULL;

enum uart_registers {
	RBR = 0,	// Receive Buffer
	THR = 0,	// Transmitter Holding
	DLL = 0,	// Divisor Latch LSB
	IER = 1,	// Interrupt Enable
	DLM = 1,	// Divisor Latch MSB
	IIR = 2,	// Interrupt Identification
	FCR = 2,	// FIFO Control
	LCR = 3,	// Line Control
	MCR = 4,	// Modem Control
	LSR = 5,	// Line Status
	MSR = 6,	// Modem Status
	SCR = 7,	// Scratch
};

// global variables for DCB of dev
struct dcb* dcb_array[4] = {NULL,NULL,NULL, NULL}; // COM1, COM2, COM3, COM4

struct context* sys_call(struct context* context_ptr)
{
    // reset global current process to NULL when queue is empty
    // if(global_ready_queue->front == NULL)
    // {
    //     global_current_process = NULL;
    // }

    int dev_int = context_ptr->EBX;
    int array_position = serial_devno(dev_int);

    // char* buffer = (char*)context_ptr->ECX;
    // int buf_len = context_ptr->EDX;
    struct dcb* temp_dcb = dcb_array[array_position];

    // check all dcbs for io completion status if so unblock pcb and either put back in ready queue or execute process
    // check for event flags
    if(temp_dcb != NULL && temp_dcb->event_flag == 1)
    { 
        // remove finished io process from blocked queue, change state to ready, and place back in ready queue
        if(temp_dcb->IOCBs->front != NULL)
        {
            struct pcb* finished_pcb = temp_dcb->pcb_ptr;
            // remove paused process from blocked queue
            pcb_remove(finished_pcb);
            finished_pcb->process_ptr->pcb_state = READY_NOT_SUSPENDED;
            pcb_insert(finished_pcb);

            // set event flag low to signal io completion has been handled
            temp_dcb->event_flag = 0;

            // place paused process back into ready queue
            return (struct context*) global_current_process->process_ptr->stack_ptr;
        }
    }

    // if operation code is IDLE
    if(context_ptr->EAX == IDLE)
    {
        if(global_context_ptr == NULL)
        {
            global_context_ptr = context_ptr;
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

            // set next_process to running state
            //next_process->process_ptr->pcb_state = RUNNING;
            
            // return pointer to stack, which contains context of process to be run next
            return (struct context*)next_process->process_ptr->stack_ptr;
        }

        // if ready not suspended queue is empty, continue with current process
        return context_ptr;
    } 
    else if (context_ptr->EAX == EXIT) 
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
        return global_context_ptr;
    }  else if (context_ptr->EAX == WRITE)
    {
        // save context_ptr into current pcb - stack top
        global_current_process->process_ptr->stack_ptr = context_ptr;
        // set status of PCB to blocked 
        global_current_process->process_ptr->pcb_state = BLOCKED_NOT_SUSPENDED;
        // place current pcb into blocked queue
        pcb_insert(global_current_process);
        /* device is located in EBX as int, i.e. COM1 = 1016 = 0x3f8, buffer is in ECX, buffer size is in EDX */
        // set variables for each
        if(temp_dcb == NULL)
        {
            return context_ptr;
        }

        // call io scheduler
        io_scheduler(context_ptr);

        // return blocked process context


        // check if device is busy
        // if(temp_dcb->allocation_status == 0)
        // {
            // device not busy, call write driver function
         //   io_scheduler(context_ptr);
            // check for i/o completion
            // if(temp_dcb != NULL && temp_dcb->event_flag == 1)
            // { 
            //     // remove finished io process from blocked queue, change state to ready, and place back in ready queue
            //     if(temp_dcb->pcb_ptr != NULL)
            //     {
            //         // get pcb from dcb
            //         struct pcb* finished_pcb = temp_dcb->pcb_ptr;
            //         // remove from blocked queue
            //         pcb_remove(finished_pcb);
            //         // set status to ready
            //         finished_pcb->process_ptr->pcb_state = READY_NOT_SUSPENDED;
            //         // insert back into appropriate queue
            //         pcb_insert(finished_pcb);
            //     }
            // }
            // // if dcb is done writing characters
            // if(temp_dcb->rw_index == temp_dcb->rw_buf_length)
            // {
            //     struct pcb* current_process = global_current_process;
            //     if(global_ready_queue->front != NULL)
            //     {
            //         // get next process to execute
            //         struct pcb* next_process = global_ready_queue->front->pcb;
            //         pcb_remove(next_process);
                    
            //         // increment global current process
            //         global_current_process = next_process;
            //     } else {
            //         return global_context_ptr;
            //     }

            //     // add the current PCB back to the ready queue
            //     if(current_process != NULL)
            //     {
            //         // update stack pointer
            //         current_process->process_ptr->stack_ptr = context_ptr;
            //         // insert the currently running process back into appropriate queue so next process can run
            //         pcb_insert(current_process);
            //     }
            //     struct pcb* next_process = global_current_process;
            //     return (struct context*)next_process->process_ptr->stack_ptr;
            // }
        // } else {
        //     // put current process in blocked state
        //     global_current_process->process_ptr->pcb_state = BLOCKED_NOT_SUSPENDED;
        //     pcb_insert(global_current_process);

        //     // remove first from queue and store in temp variable
        //     struct pcb* temp_pcb = global_ready_queue->front->pcb;
        //     pcb_remove(temp_pcb);

        //     // save context of current PCB by updating stack pointer
        //     global_current_process = temp_pcb;

        //     // device is busy, request must be scheduled by I/O scheduler
        //     io_scheduler(context_ptr);

        //     // return pointer to stack, which contains context of process to be run next
        //     return (struct context*)temp_pcb->process_ptr->stack_ptr;
        // }
    } else if (context_ptr->EAX == READ)
    {
        // save context_ptr into current pcb - stack top
        global_current_process->process_ptr->stack_ptr = context_ptr;
        // set status of PCB to blocked 
        global_current_process->process_ptr->pcb_state = BLOCKED_NOT_SUSPENDED;
        // place current pcb into blocked queue
        pcb_insert(global_current_process);
        /* device is located in EBX as int, i.e. COM1 = 1016 = 0x3f8, buffer is in ECX, buffer size is in EDX */
        // set variables for each
        if(temp_dcb == NULL)
        {
            return context_ptr;
        }

        // call io scheduler
        io_scheduler(context_ptr);


        // /* device is located in EBX as int, i.e. COM1 = 1016 = 0x3f8, buffer is in ECX, buffer size is in EDX */
        // // set variables for each
        // if(temp_dcb == NULL)
        // {
        //     return context_ptr;
        // }
        // // check if device is busy
        // if(temp_dcb->allocation_status == 0)
        // {
        //     // device not busy, call write driver function
        //     serial_read(dev_int, buffer, buf_len);
        // } else {
        //     // put current process in blocked state
        //     global_current_process->process_ptr->pcb_state = BLOCKED_NOT_SUSPENDED;
        //     // move current process to appropriate queue
        //     pcb_insert(global_current_process);

        //     // remove first from queue and store in temp variable
        //     struct pcb* temp_pcb = global_ready_queue->front->pcb;
        //     pcb_remove(temp_pcb);

        //     // save context of current PCB by updating stack pointer
        //     global_current_process = temp_pcb;

        //     // device is busy, request must be scheduled by I/O scheduler
        //     io_scheduler(context_ptr);

        //     // return pointer to stack, which contains context of process to be run next
        //     return (struct context*)temp_pcb->process_ptr->stack_ptr;
        // }
    }
    
    //context_ptr->EAX = -1;
    return context_ptr;

}

