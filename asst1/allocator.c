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
#define HEADER_FREE_MAGIC   0xBADA110C
#define HEADER_USED_MAGIC   0xCAFFE14E


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
static int chunk_is_free(Header chunk);
static int __attribute__((unused)) chunk_is_used(Header chunk);
static Header chunk_create(void* where, size_t size);
static void* get_user_memory(Header chunk);

static void   freelist_init(size_t size);
static Header freelist_bestfit(size_t size);
static void   freelist_split_chunk(Header chunk, size_t size);
static int    freelist_has_one_chunk(void);
static void   freelist_extract_chunk(Header chunk);
static void   freelist_print(void);

////////////////////////////////////////////////////////////////////////
//
// Implementation starts here
//
////////////////////////////////////////////////////////////////////////


// Create a new allocator with a given minimal size.
void allocator_init(size_t size) {
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


// Make me a sandwich
void* allocator_malloc(size_t size) {
    // Make sure the allocator has been initialised.
    assert(memory != NULL);

    // Add bookkeeping costs.
    size += sizeof(header);
    // printf("  looking for %u space\n", size);

    // printf(">> before: "); freelist_print();

    // Find a free chunk
    Header chunk = freelist_bestfit(size);
    if (chunk == NULL) {
        return NULL;
    }

    // Ensure that the chunk we received was valid.
    assert(chunk_is_free(chunk));
    assert(size <= chunk->size);

    // Perform the split, or resize the request to attain a perfect fit.
    if (size + sizeof(header) > chunk->size) {
        size = chunk->size;
    } else {
        freelist_split_chunk(chunk, size);
        assert(chunk->size >= size);
    }

    // But if this is the only chunk in the free list, bail out.
    if (freelist_has_one_chunk()) {
        // This must be true, also.
        assert(chunk == freelist_head);

        return NULL;
    }

    // Mark the chunk as used.
    chunk->magic = HEADER_USED_MAGIC;

    // Remove it from the free list.
    freelist_extract_chunk(chunk);

    // printf("<< after: "); freelist_print();

    return get_user_memory(chunk);
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


// Ensure that the magic field is properly set.
// If chunk is NULL, pretend that magic matched.
static int chunk_is_free(Header chunk) {
    return chunk == NULL || chunk->magic == HEADER_FREE_MAGIC;
}

static int chunk_is_used(Header chunk) {
    return chunk == NULL || chunk->magic == HEADER_USED_MAGIC;
}


// Set up a chunk at a specified address.
static Header chunk_create(void* where, size_t size) {
    Header chunk = where;
    chunk->magic = HEADER_FREE_MAGIC;
    chunk->size  = size;
    chunk->prev  = NULL;
    chunk->next  = NULL;
    return chunk;
}


// Returns usable memory pertaining to a given chunk.
static void* get_user_memory(Header chunk) {
    return (void*)chunk + sizeof(*chunk);
}


// Initialises the free list for entire allocated region.
static void freelist_init(size_t size) {
    freelist_head = (Header)memory;
    chunk_create(memory, size);
    freelist_head->prev = freelist_head->next = freelist_head;
}


// Check if the free list has only one chunk in it.
// This goes with the invariant as per the spec.
static int freelist_has_one_chunk(void) {
    return  freelist_head == freelist_head->prev &&
            freelist_head == freelist_head->next;
}


// Removes a given chunk from the free list.
// Assumes that this is not the only chunk in the list.
static void freelist_extract_chunk(Header chunk) {
    assert(!freelist_has_one_chunk());

    // We got to check if this is the head of the list, and if so,
    // we must update it.
    if (chunk == freelist_head) {
        freelist_head = freelist_head->next;
    }

    chunk->prev->next = chunk->next;
    chunk->next->prev = chunk->prev;

    chunk->prev = NULL;
    chunk->next = NULL;
}


// Locates a chunk that is sufficiently large for a requested size.
// Returns pointer to a corresponding header, or NULL if no chunk was
// large enough.
// Assumes the size of the header has already been accounted for.
static Header freelist_bestfit(size_t size) {
    Header chunk = freelist_head;
    Header best_fit = NULL;

    // Styleguide is against do-while. Bite me.
    // Go through all chunks in the free list.
    do {
        // If we can fit it
        if (chunk->size >= size) {
            // See if it is better than what we had before.
            if (best_fit == NULL || chunk->size < best_fit->size) {
                best_fit = chunk;
            }
        }

        chunk = chunk->next;
    } while(chunk != freelist_head);

    return best_fit;
}


// Splits a given chunk so that the requested size can fit.
// The spec says to do it using the halving method... wtf?
static void freelist_split_chunk(Header chunk, size_t size) {
    // Keep splitting while the chunk isn't "tight" enough.
    while (chunk->size / 2 >= size) {
        // Work out where to split.
        size_t split_size = chunk->size / 2;
        void* middle = (void*)chunk + split_size;

        // Create a new chunk on the right.
        Header right = chunk_create(middle, split_size);

        // Point new chunk to correct neighbours.
        right->next = chunk->next;
        right->prev = chunk;
        // Update old adjacent chunks to point properly.
        right->next->prev = right;
        right->prev->next = right;

        // Update the size of current chunk.
        chunk->size = split_size;
    }
}


// Displays the free list; for debugging.
static void __attribute__((unused)) freelist_print(void) {
    Header curr = freelist_head;

    do {
        printf("<%u> ", curr->size);
        curr = curr->next;
    } while (curr != freelist_head);

    printf("\n");
}
