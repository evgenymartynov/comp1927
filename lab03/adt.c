// Author: evgenym
// Tutorial: Tue 11 Bell
// Date: 27 Jul 2012

// Summary: Implementation of a singleton doubly-linked list.

// Details:
// We do not have any dummy nodes as sentinels on the ends; we use NULL.
// We also try to not raise errors in case of abuse.
// We have an invariant that currentNode is NULL iff list is empty.

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "Item.h"

#define TRUE  1
#define FALSE 0

typedef struct _node *ListNode;

typedef struct _node {
    ListNode prev, next;
    Item value;
} node;


// Local prototypes
static ListNode newNode(Item value);
static void disposeNode(ListNode node);
static void moveForward(int n);
static void moveBack(int n);

// The list instance. We only need to know the current node as far as
// this interface goes.
static ListNode currentNode;


////////////////////////////////////////////////////////////////////////
//
// Interface implementation code starts here
//
////////////////////////////////////////////////////////////////////////

// Initialise the list.
void LISTinit() {
    // Not required, but just to make sure.
    currentNode = NULL;
}

// Test if the list is empty.
int LISTempty() {
    return currentNode == NULL;
}

// Move n steps forward along the linked list.
// Returns 1 if the new element is a boundary element, 0 otherwise.
int LISTmove(int n) {
    int isBoundary = FALSE;

    if (LISTempty()) {
        // Catch out empty-list case.
        isBoundary = TRUE;
    } else {
        // List is not empty.
        if (n > 0) {
            moveForward(n);
        } else if (n < 0) {
            moveBack(-n);
        }

        isBoundary = ( currentNode->prev == NULL || \
                       currentNode->next == NULL    );
    }

    return isBoundary;
}

// Get element at current position.
// Here, we freak out if the list is empty.
Item LISTcurrent() {
    assert(!LISTempty());

    return currentNode->value;
}

// Insert a node with a given value BEFORE the current node, and make it
// current.
void LISTbefore(Item newItem) {
    ListNode node = newNode(newItem);

    if (LISTempty()) {
        // An empty list is a special case, again.
        node->prev = NULL;
        node->next = NULL;
    } else {
        // Non-empty list...
        node->prev = currentNode->prev;
        node->next = currentNode;

        if (currentNode->prev != NULL) {
            currentNode->prev->next = node;
        }

        currentNode->prev = node;
    }

    // As per the spec, the inserted node becomes current.
    currentNode = node;
}

// Insert a node with a given value AFTER the current node, and make it
// current.
// Note the strong similarity to LISTbefore. Can't really do much about
// it, unfortunately.
void LISTafter(Item newItem) {
    ListNode node = newNode(newItem);

    if (LISTempty()) {
        // An empty list is a special case, again.
        node->prev = NULL;
        node->next = NULL;
    } else {
        // Non-empty list...
        node->prev = currentNode;
        node->next = currentNode->next;

        if (currentNode->next != NULL) {
            currentNode->next->prev = node;
        }

        currentNode->next = node;
    }

    // As per the spec, the inserted node becomes current.
    currentNode = node;
}

// Delete the current node and make previous one current.
// If first node is being deleted, then point to the next node.
// ...wait, ITEM? Fine, I will simply return the value of deleted node.
Item LISTdelete() {
    Item deletedValue = 0;

    if (!LISTempty()) {
        // Work out the new current node.
        ListNode newCurrent;

        if (currentNode->prev == NULL) {
            // This is the first node.
            newCurrent = currentNode->next;

            // Update the links if there are elements left.
            if (newCurrent) {
                newCurrent->prev = NULL;
            }
        } else {
            // Not the first node
            newCurrent = currentNode->prev;

            // Point next node to new node
            if (currentNode->next != NULL) {
                currentNode->next->prev = newCurrent;
            }

            // Point new node to next node
            if (newCurrent != NULL) {
                newCurrent->next = currentNode->next;
            }
        }

        deletedValue = currentNode->value;
        disposeNode(currentNode);
        currentNode = newCurrent;
    }

    return deletedValue;
}

// This is a debug function, ignore it. Not part of the interface, but
// used by the tester.
void LISTprint(void) {
    ListNode start = currentNode;
    while (start != NULL && start->prev != NULL) {
        start = start->prev;
    }

    for (; start != NULL; start = start->next) {
        if (start == currentNode) {
            printf("[%d] -> ", start->value);
        } else {
            printf("%d -> ", start->value);
        }
    }
    printf("NULL\n");
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

// We assume a non-null current node.
static void moveForward(int n) {
    assert(currentNode != NULL);

    int i;
    for (i = 0; i < n && currentNode->next != NULL; i++) {
        currentNode = currentNode->next;
    }
}

// We assume a non-null current node. Once again, we have symmetry to
// moveForward, but we cannot exploit it.
static void moveBack(int n) {
    assert(currentNode != NULL);

    int i;
    for (i = 0; i < n && currentNode->prev != NULL; i++) {
        currentNode = currentNode->prev;
    }
}
