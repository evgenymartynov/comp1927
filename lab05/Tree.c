// Author: evgenym
// Tutorial: Tue 11 Bell
// Date: 12 Aug 2012

// Summary:
// Simple tree-traversal exercise from Lab05.
// Lab exercise asked for full branch coverage.
// The tests provide 100% code coverage (as determined by gcov/lcov)
// of my own functions.

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "Tree.h"

//
// Local struct typedefs
//

struct treeNode {
    Item item;
    treeLink left, right;
};

// I don't like the type-naming conventions used in labs.
typedef treeLink Node;


//
// Local prototypes
//

static Node newNode(Item value, Node leftChild, Node rightChild);


///////////////////////////////////////////////////////////////////////
//
// Interface implementation code starts here [mark this]
//
///////////////////////////////////////////////////////////////////////


// Counts and returns the number of leaf nodes of a tree
// We delegate all work to a simpler function if the tree isn't empty.
int count(Node tree) {
    if (tree == NULL) {
        // Empty tree.
        return 0;
    } else if (tree->left == NULL && tree->right == NULL) {
        // Degenerate case: just the root node.
        return 1;
    } else {
        // Recursively walk the child nodes.
        return count(tree->left) + count(tree->right);
    }
}


///////////////////////////////////////////////////////////////////////
//
// Given ADT implementation code starts here [do not mark this]
//
///////////////////////////////////////////////////////////////////////


//Creates a new tree with root a
treeLink newTree(Item a) {
    treeLink new = malloc (sizeof(struct treeNode));
    new ->item = a;
    new->left = NULL;
    new->right = NULL;
    return new;
}


//adds a node to the tree in no particularly defined way
treeLink addToTree(treeLink t, Item a) {
    if (t == NULL) {
        t = newTree(a);
    } else {
        if (t->item < a) {
            t->right = addToTree(t->right, a);
        } else {
            t->left = addToTree(t->left, a);
        }
    }
    return t;
}


//frees memory and returns NULL if successful
treeLink deleteTree(treeLink t) {
    if (t != NULL) {
        t->right = deleteTree(t->right);
        t->left = deleteTree(t->left);
        free(t);
    }
    return NULL;
}

static void printTreeA(treeLink tree, int height) {
    if (tree != NULL) {
        printTreeA(tree->left, height +1);
        int i;
        for (i = 0; i < height; i++){
            printf("   ");
        }
        printf("%d\n", tree->item);
        printTreeA(tree->right, height + 1);
    }
}

void printTree(treeLink tree) {
    printTreeA(tree, 0);
}

///////////////////////////////////////////////////////////////////////
//
// Internal implementation code starts here [mark this]
//
///////////////////////////////////////////////////////////////////////


// Creates a new tree node.
static Node newNode(Item value, Node leftChild, Node rightChild) {
    Node node = (Node)malloc(sizeof(*node));
    assert(node != NULL);

    node->item  = value;
    node->left  = leftChild;
    node->right = rightChild;

    return node;
}

// Tests the count() func.
void testCount(void) {
    printf("Testing count()...\n");

        Node root = NULL, temp = NULL;

        printf("  testing an empty (NULL) tree...\n");
            assert(count(root) == 0);
        deleteTree(root);

        printf("  testing degenerate tree (1 node)\n");
            root = newNode(5, NULL, NULL);
            assert(count(root) == 1);
        deleteTree(root);

        printf("  testing one-left-child tree\n");
            temp = newNode(2, NULL, NULL);
            root = newNode(5, temp, NULL);
            assert(count(root) == 1);
        deleteTree(root);
        printf("  testing one-right-child tree\n");
            temp = newNode(2, NULL, NULL);
            root = newNode(5, NULL, temp);
            assert(count(root) == 1);
        deleteTree(root);

        printf("  testing degenerate tree (5 linear nodes)\n");
            temp = newNode(1, NULL, NULL);
            temp = newNode(2, temp, NULL);
            temp = newNode(3, temp, NULL);
            temp = newNode(4, temp, NULL);
            root = newNode(5, temp, NULL);
            assert(count(root) == 1);
        deleteTree(root);

        printf("  testing full binary tree (7 nodes, depth 3)\n");
            root =
                newNode(1,
                    newNode(2,
                        newNode(3, NULL, NULL),
                        newNode(4, NULL, NULL)
                    ),
                    newNode(5,
                        newNode(6, NULL, NULL),
                        newNode(7, NULL, NULL)
                    )
                );
            assert(count(root) == 4);
        deleteTree(root);

    printf("Passed\n");
}
