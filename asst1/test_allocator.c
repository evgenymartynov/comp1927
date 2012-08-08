// Author: evgenym
// Tutorial: Tue 11 Bell
// Date: 8 Aug 2012
// Summary: Assignment 1, test module for the memory allocator

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "allocator.h"

typedef u_int32_t size_t;

// Test prototypes
static void test_allocator_setup(void);
static void test_allocator_malloc(void);

// Utility prototypes
static void* malloc_check(size_t size);
static void  malloc_check_fail(size_t size);

////////////////////////////////////////////////////////////////////////
//
// Implementation starts here
//
////////////////////////////////////////////////////////////////////////


int main() {
    // Don't buffer output; ignore this.
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);

    test_allocator_setup();
    test_allocator_malloc();

    return EXIT_SUCCESS;
}


// Tests that _init() and _end() do not crash or raise any errors.
static void test_allocator_setup(void) {
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


// Tests that _malloc() compiles with the specificiation.
// In particular, we CAN EXPECT to know the offset of an allocation.
// And we also can work out how much memory to ask for.
// For this, we need to know the size of the header struct.
// We also test that the memory is writeable.
#define HEADER_SIZE 16
static void test_allocator_malloc(void) {
    printf("Testing allocator_malloc()...\n");
        printf("  setting up allocator of size 256\n");
        allocator_init(256);

        printf("  malloc entire usable memory, fails the invariant\n");
        malloc_check_fail(256 - HEADER_SIZE);
        printf("  malloc half of usable memory\n");
        malloc_check(128 - HEADER_SIZE);
        printf("  now allocate 1 extra byte\n");
        malloc_check(1);

        printf("  reset\n");
        allocator_end();
        allocator_init(256);

        printf("  malloc a half\n");
        malloc_check((256-2*HEADER_SIZE)/2);
        printf("  and again, fails the invariant\n");
        malloc_check_fail((256-2*HEADER_SIZE)/2);

        printf("  reset\n");
        allocator_end();
        allocator_init(256);

        printf("  malloc in descending powers of 2\n");
            printf("    128... ");
            malloc_check(128 - HEADER_SIZE);
            printf("64... ");
            malloc_check(64 - HEADER_SIZE);
            printf("32... ");
            malloc_check(32 - HEADER_SIZE);
            printf("16... ");
            malloc_check(16 - HEADER_SIZE);
        printf("ok\n");

        printf("  reset with 4096\n");
        allocator_end();
        allocator_init(4096);

        printf("  malloc in random powers of 2\n");
            printf("    512... ");
            malloc_check(512 - HEADER_SIZE);
            printf("64... ");
            malloc_check(64 - HEADER_SIZE);
            printf("1024... ");
            malloc_check(1024 - HEADER_SIZE);
            printf("128... ");
            malloc_check(128 - HEADER_SIZE);
            printf("32... ");
            malloc_check(32 - HEADER_SIZE);
            printf("2048... ");
            malloc_check(2048 - HEADER_SIZE);
            printf("256... ");
            malloc_check(256 - HEADER_SIZE);
            printf("16... ");
            malloc_check(16 - HEADER_SIZE);
            printf("16... ");
            malloc_check_fail(16-HEADER_SIZE);
        printf("ok\n");

    printf("Passed\n");
}


// Tries to allocate memory, does write tests, but does not free it.
// Expects that the allocation will always succeed.
// Returns the allocated region address.
static void* malloc_check(size_t size) {
    void* mem = allocator_malloc(size);
    assert(mem != NULL);

    // This will crash if we were given bogus memory.
    memset(mem, 0xFF, size);

    return mem;
}

// Tries to allocate memory.
// Expects failure.
static void malloc_check_fail(size_t size) {
    void* mem = allocator_malloc(size);
    assert(mem == NULL);
}
