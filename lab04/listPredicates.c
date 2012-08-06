// Author: evgenym
// Tutorial: Tue 11 Bell
// Date: 6 Aug 2012
// Summary: Prediactes for count() of Lab04.

// I do not know whether using eq() is justtified, given the leaky
// abstraction of Item.
//
// Because of that, almost all predicates assume that Item is a POD type

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "listPredicates.h"

int predIsZero(Item value) {
    return eq(value, 0);
}

int predIsEven(Item value) {
    return value % 2 == 0;
}

int predIsOdd(Item value) {
    // Note that negativeOddNumber % 2 == -1.
    return value % 2 != 0;
}

// Tests for strict positivity (i.e. > 0)
int predIsPositive(Item value) {
    return value > 0;
}
