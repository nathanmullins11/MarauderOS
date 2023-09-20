#include <pcb.h>

void delete_pcb(const char* name)
{
    // find pcb with given name
    struct pcb* pcb_to_delete = pcb_find(name);

    // remove pcb from its associated queue
    int status_code = pcb_remove(pcb_to_delete);

    // error deleting pcb
    if(status_code == 1)
    {
        return;
    }

    // free mem
    pcb_free(pcb_to_delete);

}
