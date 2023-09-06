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
<<<<<<< HEAD
 * @brief struct Node (doubly linked list) holds the data of the node, a pointer to the next node, and a pointer to the previous node
 * @param data points to the char array data to be entered into the linked list
 * @param next points to next node in linked list
 * @param prev points to previous node in linked list
=======
 * @brief 
 * 
>>>>>>> a54a4c8 (update/clean serail.c)
 */
struct Node {
  char* data;
  struct Node* next;
  struct Node* prev;
};

/**
<<<<<<< HEAD
 * @brief place new node at front of linked list
 * 
 * @param head double pointer that points to the front or "head" of linked list
 * @param data pointer that points to the char array data to be entered into the linked list
=======
 * @brief 
 * 
 * @param head 
 * @param data 
>>>>>>> a54a4c8 (update/clean serail.c)
 */
void insertFront(struct Node** head, char* data);

/**
<<<<<<< HEAD
 * @brief insert a new node in the linked list after a user specified node
 * 
 * @param prev_node points to the node that you want to insert the new node after
 * @param data points to the array of data inserted into the new node
=======
 * @brief 
 * 
 * @param prev_node 
 * @param data 
>>>>>>> a54a4c8 (update/clean serail.c)
 */
void insertAfter(struct Node* prev_node, char* data);

/**
<<<<<<< HEAD
 * @brief insert a new node at the end of the linked list
 * 
 * @param head points to the front or "head" of linked list
 * @param data points to the array of data inserted into the new node
=======
 * @brief 
 * 
 * @param head 
 * @param data 
>>>>>>> a54a4c8 (update/clean serail.c)
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




