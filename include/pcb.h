/**
 * @file pcb.h
 * @brief pcb functions 
 */

#include <process_queue.h>
#include <processes.h>

#define PCB_STACK_SIZE 1024

/**
 * @brief stuct to describe a process control block
 */
struct pcb {
        const char* name_arr;
        struct process *process_ptr;
};

/**
 * @brief enumeration to describe a state that a process can be in
 * Encompasses the pcb's execution state and dispatching state
 */
enum state {
        READY_NOT_SUSPENDED, // 0
        READY_SUSPENDED, // 1
        BLOCKED_NOT_SUSPENDED, // 2
        BLOCKED_SUSPENDED, // 3
        RUNNING // 4
};

/**
 * @brief struct to describe a process
 */
struct process {
        int pcb_class; // class = 0 -> user app, class = 1 -> system process
        enum state pcb_state;
        int pcb_priority; // an int between 0 (high pri) and 9 (low pri)
        char pcb_stack[PCB_STACK_SIZE]; // process stack of at least 1024
        void* stack_ptr; // pointer to current location in stack
        struct process* next_process; // related pcbs
};

/**
 * @brief allocates dynamic memory for a process struct
 * 
 * @return struct pcb* a pointer to the new struct
 */
struct pcb* pcb_allocate(void);

/**
 * @brief frees a specified pcb struct from memory
 * 
 * @param process the pcb to be freed
 * @return int completion status: 0 on success, otherwise fail
 */
int pcb_free(struct pcb* process);

/**
 * @brief allocates memory for new pcb and inserts data provided about the pcb
 * 
 * @param process_name name of the process, name must be > 8 characters
 * @param class pcb's class: 0 = user, 1 = system
 * @param priority pcb's priority, must be 0 <= pri <= 9
 * @return struct pcb* new pcb setup
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
 * @brief remove's a pcb from it's queue
 * 
 * @param process pointer to pcb to be dequeued
 * @return int completion status: 0 on success, 1 on fail
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
 * @param status indicates whether to show the table header
 */
void show_pcb(const char* name, int status);

/**
 * @brief show all processes in ready state in terminal
 * @param status indicates whether to show the table header
 */
void show_ready(int status);

/**
 * @brief show all processes in blocked state in terminal 
 * @param status indicates whether to show the table header
 */
void show_blocked(int status);

/**
 * @brief show all processes in terminal 
 */
void show_all(void);

/* R3 BELOW */

/**
 * @brief causes command handler to yield the cpu, if any processes are in queue, they will execute
 * 
 */
void yield(void);

/**
 * @brief load R3 test processes
 * 
 */
void Load_R3(void);
