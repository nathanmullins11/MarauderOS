#include <stdint.h>


struct context { // STRUCT ORDER = POP ORDER
    /* general-purpose registers */
    int ESP;  // Stack Pointer (in the SS segment)
    int EAX;  // Accumulator
    int EBX;  // Pointer to data in DS
    int ECX;  // Counter for string/loop operations
    int EDX;  // I/O pointer
    int ESI;  // Pointer to data in segment pointed to by DS, source pointer
    int EDI;  // Pointer to data/destination in segment pointed to by ES
    int EBP;  // Pointer to data on the stack (in the SS)

    /* segment registers */
    int DS; // data segment
    int ES; // data segment
    int FS; // F segment (data register)
    int GS; // G segment (data register)
    int SS; // stack segment

    /* status and control registers */
    int EIP; // instruction pointer
    int CS; // code segment 
    int EFLAGS; // flags register
};

/**
 * @brief handles system calls and context switching between processes
 * @param struct context* pointer to a struct representing the context of a proccess
 * @return pointer to the context of the next process to be loaded
 */
struct context* sys_call(struct context* context_ptr);

extern struct context* global_context_ptr;

extern struct pcb* global_current_process;

