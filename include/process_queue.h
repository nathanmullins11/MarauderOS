/**
 * @file process queue
 * @brief linked list structure to use as a process queue
 */

 // TODO: Update documentation

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <memory.h>
#include <mpx/device.h>


struct node {
  struct node* next;
  struct node* prev;
  struct pcb* pcb;
};

struct queue {
    struct node* front;
    struct node* rear;
};

extern struct queue* global_ready_queue;
extern struct queue* global_suspended_ready_queue;
extern struct queue* global_blocked_queue;
extern struct queue* global_suspended_blocked_queue;

/**
 * @brief Create a queue object
 *  NOT SURE THIS IS NEEDED?
 * @return struct queue* 
 */ 
struct queue* create_queue(void);

/**
 * @brief insert a new node at the end/tail of the linked list
 * 
 * @param rear points to the end of the linked list
 * @param pcb points to the pcb to be placed into the queue
 */
void enqueue(char* status, struct pcb* pcb);

/**
 * @brief delete specified node from linked list
 * FIX COMMENTS
 * 
 * @param front points to the front or "head" of linked list
 * @param del_node points to node to be deleted from linked list
 */
void dequeue(char* status, struct pcb* pcb);

/**
 * @brief search for a process in each of the queues by checking for execution and dispatch state
 * 
 * @param pcb_ptr pointer to the relative pcb of the process
 * @return char* returns which queue the pcb is in. i.e. returns "ready", "blocked", "suspended ready", or "suspended blocked" when function is called, should be stored in a variable
 */
char* search(struct pcb* pcb_ptr);

/**
 * @brief Create a node object
 * 
 * @param pcb 
 * @return struct node* 
 */
struct node* create_node(struct pcb* pcb);

