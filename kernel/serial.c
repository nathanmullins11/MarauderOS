#include <mpx/serial.h>
#include <mpx/io.h>
#include <stddef.h>
#include <sys_req.h>
#include <string.h>
#include <time.h>
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

// function prototype
void add_history_buffer(char* old_buff, char* new_buff);

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

void clear_input_buffer(device dev) {
    while (inb(dev) != 0) {
        // Keep reading and discarding characters until the input buffer is empty
    }
}

// error message for full buffer
char err_buf_msg[53] = "\nBuffer Full Error. Clearing buffer/creating new line";
char histroy_buffer[100];

int serial_poll(device dev, char *buffer, size_t len)
{
	// clear the input buffer so no left over chars are read in
    clear_input_buffer(dev);

	/* initialize pointer, index, and count */
	int index = 0;
	int cursor = index;
	size_t count = len; // keep track of room left in buffer

	// while buffer is not full
	while (count > 0) {

		// read char into data register
		unsigned char data = inb(dev);
		
		// if enter key, new line
		if (data == 13)
		{
			// clear buffer
			buffer[index] = '\0';
			for(size_t i = 0; i < strlen(buffer); i++)
			{
				histroy_buffer[i] = buffer[i];
			}

			histroy_buffer[strlen(buffer)] = '\0';
			// decrement count
			count--;
			// increment index
			index++;
			// restart cursor on new line
			cursor = 0;
			outb(dev, '\n');
			return index;
		}

		// if space key
		else if (data == 32)
		{
				// move characters to the right of the cursor
				for (int i = index; i >= cursor; i--)
				{
					buffer[i + 1] = buffer[i];
				}

				// insert a space at the cursor position
				buffer[cursor] = ' ';

				// print the updated characters
				for (int i = cursor; i <= index; i++)
				{
					outb(dev, buffer[i]);
				}

				// move cursor back to correct position
				for (int temp_cursor = index; temp_cursor > cursor; temp_cursor--)
				{
					outb(dev, '\b');
				}

				// increment index and cursor
				index++;
				cursor++;
		}



		// if backspace key
		else if (data == 127)
		{
			if (index > 0 && cursor > 0) {
				// Move cursor back
				outb(dev, '\b');
				cursor--;

				// shift characters in buffer left
				for (int i = cursor; i < index - 1; i++) {
					buffer[i] = buffer[i + 1];
					outb(dev, buffer[i]);  // print the updated char to terminal
				}

				// overwrite last character with space
				buffer[index - 1] = ' ';
				outb(dev, ' ');

				// move cursor back to correct position
				for (int temp_cursor = index; temp_cursor > cursor; temp_cursor--) {
					outb(dev, '\b');
				}

				// decrement index
				index--;
				// place null terminator at end of buffer
				buffer[index] = '\0';
			}
		}

		
		// if arrows, delete key, or esc key
		else if (data == '\033' || data == 27)
		{
			char esc_seq[1] = {0};  // initialize escape sequence array
			inb(dev); // rid of bracket read in '['
			while (esc_seq[0] != 'C' || esc_seq[0] != 'D' || esc_seq[0] != 'A' || esc_seq[0] != 'B' || esc_seq[0] != '3')
			{
				// read in key
				esc_seq[0] = inb(dev);
				
				// if right arrow key pressed
				if (esc_seq[0] == 'C') {
					while (cursor != index )
					{
						outb(dev, '\033');  // escape character
						outb(dev, '[');
						outb(dev, 'C');
						cursor++;
						break;
					}
					break;
				// if left arrow key pressed
				} else if (esc_seq[0] == 'D') {
					while (cursor != 0)
					{
						outb(dev, '\033');  // escape character
						outb(dev, '[');
						outb(dev, 'D');
						cursor--;
						break;
					}
					break;
				// if up arrow key pressed
				} else if (esc_seq[0] == 'A') {
					// implement showing last command
					for(size_t i = 0; i <= strlen(histroy_buffer); i++)
					{
						buffer[i] = histroy_buffer[i];
						outb(dev, buffer[i]);
						count--;
						index++;
						cursor++;
					}

					buffer[strlen(buffer)] = '\0';
					break;
				// if down arrow key pressed
				} else if (esc_seq[0] == 'B') {
					// nothing as of now
					break;
				// if delete key pressed
				} else if (esc_seq[0] == '3' || esc_seq[0] == '~') {
					inb(dev); // rid of '~' read in

					while (index > cursor) 
					{
						// delete char right of the cursor
						for (int i = cursor + 1; i < index; i++) 
						{
							buffer[i - 1] = buffer[i];
							outb(dev, buffer[i - 1]);
						}

						// overwrite last character with space
						buffer[index - 1] = ' ';
						outb(dev, ' ');

						// overwrite last character with '\0' to remove spaces left in buffer
						buffer[index - 1] = '\0';


						// move cursor back to correct position
						for (int temp_cursor = index; temp_cursor > cursor; temp_cursor--) 
						{
							outb(dev, '\b');
						}

						// decrement index
						if(index > 0)
						{
							index--;
						}
						// increase count
						if(count < len)
						{
							count++;
						}
						break;
					}

					break;

				}

		  	}

		}
		// all other characters
		else if (data > 0)
		{
			// if char is first in buffer
			if (index == 0)
			{
				// write to terminal
				outb(dev, data);
				// add in array
				buffer[cursor] = data;
				count--;
				index++;
				cursor++;
			}
			else
			{
				// move characters to the right of the cursor
				for (int i = index; i >= cursor; i--)
				{
					buffer[i + 1] = buffer[i];
				}

				buffer[cursor] = data;

				// print the updated characters
				for (int i = cursor; i <= index; i++)
				{
					outb(dev, buffer[i]);
				}

				// move cursor back to correct position
				for (int temp_cursor = index; temp_cursor > cursor; temp_cursor--)
				{
					outb(dev, '\b');
				}

				// increment index and cursor
				index++;
				cursor++;
				count--;
			}
		}
	}

	if(count <= 0)
	{
		sys_req(WRITE, dev, err_buf_msg, sizeof(err_buf_msg));
		outb(dev, '\0');
		outb(dev, '\n');
	}

	return index;

}





