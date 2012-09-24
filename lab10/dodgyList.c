#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include "bool.h"
#include "item.h"

struct _list{
    Item item;
    List next;
};

//Creates a new list to be used
// @return, an empty list
List newList() {
    return NULL;
}

//Adds an item to the list
//If l is NULL returns a new list containting the item I
//Should be used in place of a create List function.
//  @param i, item to be added to the list
//  @param l, the list to be added to
//  @return the list with the item added
List add(Item i, List l) {
    List node = malloc(sizeof(struct _list));
    node->item = i;
    node->next = l;
    return node;
}

//checks if the list contains an item
//  @param i, the item to check for
//  @param l, the list to check in
//true if the list contains the item, else false
bool listContains(Item i, List l) {
    if (l == NULL) {
        return false;
    } else if (compare(l->item, i)) {
        return true;
    } else {
        return listContains(i, l->next);
    }
}

//deletes the list
//  @param l, the list to be deleted 
//  @retun NULL
List deleteList(List l) {
    if (l != NULL) {
        deleteList(l->next);
        free(l);
    }

    return NULL;
}

