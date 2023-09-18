/**
 * @file pcb.h
 * @brief pcb functions 
 */

#include <process_queue.h>

#define MAX_NAME_LEN 8

struct pcb {
        char *name_ptr;
        char name_arr[MAX_NAME_LEN];
        struct process *process_ptr;
};

struct state {
    char* execution_state; // ready, running, or blocked
    char* dispatching_state; // suspended or not suspended
};

struct process {
        char* process_name;
        int pcb_class; // class = 0 -> user app, class = 1 -> system process
        int pcb_priority; // an int between 0 (high pri) and 9 (low pri)
        struct state pcb_state;
        char pcb_stack[1024]; // process stack of at least 1024
        void* stack_ptr; // pointer to current location in stack
        struct process* next_process; // related pcbs
};

/* Declaration: Put this in your header file */
extern struct pcb *queue_head;


/* Definition: Put this in exactly one source file */
struct pcb *queue_head;

/**
 * @brief 
 * 
 * @return struct pcb* 
 */
struct pcb* pcb_allocate(void);

/**
 * @brief 
 * 
 * @param process 
 * @return int 
 */
int pcb_free(struct pcb* process);

/**
 * @brief 
 * 
 * @param process_name 
 * @param priority 
 * @return struct pcb* 
 */
struct pcb* pcb_setup(const char* process_name, int class, int priority);

/**
 * @brief searches through all process queues to find a process with the provided name
 * 
 * @param process name of process to be found
 * @return struct pcb* pointer to PCB if process with name was found, otherwise returns null
 */
struct pcb* pcb_find(const char* process);

/**
 * @brief insert a PCB into the appropiate queue based on state and priority
 * 
 * @param process a pointer to the PCB to enqueue
 */
void pcb_insert(struct pcb* process);

/**
 * @brief 
 * 
 * @param process 
 * @return int 
 */
int pcb_remove(struct pcb* process);

