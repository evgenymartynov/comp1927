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

static Node splitInHalf(Node head);
static Node mergeLists(Node left, Node right);


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

    // Check if the length of this list is sufficiently small.
    // We only need to check the first MSORT_CUTOFF_LENGTH+1 nodes.
    int length = 0;
    Node temp = head;
    while (length <= MSORT_CUTOFF_LENGTH && temp != NULL) {
        length++;
        temp = temp->next;
    }

    // If the list is small, we insertion-sort it.
    if (length <= MSORT_CUTOFF_LENGTH) {
        return insertionsortWorker(head);
    }

    // Otherwise, we run the standard mergesort procedure.
    Node mid    = splitInHalf(head);
    Node left   = mergesortWorker(head);
    Node right  = mergesortWorker(mid);
    Node sorted = mergeLists(left, right);

    return sorted;
}


// Performs quicksort with the first node taken as pivot.
static Node quicksortWorker(Node head) {
    // If we are given an empty or one-node list, it is already sorted.
    if (head == NULL || head->next == NULL) {
        return head;
    }

    // Partition the list...
    Node pivot = head;
    Node curr = head->next;
    Node left = NULL, right = NULL;

    while (curr != NULL) {
        Node next = curr->next;

        // Smaller values go to the left, larger-or-equal to the right.
        if (curr->value < pivot->value) {
            curr->next = left;
            left = curr;
        } else {
            curr->next = right;
            right = curr;
        }

        curr = next;
    }

    // Now recurse
    left  = quicksortWorker(left);
    right = quicksortWorker(right);

    // Join pivot to the right half.
    pivot->next = right;
    right = pivot;

    // Find the last node of the left list.
    Node leftTail = left;
    while (leftTail != NULL && leftTail->next != NULL) {
        leftTail = leftTail->next;
    }

    // Recombine the (possibly empty) left list with the right list.
    Node sorted;
    if (left != NULL) {
        leftTail->next = right;
        sorted = left;
    } else {
        sorted = right;
    }

    return sorted;
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


// Splits the list in half, returns start of the right half.
static Node splitInHalf(Node head) {
    // If given an empty or degenerate list, there is no second half.
    if (head == NULL || head->next == NULL) {
        return NULL;
    }

    // We walk along the list with two pointers, advancing the left ptr
    // by one step, and the right by two steps.
    Node left = head, right = head;
    Node prevLeft = NULL;

    while (right != NULL) {
        // Advance left by one
        prevLeft = left;
        left     = left->next;

        // Advance right by two
        if (right->next != NULL) {
            right = right->next->next;
        } else {
            right = right->next;
        }
    }

    assert(prevLeft != NULL);

    // Now left points to the end of left half.
    // We make right start from the next node, and return that.
    right = prevLeft->next;
    prevLeft->next = NULL;

    return right;
}


// Merges two sorted lists into one and returns the head of sorted list.
static Node mergeLists(Node left, Node right) {
    // No-op on empty lists.
    if (left == NULL && right == NULL) {
        return NULL;
    } else if (left == NULL) {
        return right;
    } else if (right == NULL) {
        return left;
    }

    // Now we know that both lists are non-empty.

    Node sorted = NULL;
    Node tail = NULL;

    // Work out the first node of the sorted list.
    if (left->value < right->value) {
        sorted = tail = left;
        left = left->next;
    } else {
        sorted = tail = right;
        right = right->next;
    }

    sorted->next = NULL;

    // Now go through the rest and merge normally
    while (left != NULL && right != NULL) {
        if (left->value < right->value) {
            tail->next = left;
            left = left->next;
        } else {
            tail->next = right;
            right = right->next;
        }

        tail = tail->next;
        tail->next = NULL;
    }

    // Now join the remaining single non-empty list to the sorted list.
    if (left != NULL) {
        tail->next = left;
    } else if (right != NULL) {
        tail->next = right;
    }

    return sorted;
}
