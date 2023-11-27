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
    // must define global device array somewhere!?! 
    // 
    // check if device is busy
    if (device_dcb->allocation_status == 0)
    {
        // device is not busy, process request immediately
        device_dcb->allocation_status = 1; // set device to busy

        struct iocb* iocb = sys_alloc_mem(sizeof(struct iocb));
     //   iocb->IO_pcb = *global_current_process;
        iocb->IO_dcb = *device_dcb;
        iocb->IO_op = (context_ptr->EAX == READ) ? READ : WRITE;
        memcpy(iocb->buf, (char*)context_ptr->ECX, context_ptr->EDX);
        iocb->buf_size = context_ptr->EDX;
        
        // call appropriate driver procedure
        if (context_ptr->EAX == READ)
        {
            serial_read(device_id, iocb->buf, iocb->buf_size);
        }
        else if (context_ptr->EAX == WRITE)
        {
            serial_write(device_id, iocb->buf, iocb->buf_size);
        }

        // free memory associated with iocb
        sys_free_mem(iocb);

    }
    else // device is busy, must add to waiting queue
    {
        struct iocb* iocb = sys_alloc_mem(sizeof(struct iocb));
      //  iocb->IO_pcb = *global_current_process;
        iocb->IO_dcb = *device_dcb;
        iocb->IO_op = (context_ptr->EAX == READ) ? READ : WRITE;
        memcpy(iocb->buf, (char*)context_ptr->ECX, context_ptr->EDX);
        iocb->buf_size = context_ptr->EDX;

        // enqueue
        // iocb->next = global_ready_queue;
        // global_ready_queue = iocb;
    }
    // go back to sys_call to dispatch next process
}

