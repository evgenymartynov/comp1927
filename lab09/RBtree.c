// Author: evgenym
// Tutorial: Tue 11 Bell
// Date: 17 Sep 2012
// Summary: Solution to Lab09: 2-3-4 trees depth and balance.
//          My code is at the end of this file.


#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#define TRUE    1
#define FALSE    0

typedef struct STnode *link;
typedef int bool;

struct STnode
{ 
    int key; 
    link left, right; 
    int red;     //link to this node is red
};

link rootNodeLink;    // pointer to root node
link emptyTree;       // dummy node representing empty tree


// Prototypes for the lab
int twoThreeFourHeight(link tree);
bool isTwoThreeFour(link tree);
static void __attribute__((unused)) testBalanced(void);
static int isBalanced(link tree);
static int max(int a, int b);


// Lecture code

link new(int key, link l, link r, int colour)
{ 
    link newLink = malloc(sizeof *newLink); 
    newLink -> key  = key; 
    newLink -> left  = l; 
    newLink -> right = r;
    newLink -> red = colour;
    return newLink;
}

void STinit(int n)
{ 
    emptyTree = new(-1, NULL, NULL, 0); 
    rootNodeLink = emptyTree;
}

link rotateR(link currentTree)
{
    link rotRTree = currentTree -> left;
    currentTree -> left = rotRTree -> right;
    rotRTree -> right = currentTree;
    return rotRTree;
}

link rotateL(link currentTree)
{
    link rotLTree = currentTree -> right;
    currentTree -> right = rotLTree -> left;
    rotLTree -> left = currentTree;
    return rotLTree;
}

link RBinsert(link currTree, int key, int inRightSubtree)
{
    if (currTree == emptyTree)
        // always part of existing node if not root, therefore we set red = TRUE
        return new(key, emptyTree, emptyTree, TRUE);  
    
    // node is a 4-node, lift it
    if ((currTree->left->red) && (currTree->right->red))
    { 
        currTree->red = TRUE; 
        currTree->left->red = FALSE; 
        currTree->right->red = FALSE; 
    }
    
    if (key < currTree->key) // insert into the left subtree
    { 
        currTree->left = RBinsert(currTree->left, key, FALSE); 
        if (currTree->red && currTree->left->red && inRightSubtree)
            currTree = rotateR(currTree); 
        
        if (currTree->left->red && currTree->left->left->red)
        { 
            currTree = rotateR(currTree); 
            currTree->red = FALSE; 
            currTree->right->red = TRUE; 
        }
    }
    else // insert into right subtree .......
    {
        currTree->right = RBinsert(currTree->right, key, TRUE); 
        if (currTree->red && currTree->right->red && !inRightSubtree)
            currTree = rotateL(currTree); 
        
        if (currTree->right->red && currTree->right->right->red)
        { 
            currTree = rotateL(currTree); 
            currTree->red = FALSE; 
            currTree->left->red = TRUE; 
        }
    }
    return currTree;
}
        
void STinsert(int item)
{ 
    rootNodeLink = RBinsert(rootNodeLink, item, FALSE); 
    rootNodeLink->red = FALSE;
}

void STtraverse(link tree, int level, int side)
{
    if (tree == emptyTree)
        return;

    printf("\n");
    int i;
    for (i = level; i != 0; i--)
        printf("  ");

    printf("%d", tree -> key);
    if (tree->red)
        printf(" *");
    STtraverse(tree -> left, level+1, 0);
    STtraverse(tree -> right, level+1, 1);
}


void STprint(link tree)
{
    if (tree == emptyTree)
        return;
    
    STprint(tree -> left);
    printf("%4d", tree -> key);
    STprint(tree -> right);
}


#if 0
int main(int argc, const char * argv[]) {
    int test[] = {22, 12, 8, 15, 11, 19, 43, 44, 45, 42, 41, 40, 39};

    STinit(0);
    
    int i;
    for (i = 0; i < (sizeof test)/(sizeof (int)); i++) {
        STinsert(test[i]);
        printf("(%4d) Insert %4d: ", i, test[i]);
        STtraverse(rootNodeLink, 0, 0);
        printf("\n");
        printf("Height: %d\n", twoThreeFourHeight(rootNodeLink));
        printf("Balanced: %d\n\n", isTwoThreeFour(rootNodeLink));
    }

    testBalanced();

    return 0;
}
#endif


////////////////////////////////////////////////////////////////////////
//
// My code starts here.
//
////////////////////////////////////////////////////////////////////////


static void testBalanced(void) {
    printf("Testing isTwoThreeFour()\n");

    assert(isTwoThreeFour(
        new(1, emptyTree, emptyTree, FALSE)
    ));

    assert(!isTwoThreeFour(
        new(1, emptyTree, emptyTree, TRUE)
    ));

    assert(isTwoThreeFour(
        new(1, new(-1, emptyTree, emptyTree, TRUE), emptyTree, FALSE)
    ));

    assert(!isTwoThreeFour(
        new(1, new(-1, emptyTree, emptyTree, FALSE), emptyTree, FALSE)
    ));

    assert(isTwoThreeFour(
        new(0,
            new(-1, emptyTree, emptyTree, FALSE),
            new(1, emptyTree, emptyTree, FALSE),
        FALSE)
    ));

    assert(!isTwoThreeFour(
        new(5,
            new(-1, emptyTree, emptyTree, FALSE),
            new(1, emptyTree, emptyTree, FALSE),
        FALSE)
    ));

    assert(!isTwoThreeFour(
        new(0,
            new(-1, emptyTree, emptyTree, FALSE),
            new(1, emptyTree, emptyTree, FALSE),
        TRUE)
    ));

    printf("Passed\n");
}


// Calculates height of a red-black tree.
// The height is calculated with respect to black nodes.
int twoThreeFourHeight(link tree) {
    if (tree == emptyTree) {
        return 0;
    }
    int leftHeight  = twoThreeFourHeight(tree->left);
    int rightHeight = twoThreeFourHeight(tree->right);
    int thisHeight  = 1;
    if (tree->red) {
        thisHeight = 0;
    }

    return max(leftHeight, rightHeight) + thisHeight;
}


// A tree is balanced if and only if the number of preceding black nodes
// is the same for all leaves. Further, value-inequalities must hold.
bool isTwoThreeFour(link tree) {
    if (tree == emptyTree) {
        return TRUE;
    }

    return !tree->red && isBalanced(tree) != -1;
}


// Walks the tree, returning -1 if heights are mismatched amongst
// children, and the height otherwise.
// Not using the height() function because O(n^2) is lame.
static int isBalanced(link tree) {
    // Base case.
    if (tree == emptyTree) {
        return 0;
    }

    // Value-inequalities.
    if (tree->left != emptyTree && tree->left->key >= tree->key) {
        return -1;
    }
    if (tree->right != emptyTree && tree->right->key <= tree->key) {
        return -1;
    }

    // Recurse to check inequalities.
    int leftHeight  = isBalanced(tree->left);
    int rightHeight = isBalanced(tree->right);
    int thisHeight  = 1;
    if (tree->red) {
        thisHeight = 0;
    }

    if (leftHeight != rightHeight || leftHeight == -1) {
        return -1;
    } else {
        return thisHeight + leftHeight;
    }
}


// Computes max of two numbers.
static int max(int a, int b) {
    if (a > b) {
        return a;
    } else {
        return b;
    }
}
