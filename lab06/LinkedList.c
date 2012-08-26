// Author: evgenym
// Tutorial: Tue 11 Bell
// Date: 20 Aug 2012
// Summary: Linked-list ADT implementation.

// This was modified since I've demonstrated the code on Tuesday.
// Primarily, I dropped the two-different-structs approach, and
// simplified a small number of things.
// No massive changes though.

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "List.h"


// Defines go here
#define MSORT_CUTOFF_LENGTH 3


// Local struct typedefs

struct _list {
    int value;
    List next;
};

// Local prototypes
static void __attribute__((unused)) printList(List head);

static List insertionSort(List head);
static void partition(List head, List pivot, List *left, List *right);
static List splitInHalf(List head);
static List mergeLists(List left, List right);


///////////////////////////////////////////////////////////////////////
//
// Interface implementation code starts here
//
///////////////////////////////////////////////////////////////////////


// Returns an empty list
List createEmptyList(void) {
    return NULL;
}


// Frees an entire list
List freeList(List list) {
    if (list != NULL) {
        freeList(list->next);
        free(list);
    }

    // We have no way to ensure error-checking. Always return NULL.
    return NULL;
}


// Prepends an item to a list
List add(List list, int value) {
    List newNode = (List)malloc(sizeof(*newNode));
    assert(newNode != NULL);

    newNode->next  = list;
    newNode->value = value;

    return newNode;
}


// Check if the list contains node with the given value.
bool contains(List list, int searchValue) {
    List curr;

    for (curr = list; curr != NULL; curr = curr->next) {
        if (curr->value == searchValue) {
            return TRUE;
        }
    }

    return FALSE;
}


// Calculate the size of the list.
int size(List list) {
    int length = 0;
    List curr;

    // Walk through the list, keeping track of how many nodes we've seen
    for (curr = list; curr != NULL; curr = curr->next) {
        length++;
    }

    return length;
}


// Check if the list is in sorted order.
bool isSorted(List list) {
    // An empty list is always sorted.
    if (size(list) == 0) {
        return TRUE;
    }

    // We assume the list is sorted until proven otherwise.
    bool sorted = TRUE;
    List curr;
    int prevValue = list->value;

    // Walk through the list, keeping track of the value previously seen
    for (curr = list; sorted && curr != NULL; curr = curr->next) {
        if (curr->value <= prevValue) {
            sorted = FALSE;
        }

        prevValue = curr->value;
    }

    return sorted;
}


// Copies the first n elements into the given array.
void copyToArray(List list, int arr[], int n) {
    List curr;
    int i;

    // Only check the first n nodes, or until we hit the end of the list
    for (i = 0, curr = list; i < n && curr != NULL;
        i++, curr = curr->next) {
        arr[i] = curr->value;
    }
}


// Performs mergesort on a list.
// Note that this sort is in-place; it does not allocate any new nodes.
List mergesortList(List list) {
    // If we are given an empty or one-node list, it is already sorted.
    if (list == NULL || list->next == NULL) {
        return list;
    }

    // Check if the length of this list is sufficiently small.
    // We only need to check the first MSORT_CUTOFF_LENGTH+1 nodes.
    // We *could* defer to calling length(), but...
    int length = 0;
    List temp = list;
    while (length <= MSORT_CUTOFF_LENGTH && temp != NULL) {
        length++;
        temp = temp->next;
    }

    // If the list is small, we insertion-sort it.
    if (length <= MSORT_CUTOFF_LENGTH) {
        return insertionSort(list);
    }

    // Otherwise, we run the standard mergesort procedure.
    List mid    = splitInHalf(list);
    List left   = mergesortList(list);
    List right  = mergesortList(mid);
    List sorted = mergeLists(left, right);

    return sorted;
}


// Performs quicksort on the list.
List quicksortList(List list) {
    // If we are given an empty or one-node list, it is already sorted.
    if (list == NULL || list->next == NULL) {
        return list;
    }

    // Partition the list.
    List left, right;
    List pivot = list; // This is a poor choice for a pivot, but...
    partition(list->next, pivot, &left, &right);

    // Sort the partitions.
    left  = quicksortList(left);
    right = quicksortList(right);

    // Join pivot to the right half.
    pivot->next = right;
    right = pivot;

    // Find the last node of the left list.
    List leftTail = left;
    while (leftTail != NULL && leftTail->next != NULL) {
        leftTail = leftTail->next;
    }

    // Recombine the (possibly empty) left list with the right list.
    List sorted;
    if (left != NULL) {
        leftTail->next = right;
        sorted = left;
    } else {
        sorted = right;
    }

    return sorted;
}


///////////////////////////////////////////////////////////////////////
//
// Internal implementation code starts here
//
///////////////////////////////////////////////////////////////////////


// Displays the list starting from a given node.
static void printList(List head) {
    for (; head != NULL; head = head->next) {
        printf("<%d|%p> ", head->value, (void*)head);
    }

    printf("<X>\n");
}


// Partitions the list into smaller/notsmaller parts.
static void partition(List head, List pivot, List *left, List *right) {
    List curr = head;
    *left = *right = NULL;

    while (curr != NULL) {
        List next = curr->next;

        // Smaller values go to the left, larger-or-equal to the right.
        if (curr->value < pivot->value) {
            curr->next = *left;
            *left = curr;
        } else {
            curr->next = *right;
            *right = curr;
        }

        curr = next;
    }
}


// Performs insertion sort on a list starting with the given node.
static List insertionSort(List head) {
    assert(head != NULL);

    // If the list one-long, it's sorted.
    if (head->next == NULL) {
        return head;
    }

    // Keep track of the rest of the list.
    List rest = head->next;

    // And of the sorted list.
    List sortedHead = head;
    sortedHead->next = NULL;

    // While there are nodes to sort...
    while (rest != NULL) {
        // extract one node...
        List curr = rest;
        rest = rest->next;
        curr->next = NULL;

        // and find a spot where we should insert it.
        // It will go between prev and next:
        List prev = NULL;
        List next = sortedHead;
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
static List splitInHalf(List head) {
    // If given an empty or degenerate list, there is no second half.
    if (head == NULL || head->next == NULL) {
        return NULL;
    }

    // We walk along the list with two pointers, advancing the left ptr
    // by one step, and the right by two steps.
    List left = head, right = head;
    List prevLeft = NULL;

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
static List mergeLists(List left, List right) {
    // No-op on empty lists.
    if (left == NULL && right == NULL) {
        return NULL;
    } else if (left == NULL) {
        return right;
    } else if (right == NULL) {
        return left;
    }

    // Now we know that both lists are non-empty.

    List sorted = NULL;
    List tail = NULL;

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
