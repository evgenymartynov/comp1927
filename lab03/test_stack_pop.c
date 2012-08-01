// Author: evgenym
// Tutorial: Tue 11 Bell
// Date: 27 Jul 2012

// Summary: Tests abort()ion of stack on empty pop.

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "Item.h"
#include "STACK.h"

int main(int argc, char *argv[]) {
    printf("Creating a stack of size 5\n");
    STACKinit(5);
    printf("Pushing 1 and 2 on the stack\n");
    STACKpush(1);
    STACKpush(2);

    printf("Popping twice... should get 2 then 1\n");
    assert(STACKpop() == 2);
    assert(STACKpop() == 1);

    printf("Popping empty stack: this should abort\n");
    STACKpop();

    assert("Failed empty-pop abortion test" != NULL);

    return EXIT_FAILURE;
}
