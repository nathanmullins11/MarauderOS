/**
 * @file doubly_linked_list.h
 * @brief linked list structure to use to store previously entered commands
 */

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <memory.h>
#include <mpx/device.h>

/**
 * @brief 
 * 
 */
struct Node {
  char* data;
  struct Node* next;
  struct Node* prev;
};

/**
 * @brief 
 * 
 * @param head 
 * @param data 
 */
void insertFront(struct Node** head, char* data);

/**
 * @brief 
 * 
 * @param prev_node 
 * @param data 
 */
void insertAfter(struct Node* prev_node, char* data);

/**
 * @brief 
 * 
 * @param head 
 * @param data 
 */
void insertEnd(struct Node** head, char* data);

/**
 * @brief 
 * 
 * @param head 
 * @param del_node 
 */
void deleteNode(struct Node** head, struct Node* del_node);

/**
 * @brief 
 * 
 * @param node 
 */
void displayList(struct Node* node);




