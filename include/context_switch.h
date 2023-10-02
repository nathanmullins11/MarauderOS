#include <stdint.h>


struct context {
    /* general-purpose registers */
    uint32_t ESP;  // Stack Pointer (in the SS segment)
    uint32_t EAX;  // Accumulator
    uint32_t EBX;  // Pointer to data in DS
    uint32_t ECX;  // Counter for string/loop operations
    uint32_t EDX;  // I/O pointer
    uint32_t ESI;  // Pointer to data in segment pointed to by DS, source pointer
    uint32_t EDI;  // Pointer to data/destination in segment pointed to by ES
    uint32_t EBP;  // Pointer to data on the stack (in the SS)

    /* segment registers */ 
    uint16_t DS; // data segment
    uint16_t ES; // data segment
    uint16_t FS; // F segment (data register)
    uint16_t GS; // G segment (data register)
    uint16_t SS; // stack segment


    /* status and control registers */
    uint32_t EIP; // instruction pointer
    uint16_t CS; // code segment 
    uint32_t EFLAGS; // flags register
};

/**
 * @brief 
 * @param struct context* pointer to a struct representing the context of a proccess
 * @return struct context* pointer to the context of the process to be loaded
 */
struct context* sys_call(struct context* context_ptr);

