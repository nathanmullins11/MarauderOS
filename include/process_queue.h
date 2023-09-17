/**
 * @file process queue
 * @brief linked list structure to use as a process queue
 */

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <memory.h>
#include <mpx/device.h>

struct ready_node {
  char* data;
  struct ready_node* next;
  struct ready_node* prev;
  int priority;
  struct pcb* attached_pcb;
};

struct blocked_node {
  char* data;
  struct blocked_node* next;
  struct blocked_node* prev;
  struct pcb* attached_pcb;
};

struct ready_queue {
    struct ready_node* front;
    struct ready_node* rear;
};

struct blocked_queue {
    struct blocked_node* front;
    struct blocked_node* rear;
};


extern struct ready_queue* global_ready_queue;
extern struct ready_queue* global_suspended_ready_queue;
extern struct blocked_queue* global_blocked_queue;
extern struct blocked_queue* global_suspended_blocked_queue;


struct ready_queue* create_ready_queue(void);

struct blocked_queue* create_blocked_queue(void);

struct suspended_ready_queue* create_suspended_ready_queue(void);

struct suspended_blocked_queue* create_suspended_blocked_queue(void);

void insert_ready(char* data, int priority);

void insert_blocked(char* data);

void insert_suspended_ready(char* data, int priority);

void insert_suspended_blocked(char* data);

/**
 * @brief insert a new node at the beggining of the linked list
 * 
 * @param head points to the front or "head" of linked list
 * @param data points to the array of data inserted into the new node
 */
void enqueue_ready(struct ready_node** head, char* data);

void enqueue_blocked(struct blocked_node** head, char* data);

/**
 * @brief delete specified node from linked list
 * 
 * @param head points to the front or "head" of linked list
 * @param del_node points to node to be deleted from linked list
 */
void deleteNode_ready(struct ready_node** head, struct ready_node* del_node);

void deleteNode_blocked(struct blocked_node** head, struct blocked_node* del_node);

