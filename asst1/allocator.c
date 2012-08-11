// Author: evgenym
// Tutorial: Tue 11 Bell
// Date: 8 Aug 2012
// Summary: Assignment 1, memory allocator

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "allocator.h"


// Local defines
#define TRUE                1
#define FALSE               0
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
static void   *buffer_base = NULL;
static size_t buffer_size;
static Header freelist_head = NULL;

//
// Local prototypes
//

static size_t round_up_power_of_two(size_t size);
static void*  get_user_memory(Header chunk);

static Header chunk_create(void* where, size_t size);
static int    chunk_is_free(Header chunk);
static int    chunk_is_used(Header chunk);
static size_t chunk_get_offset(Header chunk);

static void   freelist_init(size_t size);
static void   freelist_destroy(void);
static Header freelist_bestfit(size_t size);
static void   freelist_split_chunk(Header chunk, size_t size);
static void   freelist_merge_chunk(Header chunk);
static int    freelist_has_one_chunk(void);
static void   freelist_insert_chunk(Header chunk);
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
    if (buffer_base != NULL) {
        return;
    }

    // Round up allocation size to a power of two, if needed.
    assert(size > MIN_ALLOCATOR_SIZE);
    size = round_up_power_of_two(size);
    buffer_size = size;

    // Allocate that chunk normally
    buffer_base = malloc(size);
    assert(buffer_base != NULL);

    // Set up the free list
    freelist_init(size);
}


// Destroy the allocator.
void allocator_end(void) {
    free(buffer_base);

    buffer_base = NULL;
    buffer_size = 0;
    freelist_destroy();
}


// Make me a sandwich
void* allocator_malloc(size_t size) {
    // Make sure the allocator has been initialised.
    assert(buffer_base != NULL);

    // Add bookkeeping costs.
    size += sizeof(header);

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

    return get_user_memory(chunk);
}


// Eat me a sandwich.
void allocator_free(void* region) {
    // Get the header for region we are freeing.
    Header chunk = region - sizeof(header);

    // Ensure the header is not corrupt.
    assert(chunk_is_used(chunk));

    // Mark it as free.
    chunk->magic = HEADER_FREE_MAGIC;

    // And re-insert into the free list.
    freelist_insert_chunk(chunk);

    // And merge freed chunk together with the others.
    freelist_merge_chunk(chunk);
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


// Returns offset of a chunk relative to the buffer.
static size_t chunk_get_offset(Header chunk) {
    return (void*)chunk - buffer_base;
}


// Returns usable memory pertaining to a given chunk.
static void* get_user_memory(Header chunk) {
    return (void*)chunk + sizeof(*chunk);
}


// Initialises the free list for entire allocated region.
static void freelist_init(size_t size) {
    freelist_head = (Header)buffer_base;
    chunk_create(buffer_base, size);
    freelist_head->prev = freelist_head->next = freelist_head;
}


// Destroy the free list.
static void freelist_destroy(void) {
    // No need to free memory, as it is part of the allocator region.
    freelist_head = NULL;
}


// Check if the free list has only one chunk in it.
// This goes with the invariant as per the spec.
static int freelist_has_one_chunk(void) {
    return  freelist_head == freelist_head->prev &&
            freelist_head == freelist_head->next;
}


// Inserts a given chunk in-order into the free list.
static void freelist_insert_chunk(Header chunk) {
    Header curr = freelist_head;
    int did_check = FALSE;

    // Work out where we want to insert the given chunk.
    // That accounts for the chunk address comparison.
    // But that alone is insufficient: if we free a chunk /after/ the
    // free list, then we get an infloop. Hence the second condition
    // and the did_check flag.
    while (chunk > curr && (!did_check || curr != freelist_head)) {
        did_check = TRUE;
        curr = curr->next;
    }

    // Now, inserting before curr will maintain the order in the list.
    // Let's check that this is true.
    if (chunk < freelist_head) {
        // Chunk is before the freelist
        assert(curr == freelist_head);
    } else if (chunk > freelist_head->prev) {
        // Chunk is after the freelist
        assert(curr == freelist_head);
    } else {
        // Chunk is in the middle of the free list
        assert(curr->prev < chunk && chunk < curr);
    }

    // Now, insert the chunk.
    // We need to insert the chunk before curr.
    chunk->prev = curr->prev;
    chunk->next = curr;

    chunk->prev->next = chunk;
    chunk->next->prev = chunk;

    // Shift the freelist_head to point to earliest chunk, if needed.
    if (freelist_head > chunk) {
        freelist_head = chunk;
    }
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


// Merges a chunk with its neighbours in the free list.
static void freelist_merge_chunk(Header chunk) {
    size_t offset = chunk - freelist_head;

    // Because all sizes and offsets are a power of two, it suffices
    // to check if (offset & size) is zero or not.
    // If it is non-zero, then we are the second chunk, and have to
    // merge left.
    // Otherwise, we are the first chunk, and we merge right.
    // Here, 'first'/'second' correspond to the splitting procedure
    // as per the spec.

    if ((offset & chunk->size)) {
        // Merge given chunk to the left
        Header other_chunk = chunk->prev;
        size_t other_offset = chunk_get_offset(other_chunk);
        size_t expected_offset = offset - chunk->size;

        // We need to check if chunk immediately before us is adjacent
        // to us. If it is not, then we cannot merge (i.e. a used
        // region is in-between).

        if (other_offset == expected_offset) {
            // Resize the other chunk.
            other_chunk->size += chunk->size;

            // Update free list nodes.
            freelist_extract_chunk(chunk);
            freelist_merge_chunk(other_chunk);
        }
    } else {
        // Merge given chunk to the right.
        // Procedure is similar to the above.
        Header other_chunk = chunk->next;
        size_t other_offset = chunk_get_offset(other_chunk);
        size_t expected_offset = offset + chunk->size;

        if (other_offset == expected_offset) {
            chunk->size += other_chunk->size;

            freelist_extract_chunk(other_chunk);
            freelist_merge_chunk(chunk);
        }
    }
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
        printf("<%u|%p> ", curr->size, curr);
        curr = curr->next;
    } while (curr != freelist_head);

    printf("\n");
}
