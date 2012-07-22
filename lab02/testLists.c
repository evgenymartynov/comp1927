// Author: evgenym
// Tutorial: Tue 11 Bell
// Date: 18 Jul 2012
// Summary: Lab02 exercises

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "lists.h"


// These are not specified by lists.h, but we still need these funcs.
void disposeList(link first);
void printDoublyLinkedList(dlink list);
void disposeDoublyLinkedList(dlink first);


//
// Local prototypes
//

// Due to poor interface design, we cannot test each of the functions
// separately. We have to use sumListItems() to test fromTo() -- which
// is neither nice nor valid testing method.
static void testSinglyLinkedLists(void);

// Again, poor interface means we have to test everything ourselves.
static void testDoublyLinkedLists(void);


int main (int argc, char *argv[]) {
    testSinglyLinkedLists();
    testDoublyLinkedLists();

    return EXIT_SUCCESS;
}

////////////////////////////////////////////////////////////////////////
//
// Test code starts here
//
////////////////////////////////////////////////////////////////////////

static void testSinglyLinkedLists(void) {
    printf("Testing testFromTo()\n");

    // These tests break abstraction because the list does not have a
    // _nice_ set of interface functions.
    // The only way to test is to use multiple functions at once,
    // as well as printing out a list to see if it right.
    // But that's better than no tests at all, so...

    link list;

    printf("  testing range 5..5\n");
    list = fromTo(5, 5);
    assert(list != NULL);
    assert(sumListItems(list) == 5);
    printList(list);
    disposeList(list);

    printf("  testing range 3..6\n");
    list = fromTo(3, 6);
    assert(list != NULL);
    assert(sumListItems(list) == 18);
    printList(list);
    disposeList(list);

    printf("  testing range 4..2\n");
    list = fromTo(4, 2);
    assert(list == NULL);
    assert(sumListItems(list) == 0);
    printList(list);
    disposeList(list);

    printf("  testing negative number ranges\n");
    printf("    -5..-3\n");
    list = fromTo(-5, -3);
    assert(list != NULL);
    assert(sumListItems(list) == -12);
    printList(list);
    disposeList(list);

    printf("    -1..1\n");
    list = fromTo(-1, 1);
    assert(list != NULL);
    assert(sumListItems(list) == 0);
    printList(list);
    disposeList(list);

    printf("    -1..-1\n");
    list = fromTo(-1, -1);
    assert(list != NULL);
    assert(sumListItems(list) == -1);
    printList(list);
    disposeList(list);

    printf("  testing long lists\n");
    printf("    -100..100\n");
    list = fromTo(-100, 100);
    assert(list != NULL);
    assert(sumListItems(list) == 0);
    disposeList(list);

    list = fromTo(-5, 55);
    printf("    -5..55\n");
    assert(list != NULL);
    assert(sumListItems(list) == 1525);
    disposeList(list);

    list = fromTo(-500, -1055);
    printf("    -500..-1025\n");
    assert(list == NULL);
    assert(sumListItems(list) == 0);
    disposeList(list);

    printf("Passed\n");
}

static void testDoublyLinkedLists(void) {
    printf("Testing doublify()\n");

    link list;
    dlink doubleList;

    printf("  testing 0..10\n");
    list = fromTo(0, 10);
    printList(list);
    doubleList = doublify(list);
    printDoublyLinkedList(doubleList);
    disposeList(list);
    disposeDoublyLinkedList(doubleList);

    printf("  testing -5..-2\n");
    list = fromTo(-5, -2);
    printList(list);
    doubleList = doublify(list);
    printDoublyLinkedList(doubleList);
    disposeList(list);
    disposeDoublyLinkedList(doubleList);

    printf("  testing -5..-10 (empty list)\n");
    list = fromTo(-5, -10);
    printList(list);
    doubleList = doublify(list);
    printDoublyLinkedList(doubleList);
    disposeList(list);
    disposeDoublyLinkedList(doubleList);

    printf("Check the above by hand. Passed?\n");
}
