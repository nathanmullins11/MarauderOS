#include <mpx/serial.h>
#include <mpx/io.h>
#include <sys_req.h>
#include <string.h>

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
	// int cursor = index;
	size_t count = len;
		
	// read char into data register
	unsigned char data = inb(dev);
	//increment index after collecting data
	index++;
	
	// if char is enter key, new line
	if (data == 13)
	{
		buffer[index] = '\0';
		outb(dev, '\n');
		count--;
		index++;
		return len - count;
	}

	 // backspace key
	if (data == 127) {  // Backspace key
    	if (index > 0) 
		{
			outb(dev, '\b');   // Move the cursor back
			outb(dev, ' ');    // Overwrite with a space
			outb(dev, '\b');   // Move the cursor back again
			index--;
    	}
	}


	// delete key
	if (data == 10)
	{
		// buffer[index] = '\0';
		outb(dev, 'h');   // Move the cursor back
        outb(dev, ' ');    // Overwrite with a space
        outb(dev, '\b');   // Move the cursor back again
		index--;
	}

	// escape scenarios
	// unsigned char extra_data = inb(dev); 
	// unsigned char extra_extra = inb(dev);
	// if(data == '\033' && extra_data == '[' && extra_extra == 'D')
	// {
	// 	cursor = index;
	// 	outb(dev, '\b');
	// 	cursor--;
	// 	unsigned char extra_data = inb(dev); 
	// 	if (extra_data == '[')
	// 	{
	// 		unsigned char extra_extra = inb(dev);
	// 		if (extra_extra == 'D')
	// 		{
	// 			cursor--;
	// 			outb(dev, '\b');
	// 		}
	// 		if (extra_extra == 'C')
	// 		{
	// 			cursor++;
	// 			outb(dev, '\b');
	// 		}
	// 	}
	// write data to device and data to buffer array
	outb(dev, data);
	buffer[index] = data;

	// decrement count
	count--;

	return len - count;
}
