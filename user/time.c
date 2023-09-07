// Grant Stumpf - get_date

#include <mpx/serial.h>
#include <mpx/io.h>
#include <stdint.h>
#include <sys_req.h>
#include <string.h>
#include <time.h>
#include <itoa.h>
#include <stdlib.h>
#include <mpx/interrupts.h>

int hexToDec(uint8_t hex);
uint8_t decToHex(int decimal);
int isValidTimeFormat(const char* input);

void get_date (void) {
    
   // Read Day
    outb(0x70, 0x07);
    unsigned char day = inb(0x71);

    // Read Month
    outb(0x70, 0x08);
    unsigned char month = inb(0x71);

    // Read Year
    outb(0x70, 0x09);
    unsigned char year = inb(0x71);

    // Convert hexadecimal values to decimal strings
    char dayReadable[3];
    itoa(hexToDec(day), dayReadable, 10);

    char monthReadable[3];
    itoa(hexToDec(month), monthReadable, 10);

    char yearReadable[3];
    itoa(hexToDec(year), yearReadable, 10);

    // Create the date string in MM-DD-YYYY format
    char date[9];
    if (hexToDec(month) < 10) {
        //initialize index
    date[0] = monthReadable[0];
    date[1] = monthReadable[1];

    // swap to ones place and add a zero infront
        date[1] = date[0];
        date[0] = '0'; 
    } else {
        //print normal
    date[0] = monthReadable[0];
    date[1] = monthReadable[1];
    }
   
    date[2] = '-';
    if (hexToDec(day) < 10) {
        //initialize index
    date[3] = dayReadable[0];
    date[4] = dayReadable[1];

    // swap to ones place and add a zero infront
        date[4] = date[3];
        date[3] = '0'; 
    } else {
        //print normal
    date[3] = dayReadable[0];
    date[4] = dayReadable[1];
    }
    date[5] = '-';
     if (hexToDec(year) < 10) {
        //initialize index
    date[6] = yearReadable[0];
    date[7] = yearReadable[1];

    // swap to ones place and add a zero infront
        date[7] = date[6];
        date[6] = '0'; 
    } else {
        //print normal
    date[6] = yearReadable[0];
    date[7] = yearReadable[1];
    }
    date[8] = '\0';


  
	sys_req(WRITE, COM1, date , sizeof(date) );

    char newLine[1];
   newLine[0] = '\n';
	sys_req(WRITE, COM1, newLine , sizeof(newLine) ) ;
}

void set_date(uint8_t day, uint8_t month, uint8_t year) {

     uint8_t hexDay = decToHex(day);
    uint8_t hexMonth = decToHex(month);
    uint8_t hexYear = decToHex(year);


    //needs input filtering

     cli();
      outb(0x70,0x07);
      outb(0x71,hexDay);
      sti(); 

       cli(); 
      outb(0x70,0x08);
      outb(0x71,hexMonth);
      sti(); 

     cli(); 
      outb(0x70,0x09);
      outb(0x71,hexYear);
      sti(); 

}

void get_time(void)
{
    // Read hours
    outb(0x70, 0x04);
    unsigned char hour = inb(0x71);

    // Read minutes
    outb(0x70, 0x02);
    unsigned char min = inb(0x71);

    // Read seconds
    outb(0x70, 0x00);
    unsigned char sec = inb(0x71);

    int decimalHour = hexToDec(hour);
    int decimalMin = hexToDec(min);
    int decimalSec = hexToDec(sec);

    char time[9];

    // Format time with leading zeros
    if (decimalHour < 10) 
    {
        itoa(decimalHour, &time[1], 10);
        time[0] = '0';
    } 
    else 
    {
        itoa(decimalHour, &time[0], 10);
    }

    time[2] = ':';

    if (decimalMin < 10) 
    {
        itoa(decimalMin, &time[4], 10);
        time[3] = '0';
    } 
    else 
    {
        itoa(decimalMin, &time[3], 10);
    }

    time[5] = ':';

    if (decimalSec < 10) 
    {
        itoa(decimalSec, &time[7], 10);
        time[6] = '0';
    } 
    else 
    {
        itoa(decimalSec, &time[6], 10);
    }

    time[8] = '\n';
    
    // Display time
    sys_req(WRITE, COM1, time, sizeof(time));
}

void set_time(const char *command)
{
    char newLine[1] = "\n";
    if(isValidTimeFormat(command) != 1)
    {
        //sys_req(WRITE, COM1, command, 9);
        sys_req(WRITE, COM1, "try again", 9);
        sys_req(WRITE, COM1, newLine, 1);
        return;
    }

    // Convert to integer
    int hh = atoi(command);
    int mm = atoi(command + 3);
    int ss = atoi(command + 6);

    // Convert to hexadecimal
    uint8_t hexHH = decToHex(hh);
    uint8_t hexMM = decToHex(mm);
    uint8_t hexSS = decToHex(ss);

    // Set Hours
    cli(); // disable interrupt
    outb(0x70,0x04);
    outb(0x71, hexHH);
    sti(); //enable interrupt

    // Set Minutes
    cli(); 
    outb(0x70,0x02);
    outb(0x71, hexMM);
    sti(); 

    // Set Seconds
    cli(); 
    outb(0x70,0x00);
    outb(0x71, hexSS);
    sti();

    sys_req(WRITE, COM1, newLine, 1);
}

int isValidTimeFormat(const char* input)
{
    size_t len = strlen(input);
    if (len != 8)
    {
        return 0; // Incorrect length
    }

    for (int i = 0; i < 8; i++) 
    {
        if (i == 2 || i == 5) 
        {
            if (input[i] != ':') 
            {
                return 0; // Expected ':' at these positions
            }
        } 
        else 
        {
            if (input[i] < '0' || input[i] > '9') 
            {
                return 0; // Invalid character
            }
        }
    }

    int hh = atoi(input);
    int mm = atoi(input + 3);
    int ss = atoi(input + 6);

    // Checks if time is within valid range (00:00:00 - 23:59:59)
    if (hh < 0 || hh > 24 || mm < 0 || mm > 59 || ss < 0 || ss > 59) 
    {
        return 0;
    }

    return 1;
}

int hexToDec(uint8_t hex) {

    int hexOnes = hex & 0x0F;
    int hexTens = ((hex & 0xF0) >> 4 ) * 10;

    return hexOnes + hexTens;
}

uint8_t decToHex(int decimal) 
{
    return (uint8_t)((decimal / 10) << 4 | (decimal % 10));
}


/*int strncmp(const char* str1, const char* str2, size_t n) 
{
    for (size_t i = 0; i < n; i++) 
    {
        if (str1[i] != str2[i]) 
        {
            return (int)(str1[i]) - (int)(str2[i]);
        }
    }
    return 0; // The first n characters match
}
**/
