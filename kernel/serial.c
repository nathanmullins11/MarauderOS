#include <mpx/serial.h>
#include <mpx/io.h>
#include <sys_req.h>
#include <string.h>
#include <itoa.h>


#define ENTER_KEY 10
#define B_KEY 98

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

static int initialized[4] = { 0 };

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

int serial_init(device dev)
{
	int dno = serial_devno(dev);
	if (dno == -1) {
		return -1;
	}
	outb(dev + IER, 0x00);	//disable interrupts
	outb(dev + LCR, 0x80);	//set line control register
	outb(dev + DLL, 115200 / 9600);	//set bsd least sig bit
	outb(dev + DLM, 0x00);	//brd most significant bit
	outb(dev + LCR, 0x03);	//lock divisor; 8bits, no parity, one stop
	outb(dev + FCR, 0xC7);	//enable fifo, clear, 14byte threshold
	outb(dev + MCR, 0x0B);	//enable interrupts, rts/dsr set
	(void)inb(dev);		//read bit to reset port
	initialized[dno] = 1;
	return 0;
}

int serial_out(device dev, const char *buffer, size_t len)
{
	int dno = serial_devno(dev);
	if (dno == -1 || initialized[dno] == 0) {
		return -1;
	}
	for (size_t i = 0; i < len; i++) {
		outb(dev, buffer[i]);
	}
	return (int)len;
}

int serial_poll(device dev, char *buffer, size_t len)
{
	/* initialize pointer and index */
	int index = 0;
	int cursor = index;
	size_t count = len;

	while (count > 0) {
		// read char into data register
		unsigned char data = inb(dev);
		// if char is enter key, new line
		if (data == 13)
		{
			buffer[index] = '\0';
			count--;
			index++;
			// new line means cursor starts at beginning
			cursor = 0;
			outb(dev, '\n');

			return index;
		}

		// backspace key
		else if (data == 127)
		{
			outb(dev, '\b');   // Move the cursor back
			outb(dev, ' ');    // Overwrite with a space
			outb(dev, '\b');   // Move the cursor back again
			// move chars after backspace to left
			// for(int i = cursor; i < index; i++)
			// {
			// 	buffer[i-1] = buffer[i];
			// 	outb(dev, buffer[i-1]);  // Print the updated character to the terminal
			// }

			// check if variables are 0
			if(index > 0)
			{
				index--;
			}
			if(cursor > 0)
			{
				cursor--;
			}
		}

		// catches arrows, delete key, esc key
		else if (data == '\033')
		{
			char esc_seq[3];  // Read the next two characters for the escape sequence
			esc_seq[0] = inb(dev);
			esc_seq[1] = inb(dev);
			esc_seq[2] = '\0';

			if (strcmp(esc_seq, "[C") == 0 && (cursor < index)) {
				// Right arrow key pressed
				// Handle right arrow logic here
				outb(dev, '\033');  // Escape character
				outb(dev, '[');     // [
				outb(dev, 'C');     // C
				count--;
				cursor++;
			} else if (strcmp(esc_seq, "[D") == 0 && (cursor > 0)) {
				// Left arrow key pressed
				// Handle left arrow logic here
				outb(dev, '\033');  // Escape character
				outb(dev, '[');     // [
				outb(dev, 'D');     // D
				count--;
				// check if cursor is at position 0
				cursor--;
			} else if (strcmp(esc_seq, "[A") == 0) {
				// up arrow key pressed
				// Handle up arrow logic here
				// implement showing last command
				count--;
			} else if (strcmp(esc_seq, "[B") == 0) {
				// down arrow key pressed
				// Handle down arrow logic here
				// nothing as of now
				count--;
			} else if (strcmp(esc_seq, "[3~") == 0) {
				// delete key pressed
				// Handle delete key logic here
				outb(dev, ' ');
				outb(dev, '\b');
			}
		}
		else if (data > 0)
		{
			// write data to device and data to buffer array
			outb(dev, data);
			buffer[index] = data;

			// decrement count
			count--;

			//increment index
			index++;

			// increment cursor
			cursor++;
		}
	}

	return index;
}
