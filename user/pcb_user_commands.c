#include "memory.h"
#include "mpx/device.h"
#include "processes.h"
#include "sys_req.h"
#include <pcb.h>
#include <stdint.h>
#include <string.h>
#include <comhand.h>
#include <itoa.h>
#include <context_switch.h>

#define PCB_STACK_SIZE 1024

void delete_pcb(const char* name)
{
    // error message
    char err_msg[] = "Error deleting pcb..";
    char system_class_msg[] = "ERR: System processes cannot be deleted\n";

    // find pcb with given name
    struct pcb* pcb_to_delete = pcb_find(name);

    // pcb with given name could not be found
    if(pcb_to_delete == NULL)
    {
        print("ERR: PCB does not exist\n");
        return;
    }

    // check if pcb is a system class
    if(pcb_to_delete->process_ptr->pcb_class == 1)
    {
        // can not delete system processes, return
        sys_req(WRITE, COM1, system_class_msg, sizeof(system_class_msg));
        return;
    }

    // remove pcb from its associated queue
    int status_code = pcb_remove(pcb_to_delete);

    // error deleting pcb
    if(status_code == 1)
    {
        sys_req(WRITE, COM1, err_msg , sizeof(err_msg) ) ;
        return;
    }

    // free mem
    pcb_free(pcb_to_delete);
    char sucess_msg[] = "INFO: PCB deleted\n";
    sys_req(WRITE, COM1, sucess_msg, strlen(sucess_msg));
}

void show_pcb(const char *name, int status)
{
    // get associated pcb from name
    struct pcb* pcb_to_show = pcb_find(name);

    if(pcb_to_show != NULL)
    {
        /* set char arrays for data of process to be displayed */
        const char* process_name = pcb_to_show->name_arr;
        int process_class = pcb_to_show->process_ptr->pcb_class;
        enum state process_state = pcb_to_show->process_ptr->pcb_state;
        int priority = pcb_to_show->process_ptr->pcb_priority;

        // convert class to string based on int value
        char* class_as_string = {0};
        if(process_class == 0)
        {
            class_as_string = "user process";
        } else if (process_class == 1) {
            class_as_string = "system process";
        }

        // convert state to string based off enum value
        char* state_as_string = {0};
        char* suspended_status = {0};
        if(process_state == READY_NOT_SUSPENDED)
        {
            state_as_string = "ready\0";
            suspended_status = "not suspended\0";
        } else if (process_state == READY_SUSPENDED) {
            state_as_string = "ready\0";
            suspended_status = "suspended\0";
        } else if (process_state == BLOCKED_NOT_SUSPENDED) {
            state_as_string = "blocked\0";
            suspended_status = "not suspended\0";
        } else if (process_state == BLOCKED_SUSPENDED) {
            state_as_string = "blocked\0";
            suspended_status = "suspended\0";
        }

        /* convert priority from int to string */
        char priority_as_string[10] = {0};
        itoa(priority, priority_as_string, 10);

        // print header if just ready is called
        if (status == 1) {
            print("Name\t\tClass\t\t\tState\t\tSuspended Status\tPriority\n");
            print("----------------------------------------------------------------------------------------\n");
        }

        // print out data
        sys_req(WRITE, COM1, process_name, strlen(process_name));
        print("\t\t");
        sys_req(WRITE, COM1, class_as_string, strlen(class_as_string));
        print("\t\t");
        sys_req(WRITE, COM1, state_as_string, strlen(state_as_string));
        print("\t\t");
        sys_req(WRITE, COM1, suspended_status, strlen(suspended_status));
        print("\t\t");
        sys_req(WRITE, COM1, priority_as_string, strlen(priority_as_string));
        print("\n");
        
    } else {
        print("ERR: PCB/Process you are looking for does not exist\n");
    }
}

void show_ready(void) {
    // readying up the ready queue
    struct node* current_ready = global_ready_queue->front;
    struct node* current_suspended_ready = global_suspended_ready_queue->front;
    

    // print header if just ready is called
    print("\t\t\t   === Ready Not Suspended ===\n");
    print("Name\t\tClass\t\t\tState\t\tSuspended Status\tPriority\n");
    print("----------------------------------------------------------------------------------------\n");
    

    if(current_ready != NULL)
    {
        /* search for process in ready queue*/
        while(current_ready != NULL)
        {
            show_pcb(current_ready->pcb->name_arr, 0);

            current_ready = current_ready->next;
        }
        print("\n");
    } else {
        print("NULL\n\n");
    }

    // Ready Suspended Section
    print("\t\t\t     === Ready Suspended ===\n");
    print("Name\t\tClass\t\t\tState\t\tSuspended Status\tPriority\n");
    print("----------------------------------------------------------------------------------------\n");
    
    if(current_suspended_ready != NULL)
    { 
        /* search for process in suspended ready queue*/
        while(current_suspended_ready != NULL)
        {
            show_pcb(current_suspended_ready->pcb->name_arr, 0);

            current_suspended_ready = current_suspended_ready->next;
        }
        print("\n");
    } else {
        print("NULL\n\n");
    }
}

void show_blocked(void) {
    // readying up the blocked queue
    struct node* current_blocked = global_blocked_queue->front;
    struct node* current_suspended_blocked = global_suspended_blocked_queue->front;

    // print header if only blocked is called
    print("\t\t\t   === Blocked Not Suspended ===\n");
    print("Name\t\tClass\t\t\tState\t\tSuspended Status\tPriority\n");
    print("----------------------------------------------------------------------------------------\n");

    if (current_blocked != NULL) 
    {
        /* search for process in ready queue*/
        while(current_blocked != NULL)
        {
            show_pcb(current_blocked->pcb->name_arr, 0);

            current_blocked = current_blocked->next;
        }
        print("\n");
    } else {
        print("NULL\n\n");
    }
    
    print("\t\t\t     === Blocked Suspended ===\n");
    print("Name\t\tClass\t\t\tState\t\tSuspended Status\tPriority\n");
    print("----------------------------------------------------------------------------------------\n");

    if(current_suspended_blocked != NULL)
    {
        /* search for process in ready queue*/
        while(current_suspended_blocked != NULL)
        {
            show_pcb(current_suspended_blocked->pcb->name_arr, 0);

            current_suspended_blocked = current_suspended_blocked->next;
        }
        print("\n");
    } else {
        print("NULL\n\n");
    }

}

void show_all(void) {
    // show ready and then show blocked
    show_ready();
    show_blocked();
}

void create_pcb(const char *name, int class, int priority) {
    /* Run checks */

    // check if name is valid 
    if (strlen(name) > 8) {
        char err_name[] = "ERR: PCB name must be less than or equal to 8 characters\n";
        sys_req(WRITE, COM1, err_name, strlen(err_name));
        return;
    }

    // check if class is valid
    if (class != 0 && class != 1) {
        char err_class[] = "ERR: Invalid class\n";
        sys_req(WRITE, COM1, err_class, strlen(err_class));
        return;
    }

    // check priority
    if (priority > 9 || priority < 0) {
        char err_pri[] = "ERR: Invalid priority\n";
        sys_req(WRITE, COM1, err_pri, strlen(err_pri));
        return;
    }

    // check if name is unique
    if (pcb_find(name) != NULL) {
        char err_name[] = "ERR: Name already in use\n";
        sys_req(WRITE, COM1, err_name, strlen(err_name));
        return;
    } else if (name == NULL) {
        char err_name[] = "ERR: Name cannot be empty\n";
        sys_req(WRITE, COM1, err_name, strlen(err_name));
    }
    
    // setup new pcb
    struct pcb* new_pcb = pcb_setup(name, class, priority);

    // insert new pcb into appropiate queue
    pcb_insert(new_pcb);

    // print statement for successful creation
    print("INFO: PCB Created\n");
}

void block_pcb(const char *name) {
    // get pcb struct from name
    struct pcb* cur_pcb = pcb_find(name);

    // check if name is valid
    if (cur_pcb == NULL) {
        char err[] = "ERR: PCB does not exist\n";
        sys_req(WRITE, COM1, err, strlen(err));
        return;
    }

    // if blocked then cannot block
    if (cur_pcb->process_ptr->pcb_state == BLOCKED_SUSPENDED || cur_pcb->process_ptr->pcb_state == BLOCKED_NOT_SUSPENDED) {
        char err[] = "ERR: Cannot block a blocked process\n";
        sys_req(WRITE, COM1, err, strlen(err));
        return;
    }

    // if process is of type system, do not block
    if(cur_pcb->process_ptr->pcb_class == 1)
    {
        char sys_err[] = "ERR: Cannot block a system process\n";
        sys_req(WRITE, COM1, sys_err, strlen(sys_err));
        return;
    }

    // remove pcb from current queue
    int status = pcb_remove(cur_pcb);

    // check if removed
    if (status) {
        char err[] = "ERR: Cannot remove pcb from queue | try again\n";
        sys_req(WRITE, COM1, err, strlen(err));
        return;
    }

    // change the pcb to ready based off of its cur state i.e. in a suspended ready queue or a not suspended ready queue
    if (cur_pcb->process_ptr->pcb_state == READY_NOT_SUSPENDED) {
        cur_pcb->process_ptr->pcb_state = BLOCKED_NOT_SUSPENDED;
    } else if (cur_pcb->process_ptr->pcb_state == READY_SUSPENDED) {
        cur_pcb->process_ptr->pcb_state = BLOCKED_SUSPENDED;
    }

    // put back into relevant queue
    pcb_insert(cur_pcb);
    char sucess_msg[] = "INFO: PCB blocked\n";
    sys_req(WRITE, COM1, sucess_msg, strlen(sucess_msg));
}

void unblock_pcb(const char *name) {
    // get pcb struct from name
    struct pcb* cur_pcb = pcb_find(name);

    // check if name is valid
    if (cur_pcb == NULL) {
        char err[] = "ERR: PCB does not exist\n";
        sys_req(WRITE, COM1, err, strlen(err));
        return;
    }

    // remove pcb from current queue
    int status = pcb_remove(cur_pcb);

    // check if removed
    if (status) {
        char err[] = "ERR: Cannot remove pcb from queue | try again\n";
        sys_req(WRITE, COM1, err, strlen(err));
        return;
    }

    // if unblocked then cannot unblocked
    if (cur_pcb->process_ptr->pcb_state == READY_NOT_SUSPENDED || cur_pcb->process_ptr->pcb_state == READY_SUSPENDED) {
        char err[] = "ERR: Cannot unblock a unblocked process\n";
        sys_req(WRITE, COM1, err, strlen(err));
        return;
    }

    // change the pcb to ready based off of its cur state i.e. in a suspended ready queue or a not suspended ready queue
    if (cur_pcb->process_ptr->pcb_state == BLOCKED_SUSPENDED) {
        cur_pcb->process_ptr->pcb_state = READY_SUSPENDED;
    } else if (cur_pcb->process_ptr->pcb_state == BLOCKED_NOT_SUSPENDED) {
        cur_pcb->process_ptr->pcb_state = READY_NOT_SUSPENDED;
    }


    // put back into relevant queue
    pcb_insert(cur_pcb);
    char sucess_msg[] = "INFO: PCB unblocked\n";
    sys_req(WRITE, COM1, sucess_msg, strlen(sucess_msg));
}

void suspend_pcb(const char *name) {
    // get pcb struct from name
    struct pcb* cur_pcb = pcb_find(name);

    // check if name is valid
    if (cur_pcb == NULL) {
        char err[] = "ERR: PCB does not exist\n";
        sys_req(WRITE, COM1, err, strlen(err));
        return;
    }

    // check if it is a system process
    if (cur_pcb->process_ptr->pcb_class == 1) {
        char err[] = "ERR: System process cannot be suspended\n";
        sys_req(WRITE, COM1, err, strlen(err));
        return;
    }

    // remove pcb from current queue
    int status = pcb_remove(cur_pcb);

    // check if removed
    if (status) {
        char err[] = "ERR: Cannot remove pcb from queue | try again\n";
        sys_req(WRITE, COM1, err, strlen(err));
        return;
    }

    // if suspended then cannot suspend
    if (cur_pcb->process_ptr->pcb_state == READY_SUSPENDED || cur_pcb->process_ptr->pcb_state == BLOCKED_SUSPENDED) {
        char err[] = "ERR: Cannot suspend a suspended process\n";
        sys_req(WRITE, COM1, err, strlen(err));
        return;
    }

    // change the pcb to suspended based off of its cur state
    if (cur_pcb->process_ptr->pcb_state == READY_NOT_SUSPENDED) {
        cur_pcb->process_ptr->pcb_state = READY_SUSPENDED;
    } else if (cur_pcb->process_ptr->pcb_state == BLOCKED_NOT_SUSPENDED) {
        cur_pcb->process_ptr->pcb_state = BLOCKED_SUSPENDED;
    }

    // put back into relevant queue
    pcb_insert(cur_pcb);
    char sucess_msg[] = "INFO: PCB suspended\n";
    sys_req(WRITE, COM1, sucess_msg, strlen(sucess_msg));
}

void resume_pcb(const char *name) {
    // get pcb struct from name
    struct pcb* cur_pcb = pcb_find(name);

    // check if name is valid
    if (cur_pcb == NULL) {
        char err[] = "ERR: PCB does not exist\n";
        sys_req(WRITE, COM1, err, strlen(err));
        return;
    }

    // check if process class is system
    if(cur_pcb->process_ptr->pcb_class == 1)
    {
        char err_msg[] = "ERR: System processes cannot be suspended/unsuspended\n";
        // can not delete system processes, return
        sys_req(WRITE, COM1, err_msg, sizeof(err_msg));
        return;
    }

    // remove pcb from current queue
    int status = pcb_remove(cur_pcb);

    // check if removed
    if (status) {
        char err[] = "ERR: Cannot remove pcb from queue | try again\n";
        sys_req(WRITE, COM1, err, strlen(err));
        return;
    }

    // if not suspended then cannot resume
    if (cur_pcb->process_ptr->pcb_state == BLOCKED_NOT_SUSPENDED || cur_pcb->process_ptr->pcb_state == READY_NOT_SUSPENDED) {
        char err[] = "ERR: Cannot resume a not suspended process\n";
        sys_req(WRITE, COM1, err, strlen(err));
        return;
    }

    // change the pcb to not suspended based off of its cur state
    if (cur_pcb->process_ptr->pcb_state == READY_SUSPENDED) {
        cur_pcb->process_ptr->pcb_state = READY_NOT_SUSPENDED;
    } else if (cur_pcb->process_ptr->pcb_state == BLOCKED_SUSPENDED) {
        cur_pcb->process_ptr->pcb_state = BLOCKED_NOT_SUSPENDED;
    }

    // put back into relevant queue
    pcb_insert(cur_pcb);
    char sucess_msg[] = "INFO: PCB resumed\n";
    sys_req(WRITE, COM1, sucess_msg, strlen(sucess_msg));
}

void set_pcb_priority(const char *name, int priority)
{
    // get pcb from name
    struct pcb* cur_pcb = pcb_find(name); 

    // check if valid name
    if (cur_pcb == NULL) 
    {
        char err[] = "ERR: PCB does not exist\n";
        sys_req(WRITE, COM1, err, strlen(err));
        return;
    }

    // check if priority valid
    if (priority < 0 || priority > 9)
    {
        char err[] = "ERR: Invalid Priority Number, must be 0-9\n";
        sys_req(WRITE, COM1, err, strlen(err));
        return;
    }

    // check if process is in a blocked queue, if so only change priority because FIFO
    if(cur_pcb->process_ptr->pcb_state == BLOCKED_NOT_SUSPENDED || cur_pcb->process_ptr->pcb_state == BLOCKED_SUSPENDED)
    {
        // set new priority
        cur_pcb->process_ptr->pcb_priority = priority;
        char sucess_msg[] = "INFO: PCB priority changed\n";
        sys_req(WRITE, COM1, sucess_msg, strlen(sucess_msg));
    } else { // process is in a ready queue so priority matters
        // remove from appropriate queue
        pcb_remove(cur_pcb);

        // set new priority
        cur_pcb->process_ptr->pcb_priority = priority;
        char sucess_msg[] = "INFO: PCB priority changed\n";
        sys_req(WRITE, COM1, sucess_msg, strlen(sucess_msg)); 

        // enter back into appropriate queue
        pcb_insert(cur_pcb);
    }
}

void yield(void)
{
    sys_req(IDLE);
}

void Load_R3(void)
{
    /* load a process from processes.h */

    /* allocate mem for pcb and context */
    create_pcb("test1", 0, 1);
    // create_pcb("test2", 0, 2);
    // create_pcb("test3", 0, 3);
    // create_pcb("test4", 0, 4);
    // create_pcb("test5", 0, 5);
    if(global_ready_queue->front != NULL)
    {
        struct pcb* pcb_test1 = global_ready_queue->front->pcb;
        struct context* context_test1 = (struct context*)(((int)pcb_test1->process_ptr->stack_ptr)-sizeof(struct context) - sizeof(int));
        pcb_test1->process_ptr->stack_ptr = context_test1;

        /* set context for segment process */
        context_test1->CS = 0x08;
        context_test1->DS = 0x10;
        context_test1->ES = 0x10;
        context_test1->FS = 0x10;
        context_test1->GS = 0x10;
        context_test1->SS = 0x10;

        // EPB set to bottom of stack
        context_test1->EBP = (int)(pcb_test1->process_ptr->pcb_stack + PCB_STACK_SIZE - sizeof(struct context)) - sizeof(int);
        // ESP set to top of stack
        context_test1->ESP = (int)(pcb_test1->process_ptr->pcb_stack + PCB_STACK_SIZE - sizeof(struct context)) - sizeof(int);
        // EIP point to function proc1
        context_test1->EIP = (int)proc1;
        /* all other registers */
        context_test1->EAX = 0;
        context_test1->EBX = 0;
        context_test1->ECX = 0;
        context_test1->EDX = 0;
        context_test1->ESI = 0;
        context_test1->EDI = 0;


        // set EFLAGS
        context_test1->EFLAGS = 0x0202;
    }

    if(global_ready_queue->front != NULL)
    {
        struct pcb* pcb_test1 = global_ready_queue->front->pcb;
        struct context* context_test1 = (struct context*)(((int)pcb_test1->process_ptr->stack_ptr)-sizeof(struct context) - sizeof(int));
        pcb_test1->process_ptr->stack_ptr = context_test1;

        /* set context for segment process */
        context_test1->CS = 0x08;
        context_test1->DS = 0x10;
        context_test1->ES = 0x10;
        context_test1->FS = 0x10;
        context_test1->GS = 0x10;
        context_test1->SS = 0x10;

        // EPB set to bottom of stack
        context_test1->EBP = (int)(pcb_test1->process_ptr->pcb_stack + PCB_STACK_SIZE - sizeof(struct context)) - sizeof(int);
        // ESP set to top of stack
        context_test1->ESP = (int)(pcb_test1->process_ptr->pcb_stack + PCB_STACK_SIZE - sizeof(struct context)) - sizeof(int);
        // EIP point to function proc1
        context_test1->EIP = (int)proc2;
        /* all other registers */
        context_test1->EAX = 0;
        context_test1->EBX = 0;
        context_test1->ECX = 0;
        context_test1->EDX = 0;
        context_test1->ESI = 0;
        context_test1->EDI = 0;


        // set EFLAGS
        context_test1->EFLAGS = 0x0202;
    }

    if(global_ready_queue->front != NULL)
    {
        struct pcb* pcb_test1 = global_ready_queue->front->pcb;
        struct context* context_test1 = (struct context*)(((int)pcb_test1->process_ptr->stack_ptr)-sizeof(struct context) - sizeof(int));
        pcb_test1->process_ptr->stack_ptr = context_test1;

        /* set context for segment process */
        context_test1->CS = 0x08;
        context_test1->DS = 0x10;
        context_test1->ES = 0x10;
        context_test1->FS = 0x10;
        context_test1->GS = 0x10;
        context_test1->SS = 0x10;

        // EPB set to bottom of stack
        context_test1->EBP = (int)(pcb_test1->process_ptr->pcb_stack + PCB_STACK_SIZE - sizeof(struct context)) - sizeof(int);
        // ESP set to top of stack
        context_test1->ESP = (int)(pcb_test1->process_ptr->pcb_stack + PCB_STACK_SIZE - sizeof(struct context)) - sizeof(int);
        // EIP point to function proc1
        context_test1->EIP = (int)proc3;
        /* all other registers */
        context_test1->EAX = 0;
        context_test1->EBX = 0;
        context_test1->ECX = 0;
        context_test1->EDX = 0;
        context_test1->ESI = 0;
        context_test1->EDI = 0;


        // set EFLAGS
        context_test1->EFLAGS = 0x0202;
    }

    if(global_ready_queue->front != NULL)
    {
        struct pcb* pcb_test1 = global_ready_queue->front->pcb;
        struct context* context_test1 = (struct context*)(((int)pcb_test1->process_ptr->stack_ptr)-sizeof(struct context) - sizeof(int));
        pcb_test1->process_ptr->stack_ptr = context_test1;

        /* set context for segment process */
        context_test1->CS = 0x08;
        context_test1->DS = 0x10;
        context_test1->ES = 0x10;
        context_test1->FS = 0x10;
        context_test1->GS = 0x10;
        context_test1->SS = 0x10;

        // EPB set to bottom of stack
        context_test1->EBP = (int)(pcb_test1->process_ptr->pcb_stack + PCB_STACK_SIZE - sizeof(struct context)) - sizeof(int);
        // ESP set to top of stack
        context_test1->ESP = (int)(pcb_test1->process_ptr->pcb_stack + PCB_STACK_SIZE - sizeof(struct context)) - sizeof(int);
        // EIP point to function proc1
        context_test1->EIP = (int)proc4;
        /* all other registers */
        context_test1->EAX = 0;
        context_test1->EBX = 0;
        context_test1->ECX = 0;
        context_test1->EDX = 0;
        context_test1->ESI = 0;
        context_test1->EDI = 0;


        // set EFLAGS
        context_test1->EFLAGS = 0x0202;
    }

    if(global_ready_queue->front != NULL)
    {
        struct pcb* pcb_test1 = global_ready_queue->front->pcb;
        struct context* context_test1 = (struct context*)(((int)pcb_test1->process_ptr->stack_ptr)-sizeof(struct context) - sizeof(int));
        pcb_test1->process_ptr->stack_ptr = context_test1;

        /* set context for segment process */
        context_test1->CS = 0x08;
        context_test1->DS = 0x10;
        context_test1->ES = 0x10;
        context_test1->FS = 0x10;
        context_test1->GS = 0x10;
        context_test1->SS = 0x10;

        // EPB set to bottom of stack
        context_test1->EBP = (int)(pcb_test1->process_ptr->pcb_stack + PCB_STACK_SIZE - sizeof(struct context)) - sizeof(int);
        // ESP set to top of stack
        context_test1->ESP = (int)(pcb_test1->process_ptr->pcb_stack + PCB_STACK_SIZE - sizeof(struct context)) - sizeof(int);
        // EIP point to function proc1
        context_test1->EIP = (int)proc5;
        /* all other registers */
        context_test1->EAX = 0;
        context_test1->EBX = 0;
        context_test1->ECX = 0;
        context_test1->EDX = 0;
        context_test1->ESI = 0;
        context_test1->EDI = 0;


        // set EFLAGS
        context_test1->EFLAGS = 0x0202;
    }
    
}
