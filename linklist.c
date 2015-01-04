#include <stdlib.h>
#include <string.h>
#include "linklist.h"

node *create_linklist(void) {
	node *ret = (node *)malloc(sizeof(node));
	memset(ret, 0, sizeof(node));
	return ret;
}

void insert_head(node *hid_list, int hid) {
	node *new_node = (node *)malloc(sizeof(node));
	new_node->hid = hid;
	new_node->next = hid_list->next;
	new_node->pre = hid_list;
}

void delete_linklist(node *hid_list, node *cur) {
	cur->pre->next = cur->next;
	if (cur->next != NULL) {
		cur->next->pre = cur->pre;
	}
	free(cur);
}
