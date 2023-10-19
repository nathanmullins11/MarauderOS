#include "mpx/io.h"
#include "sys_req.h"
#include <alarms.h>
#include <pcb.h>
#include <context_switch.h>
#include <comhand.h>
#include <time.h>

void alarm(char *formatted_time)
{
    if(isValidTimeFormat(formatted_time) != 1)
    {
        //sys_req(WRITE, COM1, command, 9);
        sys_req(WRITE, COM1, "ERR: Invalid time format | use 'help' command\n", 46);
        return;
    }

    // Convert to integer
   // int hh = atoi(formatted_time);
    // int mm = atoi(formatted_time + 3);
    // int ss = atoi(formatted_time + 6);

    // Convert to hexadecimal
    // uint8_t hexHH = decToHex(hh);
    // uint8_t hexMM = decToHex(mm);
    // uint8_t hexSS = decToHex(ss);

    // once time is in hex, need to check for that time on computer
    // Read hours
    outb(0x70, 0x04);
    // unsigned char hour = inb(0x71);

    // Read minutes
    outb(0x70, 0x02);
    // unsigned char min = inb(0x71);

    // Read seconds
    outb(0x70, 0x00);
    // unsigned char sec = inb(0x71);

    if(!pcb_find("alarm"))
    {
        /* create a ready, non-suspended process that is idle */
        create_pcb("alarm", 0, 2);
            struct pcb* alarm_pcb = pcb_find("alarm");
            struct context* context_alarm1 = (struct context*)(((int)alarm_pcb->process_ptr->stack_ptr)-sizeof(struct context) - sizeof(int));
            alarm_pcb->process_ptr->stack_ptr = context_alarm1;

            /* set context for segment process */
            context_alarm1->CS = 0x08;
            context_alarm1->DS = 0x10;
            context_alarm1->ES = 0x10;
            context_alarm1->FS = 0x10;
            context_alarm1->GS = 0x10;
            context_alarm1->SS = 0x10;

            // EPB set to bottom of stack
            context_alarm1->EBP = (int)(alarm_pcb->process_ptr->pcb_stack + PCB_STACK_SIZE - sizeof(struct context)) - sizeof(int);
            
            // ESP set to top of stack
            context_alarm1->ESP = (int)(alarm_pcb->process_ptr->pcb_stack + PCB_STACK_SIZE - sizeof(struct context)) - sizeof(int);
            
            // EIP point to function proc1
            context_alarm1->EIP = (int)message_;
            
            /* all other registers */
            context_alarm1->EAX = 0;
            context_alarm1->EBX = 0;
            context_alarm1->ECX = 0;
            context_alarm1->EDX = 0;
            context_alarm1->ESI = 0;
            context_alarm1->EDI = 0;

            // set EFLAGS
            context_alarm1->EFLAGS = 0x0202;
    }

    // if(hexHH == hour)
    // {
    //     sys_req(IDLE);
    //     return;
    // }

    sys_req(IDLE);
}

void message_(char* message_from_user)
{
    (void)message_from_user;
    print("hello\n");

    sys_req(IDLE);
}

