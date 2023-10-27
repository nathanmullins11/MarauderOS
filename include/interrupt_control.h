/**
 * @file interrupt_control.h
 * @brief functions for handling and controlling interrupts
 */

// global variable for flag identifying if an event needs handled
 int global_event_flag;

 #include <pcb.h>
 #include <sys_req.h>

struct dcb {
    int allocation_status; // is resource in use, i.e. is device COM1 busy (1) or not busy (0)
    op_code cur_op; // WRITE, READ, IDLE, EXIT
    int event_flag; // event needs handled - used by sys_call to handle an I/O request (1) if I/O needs to be handled, (0) otherwise
    /* vars for ring buffer and discriptors */
    char ring_buf[12]; // add at tail, tail = head when array empty, tail is 1 less than head when full, tail < head when not full or empty
    int ring_head;
    int ring_tail;
    int buf_size; // # of requested chars or buffer size
    int chars_transferred; // # of chars already transferred
};

struct IOCB {
    struct pcb IO_pcb;
    struct dcb IO_dcb;
    op_code IO_op; // i.e. READ or WRITE
    char buf[100];
    int buf_size;
};

