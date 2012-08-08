// Author: evgenym
// Tutorial: Tue 11 Bell
// Date: 8 Aug 2012
// Summary: Assignment 1, memory allocator

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "allocator.h"


// Local defines
#define MIN_ALLOCATOR_SIZE  sizeof(header)
#define HEADER_MAGIC        0xBADA110C


// Local typedefs
typedef unsigned char byte;
typedef u_int32_t size_t;

typedef struct _header *Header;
typedef struct _header {
    int magic;
    size_t size;
    Header prev, next;
} header;

// Globals
static void *memory = NULL;
static Header freelist_head = NULL;

//
// Local protyotypes
//

static size_t round_up_power_of_two(size_t size);
static void freelist_init(size_t size);


////////////////////////////////////////////////////////////////////////
//
// Implementation starts here
//
////////////////////////////////////////////////////////////////////////

// Create a new allocator with a given minimal size.
void allocator_init(u_int32_t size) {
    // If was already initialised, then do nothing even if a different
    // size is used.
    if (memory != NULL) {
        return;
    }

    // Round up allocation size to a power of two, if needed.
    assert(size > MIN_ALLOCATOR_SIZE);
    size = round_up_power_of_two(size);

    // Allocate that chunk normally
    memory = malloc(size);
    assert(memory != NULL);

    // Set up the free list
    freelist_init(size);
}

// Destroy the allocator.
void allocator_end(void) {
    free(memory);

    memory = NULL;
    freelist_head = NULL;
}


////////////////////////////////////////////////////////////////////////
//
// Internal funcs start here
//
////////////////////////////////////////////////////////////////////////

// Rounds up an integer to the next power of two.
// Undefined behaviour occurs when 0 is given.
static size_t round_up_power_of_two(size_t num) {
    int i;
    for (i = sizeof(num)*8 - 1; i >= 0; i--) {
        // If the ith bit is set...
        if (num & (1 << i)) {
            // and if it is not the only bit set
            if (num ^ (1 << i)) {
                num = 1 << (i+1);
            }

            break;
        }
    }

    // Sanity check that it is indeed a power of two
    assert(__builtin_popcount(num) == 1);

    return num;
}

// Initialises the free list for entire allocated region.
static void freelist_init(size_t size) {
    freelist_head = (Header)memory;
    freelist_head->magic = HEADER_MAGIC;
    freelist_head->size = size;
    freelist_head->prev = freelist_head->next = freelist_head;
}
