// Author: evgenym
// Tutorial: Tue 11 Bell
// Date: 20 Aug 2012
// Summary: Linked-list ADT implementation.

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "List.h"

// Defines go here
#define MSORT_CUTOFF_LENGTH 3


// Local struct typedefs

typedef struct _node *Node;
struct _node {
    int value;
    Node next;
};

struct _list {
    Node head;
};

// Local prototypes
static Node newNode(Node next, int value);
static void freeNode(Node node);
static void __attribute__((unused)) printList(Node head);

static Node mergesortWorker(Node head);
static Node insertionsortWorker(Node head);
static Node quicksortWorker(Node head);

///////////////////////////////////////////////////////////////////////
//
// Interface implementation code starts here
//
///////////////////////////////////////////////////////////////////////


// Returns an empty list
List createEmptyList(void) {
    List list = (List)malloc(sizeof(*list));
    assert(list != NULL);

    list->head = NULL;

    return list;
}


// Frees the entire list
List freeList(List list) {
    assert(list != NULL);

    // Walk through the list, freeing node-by-node.
    Node curr;
    for (curr = list->head; curr != NULL; curr = curr->next) {
        freeNode(curr);
    }

    // Free the list struct as well.
    free(list);

    // We have no way to ensure error-checking. Always return NULL.
    return NULL;
}


// Prepends an item to a list
List add(List list, int value) {
    assert(list != NULL);

    Node node = newNode(list->head, value);
    list->head = node;

    return list;
}


// Check if the list contains node with the given value.
bool contains(List list, int searchValue) {
    assert(list != NULL);

    bool found = FALSE;
    Node curr;

    // Walk through the list, checking every element.
    for (curr = list->head; !found && curr != NULL; curr = curr->next) {
        if (curr->value == searchValue) {
            found = TRUE;
        }
    }

    return found;
}


// Calculate the size of the list.
int size(List list) {
    assert(list != NULL);

    int length = 0;
    Node curr;

    // Walk through the list, keeping track of how many nodes we've seen
    for (curr = list->head; curr != NULL; curr = curr->next) {
        length++;
    }

    return length;
}


// Check if the list is in sorted order.
bool isSorted(List list) {
    assert(list != NULL);

    // An empty list is always sorted.
    if (size(list) == 0) {
        return TRUE;
    }

    // We assume the list is sorted until proven otherwise.
    bool sorted = TRUE;
    Node curr;
    int prevValue = list->head->value;

    // Walk through the list, keeping track of the value previously seen
    for (curr = list->head; sorted && curr != NULL; curr = curr->next) {
        if (curr->value < prevValue) {
            sorted = FALSE;
        }

        prevValue = curr->value;
    }

    return sorted;
}


// Copies the first n elements into the given array.
void copyToArray(List list, int arr[], int n) {
    assert(list != NULL);

    Node curr;
    int i;

    // Only check the first n nodes, or until we hit the end of the list
    for (i = 0, curr = list->head; i < n && curr != NULL;
        i++, curr = curr->next) {
        arr[i] = curr->value;
    }
}


// Performs mergesort on the list.
List mergesort(List list) {
    assert(list != NULL);

    // We will drop the List abstraction for the sort, and instead
    // work with individual Nodes. This might be unmaintainable, but
    // that is the cleanest way of doing it, given the current ADT.
    Node sortedHead = mergesortWorker(list->head);
    list->head = sortedHead;

    return list;
}


// Performs quicksort on the list.
List quicksort(List list) {
    assert(list != NULL);

    Node sortedHead = quicksortWorker(list->head);
    list->head = sortedHead;

    return list;
}


///////////////////////////////////////////////////////////////////////
//
// Internal implementation code starts here
//
///////////////////////////////////////////////////////////////////////


// Creates a new node with the given parameters
static Node newNode(Node next, int value) {
    Node node = (Node)malloc(sizeof(*node));
    assert(node != NULL);

    node->next  = next;
    node->value = value;

    return node;
}


// Frees an allocated node
static void freeNode(Node node) {
    free(node);
}


// Displays the list starting from a given node.
static void printList(Node head) {
    for (; head != NULL; head = head->next) {
        printf("<%d|%p> ", head->value, (void*)head);
    }

    printf("<X>\n");
}


// Performs mergesort on a list starting with the given node.
// Note that this sort is in-place; it does not allocate any new nodes.
static Node mergesortWorker(Node head) {
    // If we are given an empty or one-node list, it is already sorted.
    if (head == NULL || head->next == NULL) {
        return head;
    }

    return insertionsortWorker(head);

    // // Check if the length of this list is sufficiently small.
    // // We only need to check the first MSORT_CUTOFF_LENGTH+1 nodes.
    // int length = 0;
    // Node temp = head;
    // while (length <= MSORT_CUTOFF_LENGTH && temp != NULL) {
    //     length++;
    //     temp = temp->next;
    // }

    // // If the list is small, we insertion-sort it.
    // if (length < MSORT_CUTOFF_LENGTH) {
    //     return insertionsortWorker(head);
    // }

    // // Otherwise, we run the standard mergesort procedure.
    // Node mid    = splitInHalf(head);
    // Node left   = mergesortWorker(head);
    // Node right  = mergesortWorker(mid);
    // Node sorted = mergeLists(left, right);

    // return sorted;
}


static Node quicksortWorker(Node head) {
    // If we are given an empty or one-node list, it is already sorted.
    if (head == NULL || head->next == NULL) {
        return head;
    }

    // TODO

    return insertionsortWorker(head);
}


// Performs insertion sort on a list starting with the given node.
static Node insertionsortWorker(Node head) {
    assert(head != NULL);

    // If the list one-long, it's sorted.
    if (head->next == NULL) {
        return head;
    }

    // Keep track of the rest of the list.
    Node rest = head->next;

    // And of the sorted list.
    Node sortedHead = head;
    sortedHead->next = NULL;

    // While there are nodes to sort...
    while (rest != NULL) {
        // extract one node...
        Node curr = rest;
        rest = rest->next;
        curr->next = NULL;

        // and find a spot where we should insert it.
        // It will go between prev and next:
        Node prev = NULL;
        Node next = sortedHead;
        while (next != NULL && curr->value > next->value) {
            prev = next;
            next = next->next;
        }

        // Here we have 3 cases:
        if (prev == NULL) {
            // The current node goes to the front...
            curr->next = sortedHead;
            sortedHead = curr;
        } else if (next == NULL) {
            // Or to the end...
            prev->next = curr;
        } else {
            // Or to the middle of the list.
            curr->next = next;
            prev->next = curr;
        }
    }

    // We return the start of the sorted list.
    return sortedHead;
}
