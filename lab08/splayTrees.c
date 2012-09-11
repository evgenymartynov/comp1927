// Author: evgenym
// Tutorial: Tue 11 Bell
// Date: 10 Sep 2012
// Summary: Implementing printTree function; see bottom of this file.
//          Rest is copied from the lab exercise.


//
//  Code for splay tree insertion and randomised insertion 
//  (parts of code taken from ST_bstree implementation. This
//  is *not* a complete implementation of ST ADT!)
//

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


// Should be in separate Item.h file:

#define key(item) item
#define less(key1, key2) (key1 < key2)
#define NULLitem 0

typedef int Item;
typedef int Key;



typedef struct treeNode * treeLink;

struct treeNode {
    Item item;
    treeLink left;
    treeLink right;
    int size;
};


void printTree(treeLink tree);
void labPartTwo(void);
static void printTreeWorker(treeLink tree, int depth);


static treeLink emptyTree = NULL;

treeLink NEW (Item item, treeLink l, treeLink r, int size){ 
    treeLink newLink = malloc (sizeof *newLink); 

    newLink->item = item; 
    newLink->left  = l; 
    newLink->right = r; 
    newLink->size = size;
    return newLink;
}

treeLink newTree () { 
    if (emptyTree == NULL) 
        emptyTree = NEW (NULLitem, NULL, NULL, 0); 
    return (emptyTree);
}

void printItem (Item item) {
    printf ("%d",item);
}

void printNode (treeLink ls) {
    printf (" ");
    printItem (ls->item);
    printf (" ");
    return;
}


treeLink rotLeft (treeLink currentTree) {
    assert (currentTree != emptyTree);
    assert (currentTree->right != emptyTree);
    treeLink rotLTree = currentTree->right;
    currentTree->right = rotLTree->left;
    rotLTree->left = currentTree;
    rotLTree->left->size = 
    rotLTree->left->right->size + rotLTree->left->left->size + 1;
    rotLTree->size = 
    rotLTree->left->size + rotLTree->right->size +1;
    return rotLTree;
}

treeLink rotRight (treeLink currentTree) {
    assert (currentTree != emptyTree);
    assert (currentTree->left != emptyTree);

    treeLink rotRTree = currentTree->left;
    currentTree->left = rotRTree->right;
    rotRTree->right = currentTree;
    rotRTree->right->size = 
    rotRTree->right->left->size + rotRTree->right->right->size + 1;
    rotRTree->size = 
    rotRTree->right->size+ rotRTree->left->size+1;
    return rotRTree;
}


void traverseTreePreorder (treeLink tree, void (*visit)(treeLink)) {

    if (tree == emptyTree) {
        return;
    }
    visit (tree);
    traverseTreePreorder (tree->left, visit);
    traverseTreePreorder (tree->right, visit);

}

treeLink splayInsert (treeLink tree, Item item) {
    Key v = key (item);
    if (tree == emptyTree)
        return (NEW (item, emptyTree, emptyTree, 1)); 

    if (less (v, key(tree->item))) {
        if (tree->left == emptyTree) {
            return (NEW (item, emptyTree, tree, tree->size+1));
        }
        if (less (v, key (tree->left->item))) {
            tree->left->left = splayInsert (tree->left->left, item);
            tree = rotRight (tree);
        } else {
            tree->left->right = splayInsert (tree->left->right, item);
            tree->left = rotLeft (tree->left);
        }
        return rotRight (tree);
    } else {
        if (tree->right == emptyTree) {
            return (NEW (item, tree, emptyTree, tree->size+1));
        }
        if (less (key (tree->right->item), v)) {
            tree->right->right = splayInsert (tree->right->right, item);
            tree = rotLeft (tree);
        } else {
            tree->right->left = splayInsert (tree->right->left, item);
            tree->right = rotRight (tree->right);
        }
        return rotLeft (tree);
    }
}

treeLink insertRoot (treeLink currentLink, Item item) { 

    if (currentLink == emptyTree) {
        return (NEW (item, emptyTree, emptyTree, 1));
    }
    if (less (key (item), key (currentLink->item))) {
        currentLink->left = insertRoot (currentLink->left, item);
        rotRight (currentLink);
    } else if (less (key (currentLink->item), key (item))) {
        currentLink->right = insertRoot (currentLink->right, item);
        rotLeft (currentLink);
    } else {
        currentLink->size--;
    }
    currentLink->size++;
    return (currentLink);
}


treeLink insertRandom (treeLink currTree, Item item) {
    Key currKey = key (currTree->item);
    if (currTree == emptyTree) {
        return NEW (item, emptyTree, emptyTree, 1);
    }
    if (rand () < RAND_MAX/(currTree->size+1)) {
        return (insertRoot (currTree, item));
    } else if (less (key (item), currKey)) {
        insertRandom (currTree->left, item);
    } else {
        insertRandom (currTree->right, item);
    }
    currTree->size++;
    return currTree;
} 




//  should be in separate client file for propert ST ADT 
//  implementation
//
int main () {

    treeLink tree = newTree();

    tree = splayInsert (tree,3);
    tree = splayInsert (tree,4);
    tree = splayInsert (tree,5);
    tree = splayInsert (tree,6);
    tree = splayInsert (tree,8);
    tree = splayInsert (tree,9);
    tree = splayInsert (tree,10);
    tree = splayInsert (tree,11);

    printTree(tree);
    traverseTreePreorder (tree, &printNode);
    printf ("\n size %d \n", tree->size);

    tree = splayInsert (tree,1);

    printTree(tree);
    traverseTreePreorder (tree, &printNode);
    printf ("\n size %d \n", tree->size);

    tree = splayInsert (tree, 12);

    printTree(tree);
    traverseTreePreorder (tree, &printNode);
    printf ("\n size %d \n", tree->size);


    // Part two of the lab
    labPartTwo();


    return (0);
}


// Prints the tree, indented accordingly.
void printTree(treeLink tree) {
    printTreeWorker(tree, 0);
}


// Prints a tree, indented accordingly, by keeping track of the depth
// of the current node being processed.
static void printTreeWorker(treeLink tree, int depth) {
    if (tree == emptyTree) {
        return;
    }

    // Pre-order traversal: print first, ask questions later
    int i;
    for (i = 0; i < depth; i++) {
        printf("  ");
    }
    printNode(tree);
    printf("\n");

    // We need to go deeper.
    printTreeWorker(tree->left,  depth+1);
    printTreeWorker(tree->right, depth+1);
}


// Solution to part two of the lab
void labPartTwo(void) {
    treeLink tree = newTree();

    // Any arrangement with the 2 as the last node works to produce
    // a perfectly balanced outcome.
    tree = splayInsert (tree,3);
    tree = splayInsert (tree,1);
    tree = splayInsert (tree,2);

    printf("Balanced tree with nodes 1..3:\n");
    printTree(tree);

    tree = newTree();
    tree = splayInsert (tree,3);
    tree = splayInsert (tree,2);
    tree = splayInsert (tree,1);

    tree = splayInsert (tree,6);
    tree = splayInsert (tree,5);
    tree = splayInsert (tree,7);

    tree = splayInsert (tree,4);

    printf("Balanced tree with nodes 1..7:\n");
    printTree(tree);
}
