/**
 * @file process queue
 * @brief linked list structure to use as a process queue
 */

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <memory.h>
#include <mpx/device.h>

/**
 * @brief struct to describe a node
 */
struct node {
  struct node* next;
  struct node* prev;
  struct pcb* pcb;
};

/**
 * @brief struct to describe a queue
 */
struct queue {
  struct node* front;
  struct node* rear;
};

/* define global queues */
extern struct queue* global_ready_queue;
extern struct queue* global_suspended_ready_queue;
extern struct queue* global_blocked_queue;
extern struct queue* global_suspended_blocked_queue;

/**
 * @brief Create a queue object
 *
 * @return struct queue* new queue created
 */ 
struct queue* create_queue(void);

/**
 * @brief insert a new node based on priority of process
 * 
 * @param status status of the pcb described by the enum
 * @param pcb points to the pcb to be placed into the queue
 */
void enqueue_pri(char* status, struct pcb* pcb);

/**
 * @brief insert a new node at tail/end of correct queue
 * 
 * @param status status of the pcb described by the enum
 * @param pcb points to the pcb to be placed into the queue
 */
void enqueue_reg(char* status, struct pcb* pcb);

/**
 * @brief delete specified node from linked list
 * 
 * @param status status of the pcb described by the enum
 * @param pcb points to pcb to be deleted from linked list
 */
void dequeue(const char* status, struct pcb* pcb);


/**
 * @brief search for a process in each of the queues by checking for execution and dispatch state
 * 
 * @param pcb_ptr pointer to the relative pcb of the process
 * @return char* returns which queue the pcb is in. i.e. returns "ready", "blocked", "suspended ready", 
 * or "suspended blocked" when function is called, should be stored in a variable
 */
char* search(struct pcb* pcb_ptr);

/**
 * @brief Create a node object
 * 
 * @param pcb pointer to a pcb object
 * @return struct node* 
 */
struct node* create_node(struct pcb* pcb);

