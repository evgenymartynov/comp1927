// Author: evgenym
// Tutorial: Tue 11 Bell
// Date: 8 Aug 2012
// Summary: Assignment 1, test module for the memory allocator

#include <stdio.h>
#include <stdlib.h>
#include "allocator.h"

// Test prototypes
static void testAllocatorSetup(void);


////////////////////////////////////////////////////////////////////////
//
// Implementation starts here
//
////////////////////////////////////////////////////////////////////////


int main() {
    testAllocatorSetup();

    return EXIT_SUCCESS;
}


// Tests that _init() and _end() do not crash or raise any errors.
static void testAllocatorSetup(void) {
    printf("Testing allocator set-up sequence\n");
        printf("  allocator_init(64)\n");
        allocator_init(64);
        printf("  allocator_end()\n");
        allocator_end();
        printf("  reallocating with more memory\n");
        printf("  allocator_init(128)\n");
        allocator_init(128);
        printf("  allocator_end()\n");
        allocator_end();
        printf("  reallocating with not-power-of-2 memory\n");
        printf("  allocator_init(1337)\n");
        allocator_init(1337);
        printf("  allocator_end()\n");
        allocator_end();
    printf("Passed\n");
}
