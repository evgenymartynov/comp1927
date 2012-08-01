// Author: evgenym
// Tutorial: Tue 11 Bell
// Date: 27 Jul 2012

// Summary: Tests abort()ion of stack on a full push.

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "Item.h"
#include "STACK.h"

int main(int argc, char *argv[]) {
    printf("Creating a stack of size 1\n");
    STACKinit(1);
    printf("Pushing 1 on the stack\n");
    STACKpush(1);
    printf("Pushing 2 on the full stack; should abort\n");
    STACKpush(2);

    assert("Failed full-push abortion test" != NULL);

    return EXIT_FAILURE;
}
