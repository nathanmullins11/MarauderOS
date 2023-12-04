#include <interrupt_control.h>
#include "memory.h"
#include "processes.h"
#include <sys_req.h>
#include <context_switch.h>
#include <string.h>
#include <comhand.h>

int validate_io_request(struct context* context_ptr)
{
    // validate I/O operation
    if (context_ptr->EAX != READ && context_ptr->EAX != WRITE)
    {
        return -1; // invalid operation
    }

    int device_id = context_ptr->EBX;
    if (device_id < 0)
    {
        return -2; // invalid device ID
    }

    char* buffer = (char*)context_ptr->ECX;
    int size = context_ptr->EDX;
    if (buffer == NULL || size <= 0)
    {
        return -3; // invalid buffer
    }

    return 0; // valid parameters
}


void io_scheduler(struct context* context_ptr)
{
    // validate sys_call parameters
    int validation = validate_io_request(context_ptr);
    if (validation != 0)
    {
        context_ptr->EAX = validation; // set error code in EAX
        return; // go back to sys_call, which dispatches next process
    }

    int device_id = context_ptr->EBX;
    // device_id = hex value of one of the devices, need to convert to 0-3
    int id_to_arr_pos = serial_devno(device_id);
    struct dcb* device_dcb = dcb_array[id_to_arr_pos];
    // CLEAR BEFORE COPYING MEM OVER
    memset(device_dcb->rw_buf, 0, strlen(device_dcb->rw_buf));

    memcpy(device_dcb->rw_buf, (char*)context_ptr->ECX, context_ptr->EDX);
    device_dcb->rw_buf_length = context_ptr->EDX;
    device_dcb->pcb_ptr = global_current_process;

    // check if device is busy
    if (device_dcb->allocation_status == 0) //&& device_dcb->IOCBs->front == NULL)
    {
        // device is not busy, process request immediately
        device_dcb->allocation_status = 1; // set device to busy

        /* create IOCB to hold paused pcb */
        struct iocb* iocb = sys_alloc_mem(sizeof(struct iocb));
        struct iocb_node* new_node = create_iocb_node(iocb);
        iocb->buffer = (char*)sys_alloc_mem(600);
        iocb->IO_pcb = global_current_process;
        iocb->IO_dcb = device_dcb;
        iocb->IO_op = (context_ptr->EAX == READ) ? READ : WRITE;
        memcpy(iocb->buffer, (char*)context_ptr->ECX, context_ptr->EDX);
        iocb->buf_size = context_ptr->EDX;
        
        // place iocb into queue
        if (device_dcb->IOCBs->front == NULL) {
            // If the queue is empty, add the new node at the front
            device_dcb->IOCBs->front = new_node;
            device_dcb->IOCBs->rear = new_node;
        } else {
            // If the queue is not empty, add the new node at the rear
            device_dcb->IOCBs->rear->next = new_node;
            new_node->prev = device_dcb->IOCBs->rear;
            device_dcb->IOCBs->rear = new_node;
        }

        // Set next of new_node to NULL as it's the last node in the queue
        new_node->next = NULL;
        
        // call appropriate driver procedure
        if (context_ptr->EAX == READ)
        {
            // load next process
            if(global_ready_queue->front != NULL)
            {
                struct pcb* next = global_ready_queue->front->pcb;
                pcb_remove(next);
                global_current_process = next;
            }
            
            serial_read(device_id, device_dcb->rw_buf, device_dcb->rw_buf_length);

        } else if (context_ptr->EAX == WRITE)
        {
            // load next process
            if(global_ready_queue->front != NULL)
            {
                struct pcb* next = global_ready_queue->front->pcb;
                pcb_remove(next);
                global_current_process = next;
            }
            

            serial_write(device_id, device_dcb->rw_buf, device_dcb->rw_buf_length);
        }

    } else {
        struct iocb* iocb = sys_alloc_mem(sizeof(struct iocb));
        struct iocb_node* new_node = create_iocb_node(iocb);
        iocb->IO_pcb = global_current_process;
        iocb->IO_dcb = device_dcb;
        iocb->buffer = (char*)sys_alloc_mem(100);
        iocb->IO_op = (context_ptr->EAX == READ) ? READ : WRITE;
        memcpy(iocb->buffer, (char*)context_ptr->ECX, context_ptr->EDX);
        iocb->buf_size = context_ptr->EDX;
        
        // place iocb into queue
        if (device_dcb->IOCBs->front == NULL) {
            // If the queue is empty, add the new node at the front
            device_dcb->IOCBs->front = new_node;
            device_dcb->IOCBs->rear = new_node;
        } else {
            // If the queue is not empty, add the new node at the rear
            device_dcb->IOCBs->rear->next = new_node;
            new_node->prev = device_dcb->IOCBs->rear;
            device_dcb->IOCBs->rear = new_node;
        }

        // Set next of new_node to NULL as it's the last node in the queue
        new_node->next = NULL;
    }
    // go back to sys_call to dispatch next process
}

