// Author: evgenym
// Tutorial: Tue 11 Bell
// Date: 18 Jul 2012
// Summary: Lab02 exercises

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "lists.h"

//
// Local prototypes
//

// Dispose of an entire list.
// Required by testLists.c also, thus not static.
void disposeList(link first);
void disposeDoublyLinkedList(dlink first);

// Create a new node, initialised with the item provided.
static link newNode(Item item);

// Prepends a list with a given node, returning the updated list.
// Assumes that newNode has no successor elements -- they are discarded.
// Also works on a NULL list; however new node must be non-NULL.
static link prependList(link list, link newNode);

// Two different implementations of fromTo() as per exercise.
// Not defined static, as GCC will throw warnings if one of them is
// unused.
// Can be avoided with GCC's __attrubute__((unused)), but that is
// against the style guide.
link fromToLoop(int start, int end);
link fromToRecursive(int start, int end);

// Create a new node for a doubly-linked list.
static dlink newDoublyLinkedNode(Item item);

// Join 2 nodes of a doubly-linked list.
// End result looks like "... <=> first <=> second <=> ..."
// Nether link can be NULL.
static void linkDoubleNodes(dlink first, dlink second);

// Printing function exposed to testLists.c
// Prints out a doubly-linked list, while also checking link
// consistency.
void printDoublyLinkedList(dlink list);

////////////////////////////////////////////////////////////////////////
//
// Interface implementation code starts here
//
////////////////////////////////////////////////////////////////////////

// Pretty-print a list.
// Adapted from the given lists.c
void printList (link list) {
    link curr = list;

    while (curr != NULL) {
        printf ("[%d]->", curr->item);
        curr = curr->next;
    }
    printf ("[X]\n");
}

// Add up all items in a list and return the sum.
int sumListItems(link first) {
    int total = 0;
    link current = first;

    // Walk through the entire list, starting from the given node.
    while (current != NULL) {
        total += current->item;
        current = current->next;
    }

    return total;
}

// Create a list with elements in range [start..end].
// Returns empty list if start > end.
link fromTo(int start, int end) {
    return fromToRecursive(start, end);
    // return fromToLoop(start, end);
}

// Convert a given singly-linked list into a doubly-linked list.
// The relative order of items is preserved.
dlink doublify(link list) {
    // Stores the start of the new list.
    dlink newList = NULL;

    // Stores the end of the new list.
    dlink tailNode = NULL;

    link curr = list;
    while (curr) {
        dlink newNode = newDoublyLinkedNode(curr->item);

        if (tailNode == NULL) {
            // This is the very first node.
            newList = newNode;
        } else {
            // We have something to append to.
            linkDoubleNodes(tailNode, newNode);
        }

        newNode->item = curr-> item;
        curr = curr->next;

        // Last node is now the new node.
        tailNode = newNode;
    }

    return newList;
}


////////////////////////////////////////////////////////////////////////
//
// Internal implementation code for singly-linked lists starts here.
// Functions below are used internally, or by testLists.c
// but are not exposed through lists.h
//
////////////////////////////////////////////////////////////////////////


static link newNode(Item item) {
    link node = (link)malloc(sizeof(*node));
    assert(node != NULL);

    node->item = item;
    node->next = NULL;

    return node;
}

void disposeList(link first) {
    link current = first;
    while (current != NULL) {
        link next = current->next;
        free (current);
        current = next;
    }
}

// Loop implementation of fromTo().
// Walks through the number range in reverse, prepending the resulting
// list with new nodes.
link fromToLoop(int start, int end) {
    link list = NULL;

    int i;
    for (i = end; i >= start; i--) {
        list = prependList(list, newNode(i));
    }

    return list;
}

// Recursive implementation of fromTo().
// Generates a list with range [start+1, end], and prepends node start.
// Maximal list length is effectively bound by the stack size.
link fromToRecursive(int start, int end) {
    link list = NULL;

    // If there are nodes to be created...
    if (start <= end) {
        // Create the tail of the list
        list = fromToRecursive(start+1, end);
        // Attach the head to tail
        list = prependList(list, newNode(start));
    }

    return list;
}


static link prependList(link list, link newNode) {
    assert(newNode != NULL);
    assert(newNode->next == NULL);

    // newNode is now the head of the list
    newNode->next = list;
    return newNode;
}


////////////////////////////////////////////////////////////////////////
//
// Internal implementation code for doubly-linked lists starts here.
//
////////////////////////////////////////////////////////////////////////

static dlink newDoublyLinkedNode(Item item) {
    dlink node = (dlink)malloc(sizeof(*node));
    assert(node != NULL);

    node->prev = NULL;
    node->next = NULL;
    node->item = item;

    return node;
}

static void linkDoubleNodes(dlink first, dlink second) {
    assert(first != NULL);
    assert(second != NULL);

    first->next = second;
    second->prev = first;
}

void disposeDoublyLinkedList(dlink first) {
    dlink curr = first;

    while (curr) {
        dlink next = curr->next;
        free(curr);
        curr = next;
    }
}

void printDoublyLinkedList(dlink list) {
    dlink prev = NULL;
    dlink curr = list;

    while (curr != NULL) {
        assert(prev == curr->prev);

        printf("[%d]<=>", curr->item);

        prev = curr;
        curr = curr->next;
    }

    printf("[X]\n");
}
