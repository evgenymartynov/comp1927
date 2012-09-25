#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "hashTable.h"
#include "list.h"
#include "item.h"
#include "bool.h"

#define NUM_BUCKETS 255

struct _hashTable{ 
    List* buckets;
    int size;
};

//Creates a new hash table
//   @return a new hash table
HashTable newHashTable() {
    HashTable new = malloc(sizeof(struct _hashTable));
    new->buckets = malloc(sizeof(List) * NUM_BUCKETS);

    int i;
    for (i = 0; i < NUM_BUCKETS; i++) {
        new->buckets[i] = newList();
    }

    new->size = NUM_BUCKETS;

    return new;
}

//Adds the item to the hashTable given
// @param i, the item to be hashed and added to the hashTable
// @param h, the hashtable to be added to and returned
// @return The hashtable passed in and modified
HashTable put(Item i, HashTable h) {
    int hashval = hash(i) % NUM_BUCKETS;
    h->buckets[hashval] = add(i, h->buckets[hashval]);

    return h;
}

//Returns true if the hashTable contains a particular element
// @param i, the item to be hashed and added to the hashTable
// @param h, the hashtable to be added to and returned
// @return true if the hashtable is correct, else false
bool hashTableContains(Item i, HashTable h) {
    int hashval = hash(i) % NUM_BUCKETS;
    return listContains(i, h->buckets[hashval]);
}

HashTable deleteHashTable(HashTable h) {
    int i;
    for (i = 0; i < h->size; i++) {
        deleteList(h->buckets[i]);
    }
    free(h->buckets);
    free(h);

    return NULL;
}
