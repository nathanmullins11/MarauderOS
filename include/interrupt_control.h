/**
 * @file interrupt_control.h
 * @brief functions for handling and controlling interrupts
 */

// global variable for flag identifying if an event needs handled
 int global_event_flag;

 #include <pcb.h>
 #include <sys_req.h>

struct dcb {
    int device; // associated device with dcb
    int allocation_status; // is resource in use, i.e. is device COM1 busy (1) or not busy (0)
    op_code cur_op; // WRITE, READ, IDLE, EXIT
    int event_flag; // event needs handled - used by sys_call to handle an I/O request (1) if I/O needs to be handled, (0) otherwise

    /* vars for ring buffer and discriptors */
    char ring_buf[20]; // add at tail, tail = head when array empty, tail is 1 less than head when full, tail < head when not full or empty
    int ring_head; // head of ring buffer
    int ring_tail; // tail of ring buffer
    int ring_buf_size; // # of requested chars or buffer size
    int ring_chars_transferred; // # of chars already transferred

    struct queue* IOCBs; // pointer to head of queue storing IOCB
    char *rw_buf; // pointer buffer to hold chars being read or written
    int rw_buf_length; // length of rw buffer
    int rw_index; // current index in rw buffer

    // read operation is an in buffer
    // write operation is an out buffer
};

struct iocb {
    struct pcb IO_pcb;
    struct dcb IO_dcb;
    op_code IO_op; // i.e. READ or WRITE
    char buf[100];
    int buf_size;
};

/**
 * @brief 
 * 
 * @param dev 
 * @param speed 
 * @return int 
 */
int serial_open(device dev, int speed);

/**
 * @brief 
 * 
 * @param dev 
 * @return int 
 */
int serial_close(device dev);

/**
 * @brief 
 * 
 * @param dev 
 * @param buf 
 * @param len 
 * @return int 
 */
int serial_read(device dev, char *buf, size_t len);

/**
 * @brief 
 * 
 * @param dev 
 * @param buf 
 * @param len 
 * @return int 
 */
int serial_write(device dev, char *buf, size_t len);

/**
 * @brief second level handler for the input interrupt 
 * 
 * @param dcb 
 */
void serial_input_interrupt(struct dcb *dcb);

/**
 * @brief second level handler for the output interrupt
 * 
 * @param dcb 
 */
void serial_output_interrupt(struct dcb *dcb);
