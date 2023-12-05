/**
 * @file interrupt_control.h
 * @brief functions for handling and controlling interrupts
 */

#include <pcb.h>
#include <sys_req.h>

/**
 * @brief struct to describe a device control block (dcb)
 * read operation is an in buffer
 * write operation is an out buffer
 */
struct dcb {
    /* device vars */
    int device;              // associated device with dcb
    int allocation_status;   // is resource in use? (busy=1)
    op_code cur_op;       // WRITE, READ, IDLE, EXIT
    int event_flag;         // event needs handled (1=I/0 needs handled)

    /* vars for ring buffer */
    /*
     * Ring buffer conditions 
     *   tail = head => array empty
     *   tail - 1 = head => array full
     *   tail < head => not empty 
     */
    char ring_buf[20];
    int ring_head;
    int ring_tail; 
    int ring_buf_size;              // # of requested chars or buffer size
    int ring_chars_transferred;   // # of chars already transferred

    // pointer to head of queue storing IOCB
    struct iocb_queue* IOCBs; 
    
    /* vars for r/w buffer */
    char *rw_buf; 
    int rw_buf_length;
    int rw_index; 

    // store current process
    struct pcb* pcb_ptr; 
};

/**
 * @brief struct to describe an i/o control block (iocb)
 */
struct iocb {
    struct pcb* IO_pcb;
    struct dcb* IO_dcb;
    op_code IO_op;      // i.e. READ or WRITE
    char* buffer;
    int buf_size;
};

/**
 * @brief queue to hold iocb nodes
 */
struct iocb_queue {
    struct iocb_node* front;
    struct iocb_node* rear;
};

/**
 * @brief node to hold iocb infomation
 * 
 */
struct iocb_node {
    struct iocb_node* next;
    struct iocb_node* prev;
    struct iocb* iocb;
};

/**
 * @brief global variable for flag identifying if an event needs handled
 */
extern int global_event_flag;

/**
 * @brief global array to hold dcb for each of the four devices (COM1 - COM4)
 */
extern struct dcb* dcb_array[4];

/**
 * @brief init serial port associated with the device parameter
 * init DCB, install ISR, set port speed, set other line characteristics, enable interrupts
 * 
 * @param dev device to open and setup
 * @param speed baud rate for the device
 * @return int 1 for success, otherwise failure
 */
int serial_open(device dev, int speed);

/**
 * @brief end a serial port session
 * 
 * @param dev device to close
 * @return int 1 for success, otherwise failure
 */
int serial_close(device dev);

/**
 * @brief read input from the device
 * 
 * @param dev device to read from
 * @param buf buffer to read into
 * @param len size of buffer to read into
 * @return int number of characters read
 */
int serial_read(device dev, char *buf, size_t len);

/**
 * @brief write onto a device
 * 
 * @param dev device to write to
 * @param buf buffer to write from
 * @param len size of the buffer to write from
 * @return int 0 on success, otherwise failure
 */
int serial_write(device dev, char *buf, size_t len);

/**
 * @brief first level interrupt handler to determine source of interrupt 
 */
void serial_interrupt(void);

/**
 * @brief second level handler for the input interrupt 
 * 
 * @param dcb dcb associated with device that caused interrupt
 */
void serial_input_interrupt(struct dcb *dcb);

/**
 * @brief second level handler for the output interrupt
 * 
 * @param dcb dcb associated with device that caused interrupt
 */
void serial_output_interrupt(struct dcb *dcb);

/**
 * @brief remove specified pcb from dcb's iocb queue
 * 
 * @param pcb process control block to remove
 * @param dcb device control bock to remove pcb from
 */
void iocb_remove(struct pcb* pcb, struct dcb* dcb);

/**
 * @brief Create a iocb queue object
 * 
 * @return struct iocb_queue* 
 */
struct iocb_queue* create_iocb_queue(void);

/**
 * @brief Create a iocb node object
 * 
 * @param iocb iocb to be associated with node
 * @return struct iocb_node* 
 */
struct iocb_node* create_iocb_node(struct iocb* iocb);

/**
 * @brief returns device number from associated device 
 * 
 * @param dev device to get device number from
 * @return int device number, -1 if doesn't exist
 */
extern int serial_devno(device dev);

int check_cur_dcb(void);
