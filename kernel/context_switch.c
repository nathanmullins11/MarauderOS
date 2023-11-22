#include "memory.h"
#include "mpx/io.h"
#include "processes.h"
#include <sys_req.h>
#include <context_switch.h>
#include <string.h>
#include <comhand.h>
#include <interrupt_control.h>

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

struct context* sys_call(struct context* context_ptr)
{
    // reset global current process to NULL when queue is empty
    if(global_ready_queue->front == NULL)
    {
        global_current_process = NULL;
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
    } else if (context_ptr->EAX == WRITE)
    {
        // Ensure that the input parameters are valid
        int dno = context_ptr->EBX;
        char* buf = (char*)context_ptr->ECX;
        size_t len = (size_t)context_ptr->EDX;

        // Ensure that the port is currently open and idle
        // if (dcb_array[dno] == NULL)
        // {
        //     return context_ptr; // Handle the error appropriately
        // }

        struct dcb* temp_dcb = NULL; //dcb_array[dno];

        if (temp_dcb->cur_op != IDLE)
        {
            return context_ptr; // Handle the error appropriately
        }

        // Install the buffer pointer and counters in the DCB, and set the current status to writing
        temp_dcb->rw_buf = buf;
        temp_dcb->rw_buf_length = len;
        temp_dcb->cur_op = WRITE;

        // clear the caller's event flag
        temp_dcb->event_flag = 0;

        // Get the first character from the requestor's buffer and store it in the output register
        outb(dno, temp_dcb->rw_buf[temp_dcb->rw_index]);
        temp_dcb->rw_index++;

        // Enable write interrupts by setting bit 1 of the Interrupt Enable register.
        // This must be done by setting the register to the logical OR of its previous contents and 0x02.
        outb(dno + IER, inb(dno + IER) | 0x02);

        // Move the process to BLOCKED state and dispatch a new process as though the requested operation was IDLE
        // Note: You need to implement the I/O scheduler for this part
        // For simplicity, we'll just return the current process context for now
        return context_ptr;

    } else if (context_ptr->EAX == READ)
    {
        // Similar logic as for WRITE operation
        // ...

        // Move the process to BLOCKED state and dispatch a new process as though the requested operation was IDLE
        // Note: You need to implement the I/O scheduler for this part
        // For simplicity, we'll just return the current process context for now
        return context_ptr;
    }
    
    context_ptr->EAX = -1;
    return context_ptr;

}

