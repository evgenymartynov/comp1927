// Author: evgenym
// Tutorial: Tue 11 Bell
// Date: 6 Aug 2012
// Summary: Lab04 exercises that make me want to gouge my eyes out.
//          My code is at the end of the file.

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "Item.h"

// Local typedefs
typedef struct _node *link;
struct _node {
    Item item;
    link next;
} node;


// Local prototypes
link newNode (Item);
void freeNode (link);

void insertNext (link, link);
link insertHead (link, link);
link deleteNext (link);

link getNext (link); 
Item getItem (link);
int isEmptyList (link);

Item getItem (link);
void printItem (Item);

// Destroy the list
void freeList(link list);

// Given a link to a node, square its value in-place.
static void square(link node);

// Square items of the entire list pointed to by the link provided.
void squareAll(link list);

// Test square and squareAll functions
static void testSquares(void);


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
static void square(link node) {
    assert(node != NULL);
    node->item *= node->item;
}

// Square items of the entire list pointed to by the link provided.
void squareAll(link node) {
    traverse(node, square);
}

// Test square and squareAll functions
static void testSquares(void) {
    printf("Testing square()...\n");

    printf("  squaring 0\n");
        link zero = newNode(0);
        square(zero);
        assert(getItem(zero) == 0);
    freeNode(zero);

    printf("  squaring 1\n");
        link one = newNode(1);
        square(one);
        assert(getItem(one) == 1);
    freeNode(one);

    printf("  squaring -1\n");
        link negOne = newNode(-1);
        square(negOne);
        assert(getItem(negOne) == 1);
    freeNode(negOne);

    printf("  squaring a few random numbers\n");
        link node;
        node = newNode(5); square(node);
        assert(getItem(node) == 25);
        freeNode(node);

        node = newNode(8); square(node);
        assert(getItem(node) == 64);
        freeNode(node);

        node = newNode(-10); square(node);
        assert(getItem(node) == 100);
        freeNode(node);

        node = newNode(-256); square(node);
        assert(getItem(node) == 65536);
    freeNode(node);
    printf("passed\n");

    printf("Testing squareAll()...\n");
    link head, list;

    printf("  squaring 0->1->[X]\n");
        head = list = newNode(0);
        insertNode(list, 1);
        squareAll(list);
        assert(getItem(list) == 0); list = getNext(list);
        assert(getItem(list) == 1); list = getNext(list);
        assert(list == NULL);
        freeList(head);

    printf("  squaring -5->-11->124->[X]\n");
        head = list = newNode(-5);
        insertNode(list, 124);
        insertNode(list, -11);
        squareAll(list);
        assert(getItem(list) == 25); list = getNext(list);
        assert(getItem(list) == 121); list = getNext(list);
        assert(getItem(list) == 124*124); list = getNext(list);
        assert(list == NULL);
        freeList(head);

    printf("  squaring [X] to check for weird bugs\n");
        head = list = NULL;
        squareAll(list);
        assert(isEmptyList(list));
        freeList(head);

    printf("passed\n");
}

int main () {
    testSquares();

    link ls = newNode (1);
    insertNode (ls, 4);
    insertNode (ls, 3);
    insertNode (ls, 2);
    traverseR (ls, printNode);
    printf ("\n");
    traverse (ls, printNode);
    printf ("\n");

    return (0);
}
