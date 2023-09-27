#include "mpx/device.h"
#include "sys_req.h"
#include <pcb.h>
#include <string.h>
#include <comhand.h>
#include <itoa.h>

void delete_pcb(const char* name)
{
    // error message
    char err_msg[] = "Error deleting pcb..";
    char system_class_msg[] = "system processes cannot be deleted..\n";
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

void show_pcb(const char *name)
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

        // print out data
        sys_req(WRITE, COM1, process_name, strlen(process_name));
        print("\t");
        sys_req(WRITE, COM1, class_as_string, strlen(class_as_string));
        print("\t\t");
        sys_req(WRITE, COM1, state_as_string, strlen(state_as_string));
        print("\t\t");
        sys_req(WRITE, COM1, suspended_status, strlen(suspended_status));
        print("\t\t");
        sys_req(WRITE, COM1, priority_as_string, strlen(priority_as_string));
        print("\n");
        
    } else {
        print("ERR: PCB/process you are looking for does not exist... \n");
    }
}

void show_ready(int status) {
    // readying up the ready queue
    struct node* current_ready = global_ready_queue->front;
    struct node* current_suspended_ready = global_suspended_ready_queue->front;

    // print header if just ready is called
    if (status == 1) {
        print("Name\t\tClass\t\t\tState\t\tSuspended Status\tPriority\n");
        print("----------------------------------------------------------------------------------------\n");
    }

    if(current_ready != NULL)
    {
        /* search for process in ready queue*/
        while(current_ready != NULL)
        {
            show_pcb(current_ready->pcb->name_arr);

            current_ready = current_ready->next;
        }
    }

    // Ready Suspended Section
    if(current_suspended_ready != NULL)
    { 
        /* search for process in suspended ready queue*/
        while(current_suspended_ready != NULL)
        {
            show_pcb(current_suspended_ready->pcb->name_arr);

            current_suspended_ready = current_suspended_ready->next;
        }
    }
}

void show_blocked(int status) {
    // readying up the blocked queue
    struct node* current_blocked = global_blocked_queue->front;
    struct node* current_suspended_blocked = global_suspended_blocked_queue->front;

    // print header if only blocked is called
    if (status == 1) {
        print("Name\t\tClass\t\t\tState\t\tSuspended Status\tPriority\n");
        print("----------------------------------------------------------------------------------------\n");
    }

    if (current_blocked != NULL) 
    {
        /* search for process in ready queue*/
        while(current_blocked != NULL)
        {
            show_pcb(current_blocked->pcb->name_arr);

            current_blocked = current_blocked->next;
        }
    }
    
    if(current_suspended_blocked != NULL)
    {
        /* search for process in ready queue*/
        while(current_suspended_blocked != NULL)
        {
            show_pcb(current_suspended_blocked->pcb->name_arr);

            current_suspended_blocked = current_suspended_blocked->next;
        }
    }

}

void show_all(void) {
    // print header
    print("Name\t\tClass\t\t\tState\t\tSuspended Status\tPriority\n");
    print("----------------------------------------------------------------------------------------\n");

    // show ready and then show blocked
    show_ready(0);
    show_blocked(0);
}

void create_pcb(const char *name, int class, int priority) {
    /* Run checks */

    // check if name is valid 
    if (strlen(name) < 8) {
        char err_name[] = "ERR: PCB name must be greater than 8 characters\n";
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

    // remove pcb from current queue
    int status = pcb_remove(cur_pcb);

    // check if it is a system process
    if (cur_pcb->process_ptr->pcb_class == 1) {
        char err[] = "ERR: System process cannot be suspended\n";
        sys_req(WRITE, COM1, err, strlen(err));
        return;
    }

    // check if removed
    if (status) {
        char err[] = "ERR: Cannot remove pcb from queue | try again\n";
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

    // remove pcb from current queue
    int status = pcb_remove(cur_pcb);

    // check if removed
    if (status) {
        char err[] = "ERR: Cannot remove pcb from queue | try again\n";
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
    struct pcb* cur_pcb = pcb_find(name); // get pcb from name

    if (cur_pcb == NULL) // check if valid name
    {
        char err[] = "ERR: PCB does not exist\n";
        sys_req(WRITE, COM1, err, strlen(err));
        return;
    }

    if (priority < 0 || priority > 9)
    {
        char err[] = "ERR: Invalid Priority Number, must be 0-9\n";
        sys_req(WRITE, COM1, err, strlen(err));
        return;
    }

    cur_pcb->process_ptr->pcb_priority = priority; // set new priority
    char sucess_msg[] = "INFO: PCB priority changed\n";
    sys_req(WRITE, COM1, sucess_msg, strlen(sucess_msg)); 

}
