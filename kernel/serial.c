#include <mpx/serial.h>
#include <mpx/io.h>
#include <stddef.h>
#include <sys_req.h>
#include <string.h>
#include <time.h>
#include <itoa.h>
#include <doubly_linked_list.h>

// initialize pointer to keep track of the current command in terminal
struct Node* currentCommand = NULL;

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

void clear_input_buffer(device dev) {
    while (inb(dev) != 0) {
        // Keep reading and discarding characters until the input buffer is empty
    }
}

// error message for full buffer
char err_buf_msg[53] = "\nBuffer Full Error. Clearing buffer/creating new line";

/* initialize first and last node in linked list, and temp buffer to hold contents of real buffer */
struct Node* head = NULL;
char temp_buffer[100] = {0};
struct Node* lastNode = NULL;

int serial_poll(device dev, char *buffer, size_t len)
{
	// clear the input buffer so no left over chars are read in
    clear_input_buffer(dev);

	/* initialize pointer, index, and count */
	int index = 0;
	int cursor = index;
	
	// indent terminal to differentiate between input and output
	outb(dev, '>');
	outb(dev, ' ');

	size_t count = len; // keep track of room left in buffer


	// while buffer is not full
	while (count > 0) {
		// read char into data register
		unsigned char data = inb(dev);
		
		// ENTER KEY
		if (data == 13)
		{
			// clear buffer
			buffer[index] = '\0';

			if (temp_buffer[0] != '0')
			{
				// remove all chars in temp buffer except first '0'
				size_t temp_len = strlen(temp_buffer);
				while (temp_len > 1) {
					for (size_t i = 0; i < temp_len - 1; i++) {
						temp_buffer[i] = temp_buffer[i + 1];
					}
					temp_len--;
				}
				temp_buffer[1] = '\0'; // Null-terminate after '0'
			}

			// copy data from buffer to temp_buffer
			for (size_t i = 0; i < strlen(buffer); i++)
			{
				temp_buffer[i] = buffer[i];
			}
			temp_buffer[strlen(buffer)] = '\0';

			// add buffer contents to the front of linked list
			insertFront(&head, temp_buffer);

			// update the last node
			lastNode = head;

			// update currentCommand pointer
			currentCommand = lastNode->prev;

			// decrement count
			count--;
			// increment index
			index++;
			// restart cursor on new line
			cursor = 0;
			outb(dev, '\n');
			return index;
		}


		// SPACE KEY
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

				/* increment index and cursor */
				index++;
				cursor++;
		}



		// BACKSPACE KEY
		else if (data == 127)
		{
			if (index > 0 && cursor > 0) {
				// move cursor back
				outb(dev, '\b');
				cursor--;

				// shift characters in buffer left
				for (int i = cursor; i < index - 1; i++) {
					buffer[i] = buffer[i + 1];
					outb(dev, buffer[i]);  // print updated char to terminal
				}

				// overwrite last char with space
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

		
		// ARROW KEYS, DELETE KEY, ESC KEY
		else if (data == '\033' || data == 27)
		{
			char esc_seq[1] = {0};  // initialize escape sequence array
			inb(dev); // rid of bracket read in as '['
			while (esc_seq[0] != 'C' || esc_seq[0] != 'D' || esc_seq[0] != 'A' || esc_seq[0] != 'B' || esc_seq[0] != '3')
			{
				// read in key
				esc_seq[0] = inb(dev);
				
				// RIGHT ARROW KEY
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
				// LEFT ARROW KEY
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
				// UP ARROW KEY
				} else if (esc_seq[0] == 'A') {
					// If currentCommand is NULL, set it to the last command/tail of linked list
					if (currentCommand == NULL && lastNode != NULL) {
						currentCommand = lastNode;
					} else if (currentCommand != NULL && currentCommand->next != NULL) {
						// move to the previous command if it is not null
						currentCommand = currentCommand->next;
					}
					
					// display the current command
					if (currentCommand != NULL) {
						// clear terminal 
						for(size_t length = strlen(buffer); length > 0; length--)
						{
							if (index > 0 && cursor > 0) {
								// move cursor back
								outb(dev, '\b');
								cursor--;

								// shift chars in buffer left
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
						/* write last command to terminal and add last command to buffer */
						for(size_t i = 0; i < strlen(currentCommand->data); i++)
						{
							buffer[i] = currentCommand->data[i];
							outb(dev, buffer[i]);
							cursor++;
							index++;
							count--;
						}
						// set last place to null terminator in buffer
						buffer[strlen(buffer)] = '\0';
					}
					
					break;
				// DOWN ARROW KEY
				} else if (esc_seq[0] == 'B') {
					// ff currentCommand is NULL, set it to the last command/end of linked list
					if (currentCommand == NULL && lastNode != NULL) {
						currentCommand = lastNode;
					} else if (currentCommand != NULL && currentCommand->prev != NULL) {
						// move to the previous command if it is not null
						currentCommand = currentCommand->prev;
					}
					
					// display the content of the current command
					if (currentCommand != NULL) {
						for(size_t length = strlen(buffer); length > 0; length--)
						{
							if (index > 0 && cursor > 0) {
								// move cursor back
								outb(dev, '\b');
								cursor--;
			
								// shift chars in buffer left
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
						/* write last command to terminal and add last command to buffer */
						for(size_t i = 0; i < strlen(currentCommand->data); i++)
						{
							buffer[i] = currentCommand->data[i];
							outb(dev, buffer[i]);
							cursor++;
							index++;
							count--;
						}
						// set last place to null terminator in buffer
						buffer[strlen(buffer)] = '\0';

					}
					break;
				// DELETE KEY
				} else if (esc_seq[0] == '3') {
					// read until a ~ is found
					while (esc_seq[0] != '~')
					{
						esc_seq[0] = inb(dev);
					}
					// inb(dev); // rid of '~' read in

					while (index > cursor) 
					{
						// delete char right of the cursor
						for (int i = cursor + 1; i < index; i++) 
						{
							buffer[i - 1] = buffer[i];
							outb(dev, buffer[i - 1]);
						}

						// overwrite last char with space
						buffer[index - 1] = ' ';
						outb(dev, ' ');

						// overwrite last char with '\0' to remove spaces left in buffer
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

				} else if (strlen(esc_seq) == 0) {
					// read in to try and catch
				}

		  	}

		}
		// ALL OTHER CHARS
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

	// check if buffer is full
	if(count <= 0)
	{
		sys_req(WRITE, dev, err_buf_msg, sizeof(err_buf_msg));
		outb(dev, '\0');
		outb(dev, '\n');
	}

	return index;

}
