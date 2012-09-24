#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "hashTable.h"

#define MAX_DICT_SIZE       350000
// Note longest word to be seen in dictionary is
// Llanfairpwllgwyngyllgogerychwyrndrobwllllantysiliogogogoch
#define MAX_STRING_LENGTH   59


static void process_word(HashTable h, char *word) {
    if (hashTableContains(word, h)) {
        printf("\nSeen %s\n", word);
    } else {
        printf("\nNot seen %s\n", word);
        printf("Adding: %s\n", word);
        put(word, h);
    }
    assert(hashTableContains(word, h));
}


int main () {
    // Create the hash table we are going to use
    HashTable h = newHashTable();

    // Open up a dictionary of words to use from the start
    FILE * input = fopen("dictionary.txt", "r");
    if (input == NULL) return EXIT_FAILURE;

    // Malloc some space for the strings we are reading in from the dictionary
    char **temp = malloc(sizeof(char*) * MAX_DICT_SIZE);
    int k = 0;
    for (k = 0; k < MAX_DICT_SIZE; k++) {
        temp[k] = malloc(sizeof(char) * MAX_STRING_LENGTH);
    }

    // Read in the input dictionary.
    int i;
    for (i = 0; fgets(temp[i], MAX_STRING_LENGTH, input); i++) {
        int k;
        char *word = temp[i];
        for (k = 0; k < MAX_STRING_LENGTH && word[k]; k++)
            if (word[k] == '\n')
                word[k] = '\0';

        if (strlen(word) != 0) {
            process_word(h, word);
        }
    }

    // Close the dictionary
    fclose(input);

    /* TODO: read in some strings from the user using scanf
    until they enter an empty string
    or until the dictionary is full

    tell them if we have ever seen them before
    add them to the hash table
    */

    while (true) {
        char word[MAX_STRING_LENGTH] = {};
        if (scanf("%s", word) != 1) {
            break;
        } else {
            process_word(h, word);
        }
    }

    // Free all the strings we have malloc'ed
    for (k=0; k < MAX_DICT_SIZE; k++) {
        free(temp[k]);
    }
    free(temp);

    // Delete the hash table we used
    h = deleteHashTable(h);

    // Exit
    return EXIT_SUCCESS;
}
