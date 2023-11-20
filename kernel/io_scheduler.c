#include <interrupt_control.h>
#include "memory.h"
#include "processes.h"
#include <sys_req.h>
#include <context_switch.h>
#include <string.h>
#include <pcb.h>
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
    struct dcb* device_dcb = &global_device_array[device_id];
    // must define global device array somewhere!?! 
    // 

    // check if device is busy
    if (device_dcb->allocation_status == 0)
    {
        // device is not busy, process request immediately
        device_dcb->allocation_status = 1; // set device to busy

        struct iocb* iocb = NULL;
        iocb->IO_pcb = *global_current_process;
        iocb->IO_dcb = *device_dcb;
        iocb->IO_op = (context_ptr->EAX == READ) ? READ : WRITE;

    }

}