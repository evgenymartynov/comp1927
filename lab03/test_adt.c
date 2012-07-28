// Author: evgenym
// Tutorial: Tue 11 Bell
// Date: 27 Jul 2012
// Summary: Tests for Lab03 lists ADT.

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "Item.h"
#include "adt.h"

static void testLists(void);
// Exposed by my adt.c
void LISTprint(void);


int main(int argc, char *argv[]) {
    testLists();

    return EXIT_SUCCESS;
}


static void testLists(void) {
    Item zeroth = 0;
    Item first  = 7;
    Item second = 13;
    Item third  = 42;
    Item fourth = 50;

    printf("Testing initialisation\n");
        printf("  calling LISTinit()\n");
        LISTinit();
        printf("  ensure new list is empty\n");
        assert(LISTempty());
    printf("Pass: initialisation looks sane\n");

    printf("Inserting initial elements\n");
        printf("  insert %d\n", first);
        LISTafter(first);
        assert(LISTcurrent() == first);
        assert(!LISTempty());
        printf("  insert %d\n", second);
        LISTafter(second);
        assert(LISTcurrent() == second);
        assert(!LISTempty());
        printf("  insert %d\n", third);
        LISTafter(third);
        assert(LISTcurrent() == third);
        assert(!LISTempty());
    printf("Pass: inserted items look sane\n");

    printf("Test moving and querying\n");
        printf("  move left 1\n");
        assert(!LISTmove(-1));
        assert(LISTcurrent() == second);
        printf("  move left 1\n");
        assert(LISTmove(-1));
        assert(LISTcurrent() == first);
        printf("  move right 2\n");
        assert(LISTmove(2));
        assert(LISTcurrent() == third);
        printf("  move left 2\n");
        assert(LISTmove(-2));
        assert(LISTcurrent() == first);
    printf("Pass: moving seems to work\n");

    printf("Testing edge insertion\n");
        printf("  insert %d\n", zeroth);
        LISTbefore(zeroth);
        assert(LISTcurrent() == zeroth);
        printf("  move to right end\n");
        assert(LISTmove(3));
        assert(LISTcurrent() == third);
        printf("  insert %d\n", fourth);
        LISTafter(fourth);
        assert(LISTcurrent() == fourth);
    printf("Pass: edge insertion seems to work\n");

    printf("Testing middle insertion\n");
        printf("  move left 2\n");
        assert(!LISTmove(-2));
        assert(LISTcurrent() == second);
        printf("  insert -5 left\n");
        LISTbefore(-5);
        assert(LISTcurrent() == -5);
        printf("  insert 123 right\n");
        LISTafter(123);
        assert(LISTcurrent() == 123);
    printf("Pass: middle insertion seems to work\n");

    printf("Testing deletion\n");
        printf("  make sure list isn't empty\n");
        assert(!LISTempty());
        printf("  deleting 123, moving current to -5\n");
        assert(LISTcurrent() == 123);
        LISTdelete();
        assert(LISTcurrent() == -5);
        printf("  ensure still not empty\n");
        assert(!LISTempty());
        printf("  delete -5, leaving zeroth-[first]-...-fourth\n");
        LISTdelete();
        assert(LISTcurrent() == first);
        printf("  delete that\n");
        LISTdelete();
        assert(LISTcurrent() == zeroth);
        printf("  delete first elem\n");
        assert(!LISTempty() && LISTcurrent() == zeroth);
        LISTdelete();
        printf("  move to the end and keep deleting\n");
        assert(LISTmove(2));
        assert(!LISTempty() && LISTcurrent() == fourth);
        LISTdelete();
        assert(!LISTempty() && LISTcurrent() == third);
        LISTdelete();
        assert(!LISTempty() && LISTcurrent() == second);
        LISTdelete();
        assert(LISTempty());
    printf("Pass: deletion seems to work\n");

    printf("\nAll tests passed.\n\n");
}
