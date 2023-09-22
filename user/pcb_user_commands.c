#include "mpx/device.h"
#include "sys_req.h"
#include <pcb.h>
#include <string.h>
#include <comhand.h>

void delete_pcb(const char* name)
{
    // error message
    char err_msg[] = "Error deleting pcb..";
    // find pcb with given name
    struct pcb* pcb_to_delete = pcb_find(name);

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

}

void show_pcb(const char *name)
{
    // get associated pcb from name
    struct pcb* pcb_to_show = pcb_find(name);

    /* set char arrays for data of process to be displayed */
    const char* process_name = pcb_to_show->process_ptr->process_name;
    int process_class = pcb_to_show->process_ptr->pcb_class;
    enum state process_state = pcb_to_show->process_ptr->pcb_state;
    int priority = pcb_to_show->process_ptr->pcb_priority;

    /* display char arrays to the terminal */
    sys_req(WRITE, COM1, process_name, sizeof(process_name));
    sys_req(WRITE, COM1, process_class, sizeof(process_class));
    sys_req(WRITE, COM1, process_state, sizeof(process_state));
    // sys_req(WRITE, COM1, dispatching_state, sizeof(dispatching_state));
    sys_req(WRITE, COM1, priority, sizeof(priority));
}

void show_ready(void) {
     // readying up the ready queue
    struct node* current_ready = global_ready_queue->front;

    /* search for process in ready queue*/
    while(current_ready != NULL)
    {
     const char* name = current_ready->pcb->process_ptr->process_name;
     int class = current_ready->pcb->process_ptr->pcb_class;
  //  STATE IS UNDER CONSTRUCTION... status also in the works  struct state state = current_ready->pcb->process_ptr;
    int priority = current_ready->pcb->process_ptr->pcb_priority;

    print("Process Name: ");
    sys_req(WRITE, COM1, name, sizeof(name));
     print("\nProcess Class: ");
    sys_req(WRITE, COM1, class, sizeof(class));
     print("\nProcess Priority: ");
    sys_req(WRITE, COM1, priority, sizeof(priority));



        current_ready = current_ready->next;
    }
}

void show_blocked(void) {

     // readying up the blocked queue
     struct node* current_blocked = global_blocked_queue->front;

    /* search for process in ready queue*/
    while(current_blocked != NULL)
    {
     const char* name = current_blocked->pcb->process_ptr->process_name;
     int class = current_blocked->pcb->process_ptr->pcb_class;
  //  STATE IS UNDER CONSTRUCTION... status also in the works  struct state state = current_ready->pcb->process_ptr;
    int priority = current_blocked->pcb->process_ptr->pcb_priority;

    print("Process Name: ");
    sys_req(WRITE, COM1, name, sizeof(name));
     print("\nProcess Class: ");
    sys_req(WRITE, COM1, class, sizeof(class));
     print("\nProcess Priority: ");
    sys_req(WRITE, COM1, priority, sizeof(priority));



        current_blocked = current_blocked->next;
    }

}

void create_pcb(const char *name, int class, int priority) {
    /* Run checks */
    // check if class is valid
    if (class != 0 && class != 1) {
        char err_class[] = "ERR: Invalid class\n";
        sys_req(WRITE, COM1, err_class, strlen(err_class));
    }

    // check priority
    if (priority > 9 || priority < 0) {
        char err_pri[] = "ERR: Invalid priority\n";
        sys_req(WRITE, COM1, err_pri, strlen(err_pri));
    }

    // check if name is unique
    if (pcb_find(name) != NULL) {
        char err_name[] = "ERR: Name already in use\n";
        sys_req(WRITE, COM1, err_name, strlen(err_name));
    } else if (name == NULL) {
        char err_name[] = "ERR: Name cannot be empty\n";
        sys_req(WRITE, COM1, err_name, strlen(err_name));
    }

    print("before setting up new pcb\n");
    
    // setup new pcb
    print("setting up pcb\n");
    struct pcb* new_pcb = pcb_setup(name, class, priority);

    print("after setting up pcb\n");

    // insert new pcb into appropiate queue
    print("setup pcb\ninserting\n");
    pcb_insert(new_pcb);
    print("inserted\n");
}

void block_pcb(const char *name) {
    // get pcb struct from name
    struct pcb* cur_pcb = pcb_find(name);

    // check if name is valid
    if (cur_pcb == NULL) {
        char err[] = "ERR: PCB does not exist\n";
        sys_req(WRITE, COM1, err, strlen(err));
    }

    // remove pcb from current queue
    int status = pcb_remove(cur_pcb);

    // check if removed
    if (!status) {
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
}

void unblock_pcb(const char *name) {
    // get pcb struct from name
    struct pcb* cur_pcb = pcb_find(name);

    // check if name is valid
    if (cur_pcb == NULL) {
        char err[] = "ERR: PCB does not exist\n";
        sys_req(WRITE, COM1, err, strlen(err));
    }

    // remove pcb from current queue
    int status = pcb_remove(cur_pcb);

    // check if removed
    if (!status) {
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
}

void suspend_pcb(const char *name) {
    // get pcb struct from name
    struct pcb* cur_pcb = pcb_find(name);

    // check if name is valid
    if (cur_pcb == NULL) {
        char err[] = "ERR: PCB does not exist\n";
        sys_req(WRITE, COM1, err, strlen(err));
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
    if (!status) {
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
}

void resume_pcb(const char *name) {
    // get pcb struct from name
    struct pcb* cur_pcb = pcb_find(name);

    // check if name is valid
    if (cur_pcb == NULL) {
        char err[] = "ERR: PCB does not exist\n";
        sys_req(WRITE, COM1, err, strlen(err));
    }

    // remove pcb from current queue
    int status = pcb_remove(cur_pcb);

    // check if removed
    if (!status) {
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
}

