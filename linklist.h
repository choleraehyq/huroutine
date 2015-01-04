#ifndef _LINKLIST_H
#define _LINKLIST_h

typedef struct NODE {
	int hid;
	struct NODE *next;
	struct NODE *pre;
} node;

node *create_linklist(void);
void insert_head(node *list, int hid);
void delete_linklist(node *, node *);

#endif
