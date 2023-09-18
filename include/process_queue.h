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

struct queue* create_queue(void);

/**
 * @brief insert a new node at the beggining of the linked list
 * 
 * @param front points to the front or "head" of linked list
 * @param data points to the array of data inserted into the new node
 */
void enqueue(struct node** rear, struct pcb* pcb);

/**
 * @brief delete specified node from linked list
 * FIX COMMENTS
 * 
 * @param front points to the front or "head" of linked list
 * @param del_node points to node to be deleted from linked list
 */
void dequeue(struct node** front, struct node* pcb);

