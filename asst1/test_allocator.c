// Author: evgenym
// Tutorial: Tue 11 Bell
// Date: 8 Aug 2012
// Summary: Assignment 1, test module for the memory allocator

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "allocator.h"

typedef u_int32_t size_t;

// Test prototypes
static void test_allocator_setup(void);
static void test_allocator_malloc(void);
static void test_allocator_no_reclamation(void);
static void test_allocator_with_merge(void);
static void test_allocator_magic_corruption(void);

// Utility prototypes
// Creates a new allocator of given size.
static void test_begin(size_t size);
static void test_end(void);
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
    test_allocator_no_reclamation();
    test_allocator_with_merge();
    test_allocator_magic_corruption();

    return EXIT_SUCCESS;
}


// Tests that _init() and _end() do not crash or raise any errors.
static void test_allocator_setup(void) {
    printf("Testing allocator set-up sequence\n");
        test_begin(64);
        test_end();

        printf("  reallocating with more memory\n");
        test_begin(128);
        test_end();

        printf("  reallocating with not-power-of-2 memory\n");
        test_begin(1337);
        test_end();

        printf("  check that re-initialising allocator does nothing\n");
        test_begin(1024);
        allocator_init(512);
        test_end();

    printf("Passed\n");
}


// Tests that _malloc() complies with the specificiation.
// In particular, we CAN EXPECT to know the offset of an allocation.
// And we also can work out how much memory to ask for.
// For this, we need to know the size of the header struct.
// We also test that the memory is writeable.
#define HEADER_SIZE 16
static void test_allocator_malloc(void) {
    printf("Testing allocator_malloc()...\n");
        test_begin(256);

        printf("  malloc entire usable memory, fails the invariant\n");
        malloc_check_fail(256 - HEADER_SIZE);
        printf("  malloc half of usable memory\n");
        malloc_check(128 - HEADER_SIZE);
        printf("  now allocate 1 extra byte\n");
        malloc_check(1);

        printf("  check that re-initialising allocator does nothing\n");
        allocator_init(1024);
        malloc_check_fail(128 - HEADER_SIZE);

        test_end();
        test_begin(256);

        printf("  malloc a half\n");
        malloc_check((256-2*HEADER_SIZE)/2);
        printf("  and again, fails the invariant\n");
        malloc_check_fail((256-2*HEADER_SIZE)/2);

        test_end();
        test_begin(256);

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

        test_end();
        test_begin(4096);

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
            malloc_check_fail(16 - HEADER_SIZE);
        printf("ok\n");

        test_end();

    printf("Passed\n");
}


// Tests freeing, assuming merging is not implemented.
// Expects the best-fit strategy to be implemented
// and that the memory is allocated as per the spec method.
static void test_allocator_no_reclamation(void) {
    printf("Testing allocator without merging...\n");
        test_begin(4096);

        void *ptr;
        void *beginning;

        printf("    malloc(128)\n");
        beginning = ptr = malloc_check(128 - HEADER_SIZE);
        printf("    freeing it\n");
        allocator_free(ptr);

        printf("    malloc(2048)\n");
        ptr = malloc_check(2048 - HEADER_SIZE);
        printf("    freeing it\n");
        allocator_free(ptr);

        printf("    malloc(512)\n");
        ptr = malloc_check(512 - HEADER_SIZE);
        printf("    freeing it\n");
        allocator_free(ptr);

        printf("    malloc(2048)\n");
        ptr = malloc_check(2048 - HEADER_SIZE);
        printf("    freeing it\n");
        allocator_free(ptr);

        printf("    malloc(128), ensure matches first malloc\n");
        ptr = malloc_check(128 - HEADER_SIZE);
        assert(ptr == beginning);
        printf("    freeing it\n");
        allocator_free(ptr);

        printf("    malloc(64, 128, 32)\n");
        void *first  = malloc_check(64 - HEADER_SIZE);
        void *second = malloc_check(128 - HEADER_SIZE);
        void *third  = malloc_check(32 - HEADER_SIZE);
        printf("    ensure the right memregions are returned\n");
        assert(first == beginning);
        assert(second == (char*)beginning + 128);
        assert(third == (char*)beginning + 64);
        printf("    freeing 128, 32, 64\n");
        allocator_free(second);
        allocator_free(third);
        allocator_free(first);

        test_end();
    printf("Passed\n");
}


// Tests merging.
// Expects all prior milestones to be completed and correct.
static void test_allocator_with_merge(void) {
    printf("Testing allocator with merging...\n");
        test_begin(2048);

        printf("  allocating 7x256 blocks...\n");
        void* first   = malloc_check(256 - HEADER_SIZE);
        void* second  = malloc_check(256 - HEADER_SIZE);
        void* third   = malloc_check(256 - HEADER_SIZE);
        void* fourth  = malloc_check(256 - HEADER_SIZE);
        void* fifth   = malloc_check(256 - HEADER_SIZE);
        void* sixth   = malloc_check(256 - HEADER_SIZE);
        void* seventh = malloc_check(256 - HEADER_SIZE);

        // 1 2 3 4 5 6 7

        printf("  check that we are out of memory\n");
        malloc_check_fail(256);

        printf("  freeing third and fourth (merge left)\n");
        allocator_free(third);
        allocator_free(fourth);

        printf("  allocate 512 bytes from merged");
        void* mrg_one = malloc_check(512 - HEADER_SIZE);

        // 1 2 m1 5 6 7 {256}

        printf("  freeing seventh and sixth (merge right)\n");
        allocator_free(seventh);
        allocator_free(sixth);

        printf("  allocate 512 bytes from merged\n");
        void* mrg_two = malloc_check(512 - HEADER_SIZE);

        // 1 2 m1 5 m2 {256}

        printf("  now we should have 256|256|512|256|{256}|512\n");
        printf("  make sure we can't allocate more\n");
        malloc_check_fail(256);

        printf("  freeing first and second (merge right)\n");
        allocator_free(first);
        allocator_free(second);

        printf("  allocate 512 bytes from merged\n");
        void* mrg_three = malloc_check(512 - HEADER_SIZE);

        // m3 m1 5 m2 {256}

        printf("  now we should have 512|512|256|{512}|256\n");
        printf("  make sure we can't allocate more\n");
        malloc_check_fail(256);

        printf("freeing first two (merged) chunks, merge left\n");
        allocator_free(mrg_three);
        allocator_free(mrg_one);

        // {1024} 5 m2 {256}

        printf("  now we should have {1024}|256|{256}|512\n");
        printf("  request that merged chunk\n");
        void* mrg_four = malloc_check(1024 - HEADER_SIZE);

        printf("  free the ones in the middle; three-way merge\n");
        allocator_free(fifth);
        allocator_free(mrg_two);

        // m4 {1024}

        printf("  split into powers of two\n");
        void* mrg_five  = malloc_check(512 - HEADER_SIZE);
        void* mrg_six   = malloc_check(256 - HEADER_SIZE);
        void* mrg_seven = malloc_check(128 - HEADER_SIZE);
        void* mrg_eight = malloc_check(64 - HEADER_SIZE);

        // m4 m5 m6 m7 m8 {64}
        printf("  now we should have 1024|512|256|128|64|{64}");
        printf("  make sure we can't allocate more\n");
        malloc_check_fail(64);

        printf("  free all the things\n");
        allocator_free(mrg_four);
        allocator_free(mrg_five);
        allocator_free(mrg_six);
        allocator_free(mrg_seven);
        allocator_free(mrg_eight);

        // all free now

        printf("  allocate 1024 (largest possible)\n");
        void* mrg_nine = malloc_check(1024 - HEADER_SIZE);
        printf("  then try 1024 again, then free\n");
        malloc_check_fail(1024 - HEADER_SIZE);
        allocator_free(mrg_nine);

        printf("  allocate 1025 and 2048, check for fail\n");
        malloc_check_fail(1025 - HEADER_SIZE);
        malloc_check_fail(2048 - HEADER_SIZE);

        printf("  allocate 512, 512, then merge and ask for 1024\n");
        void* chk_one = malloc_check(512 - HEADER_SIZE);
        void* chk_two = malloc_check(512 - HEADER_SIZE);
        printf("  free and alloc 1024\n");
        allocator_free(chk_one);
        allocator_free(chk_two);
        void* chk_three = malloc_check(1024 - HEADER_SIZE);
        printf("  free that\n");
        allocator_free(chk_three);

        printf("  allocate descending powers of two\n");
        void* pow_one   = malloc_check(1024 - HEADER_SIZE);
        void* pow_two   = malloc_check(512 - HEADER_SIZE);
        void* pow_three = malloc_check(256 - HEADER_SIZE);
        void* pow_four  = malloc_check(128 - HEADER_SIZE);
        void* pow_five  = malloc_check(64 - HEADER_SIZE);
        void* pow_six   = malloc_check(32 - HEADER_SIZE);
        printf("  merge out of order\n");
        allocator_free(pow_four);
        allocator_free(pow_three);
        allocator_free(pow_two);
        allocator_free(pow_six);
        allocator_free(pow_one);
        allocator_free(pow_five);

        printf("  allocate random powers of two\n");
        void* rnd_one   = malloc_check(64 - HEADER_SIZE);
        void* rnd_two   = malloc_check(32 - HEADER_SIZE);
        void* rnd_three = malloc_check(1024 - HEADER_SIZE);
        void* rnd_four  = malloc_check(256 - HEADER_SIZE);
        void* rnd_five  = malloc_check(512 - HEADER_SIZE);
        void* rnd_six   = malloc_check(128 - HEADER_SIZE);
        printf("  merge in order of size\n");
        allocator_free(rnd_three);
        allocator_free(rnd_five);
        allocator_free(rnd_four);
        allocator_free(rnd_six);
        allocator_free(rnd_one);
        allocator_free(rnd_two);

        test_end();
    printf("Passed\n");
}


// Here we check that abort() is called if magic numbers are corrupted.
// This will fork() to test, because SIGABRT can't be blocked.
// We then check the exit status of the forked process.
static void test_allocator_magic_corruption(void) {
    printf("Testing magic-overwriting\n");

        pid_t child;
        const int test_sz = 2048;

        printf("  clobber allocated header and then free\n");
        test_begin(test_sz);
            child = fork();
            if (child != 0) {
                int status = 0;
                waitpid(child, &status, 0);

                // Ensure SIGABRT was raised, i.e. abort() was called.
                if (WTERMSIG(status) != SIGABRT) {
                    assert(0 && "Allocator did not call abort() as expected");
                } else {
                    printf("  abort() was called, as expected\n");
                }
            } else {
                // Clobber the header of the region.
                char *reg = malloc_check(256 - HEADER_SIZE);
                memset(reg-HEADER_SIZE, 0, HEADER_SIZE);

                allocator_free(reg);

                // At this point, we must have aborted.
                // We exit w/failure as the parent expects SIGABRT.
                exit(EXIT_FAILURE);
            }
        test_end();


        printf("  clobber free header; then free and merge\n");
        test_begin(test_sz);
            child = fork();
            if (child != 0) {
                int status = 0;
                waitpid(child, &status, 0);

                if (WTERMSIG(status) != SIGABRT) {
                    assert(0 && "Allocator did not call abort() as expected");
                } else {
                    printf("  abort() was called, as expected\n");
                }
            } else {
                // Clobber.
                char *reg = malloc_check(256 - HEADER_SIZE);
                memset(reg - HEADER_SIZE + 256, 0, test_sz);

                // Free and merge.
                allocator_free(reg);
                exit(EXIT_FAILURE);
            }
        test_end();


        printf("  clobber non-adjacent free header; free, and merge\n");
        test_begin(test_sz);
            child = fork();
            if (child != 0) {
                int status = 0;
                waitpid(child, &status, 0);

                if (WTERMSIG(status) != SIGABRT) {
                    assert(0 && "Allocator did not call abort() as expected");
                } else {
                    printf("  abort() was called, as expected\n");
                }
            } else {
                // Clobber.
                char *reg = malloc_check(256 - HEADER_SIZE);
                memset(reg - HEADER_SIZE + 512, 0, test_sz - 512);

                // Free and merge.
                allocator_free(reg);
                exit(EXIT_FAILURE);
            }
        test_end();

    printf("Passed\n");
}


// Creates a new allocator of given size.
static void test_begin(size_t size) {
    printf("  allocating %u-byte slab\n", size);
    allocator_init(size);
}


// Destroys the allocator.
static void test_end(void) {
    printf("  destroying allocator\n");
    allocator_end();
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
