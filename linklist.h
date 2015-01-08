#ifndef _LINKLIST_H
#define _LINKLIST_H


struct NODE {
	int hid;
	struct NODE *next;
	struct NODE *pre;
};

typedef struct NODE node;

node *create_linklist(void);
void insert_head(node *list, int hid);
void delete_linklist(node *, node *);
node *fetchfirst_linklist(node *);

#endif
