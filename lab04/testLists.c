// Author: evgenym
// Tutorial: Tue 11 Bell
// Date: 6 Aug 2012
// Summary: Lab04 tests

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "Item.h"
#include "lists.h"
#include "listPredicates.h"

//
// Local defines
//

#define NUM_PREDICATES 4
#define NUM_TEST_LISTS 5

//
// Local prototypes
//

// These create different types of lists to assist with tests. We can
// create many different lists, and then ensure that all predicates work
// on all of them.
// They also print out the list that they create, to remove clutter from
// the rest of the testing code.
static link createList1(void);
static link createList2(void);
static link createList3(void);
static link createList4(void);
static link createList5(void);

// Test functions
static void testSquares(void);
static void testCount(void);

// Provided by *my* lists.c, missing from lists.h
void freeList(link list);

////////////////////////////////////////////////////////////////////////
//
// Implementation code starts here
//
////////////////////////////////////////////////////////////////////////

static link createList1(void) {
    printf("    0->1->2->3->[X]\n");

    link head = newNode(0);
    insertNode(head, 3);
    insertNode(head, 2);
    insertNode(head, 1);

    return head;
}

static link createList2(void) {
    printf("    2->0->0->0->[X]\n");

    link head = newNode(2);
    insertNode(head, 0);
    insertNode(head, 0);
    insertNode(head, 0);

    return head;
}

static link createList3(void) {
    printf("    -5->4->-3->-2->0->[X]\n");

    link head = newNode(-5);
    insertNode(head, 0);
    insertNode(head, -2);
    insertNode(head, -3);
    insertNode(head, 4);

    return head;
}

static link createList4(void) {
    printf("    100->13->-42->1337->17->[X]\n");

    link head = newNode(100);
    insertNode(head, 17);
    insertNode(head, 1337);
    insertNode(head, -42);
    insertNode(head, 13);

    return head;
}

static link createList5(void) {
    printf("    12->50->-100->1->-1->[X]\n");

    link head = newNode(12);
    insertNode(head, -1);
    insertNode(head, 1);
    insertNode(head, -100);
    insertNode(head, 50);

    return head;
}


// Test square and squareAll functions
static void testSquares(void) {
    printf("Testing square()...\n");

    printf("  squaring 0\n");
        link zero = newNode(0);
        square(zero);
        assert(getItem(zero) == 0);
    freeNode(zero);

    printf("  squaring 1\n");
        link one = newNode(1);
        square(one);
        assert(getItem(one) == 1);
    freeNode(one);

    printf("  squaring -1\n");
        link negOne = newNode(-1);
        square(negOne);
        assert(getItem(negOne) == 1);
    freeNode(negOne);

    printf("  squaring a few random numbers\n");
        link node;
        node = newNode(5); square(node);
        assert(getItem(node) == 25);
        freeNode(node);

        node = newNode(8); square(node);
        assert(getItem(node) == 64);
        freeNode(node);

        node = newNode(-10); square(node);
        assert(getItem(node) == 100);
        freeNode(node);

        node = newNode(-256); square(node);
        assert(getItem(node) == 65536);
    freeNode(node);
    printf("passed\n");

    printf("Testing squareAll()...\n");
    link head, list;

    printf("  squaring 0->1->[X]\n");
        head = list = newNode(0);
        insertNode(list, 1);
        squareAll(list);
        assert(getItem(list) == 0); list = getNext(list);
        assert(getItem(list) == 1); list = getNext(list);
        assert(list == NULL);
        freeList(head);

    printf("  squaring -5->-11->124->[X]\n");
        head = list = newNode(-5);
        insertNode(list, 124);
        insertNode(list, -11);
        squareAll(list);
        assert(getItem(list) == 25); list = getNext(list);
        assert(getItem(list) == 121); list = getNext(list);
        assert(getItem(list) == 124*124); list = getNext(list);
        assert(list == NULL);
        freeList(head);

    printf("  squaring [X] to check for weird bugs\n");
        head = list = NULL;
        squareAll(list);
        assert(isEmptyList(list));
        freeList(head);

    printf("passed\n");
}

static void testCount(void) {
    link head;
    printf("Testing count() and its predicates...\n");

    // Now, I am automating this process so that the tests could be
    // extended with more predicates. This is probably a bit too much,
    // and it looks ugly.

    // But, it saves me the cost of rewriting code and possibly getting
    // something wrong, should I decide to add more predicates or more
    // test cases.

    // These are tests, and so the styleguide is relaxed.
    // I feel like this is a more condensed and more to-the-point way of
    // writing tests.

    int (*predicates[NUM_PREDICATES])(Item) = {
        predIsZero,
        predIsEven,
        predIsOdd,
        predIsPositive
    };

    char *predicateNames[NUM_PREDICATES] = {
        "zero", "even", "odd", "positive"
    };

    link (*testLists[NUM_TEST_LISTS])(void) = {
        createList1,
        createList2,
        createList3,
        createList4,
        createList5
    };

    int answers[NUM_PREDICATES][NUM_TEST_LISTS] = {
        {1, 3, 1, 0, 0, },  // zero
        {2, 4, 3, 2, 3, },  // even
        {2, 0, 2, 3, 2, },  // odd
        {3, 1, 1, 4, 3, },  // positive
    };

    int pred;
    for (pred = 0; pred < NUM_PREDICATES; pred++) {
        printf("  testing with %s predicate\n", predicateNames[pred]);

        int caseNo;
        for (caseNo = 0; caseNo < NUM_TEST_LISTS; caseNo++) {
            // We create a fresh list at every run, just in case
            // something got mangled.
            head = (testLists[caseNo])();

            int correctAnswer = answers[pred][caseNo];
            int answer = count(head, predicates[pred]);

            if (answer != correctAnswer) {
                printf("    count() returned %d, expected %d\n",
                        answer, correctAnswer);

                // This is now guaranteed to be false.
                assert(answer == correctAnswer);
            }

            freeList(head);
        }

        printf("  passed\n");
    }

    printf("passed\n");
}

int main(int argc, char *argv[]) {
    // Prevent stuff-ups when printing out, ignore this.
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);

    testSquares();
    testCount();

    return EXIT_SUCCESS;
}
