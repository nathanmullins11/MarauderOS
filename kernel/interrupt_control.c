#include "memory.h"
#include "mpx/device.h"
#include "mpx/io.h"
#include "sys_req.h"
#include <interrupt_control.h>
#include <mpx/interrupts.h>
#include <mpx/serial.h>
#include <comhand.h>
#include <string.h>
#include <mpx/device.h>
#include <context_switch.h>

// declare assembly function
extern void serial_isr(void*);

int i = 0;

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

int serial_devno(device dev)
{
	switch (dev) {
        case COM1: return 0;
        case COM2: return 1;
        case COM3: return 2;
        case COM4: return 3;
	}
	return -1;
}

int serial_open(device dev, int speed)
{
    // variable for array of devices
    int COM_num;

    // check if dev is valid, i.e. COM1, COM2, COM3, or COM4
    int dno = serial_devno(dev);
	if (dno == -1) {
        // invalid event flag pointer
		return -101; 
	}

    int COM_state;
    //(void)COM_state;

    if(dno == 0)
    {
        COM_state = 0x24; // COM1 interrupt vector
        if(dcb_array[0] != NULL) // if dev is open
        {
            return -103;
        }
        COM_num = 0;
    } else if (dno == 1) {
        COM_state = 0x23; // COM2 interrupt vector
        if(dcb_array[1] != NULL) // if dev is open
        {
            return -103;
        }
        COM_num = 1;
    } else if (dno == 2) {
        COM_state = 0x24; // COM3 interrupt vector
        if(dcb_array[2] != NULL) // if dev is open
        {
            return -103;
        }
        COM_num = 2;
    } else {
        COM_state = 0x23; // COM4 interrupt vector
        if(dcb_array[3] != NULL) // if dev is open
        {
            return -103;
        }
        COM_num = 3;
    }

    // speed=baud_rate, check if valid: accepted rates are 110, 150, 300, 600, 1200, 2400, 4800, 9600, 19200
    if(speed != 110 && speed != 150 && speed != 300 && speed != 600 && speed != 1200 && speed != 2400 && speed != 4800 && speed != 9600 && speed != 19200)
    {
        return -102; // code for invalid baud rate divisor
    }

    // initialize dcb for device after checks have passed
    struct dcb* dev_dcb = (struct dcb*)sys_alloc_mem(sizeof(struct dcb));
    dev_dcb->rw_buf = (char*)sys_alloc_mem(600);
    struct iocb_queue* temp_q = create_iocb_queue();
    dev_dcb->IOCBs = temp_q;

    if (dev_dcb == NULL)
    {
        // error allcating mem for dcb
        return 0; 
    }

    // initialize DCB for dev
    dev_dcb->device = dno; // set device in use to corresponding COM
    dev_dcb->event_flag = 0; // set event flag to 0;
    dev_dcb->cur_op = IDLE; // set initial status IDLE

    /* initialize buffer parameters */
    dev_dcb->ring_chars_transferred = 0;
    dev_dcb->ring_head = 0;
    dev_dcb->ring_tail = 0;
    dev_dcb->ring_buf_size = sizeof(dev_dcb->ring_buf);
    dev_dcb->rw_buf_length = 0;
    dev_dcb->rw_index = 0;

    // place created dcb in appropriate index
    dcb_array[COM_num] = dev_dcb; 

    idt_install(COM_state, serial_isr);

    // Compute the required baud rate - formula given
    int baud_rate_div = 115200 / (long) speed;

    // set line control register
    outb(dev + LCR, 0x80);

    // store high and low order bytes of baud rate divisor into MSB and LSB registers respectively
    outb(dev + DLL, baud_rate_div & 0xFF); // LSB
    outb(dev + DLM, (baud_rate_div >> 8) & 0xFF); // MSB

    // set Line Control Register again
    outb(dev + LCR, 0x03); 

    // Enable appropriate PIC mask register
    cli(); // Disable interrupts to prevent any issues during modification
    int mask = inb(0x21); // Read current mask value
    mask &= ~(0x10);
    outb(0x21, mask); // Write modified mask value back to the PIC
    sti(); // Enable interrupts again

    // set MCR register to 0x08
    outb(dev + MCR, 0x08); 

    // set IER to 0x01    
    outb(dev + IER, 0x01);

    return 1;
}

int serial_close(device dev) {

    // check if dev is valid, i.e. COM1, COM2, COM3, or COM4
    int dno = serial_devno(dev);
	if (dno == -1) {
		return 1;
    }

    // check if the device is open
    if (dcb_array[dno] == NULL) {
        print("-201   serial port not open");
        return 1;  
    } else {
        dcb_array[dno] = NULL;
    }

    // Disable appropriate PIC mask register
    cli(); // Disable interrupts to prevent any issues during modification
    int mask = inb(0x21); // Read current mask value
    mask |= (1 << 7); // Enable hardware IRQ 8 (assuming it's represented by bit 7)
    outb(0x21, mask); // Write modified mask value back to the PIC
    sti(); // Enable interrupts again

    // Disable all Interrupts in ACC
    outb(dev + IER, 0x00);   // Clear Interrupt Enable Register
    outb(dev + MSR, 0x00);   // Clear Modem Status Register

    return 0;
}

int serial_read(device dev, char *buf, size_t len)
{
    /* step 1: validate parameters */
    // check if dev is valid, i.e. COM1, COM2, COM3, or COM4
    int dno = serial_devno(dev); 
	if (dno == -1)
    {
        // invalid event flag pointer
		return -101;
	}

    // check if buf address is NULL
    if(buf == NULL)
    {
        // invalid buffer address
        return -302; 
    }

    // len must be > 0 
    if(len < 0)
    {
        // invalid count address/ count value
        return -304; 
    }

    /* step 2: ensure port open and status = IDLE */
    // check index of array associated with dev; if NULL, port is closed
    if(dcb_array[dno] == NULL) 
    {
        // code for port not open
        return -301; 
    }

    // set temp var for dcb
    struct dcb* temp_dcb = dcb_array[dno];

    // if status of dcb is not idle
    if (temp_dcb->cur_op != IDLE) 
    {
        // device busy, i.e. not idle
        return -304; 
    }

    /* step 3: initialize input buf variables and set status to reading */
    temp_dcb->rw_buf_length = 0; // # of chars to be read
    temp_dcb->rw_index = buf[0]; // set index to first # in buf array
    temp_dcb->cur_op = READ; // set status to reading

    /* step 4: clear the callers flag event */
    // clear event flag
    temp_dcb->event_flag = 0; 

    /* step 4: copy chars from ring bug to rw buf */
    cli();
    for(size_t i = 0; i < strlen(temp_dcb->ring_buf); i++)
    {
        // has requested count of chars been reached?
        if(i == sizeof(temp_dcb->ring_buf))
        {
            break;
        }

        // has an enter key been found?
        if(temp_dcb->ring_buf[i] == 13)
        {
            break;
        }

        // transfer char
        buf[i] = temp_dcb->ring_buf[i];

        // empty associated index
        temp_dcb->ring_buf[i] = '\0';

        // update variables associated with ring buf & buf
        temp_dcb->ring_chars_transferred++;
        temp_dcb->rw_buf_length++;
        temp_dcb->rw_index++;
    }

    // re-enable interrupts
    sti(); 

    /* step 7: If more characters are needed, return. If the block is complete, continue with step 7 */
    if((size_t) temp_dcb->rw_buf_length < len)
    {
        // rw_buf_length = # of chars transfered, so if it less than len more chars are needed
        return 0; 
    }

    /* step 7: set dcb status to idle, set event flag, return actual count */
    temp_dcb->cur_op = IDLE; // set status IDLE
    temp_dcb->event_flag = 1; // set event flag
    temp_dcb->allocation_status = 0; 

    return temp_dcb->rw_buf_length;
}

int serial_write(device dev, char *buf, size_t len)
{
    /* step 1: validate parameters */
    int dno = serial_devno(dev);
    
    // check if dev is valid, i.e. COM1, COM2, COM3, or COM4
	if (dno == -1) {
        // invalid event flag pointer
		return -101;
	}

    // check if buf address is NULL
    if(buf == NULL)
    {
        // invalid buffer address
        return -302; 
    }

    // len must be > 0 
    if(len < 0)
    {
        // invalid count address/ count value
        return -304; 
    }

    /* step 2: ensure port open and status=IDLE */
    // check index of array associated with dev; if NULL, port is closed
    if(dcb_array[dno] == NULL) 
    {
        // code for port not open
        return -301; 
    }

    // set temp var for dcb
    struct dcb* temp_dcb = dcb_array[dno];
    
    /* step 3: Install the buffer pointer and counters in the DCB, and set the current status to writing */
    temp_dcb->rw_index = 0; // Set index to 0 to start from the beginning of the buffer
    temp_dcb->rw_buf_length = len; // Store the length of the buffer
    memcpy(temp_dcb->rw_buf, buf, len); // Install the buffer pointer
    temp_dcb->cur_op = WRITE; // Set the current operation to WRITE

    /* step 4: cleat the caller's event flag */
    // clear event flag
    temp_dcb->event_flag = 0; 

    /* step 5: Get the first character from the requestor’s buffer and store it in the output register */
    outb(dev, temp_dcb->rw_buf[temp_dcb->rw_index]);
    temp_dcb->rw_index++;

    // Set the bit 1 by logical OR with 0x02 and write back to the register
    cli();

    /* step 6: Enable write/read interrupts by setting bit 1 of the Interrupt Enable register */
    // Retrieve the current value of the Interrupt Enable register
    int current_ier = inb(dev + IER);
    outb(dev + IER, current_ier | 0x02);
    sti();

    return 0;
}


void serial_interrupt(void) {
    // Read Interrupt ID
    int interrupt_ID = inb(COM1 + IIR) & 7; 
    if((interrupt_ID & 1) != 0)
    {
        return;
    }

    // store current dcb into temp pointer
    int dcb_index = check_cur_dcb();
    // if invalid index, return
    if(dcb_index < 0)
    {
        return;
    }
    
    struct dcb* temp_dcb = dcb_array[dcb_index];

    // Check bit 1 and 2 for correct interrupt transfer
    if (interrupt_ID >> 1 == 0 && interrupt_ID >> 2 == 0) { 
        // Modem Status Interrupt
        inb(COM1+MSR);
    } else if (((interrupt_ID >> 1) == 1) && ((interrupt_ID >> 2) == 0)) { 
        // Output Interrupt
        serial_output_interrupt(temp_dcb);
    } else if ( interrupt_ID == 4 ) {
        // Input Interrupt
        serial_input_interrupt(temp_dcb);
    } else if (interrupt_ID >> 1 == 1 && interrupt_ID >> 2 == 1) {
        // Line Status Interrupt
        inb(COM1+LSR);
    } 

    // clear interrupt
    outb(0x20,0x20); 
}

void serial_input_interrupt(struct dcb *dcb) {
    int cursor = dcb->rw_index;

    // read a character from the input register
    char in_char = inb(COM1);

    /* KEYBOARD HANDLING */
    // backspace
    if (in_char == 127 || in_char == '\177') {
        if (dcb->rw_index > 0 && cursor > 0) {
            // move cursor back
            outb(COM1, '\b');
            cursor--;

            // shift characters in buffer left
            for (int j = cursor; j < dcb->rw_index - 1; j++) {
                dcb->rw_buf[j] = dcb->rw_buf[j + 1];
                dcb->ring_buf[j] = dcb->ring_buf[j+1];
                outb(COM1, dcb->rw_buf[j]);  // print updated char to terminal
            }

            // overwrite last char with space
            dcb->rw_buf[dcb->rw_index - 1] = ' ';
            dcb->ring_buf[dcb->rw_index - 1] = ' ';
            outb(COM1, ' ');

            // move cursor back to correct position
            for (int temp_cursor = dcb->rw_index; temp_cursor > cursor; temp_cursor--) {
                outb(COM1, '\b');
            }

            // decrement index
            dcb->rw_index--;
            dcb->ring_chars_transferred--;
            // place null terminator at end of buffer
            dcb->rw_buf[dcb->rw_index] = '\0';
            dcb->ring_buf[dcb->ring_chars_transferred] = '\0';
        }
    }

    // Arrow keys, delete key, esc key
    else if (in_char == '\033' || in_char == 27) {
        // discard these keys for now
        inb(COM1);
        inb(COM1);
        
    }

    // check for enter sequence
    else if(in_char == '\r')
    {
        // set event flag high 
        dcb->event_flag = 1;

        // set status to idle
        dcb->cur_op = IDLE;

        // if ring buffer not empty, put contents in rw buf
        // Assuming dcb_ring is a pointer to your DCB structure
        if (dcb->ring_chars_transferred > 0) {
            // Copy the contents of ring_buf to dcb_rw_buf
            memcpy(dcb->rw_buf, dcb->ring_buf, dcb->ring_chars_transferred);

            // Update other relevant information in dcb_rw_buf
            dcb->rw_buf_length = dcb->ring_chars_transferred;
            dcb->rw_index = 0;

            // Reset ring_buf since you've transferred its contents
            dcb->ring_chars_transferred = 0;
            dcb->ring_head = 0;
            dcb->ring_tail = 0;
            dcb->ring_buf_size = sizeof(dcb->ring_buf);
        }

        // clear iocb queue;
        dcb->IOCBs->front->iocb = NULL;

        outb(COM1, '\n');

        return;
    }
    
    // other chars
    else if (in_char > 0) {
        if (dcb->rw_index == 0) {
            outb(COM1, in_char);
            dcb->rw_buf[cursor] = in_char;
            dcb->rw_index++;
            cursor++;
        } else {
            // move characters to the right of the cursor
            for (int j = dcb->rw_index; j >= cursor; j--)
            {
                dcb->rw_buf[j + 1] = dcb->rw_buf[j];
            }

            dcb->rw_buf[cursor] = in_char;

            // print the updated characters
            for (int j = cursor; j <= dcb->rw_index; j++)
            {
                outb(COM1, dcb->rw_buf[j]);
            }

            // move cursor back to correct position
            for (int temp_cursor = dcb->rw_index; temp_cursor > cursor; temp_cursor--)
            {
                outb(COM1, '\b');
            }

            // increment index and cursor
            dcb->rw_index++;
            cursor++;
        }
    }

    // check current status
    if (dcb->cur_op != READ) {
        // if the current status is not READ, store char in the ring buffer
        // check if ring is full 
        if ( (dcb->ring_tail - 1) == dcb->ring_head ) {
            // if full, then discard character
        } else if (in_char == '\r' || in_char == '\177' || in_char == '\033') { 
            // do not add escape characters
        } else {
            // otherwise, put in ring buffer
            dcb->ring_buf[dcb->ring_chars_transferred] = in_char;
            dcb->ring_chars_transferred++;
        }

        // return to first level handler, with no indication of complete
        return;
    } else {
        // current status is READ, store in requestors input buffer
       // int dev = dcb->device;
        if (in_char != '\r' || in_char != '\177') {
           // dcb_array[dev]->rw_buf[i] = in_char;
        }
       // i++;
    }

    // check if count complete and character is not new line
    if ((dcb->ring_chars_transferred == dcb->ring_buf_size) && (strcmp(&in_char, "\n") != 0)) {
        // return without indication of completion
        return;
    } else {
        // transfer complete, set to IDLE and set event flag
        dcb->cur_op = IDLE;
        dcb->event_flag = 0;
        return;
    }
}

void serial_output_interrupt(struct dcb *dcb) {
    // check if status is not writing
    if (dcb->cur_op != WRITE) {
        // return with no indication of complete
        return;
    }

    // check if count has not been exhausted
    if (dcb->rw_index < dcb->rw_buf_length) {
        // get requestors char and store it in output register
        int dev = dcb->device;
        dcb->rw_buf = dcb_array[dev]->rw_buf;
        outb(COM1, dcb->rw_buf[dcb->rw_index]);
        dcb->rw_index++;

        return;
    } else {
        // otherwise all characters have been transferred
        dcb->cur_op = IDLE;
        dcb->event_flag = 1;

        int dev = dcb->device;
        outb(dev + IER, 0x00);

        // set allocation status of device back to open
        dcb->allocation_status = 0;

        dcb->IOCBs->front->iocb = NULL;

        memset(dcb->rw_buf, 0, strlen(dcb->rw_buf));

        // reset the indexes for buffer
        dcb->rw_index = 0;
        dcb->ring_chars_transferred = 0;
        dcb->ring_head = 0;
        dcb->ring_tail = 0;

        return;
    }
}

struct iocb_queue* create_iocb_queue(void) {
    // Allocate memory for the queue structure
    struct iocb_queue* new_queue = (struct iocb_queue*)sys_alloc_mem(sizeof(struct iocb_queue));

    // Initialize the front and rear pointers to NULL since the queue is empty
    if (new_queue != NULL) {
        new_queue->front = NULL;
        new_queue->rear = NULL;
    }

    // Return the pointer to the new queue
    return new_queue; 
}


struct iocb_node* create_iocb_node(struct iocb* iocb) 
{
    struct iocb_node* new_node = (struct iocb_node*)sys_alloc_mem(sizeof(struct iocb_node));

    if (new_node == NULL) {
        // Handle memory allocation failure
        return NULL;
    }

    new_node->iocb = iocb;
    new_node->next = NULL;
    new_node->prev = NULL;

    return new_node;
}

void iocb_remove(struct pcb* pcb, struct dcb* dcb)
{
    // if no other iocbs are in queue
    if (dcb->IOCBs->front == NULL) {
        return; // Queue is already empty
    }

    // Check if the first node contains the PCB to be removed
    if (dcb->IOCBs->front->iocb->IO_pcb == pcb) {
        struct iocb_node* temp = dcb->IOCBs->front;
        dcb->IOCBs->front = temp->next;
        sys_free_mem(temp->iocb); // Free memory allocated for iocb
        sys_free_mem(temp); // Free memory allocated for iocb_node
        return;
    }

    // Otherwise, iterate through the queue to find the matching PCB
    struct iocb_node* current = dcb->IOCBs->front;

    while (current->next != NULL) {
        if (current->next->iocb->IO_pcb == pcb) {
            struct iocb_node* temp = current->next;
            current->next = temp->next;
            sys_free_mem(temp->iocb); // Free memory allocated for iocb
            sys_free_mem(temp); // Free memory allocated for iocb_node
            return;
        }
        current = current->next;
    }
}

int check_cur_dcb(void)
{
    if(dcb_array[0] != NULL)
    {
        return 0;
    } else if (dcb_array[1] != NULL) {
        return 1;
    } else if (dcb_array[2] != NULL) {
        return 2;
    } else if (dcb_array[3] != NULL) {
        return 3;
    } else {
        return -1;
    }
}
