// Author: evgenym
// Tutorial: Tue 11 Bell
// Date: 6 Aug 2012
// Summary: Lab04 exercises with the lecture implementation of lists.
//          My code is at the end of the file.

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "Item.h"
#include "lists.h"

// Local typedefs
struct _node {
    Item item;
    link next;
} node;


////////////////////////////////////////////////////////////////////////
//
// Implementation starts here.
// Originally from lists.c from Week03 lecture.
//
////////////////////////////////////////////////////////////////////////

void insertNode (link ls, Item item) {
    link newN = newNode (item);
    assert (ls != NULL); 
    newN->next = ls->next;
    ls->next = newN;
}

link insertHead (link ls, link newNode) {
    assert (newNode);
    newNode->next = ls;
    return newNode;
}


link newNode (Item item) {
    link ls;
    ls = malloc (sizeof (*ls));
    ls->item = item;
    ls->next = NULL; // Bugfix; Keller didn't have this. Not happy.
    return ls;     
}

void freeNode (link l) {
    assert (l);
    free (l);
}

Item getItem (link l) {
    assert (l);
    return (l->item);
}


link deleteNext (link ls) {
    assert (ls != NULL);
    assert (ls->next != NULL);
    link result = ls->next;
    ls->next = ls->next->next;
    return result;
}


void insertNext (link ls, link newN) {
    assert (ls != NULL); 
    newN->next = ls->next;
    ls->next = newN;
}


link getNext (link ls) {
    assert (ls);
    return (ls->next);
}  


int isEmptyList (link ls) {
    return (ls == NULL);
}

void printItem (Item item) {
    printf ("%d",item);
}

void printList (link ls) {
    link curr = ls;
    while (! isEmptyList (curr)) {
        printf ("->[");
        printItem (getItem (curr));
        printf ("]");
        curr = getNext (curr);
    }
    printf ("->[X]\n");
}

int length (link ls) {
    if (isEmptyList (ls)) {
        return 0;
    }
    return (1 + length (ls->next));
}

int length2 (link ls, int prevLength) {
    if (isEmptyList (ls)) {
        return prevLength;
    }
    return (length2 (ls->next, prevLength));
}

void printNode (link ls) {
    printf (" ");
    printItem (ls->item);
    printf (" ");
    return;
}

void traverse (link ls, void (*visit)(link)) {
    if (ls == NULL) {
        return;
    }
    (*visit) (ls);
    traverse (ls->next, visit);
    return;
}

void traverseR (link ls, void (*visit)(link)) {
    if (ls == NULL) {
        return;
    }
    traverseR (ls->next, visit);
    (*visit)(ls);
    return;
}

////////////////////////////////////////////////////////////////////////
//
// My code starts here.
//
////////////////////////////////////////////////////////////////////////

void freeList(link list) {
    traverseR(list, freeNode);
}

// Given a link to a node, square its value in-place.
void square(link node) {
    assert(node != NULL);
    node->item *= node->item;
}

// Square items of the entire list pointed to by the link provided.
void squareAll(link node) {
    traverse(node, square);
}

// Count the number of nodes for which a predicate returns non-zero.
int count(link list, int (*predicate)(Item value)) {
    link curr;
    int total = 0;

    for (curr = list; curr != NULL; curr = getNext(curr)) {
        if (predicate(curr->item)) {
            total++;
        }
    }

    return total;
}
