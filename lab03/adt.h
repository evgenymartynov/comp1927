#include "Item.h"

void LISTinit ();                  /* initialise the list */
int  LISTempty ();                 /* is the list empty? */
int  LISTmove (int n);             /* move current position */
Item LISTcurrent ();               /* return element at current position */
void LISTbefore (Item newItem);    /* insert element before current */
void LISTafter (Item newItem);     /* insert element after current */
Item LISTdelete ();                /* delete current element */
