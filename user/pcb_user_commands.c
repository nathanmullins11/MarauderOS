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
    char* process_state = pcb_to_show->process_ptr->pcb_state.execution_state;
    char* dispatching_state = pcb_to_show->process_ptr->pcb_state.dispatching_state;
    int priority = pcb_to_show->process_ptr->pcb_priority;

    /* display char arrays to the terminal */
    sys_req(WRITE, COM1, process_name, sizeof(process_name));
    sys_req(WRITE, COM1, process_class, sizeof(process_class));
    sys_req(WRITE, COM1, process_state, sizeof(process_state));
    sys_req(WRITE, COM1, dispatching_state, sizeof(dispatching_state));
    sys_req(WRITE, COM1, priority, sizeof(priority));
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
    
    // setup new pcb
    struct pcb* new_pcb = pcb_setup(name, class, priority);

    // insert new pcb into appropiate queue
    pcb_insert(new_pcb);
}
