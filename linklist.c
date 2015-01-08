#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "linklist.h"
#include "err.h"

node *create_linklist(void) {
	node *ret = (node *)malloc(sizeof(node));
	if (ret == NULL) {
		errexit("malloc in create_linklist error");
	}
	memset(ret, 0, sizeof(node));
	ret->next = ret->pre = NULL;
	return ret;
}

void insert_head(node *hid_list, int hid) {
	node *new_node = (node *)malloc(sizeof(node));
	if (new_node == NULL) {
		errexit("malloc in insert_head error");
	}
	new_node->hid = hid;
	new_node->next = hid_list->next;
	new_node->pre = hid_list;
	if (hid_list->next != NULL) {
		hid_list->next->pre = new_node;
	}
	hid_list->next = new_node;
}

void delete_linklist(node *hid_list, node *cur) {
	cur->pre->next = cur->next;
	if (cur->next != NULL) {
		cur->next->pre = cur->pre;
	}
	free(cur);
}

node *fetchfirst_linklist(node *queue) {
	return queue->next;
}
