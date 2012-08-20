#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "List.h"

int main () {
   printf("Testing we can create a list...\n");
   List l = add(NULL, 200);

   assert(size(l) == 1);
   assert(contains(l, 200))

   printf("Passed\n");
   printf("Testing we can add more numbers to the list...\n");
   l = add(l, 150);

   assert(contains(l, 200));
   assert(contains(l, 150));
   assert(size(l) == 2);

   l = add(l, 100);

   assert(contains(l, 200));
   assert(contains(l, 150));
   assert(contains(l, 100));
   assert(size(l) == 3);

   l = add(l, 60);

   assert(contains(l, 200));
   assert(contains(l, 150));
   assert(contains(l, 100));
   assert(contains(l, 60));
   assert(size(l) == 4);

   l = add(l, -10);

   assert(contains(l, 200));
   assert(contains(l, 150));
   assert(contains(l, 100));
   assert(contains(l, 60));
   assert(contains(l, -10));
   assert(size(l) == 5);

   l = add(l, -200);

   assert(contains(l, 200));
   assert(contains(l, 150));
   assert(contains(l, 100));
   assert(contains(l, 60));
   assert(contains(l, -10));
   assert(contains(l, -200));
   assert(size(l) == 6);

   l = add(l, 40);

   assert(contains(l, 200));
   assert(contains(l, 150));
   assert(contains(l, 100));
   assert(contains(l, 60));
   assert(contains(l, -10));
   assert(contains(l, -200));
   assert(contains(l, 40));
   assert(size(l) == 7);

   printf("Passed\n");
   printf("Testing we can copy the list to an array of given size...\n");

   int array[7];

   copyToArray(l, array, size(l));

   assert(array[0] == 200);
   assert(array[1] == 150);
   assert(array[2] == 100);
   assert(array[3] == 60);
   assert(array[4] == -10);
   assert(array[5] == -200);
   assert(array[6] == 40);

   printf("Passed\n");
   printf("Testing we can have more than one instance of a list...\n");

   List l2 = add(add(add(add(add(add(add(NULL, 200), 150), 100), 60), -10), -200), 40);

   int array2[7];

   copyToArray(l, array2, size(l2));

   assert(array2[0] == 200);
   assert(array2[1] == 150);
   assert(array2[2] == 100);
   assert(array2[3] == 60);
   assert(array2[4] == -10);
   assert(array2[5] == -200);
   assert(array2[6] == 40);

   printf("Passed\n");
   printf("Testing the mergesort function sorts...\n");
   l = mergesort(l);

   copyToArray(l, array, size(l));

   assert(array[0] == -200);
   assert(array[1] == -50);
   assert(array[2] == 40);
   assert(array[3] == 60);
   assert(array[4] == 100);
   assert(array[5] == 150);
   assert(array[6] == 200);

   printf("Passed\n");
   printf("Testing the samplesort function sorts...\n");
   l2 = samplesort(l2);
   copyToArray(l, array2, size(l2));

   assert(array2[0] == -200);
   assert(array2[1] == -50);
   assert(array2[2] == 40);
   assert(array2[3] == 60);
   assert(array2[4] == 100);
   assert(array2[5] == 150);
   assert(array2[6] == 200);

   printf("Passed\n");
   printf("SUCCESS!! Great job, maybe write some tests on your own??\n");
   printf("Cause that would make you even more AWESOME!!\n");
   return EXIT_SUCCESS;

}
