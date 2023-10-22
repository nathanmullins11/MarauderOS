#include "memory.h"
#include "mpx/io.h"
#include "sys_req.h"
#include <alarms.h>
#include <pcb.h>
#include <context_switch.h>
#include <comhand.h>
#include <string.h>
#include <time.h>

/* global variables for message and time */
char* alarm_messages[5] = {NULL, NULL, NULL, NULL, NULL};
char* alarm_times[5] = {NULL, NULL, NULL, NULL, NULL};

/* Define a global array of pointers to the pcb structure */
struct pcb* alarm_processes[5] = {NULL, NULL, NULL, NULL, NULL};

void alarm(char *formatted_time, char* message)
{

    if(isValidTimeFormat(formatted_time) != 1)
    {
        //sys_req(WRITE, COM1, command, 9);
        sys_req(WRITE, COM1, "ERR: Invalid time format | use 'help' command\n", 46);
        return;
    }

    if(!pcb_find("alarm0"))
    {
        alarm_times[0] = (char*)sys_alloc_mem(100 * sizeof(char));
        alarm_messages[0] = (char*)sys_alloc_mem(100 * sizeof(char));

        // set global message & time to generated by user
        memcpy(alarm_times[0], formatted_time, strlen(formatted_time));
        memcpy(alarm_messages[0], message, strlen(message));

        /* create a ready, non-suspended process that is idle */
        create_pcb("alarm0", 0, 2);
            struct pcb* alarm0_pcb = pcb_find("alarm0");
            struct context* context_alarm0 = (struct context*)(((int)alarm0_pcb->process_ptr->stack_ptr)-sizeof(struct context) - sizeof(int));
            alarm0_pcb->process_ptr->stack_ptr = context_alarm0;

            /* set context for segment process */
            context_alarm0->CS = 0x08;
            context_alarm0->DS = 0x10;
            context_alarm0->ES = 0x10;
            context_alarm0->FS = 0x10;
            context_alarm0->GS = 0x10;
            context_alarm0->SS = 0x10;

            // EPB set to bottom of stack
            context_alarm0->EBP = (int)(alarm0_pcb->process_ptr->pcb_stack + PCB_STACK_SIZE - sizeof(struct context)) - sizeof(int);
            
            // ESP set to top of stack
            context_alarm0->ESP = (int)(alarm0_pcb->process_ptr->pcb_stack + PCB_STACK_SIZE - sizeof(struct context)) - sizeof(int);
            
            // EIP point to function proc1
            context_alarm0->EIP = (int)print_message;
            
            /* all other registers */
            context_alarm0->EAX = 0;
            context_alarm0->EBX = 0;
            context_alarm0->ECX = 0;
            context_alarm0->EDX = 0;
            context_alarm0->ESI = 0;
            context_alarm0->EDI = 0;

            // set EFLAGS
            context_alarm0->EFLAGS = 0x0202;

            alarm_processes[0] = alarm0_pcb; // add process to array
    } else if(!pcb_find("alarm1"))
    {
        alarm_times[1] = (char*)sys_alloc_mem(100 * sizeof(char));
        alarm_messages[1] = (char*)sys_alloc_mem(100 * sizeof(char));

        // set global message & time to generated by user
        memcpy(alarm_times[1], formatted_time, strlen(formatted_time));
        memcpy(alarm_messages[1], message, strlen(message));

        /* create a ready, non-suspended process that is idle */
        create_pcb("alarm1", 0, 2);
            struct pcb* alarm1_pcb = pcb_find("alarm1");
            struct context* context_alarm1 = (struct context*)(((int)alarm1_pcb->process_ptr->stack_ptr)-sizeof(struct context) - sizeof(int));
            alarm1_pcb->process_ptr->stack_ptr = context_alarm1;

            /* set context for segment process */
            context_alarm1->CS = 0x08;
            context_alarm1->DS = 0x10;
            context_alarm1->ES = 0x10;
            context_alarm1->FS = 0x10;
            context_alarm1->GS = 0x10;
            context_alarm1->SS = 0x10;

            // EPB set to bottom of stack
            context_alarm1->EBP = (int)(alarm1_pcb->process_ptr->pcb_stack + PCB_STACK_SIZE - sizeof(struct context)) - sizeof(int);
            
            // ESP set to top of stack
            context_alarm1->ESP = (int)(alarm1_pcb->process_ptr->pcb_stack + PCB_STACK_SIZE - sizeof(struct context)) - sizeof(int);
            
            // EIP point to function proc1
            context_alarm1->EIP = (int)print_message;
            
            /* all other registers */
            context_alarm1->EAX = 0;
            context_alarm1->EBX = 0;
            context_alarm1->ECX = 0;
            context_alarm1->EDX = 0;
            context_alarm1->ESI = 0;
            context_alarm1->EDI = 0;

            // set EFLAGS
            context_alarm1->EFLAGS = 0x0202;

             alarm_processes[1] = alarm1_pcb; // add process to array
    } else if(!pcb_find("alarm2"))
    {
        alarm_times[2] = (char*)sys_alloc_mem(100 * sizeof(char));
        alarm_messages[2] = (char*)sys_alloc_mem(100 * sizeof(char));

        // set global message & time to generated by user
        memcpy(alarm_times[2], formatted_time, strlen(formatted_time));
        memcpy(alarm_messages[2], message, strlen(message));

        /* create a ready, non-suspended process that is idle */
        create_pcb("alarm2", 0, 2);
            struct pcb* alarm2_pcb = pcb_find("alarm2");
            struct context* context_alarm2 = (struct context*)(((int)alarm2_pcb->process_ptr->stack_ptr)-sizeof(struct context) - sizeof(int));
            alarm2_pcb->process_ptr->stack_ptr = context_alarm2;

            /* set context for segment process */
            context_alarm2->CS = 0x08;
            context_alarm2->DS = 0x10;
            context_alarm2->ES = 0x10;
            context_alarm2->FS = 0x10;
            context_alarm2->GS = 0x10;
            context_alarm2->SS = 0x10;

            // EPB set to bottom of stack
            context_alarm2->EBP = (int)(alarm2_pcb->process_ptr->pcb_stack + PCB_STACK_SIZE - sizeof(struct context)) - sizeof(int);
            
            // ESP set to top of stack
            context_alarm2->ESP = (int)(alarm2_pcb->process_ptr->pcb_stack + PCB_STACK_SIZE - sizeof(struct context)) - sizeof(int);
            
            // EIP point to function proc1
            context_alarm2->EIP = (int)print_message;
            
            /* all other registers */
            context_alarm2->EAX = 0;
            context_alarm2->EBX = 0;
            context_alarm2->ECX = 0;
            context_alarm2->EDX = 0;
            context_alarm2->ESI = 0;
            context_alarm2->EDI = 0;

            // set EFLAGS
            context_alarm2->EFLAGS = 0x0202;

            alarm_processes[2] = alarm2_pcb; // add process to array
    } else if(!pcb_find("alarm3"))
    {
        alarm_times[3] = (char*)sys_alloc_mem(100 * sizeof(char));
        alarm_messages[3] = (char*)sys_alloc_mem(100 * sizeof(char));

        // set global message & time to generated by user
        memcpy(alarm_times[3], formatted_time, strlen(formatted_time));
        memcpy(alarm_messages[3], message, strlen(message));

        /* create a ready, non-suspended process that is idle */
        create_pcb("alarm3", 0, 2);
            struct pcb* alarm3_pcb = pcb_find("alarm3");
            struct context* context_alarm3 = (struct context*)(((int)alarm3_pcb->process_ptr->stack_ptr)-sizeof(struct context) - sizeof(int));
            alarm3_pcb->process_ptr->stack_ptr = context_alarm3;

            /* set context for segment process */
            context_alarm3->CS = 0x08;
            context_alarm3->DS = 0x10;
            context_alarm3->ES = 0x10;
            context_alarm3->FS = 0x10;
            context_alarm3->GS = 0x10;
            context_alarm3->SS = 0x10;

            // EPB set to bottom of stack
            context_alarm3->EBP = (int)(alarm3_pcb->process_ptr->pcb_stack + PCB_STACK_SIZE - sizeof(struct context)) - sizeof(int);
            
            // ESP set to top of stack
            context_alarm3->ESP = (int)(alarm3_pcb->process_ptr->pcb_stack + PCB_STACK_SIZE - sizeof(struct context)) - sizeof(int);
            
            // EIP point to function proc1
            context_alarm3->EIP = (int)print_message;
            
            /* all other registers */
            context_alarm3->EAX = 0;
            context_alarm3->EBX = 0;
            context_alarm3->ECX = 0;
            context_alarm3->EDX = 0;
            context_alarm3->ESI = 0;
            context_alarm3->EDI = 0;

            // set EFLAGS
            context_alarm3->EFLAGS = 0x0202;

            alarm_processes[3] = alarm3_pcb; // add process to array
    } else if(!pcb_find("alarm4"))
    {
        alarm_times[4] = (char*)sys_alloc_mem(100 * sizeof(char));
        alarm_messages[4] = (char*)sys_alloc_mem(100 * sizeof(char));

        // set global message & time to generated by user
        memcpy(alarm_times[4], formatted_time, strlen(formatted_time));
        memcpy(alarm_messages[4], message, strlen(message));
        
        /* create a ready, non-suspended process that is idle */
        create_pcb("alarm4", 0, 2);
            struct pcb* alarm4_pcb = pcb_find("alarm4");
            struct context* context_alarm4 = (struct context*)(((int)alarm4_pcb->process_ptr->stack_ptr)-sizeof(struct context) - sizeof(int));
            alarm4_pcb->process_ptr->stack_ptr = context_alarm4;

            /* set context for segment process */
            context_alarm4->CS = 0x08;
            context_alarm4->DS = 0x10;
            context_alarm4->ES = 0x10;
            context_alarm4->FS = 0x10;
            context_alarm4->GS = 0x10;
            context_alarm4->SS = 0x10;

            // EPB set to bottom of stack
            context_alarm4->EBP = (int)(alarm4_pcb->process_ptr->pcb_stack + PCB_STACK_SIZE - sizeof(struct context)) - sizeof(int);
            
            // ESP set to top of stack
            context_alarm4->ESP = (int)(alarm4_pcb->process_ptr->pcb_stack + PCB_STACK_SIZE - sizeof(struct context)) - sizeof(int);
            
            // EIP point to function proc1
            context_alarm4->EIP = (int)print_message;
            
            /* all other registers */
            context_alarm4->EAX = 0;
            context_alarm4->EBX = 0;
            context_alarm4->ECX = 0;
            context_alarm4->EDX = 0;
            context_alarm4->ESI = 0;
            context_alarm4->EDI = 0;

            // set EFLAGS
            context_alarm4->EFLAGS = 0x0202;

            alarm_processes[4] = alarm4_pcb; // add process to array
    } else {
        print("ERR: Alarm process maximum of 5 reached. An alarm must dispatch before a new alarm can be created.\n");
        return;
    }

    sys_req(IDLE);
}

void print_message(void)
{
    for(;;)
    {
        if(check_time() == 1)
        {
            // get currently running alarm 
            int cur_alarm = check_running_process();
            // set message to print to global message
            char* message_from_user = alarm_messages[cur_alarm];

            // print message
            print(message_from_user);
            print("\n");

            // remove alarm from process array, message array, and time array
            alarm_processes[cur_alarm] = NULL;
            alarm_messages[cur_alarm] = NULL;
            alarm_times[cur_alarm] = NULL;

            // exit/remove alarm
            sys_req(EXIT);
        } else {
            sys_req(IDLE);
        }
        // sys_req(IDLE);
    }

}

int check_time(void)
{
    // get running process
    int alarm_num = check_running_process();

    // initialize variables for time of alarm
    int hh, mm, ss;

    for(int i = 0; i < 5; i++)
    {
        if(alarm_num == i)
        {
            // Convert to integer
            hh = atoi(alarm_times[i]);
            mm = atoi(alarm_times[i]+ 3);
            ss = atoi(alarm_times[i]+ 6);
        }
    }

    // Convert to hexadecimal
    // uint8_t hh = decToHex(hh);
    // uint8_t hexMM = decToHex(mm);
    // uint8_t hexSS = decToHex(ss);

    // once time is in hex, need to check for that time on computer
    // Read hours
    outb(0x70, 0x04);
    unsigned char hour = inb(0x71);
    int system_hour = hexToDec(hour);

    // Read minutes
    outb(0x70, 0x02);
    unsigned char min = inb(0x71);
    int system_min = hexToDec(min);

    // Read seconds
    outb(0x70, 0x00);
    unsigned char sec = inb(0x71);
    int system_sec = hexToDec(sec);

    /* compare time on computer with formatted time given by user */
    if((hh == system_hour) && (mm == system_min) && (ss == system_sec)) // checks if time is the same as system
    {
        return 1;
    } 

    if (hh < system_hour) { // hour is greater than system
        return 1;
    } else if((hh == system_hour) && (mm < system_min)) { // same hour but minutes is greater than system
       return 1;
    } else if ((hh == system_hour) && (mm == system_min) && (ss < system_sec)) {  // same hour and minute but seconds is greater than system
        return 1;
    } else {
        return 0;
    }

}

int check_running_process(void)
{
    // iterate through alarm array until running one is found
    for(int i = 0; i < 5; i++)
    {
        if(alarm_processes[i] != NULL)
        {
            struct pcb* alarm = alarm_processes[i];
            // if alarm is in array and not in ready queue, it is executing
            if(!pcb_find(alarm->name_arr))
            {
                return i;
            }
        }
    }
    return 6; // error occured
}
