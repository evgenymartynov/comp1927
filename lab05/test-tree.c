// Author: evgenym
// Tutorial: Tue 11 Bell
// Date: 12 Aug 2012

// Summary: Tests for Lab05 tree traversal.
// Lab exercise asked for full branch coverage.
// The tests provide 100% code coverage (as determined by gcov/lcov).

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


// Seeing that the ADT isn't well defined (addToTree in particular),
// I've decided to stick the test function into the actual ADT file.
void testCount(void);


int main(int argc, char *argv[]) {
    // Disable line-buffering
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);

    testCount();

    return EXIT_SUCCESS;
}
