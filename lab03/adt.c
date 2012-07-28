// Author: evgenym
// Tutorial: Tue 11 Bell
// Date: 27 Jul 2012

// Summary: Implementation of a singleton doubly-linked list.

// Details:
// 1.   We use a single dummy node as a sentinel on both ends.
//      List can thus be deemed circular.
// 2.   We also try to not raise errors in case of abuse, and recover if
//      possible.
// 3.   We have an invariant that currentNode is the dummy iff list is
//      empty.
//      Otherwise, current node points to some real node. Never NULL.
// 4.   As such, NULL never occurs in our pointers.

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "Item.h"

// Bools
#define TRUE  1
#define FALSE 0

// Where to insert a node relative to current node.
#define BEFORE -1
#define AFTER  1

typedef struct _node *ListNode;

typedef struct _node {
    ListNode prev, next;
    Item value;
} node;


// Local prototypes
static ListNode newNode(Item value);
static void disposeNode(ListNode node);
static int isEdgeNode(ListNode node);
static void moveForward(int numSteps);
static void moveBack(int numSteps);
static void insertNode(ListNode prev, ListNode newNode, ListNode next);
static void insertNodeRelativeToCurrent(Item newItem, int where);

// The list instance. We only need to know the current node as far as
// this interface goes.
static ListNode currentNode;

// This is the special edge node.
static ListNode dummyNode;


////////////////////////////////////////////////////////////////////////
//
// Interface implementation code starts here
//
////////////////////////////////////////////////////////////////////////

// Initialise the list.
void LISTinit() {
    currentNode = dummyNode = newNode(0);
    // Short-circuit the list.
    dummyNode->prev = dummyNode->next = dummyNode;
}

// Test if the list is empty.
int LISTempty() {
    return currentNode == dummyNode;
}

// Move n steps forward along the linked list.
// Returns 1 if the new element is a boundary element, 0 otherwise.
int LISTmove(int numSteps) {
    // Watch out for the empty-list case.
    if (!LISTempty()) {
        if (numSteps > 0) {
            moveForward(numSteps);
        } else if (numSteps < 0) {
            moveBack(-numSteps);
        }
    }

    return isEdgeNode(currentNode);
}

// Get element at current position.
// Here, we cannot recover if the list is empty.
Item LISTcurrent() {
    assert(!LISTempty());

    return currentNode->value;
}

// Insert a node with a given value BEFORE the current node, and make it
// current.
void LISTbefore(Item newItem) {
    insertNodeRelativeToCurrent(newItem, BEFORE);
}

// Insert a node with a given value AFTER the current node, and make it
// current.
void LISTafter(Item newItem) {
    insertNodeRelativeToCurrent(newItem, AFTER);
}

// Delete the current node and make previous one current.
// If first node is being deleted, then point to the next node.
// ...wait, ITEM? Fine, I will simply return the value of deleted node.
// The return value is thus unspecified for an empty list.
Item LISTdelete() {
    Item deletedValue = currentNode->value;

    if (!LISTempty()) {
        // Here, we work out the new current node.
        ListNode newCurrent;

        // Because we have dummy nodes, we can delete the node as if it
        // were in the middle of the list (not the beginning), and only
        // then adjust the new position.

        ListNode prevNode = currentNode->prev;
        ListNode nextNode = currentNode->next;

        // The former previous node becomes the new current node.
        newCurrent = prevNode;

        // which is then followed by the former next node and preceded
        // by the former previous node.
        nextNode->prev = prevNode;
        prevNode->next = nextNode;

        // If we've just deleted the first node...
        if (newCurrent == dummyNode) {
            // ...advance the list one step forward.
            newCurrent = newCurrent->next;
        }

        disposeNode(currentNode);
        currentNode = newCurrent;
    }

    return deletedValue;
}

// This is a debug function, ignore it. Not part of the interface, but
// used by the tester.
void LISTprint(void) {
    ListNode curr = dummyNode;

    printf("D -> ");
    while (curr->next != dummyNode) {
        curr = curr->next;

        if (curr == currentNode) {
            printf("[%d] -> ", curr->value);
        } else {
            printf("%d -> ", curr->value);
        }
    }
    printf("D\n");
}

////////////////////////////////////////////////////////////////////////
//
// Internal implementation code starts here
//
////////////////////////////////////////////////////////////////////////

static ListNode newNode(Item value) {
    ListNode node = (ListNode)malloc(sizeof(*node));
    assert(node != NULL);

    node->value = value;

    return node;
}

static void disposeNode(ListNode node) {
    free(node);
}

// Checks if a given node lies at the extremes of the list.
// We treat the dummy node as being an edge node for consistency.
static int isEdgeNode(ListNode node) {
    return  node == dummyNode       || \
            node->prev == dummyNode || \
            node->next == dummyNode ;
}

// Move numSteps forward along the list.
static void moveForward(int numSteps) {
    int i;
    for (i = 0; i < numSteps && currentNode->next != dummyNode; i++) {
        currentNode = currentNode->next;
    }
}

// Move numSteps back along the list.
// We have symmetry to moveForward, but we cannot exploit it.
static void moveBack(int numSteps) {
    int i;
    for (i = 0; i < numSteps && currentNode->prev != dummyNode; i++) {
        currentNode = currentNode->prev;
    }
}

// Inserts a node between the other two.
static void insertNode(ListNode prev, ListNode newNode, ListNode next) {
    prev->next = newNode;
    next->prev = newNode;

    newNode->prev = prev;
    newNode->next = next;
}

// Inserts a node with a given value relative to the current node.
// "Relative" means either immediately before or immediately after it.
static void insertNodeRelativeToCurrent(Item newItem, int where) {
    assert(where == BEFORE || where == AFTER);

    ListNode node = newNode(newItem);

    if (LISTempty()) {
        // An empty list is a special case.
        insertNode(dummyNode, node, dummyNode);
    } else {
        // Non-empty list...
        if (where == BEFORE) {
            insertNode(currentNode->prev, node, currentNode);
        } else {
            insertNode(currentNode, node, currentNode->next);
        }
    }

    // As per the spec, the inserted node becomes current.
    currentNode = node;
}
