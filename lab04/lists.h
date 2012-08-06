typedef struct _node *link;

link newNode (Item);
void freeNode (link);

void insertNode (link ls, Item item);
void insertNext (link, link);
link insertHead (link, link);
link deleteNext (link);

link getNext (link); 
Item getItem (link);
int isEmptyList (link);

void square (link ls);
void squareAll (link ls);

int count (link ls, int (*pred)(Item));

void freeList(link list);
