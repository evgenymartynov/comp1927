// Author: evgenym
// Tutorial: Tue 11 Bell
// Date: 27 Jul 2012

////////////////////////////////////////////////////////////////////////
//                           == Warning ==
//                   Lots of sardonic comments ahead.
////////////////////////////////////////////////////////////////////////

// Summary: Implementation of a LIFO data structure in form of a
// singleton pattern. Why we are doing a singleton instead of a normal
// ADT is beyond me -- did someone get too lazy and copy out Sedgewick's
// code? Just curious.

// P.S. no, we did *not* go into singleton pattern in the lecture.

// P.P.S. I have adopted the structure of the code given to us -- but
// I've renamed some stuff. "static Item *s", seriously?

// P.P.P.S. The current interface does not let you destroy the stack.
// Whoopsmemoryleak?

////////////////////////////////////////////////////////////////////////

// Array-based stack implementation
// (from Sedgewick, Algorithms in C)
#include <stdio.h>
#include <stdlib.h>
#include "Item.h"
#include "STACK.h"

// Combine some stuff into a logical block.
typedef struct {
    Item *dataItems;
    int currentSize;
    int capacity;
} Stack;

// The singleton instance... sigh.
static Stack stack;

// Wrapper for LIFO-raised errors.
static void STACKerror(char *message);


////////////////////////////////////////////////////////////////////////
//
// Interface implementation code starts here
//
////////////////////////////////////////////////////////////////////////

// Initialise the LIFO instance with a given maximal size.
// Must be called before any other functions.
void STACKinit(int maxSize) {
    stack.dataItems = (Item*)malloc(maxSize * sizeof(Item));
    stack.currentSize = 0;
    stack.capacity = maxSize;
}

// Check whether the LIFO is empty.
int STACKempty() {
    return stack.currentSize == 0;
}

// Add an item to the LIFO.
void STACKpush(Item item) {
    // Sanity-check for exceeding capacity
    if (stack.currentSize >= stack.capacity) {
        STACKerror("stack overflow");
    }

    stack.dataItems[stack.currentSize] = item;
    stack.currentSize++;
}

// Get an item off the LIFO.
Item STACKpop() {
    // Sanity-check for exceeding capacity
    if (stack.currentSize <= 0) {
        STACKerror("stack underflow");
    }

    stack.currentSize--;
    return stack.dataItems[stack.currentSize];
}

// Print out an error message and do whatever. In this case, abort.
// Ideally, this also should free allocated memory...
static void STACKerror(char *message) {
    printf("%s\n", message);
    abort();
}
