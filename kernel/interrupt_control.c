#include "mpx/device.h"
#include "mpx/io.h"
#include "sys_req.h"
#include <interrupt_control.h>
#include <mpx/interrupts.h>
#include <mpx/serial.h>


// global variables for DCB of dev
struct dcb* dcb_array[4] = {NULL,NULL,NULL, NULL}; // COM1, COM2, COM3, COM4

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

static int serial_devno(device dev)
{
	switch (dev) {
	case COM1: return 0;
	case COM2: return 1;
	case COM3: return 2;
	case COM4: return 3;
	}
	return -1;
}

int serial_open(device dev, int speed) // return 1 for success, anything else for failure
{
    // variable for array of devices
    int COM_num;

    // check if dev is valid, i.e. COM1, COM2, COM3, or COM4
    int dno = serial_devno(dev);
	if (dno == -1) {
		return -101; // invalid event flag pointer ??
	}

    int COM_state;
    (void)COM_state;
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
        // else speed is valid
    }

    struct dcb* dev_dcb = NULL; // initialize dcb for device after checks have passed

    // initialize DCB for dev
    dev_dcb->device = dno; // set device in use to corresponding COM
    dev_dcb->event_flag = 0; // set event flag to 0;
    dev_dcb->cur_op = IDLE; // set initial status IDLE
    /* initialize buffer parameters */
    dev_dcb->ring_chars_transferred = 0;
    dev_dcb->ring_head = 0;
    dev_dcb->ring_tail = 0;
    dev_dcb->ring_buf_size = sizeof(dev_dcb->ring_buf);
    dev_dcb->IOCBs = NULL;
    dev_dcb->rw_buf_length = 0;
    dev_dcb->rw_index = 0;

    // place created dcb in appropriate
    dcb_array[COM_num] = dev_dcb;

   // idt_install(COM_state, *serial_isr()(void*));

    // Compute the required baud rate - formula given
    int baud_rate_div = 115200 / (long) speed;

    outb(dev + LCR, 0x80);	// set line control register

    // store high and low order bytes of baud rate divisor into MSB and LSB registers respectively*/
    outb(dev + DLL, baud_rate_div & 0xFF); // LSB
    outb(dev + DLM, (baud_rate_div >> 8) & 0xFF); // MSB

    outb(dev + LCR, 0x03); // set Line Control Register again

    // Enable appropriate PIC mask register???
    cli(); // Disable interrupts to prevent any issues during modification
    int mask = inb(0x21); // Read current mask value
    mask &= ~(1 << 7); // Enable hardware IRQ 8 (assuming it's represented by bit 7)
    outb(0x21, mask); // Write modified mask value back to the PIC
    sti(); // Enable interrupts again

    // set MCR register to 0x08
    outb(dev + MCR, 0x08); 

    // set IER to 0x01    
    outb(dev + IER, 0x01);

    return 1;
}

