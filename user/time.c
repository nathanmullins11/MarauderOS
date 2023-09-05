// Grant Stumpf - get_date

#include <mpx/serial.h>
#include <mpx/io.h>
#include <stdint.h>
#include <sys_req.h>
#include <string.h>
#include <time.h>
#include <itoa.h>

int hexToDec(uint8_t hex);
//const char* extract_time(const char* command);
int strncmp(const char* str1, const char* str2, size_t n);

void get_date (void) {
    
 //Read Day
    outb(0x70,0x07);
    unsigned char day = inb(0x71);

    //Read Month
     outb(0x70,0x08);
     unsigned char month = inb(0x71);

     //Read Year
     outb(0x70,0x09);
     unsigned char year = inb(0x71);

    char dayReadable[2];
     itoa(hexToDec(day), dayReadable, 10 );

      char monthReadable[2];
     itoa(hexToDec(month), monthReadable, 10 );

 char yearReadable[4];
     itoa(hexToDec(year), yearReadable, 10 );

    char date[9] = {monthReadable[0], monthReadable[1],'-',dayReadable[0], dayReadable[1],'-',yearReadable[0],yearReadable[1],'\n'};

    sys_req(WRITE, COM1, date , sizeof(date) ) ;


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
    //command[8] = '\n';
    sys_req(WRITE, COM1, command, 9);

    
    


}

int hexToDec(uint8_t hex) {

    int hexOnes = hex & 0x0F;
    int hexTens = ((hex & 0xF0) >> 4 ) * 10;

    return hexOnes + hexTens;
}

/**const char* extract_time(const char* command)
{
    const char* delimiter = "set time ";
    const char* timeStart = strstr(command, delimiter);

    if (timeStart != NULL) 
    {
        return timeStart + strlen(delimiter);
    } 
    else 
    {
        return NULL;
    }
}*/

int strncmp(const char* str1, const char* str2, size_t n) 
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









// void set_time(uint8_t data_get_time) 
// {
//     // Set Hours
//     cli(); // disable interrupt
//     outb(RTC_INDEX_PORT,0x04);
//     outb(RTC_DATA_PORT, data_get_time);
//     sti(); //enable interrupt

//     // Set Minutes
//     cli(); 
//     outb(RTC_INDEX_PORT,0x02);
//     outb(RTC_DATA_PORT, data_get_time);
//     sti(); 

//     // Set Seconds
//     cli(); 
//     outb(RTC_INDEX_PORT,0x00);
//     outb(RTC_DATA_PORT, data_get_time);
//     sti(); 



    

// }
