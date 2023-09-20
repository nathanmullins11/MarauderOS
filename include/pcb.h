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
        const char* process_name;
        int pcb_class; // class = 0 -> user app, class = 1 -> system process
        int pcb_priority; // an int between 0 (high pri) and 9 (low pri)
        struct state pcb_state;
        char pcb_stack[1024]; // process stack of at least 1024
        void* stack_ptr; // pointer to current location in stack
        struct process* next_process; // related pcbs
};

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

/**
 * @brief creates a new PCB and inserts it into the appropiate queue
 * 
 * @param name process name
 * @param class process class
 * @param priority process priority
 */
void create_pcb(const char* name, int class, int priority);

/**
 * @brief finds requested process and removed it from the queue
 * frees all associated memory
 * 
 * @param name process name 
 */
void delete_pcb(const char* name);

/**
 * @brief puts a process in the block state, moves to appropiate queue
 * 
 * @param name process name
 */
void block_pcb(const char* name);

/**
 * @brief puts a process into the ready state, moves to appropiate queue
 * 
 * @param name process name
 */
void unblock_pcb(const char* name);

/**
 * @brief puts a process in the suspended state, moves to appropiate queue
 * 
 * @param name process name
 */
void suspend_pcb(const char* name);

/**
 * @brief puts process into not suspended state, moves to appropiate queue
 * 
 * @param name process name
 */
void resume_pcb(const char* name);

/**
 * @brief changes a process' priority, moves in queue
 * 
 * @param name process name
 * @param priority new priority
 */
void set_pcb_priority(const char* name, int priority);

/**
 * @brief display's a process' data to the terminal
 * 
 * @param name process name
 */
void show_pcb(const char* name);

/**
 * @brief show all processes in ready state in terminal
 */
void show_ready(void);

/**
 * @brief show all processes in blocked state in terminal 
 */
void show_blocked(void);

/**
 * @brief show all processes in terminal 
 */
void show_all(void);
