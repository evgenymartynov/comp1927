// Author: evgenym
// Tutorial: Tue 11 Bell
// Date: 12 Aug 2012

// Summary:
// Simple tree-traversal exercise from Lab05.
// Lab exercise asked for full branch coverage.
// The tests provide 100% code coverage (tested with gcov/lcov).

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "Item.h"

//
// Local struct typedefs
//

typedef struct treeNode *treeLink;
typedef struct treeNode {
    Item item;
    treeLink left, right;
} node;

// I really dislike the type-naming conventions used in labs.
typedef treeLink Node;


//
// Local prototypes
//

int         count(Node tree);

static Node newNode(Item value, Node leftChild, Node rightChild);
static void destroyTree(Node root);
static void testCount(void);


int main(int argc, char *argv[]) {
    // Disable line-buffering
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);

    testCount();

    return EXIT_SUCCESS;
}


///////////////////////////////////////////////////////////////////////
//
// Interface implementation code starts here
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
// Internal implementation code starts here
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

// Frees up memory taken up by a tree.
// Takes in NULL trees, too.
static void destroyTree(Node root) {
    if (root != NULL) {
        destroyTree(root->left);
        destroyTree(root->right);
        free(root);
    }
}

// Tests the count() func.
void testCount(void) {
    printf("Testing count()...\n");

        Node root = NULL, temp = NULL;

        printf("  testing an empty (NULL) tree...\n");
            assert(count(root) == 0);
        destroyTree(root);

        printf("  testing degenerate tree (1 node)\n");
            root = newNode(5, NULL, NULL);
            assert(count(root) == 1);
        destroyTree(root);

        printf("  testing one-left-child tree\n");
            temp = newNode(2, NULL, NULL);
            root = newNode(5, temp, NULL);
            assert(count(root) == 1);
        destroyTree(root);
        printf("  testing one-right-child tree\n");
            temp = newNode(2, NULL, NULL);
            root = newNode(5, NULL, temp);
            assert(count(root) == 1);
        destroyTree(root);

        printf("  testing degenerate tree (5 linear nodes)\n");
            temp = newNode(1, NULL, NULL);
            temp = newNode(2, temp, NULL);
            temp = newNode(3, temp, NULL);
            temp = newNode(4, temp, NULL);
            root = newNode(5, temp, NULL);
            assert(count(root) == 1);
        destroyTree(root);

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
        destroyTree(root);

    printf("Passed\n");
}
