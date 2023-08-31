// Riley Stauffer

#include <mpx/serial.h>
#include <mpx/io.h>
#include <sys_req.h>
#include <string.h>
#include <time.h>


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

    char date[] = [month, day, year];

    sys_req(WRITE, COM1, date , sizeof(date) ) ;







}


char hexToASCII(char hex) {
    if (hex >= '0' && hex <= '9') {
        return hex - '0';
    } else if (hex >= 'A' && hex <= 'F') {
        return hex - 'A' + 10;
    } else if (hex >= 'a' && hex <= 'f') {
        return hex - 'a' + 10;
    } else {
        return '\0';
    }
}


// #define RTC_INDEX_PORT 0x70
// #define RTC_DATA_PORT 0x71


// void get_time() //get time and date
// {
//     //Read Hours
//     outb(RTC_INDEX_PORT,0x04);
//     unsigned char hour = inb(0x71);

//     //Read Minutes
//     outb(RTC_INDEX_PORT,0x02);
//     unsigned char minute = inb(0x71);

//     //Read Seconds
//     outb(RTC_INDEX_PORT,0x00);
//     unsigned char second = inb(0x71);
// }

// unsigned char convert_from_cdb(unsigned char dec)
// {
//     switch(dec)
//     {
//         case '0x00': 
//             return 0x30;
//             break;
//         case '0x01':
//             return 0x31;
//             break;
//         case '0x02': 
//             return 0x32;
//             break;
//         case '0x03':
//             return 0x33;
//             break;
//         case '0x04': 
//             return 0x34;
//             break;
//         case '0x05':
//             return 0x35;
//             break;
//         case '0x06': 
//             return 0x36;
//             break;
//         case '0x07':
//             return 0x37;
//             break;
//         case '0x08': 
//             return 0x39;
//             break;
//         case '0x09':
//             return 0x39;
//             break;
//     }

// }






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
