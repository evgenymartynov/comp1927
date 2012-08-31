// Author: evgenym
// Tutorial: Tue 11 Bell
// Date: 8 Aug 2012

// Summary:
// Complete solution for Assignment 1: memory allocator.

// I have followed the asst spec almost to the letter, only departing
// from it (as far as I can tell) only in my choice of types and names.

// In particular, I have decided to use void* for the memory buffer
// location, as it makes more sense than char*. However, char* has well-
// defined pointer arithmetic, whereas void* arithmetic is undefined.
// Using void* makes sense for me, for pointing to the memory buffer.
// So you will see [some magic with] repeated char* and void* casts.

// Further, I decided to use size_t instead of u_int32_t, for two
// reasons.
// First, it is shorter, and has a better name.
// Second, it makes no difference anyway.

////////////////////////////////////////////////////////////////////////
//
// Possible to-do list:
// 1) Add more reliable error-checking to headers.
//    For example, set magic = CONST ^ size, or something.
//    That will catch most (but not all) header corruptions.
//
////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
static void   *memory = NULL;
static size_t memory_size;
static Header free_list_ptr = NULL;

//
// Local prototypes
//

static size_t round_up_power_of_two(size_t size);
static void*  get_user_memory(Header chunk);

static Header chunk_create(void* where, size_t size);
static void   chunk_ensure_free(Header chunk);
static void   chunk_ensure_used(Header chunk);
static size_t chunk_get_offset(Header chunk);

static void   freelist_init(size_t size);
static void   freelist_destroy(void);
static Header freelist_bestfit(size_t size);
static Header freelist_firstfit(size_t size);
static void   freelist_split_chunk(Header chunk, size_t size);
static void   freelist_merge_chunk(Header chunk);
static int    freelist_has_one_chunk(void);
static void   freelist_insert_chunk(Header chunk);
static void   freelist_extract_chunk(Header chunk);

// Debugging functions, please ignore.
static void freelist_print(void);
static void freelist_print_list(void);
static void freelist_print_bar(void);

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
    memory_size = size;

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
    memory_size = 0;
    freelist_destroy();
}


// Make me a sandwich
void* allocator_malloc(size_t size) {
    // Make sure the allocator has been initialised.
    assert(memory != NULL);

    // Add bookkeeping costs.
    size += sizeof(header);

    // Find a free chunk
    Header chunk = freelist_firstfit(size);
    if (chunk == NULL) {
        return NULL;
    }

    // Ensure that the chunk we received was valid.
    chunk_ensure_free(chunk);
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
        assert(chunk == free_list_ptr);

        return NULL;
    }

    // Mark the chunk as used.
    chunk->magic = HEADER_USED_MAGIC;

    // Remove it from the free list.
    freelist_extract_chunk(chunk);

    freelist_print();

    return get_user_memory(chunk);
}


// Eat me a sandwich.
void allocator_free(void* region) {
    // Get the header for region we are freeing.
    Header chunk = (Header)((char*)region - sizeof(header));

    // Ensure the header is not corrupt.
    chunk_ensure_used(chunk);

    // Mark it as free.
    chunk->magic = HEADER_FREE_MAGIC;

    // And re-insert into the free list.
    freelist_insert_chunk(chunk);

    // And merge freed chunk together with the others.
    freelist_merge_chunk(chunk);

    freelist_print();
}


////////////////////////////////////////////////////////////////////////
//
// Internal funcs start here
//
////////////////////////////////////////////////////////////////////////


// Rounds up an integer to the next power of two.
// If num is larger than 1<<31, we be screwed.
static size_t round_up_power_of_two(size_t num) {
    size_t rounded = 1;

    // We check for overflows; unsigned-int overflows are well-defined.
    while (rounded && rounded < num) {
        rounded *= 2;
    }

    return rounded;
}


// Ensure that the magic field is properly set.
// If chunk is NULL, pretend that magic matched.
static void chunk_ensure_free(Header chunk) {
    if (chunk != NULL && chunk->magic != HEADER_FREE_MAGIC) {
        fprintf(stderr, "allocator: expected free magic number\n");
        abort();
    }
}

static void chunk_ensure_used(Header chunk) {
    if (chunk != NULL && chunk->magic != HEADER_USED_MAGIC) {
        fprintf(stderr, "allocator: expected used magic number\n");
        abort();
    }
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
    return (char*)chunk - (char*)memory;
}


// Returns usable memory pertaining to a given chunk.
static void* get_user_memory(Header chunk) {
    return (void*)((char*)chunk + sizeof(*chunk));
}


// Initialises the free list for entire allocated region.
static void freelist_init(size_t size) {
    free_list_ptr = (Header)memory;
    chunk_create(memory, size);
    free_list_ptr->prev = free_list_ptr->next = free_list_ptr;
}


// Destroy the free list.
static void freelist_destroy(void) {
    // No need to free memory, as it is part of the allocator region.
    free_list_ptr = NULL;
}


// Check if the free list has only one chunk in it.
// This goes with the invariant as per the spec.
static int freelist_has_one_chunk(void) {
    return  free_list_ptr == free_list_ptr->prev &&
            free_list_ptr == free_list_ptr->next;
}


// Inserts a given chunk in-order into the free list.
static void freelist_insert_chunk(Header chunk) {
    Header curr = free_list_ptr;
    int did_check = FALSE;

    // Work out where we want to insert the given chunk.
    // That accounts for the chunk address comparison.
    // But that alone is insufficient: if we free a chunk /after/ the
    // free list, then we get an infloop. Hence the second condition
    // and the did_check flag.
    while (chunk > curr && (!did_check || curr != free_list_ptr)) {
        did_check = TRUE;
        curr = curr->next;
    }

    // Make sure the magics are preserved.
    chunk_ensure_free(curr);
    chunk_ensure_free(chunk);

    // Now, inserting before curr will maintain the order in the list.
    // Let's check that this is true.
    if (chunk < free_list_ptr) {
        // Chunk is before the freelist
        assert(curr == free_list_ptr);
    } else if (chunk > free_list_ptr->prev) {
        // Chunk is after the freelist
        assert(curr == free_list_ptr);
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

    // Shift the free_list_ptr to point to earliest chunk, if needed.
    if (free_list_ptr > chunk) {
        free_list_ptr = chunk;
    }

    // Finally, make sure the free list's magic is preserved.
    chunk_ensure_free(free_list_ptr);
}


// Removes a given chunk from the free list.
// Assumes that this is not the only chunk in the list.
static void freelist_extract_chunk(Header chunk) {
    assert(!freelist_has_one_chunk());

    // We got to check if this is the head of the list, and if so,
    // we must update it.
    if (chunk == free_list_ptr) {
        free_list_ptr = free_list_ptr->next;
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
static Header __attribute__((unused)) freelist_bestfit(size_t size) {
    Header chunk = free_list_ptr;
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
    } while(chunk != free_list_ptr);

    return best_fit;
}


// First-fit finder for a free chunk.
// Returns pointer to a corresponding header, or NULL if no chunk was
// large enough.
// Assumes the size of the header has already been accounted for.
static Header __attribute__((unused)) freelist_firstfit(size_t size) {
    Header chunk = free_list_ptr;

    do {
        if (chunk->size >= size) {
            return chunk;
        }

        chunk = chunk->next;
    } while(chunk != free_list_ptr);

    // Didn't find anything.
    return NULL;
}


// Merges a chunk with its neighbours in the free list.
static void freelist_merge_chunk(Header chunk) {
    size_t offset = chunk_get_offset(chunk);

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

        chunk_ensure_free(other_chunk);

        if (other_offset == expected_offset &&
                                    other_chunk->size == chunk->size) {
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

        chunk_ensure_free(other_chunk);

        if (other_offset == expected_offset &&
                                    other_chunk->size == chunk->size) {
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
        void* middle = (void*)((char*)chunk + split_size);

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


////////////////////////////////////////////////////////////////////////
//
// Debug functions, please ignore when marking.
//
////////////////////////////////////////////////////////////////////////

// Prints out the free list representation.
static void __attribute__((unused)) freelist_print(void) {
    freelist_print_bar();
    freelist_print_list();
}

// Escape codes for colours
#define COL_WHITE "\e[0;0m"
#define COL_RED   "\e[1;31m"
#define COL_GRE   "\e[1;32m"
#define COL_BLU   "\e[1;34m"

// Displays the free list's nodes as a linked list.
static void __attribute__((unused)) freelist_print_list(void) {
    Header curr = free_list_ptr;

    do {
        printf("<"COL_BLU"%u"COL_WHITE"|%p> ", curr->size, (void*)curr);
        curr = curr->next;
    } while (curr != free_list_ptr);

    printf("\n");
}

// Displays ASCII-art-ish representation of the buffer utilisation.
static void __attribute__((unused)) freelist_print_bar(void) {
    Header curr = free_list_ptr;
    size_t i;
    size_t bytes_per_symbol = sizeof(*curr) * 2;
    size_t display_width = memory_size / bytes_per_symbol;

    char *display = (char*)calloc(display_width + 1, 1);
    memset(display, 'X', display_width);

    do {
        size_t chunk_width = curr->size / bytes_per_symbol;
        size_t chunk_begin = chunk_get_offset(curr) / bytes_per_symbol;
        memset(display + chunk_begin, '-', chunk_width);
        curr = curr->next;
    } while (curr != free_list_ptr);

    printf("\n[");
        for (i = 0; display[i]; i++) {
            printf(display[i] == '-' ? COL_GRE : COL_RED);
            printf("%c" COL_WHITE, display[i]);
        }
    printf("]\n");

    free(display);
}
