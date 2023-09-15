/**
 * @file pcb.h
 * @brief pcb functions 
 */

#define MAX_NAME_LEN 1

struct pcb {
        char *name_ptr;
        char name_arr[MAX_NAME_LEN];
};

struct state {
    char* execution_state; // ready, running, or blocked
    char* dispatching_state; // suspended or not suspended
};

struct process {
        char* pcb_name;
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

struct pcb* pcb_allocate(void);

int pcb_free(struct pcb*);

struct pcb* pcb_setup(const char*, int, int);

struct pcb* pcb_find(const char*);

void pcb_insert(struct pcb*);

int pcb_remove(struct pcb*);

