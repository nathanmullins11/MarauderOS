#include "sys_req.h"
#include <pcb.h>

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
