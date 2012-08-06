#ifndef __LIST_PREDIACTES_H__
#define __LIST_PREDIACTES_H__

#include "Item.h"

// A few self-explanatory predicates for count()
int predIsZero(Item value);
int predIsEven(Item value);
int predIsOdd(Item value);
// Tests for strict positivity (i.e. > 0)
int predIsPositive(Item value);

#endif
