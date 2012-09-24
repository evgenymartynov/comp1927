#include <stdlib.h>
#include <string.h>
#include "item.h"

int hash(Item a) {
    if (a == NULL) {
        return 0;
    }

    // We use Knuth's (I think) method of hashing;
    // as presented in TAOCP (I think).
    int i, result = 0x19271927;
    for (i = 0; a[i]; i++) {
        result = (result * 33) ^ a[i];
    }

    // abs() to avoid dealing with negative indices.
    // Ideally, hash() should return unsigned.
    return abs(result);
}

bool compare(Item a, Item b) {
    if (strcmp(a, b) == 0) return true;
    return false;
}

