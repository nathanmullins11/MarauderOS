/**
 * @file doubly_linked_list.h
 * @brief linked list structure to use to store previously entered commands
 */

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <memory.h>
#include <mpx/device.h>

struct Node {
  char* data;
  struct Node* next;
  struct Node* prev;
};

void insertFront(struct Node** head, char* data);

void insertAfter(struct Node* prev_node, char* data);

void insertEnd(struct Node** head, char* data);

void deleteNode(struct Node** head, struct Node* del_node);

void displayList(struct Node* node);




