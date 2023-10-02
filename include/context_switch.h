#include <stdint.h>


struct context { // STRUCT ORDER = POP ORDER
    int ESP;
    int GS;
    int FS;
    int ES;
    int DS;
    int SS;
    int EDI;
    int ESI;
    int EBP;
    int EBX;
    int EDX;
    int ECX;
    int EAX;
    int EIP;
    int CS;
    int EFLAGS;
};


/**
 * @brief 
 * @param struct context* pointer to a struct representing the context of a proccess
 * @return struct context* pointer to the context of the process to be loaded
 */
struct context* sys_call(struct context* context_ptr);

