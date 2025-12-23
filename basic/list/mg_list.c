#include <stdlib.h>
#include <string.h>
#include "mg_list.h"

#define get_last(it, lst) \
	for(it = mg_list_node_first(lst); it && mg_list_node_next(it) != NULL; it = mg_list_node_next(it) ) { }

struct MGListNode_
{
	bool detached;

	MGListNode *next;

	size_t data_sz;
	unsigned char data[0];
};

struct MGList_
{
	size_t count;

	MGListDataCmpFun  cb_cmp;
	MGListDataDelFun  cb_del;
	MGListDataCopyFun cb_cpy;

	MGListNode *first;
	MGListNode *last;
};

static bool sdefault_compare(void* n1, void* n2, size_t len)
{
	return ( memcmp (n1, n2, len) == 0 );
}

static bool sdefault_copy(void* src, void* dst, size_t len)
{
	memcpy(dst, src, len);
	return true;
}

static inline void* smg_alloc_list_node(size_t data_sz)
{
	MGListNode* node = (MGListNode*) malloc ( sizeof (MGListNode) + data_sz);
	if (!node)
		return NULL;

	node->next = NULL;
	node->detached = true;
	node->data_sz = data_sz;
	return node;
}

static void smg_list_add_node(MGList *lst, MGListNode *node)
{
	node->next = NULL;
	node->detached = false;

	if (lst->last)
		lst->last->next = node;
	else
		lst->first = node;

	lst->last = node;
	lst->count++;
}

bool mg_list_add(MGList *lst, void* data, size_t len)
{
	MGListNode* node = (MGListNode*)smg_alloc_list_node(len);
	if (!node)
		return false;

	node->data_sz = len;
	if (!lst->cb_cpy(data, node->data, node->data_sz)) {
		free(node);
		return false;
	}

	smg_list_add_node(lst, node);

	return true;
}

bool mg_list_detach(MGList *lst, MGListNode *node)
{
	MGListNode *it;
	if (lst->first == NULL)
		return false;

	if (lst->first == node) {
		lst->first = lst->first->next;
		lst->count--;
		node->detached = true;
		return true;
	}

	for (it = lst->first->next; it != NULL; it = mg_list_node_next(it)) {
		if (it->next == node) {
			it->next = node->next;
			lst->count--;
			node->detached = true;
			return true;
		}
	}
	return false;
}

bool mg_list_node_destroy(MGListNode *node, MGListDataDelFun fun)
{
	if (!node->detached)
		return false;
	if (fun)
		fun(node->data, node->data_sz);
	free(node);
	return true;
}

void mg_list_remove(MGList* lst, MGListNode* node)
{
	mg_list_detach(lst, node);
	mg_list_node_destroy(node, lst->cb_del);
}

MGList* mg_list_create(MGListDataCmpFun fun_cmp, MGListDataDelFun fun_del, MGListDataCopyFun fun_cpy)
{
	MGList* lst = (MGList*) malloc(sizeof(MGList));
	if (!lst)
		return NULL;

	lst->count = 0;
	lst->first = NULL;
	lst->last = NULL;

	lst->cb_del = NULL;
	lst->cb_cmp = sdefault_compare;
	lst->cb_cpy = sdefault_copy;

	if (fun_cmp)
		lst->cb_cmp = fun_cmp;

	if (fun_cpy)
		lst->cb_cpy = fun_cpy;

	if (fun_del)
		lst->cb_del = fun_del;

	return lst;
}

void mg_list_set_cmp_cb(MGList* lst, MGListDataCmpFun fun)
{
	if (!fun)
		fun = sdefault_compare;
	lst->cb_cmp = fun;
}

void mg_list_set_del_cb(MGList* lst, MGListDataDelFun fun)
{
	lst->cb_del = fun;
}

void mg_list_set_cpy_cb(MGList* lst, MGListDataCopyFun fun)
{
	if (!fun)
		fun = sdefault_copy;
	lst->cb_cpy = fun;
}

void mg_list_destroy(MGList* lst)
{
	while(lst->count) {
		mg_list_remove(lst, lst->first);
	}

	free(lst);
}

bool mg_list_contains(MGList* lst, void* data, size_t len)
{
	MGListNode* it;
	foreach(it, lst) {
		if (lst->cb_cmp(it->data, data, len)) {
			return true;
		}
	}
	return false;
}

MGListNode* mg_list_search2(MGList* lst, void* data, size_t len, MGListDataCmpFun fun_cmp)
{
	if (!mg_list_count(lst))
		return NULL;

	MGListNode* it;
	if (fun_cmp == NULL)
		fun_cmp = lst->cb_cmp;

	foreach(it, lst) {
		if (fun_cmp(it->data, data, len)) {
			return it;
		}
	}
	return NULL;
}

MGListNode* mg_list_search(MGList* lst, void* data, size_t len)
{
	return mg_list_search2(lst, data, len, lst->cb_cmp);
}

size_t mg_list_count(MGList* lst)
{
	return lst->count;
}

const void* mg_list_data_const(MGListNode* node)
{
	return (const void*)node->data;
}

void* mg_list_data(MGListNode* node)
{
	return node->data;
}

size_t mg_list_data_len(MGListNode* node)
{
	return node->data_sz;
}

void* mg_list_data_copy(MGList* lst, MGListNode* node)
{
	void* data = malloc( mg_list_data_len(node) );
	if (!data)
		return NULL;

	lst->cb_cpy( node->data, data, mg_list_data_len(node) );
	return data;
}

MGListNode* mg_list_node_next(MGListNode* node)
{
	return node->next;
}

MGListNode* mg_list_node_first(MGList* lst)
{
	return lst->first;
}

bool mg_list_copy(MGList* src, MGList* dst)
{
	MGListNode* it;
	foreach(it, src) {
		if (!mg_list_add(dst, mg_list_data(it), mg_list_data_len(it)))
			return false;
	}
	return true;
}

MGList* mg_list_clone(MGList* src)
{
	MGList* dst = mg_list_create(src->cb_cmp, src->cb_del, src->cb_cpy);
	if (!dst)
		return NULL;
	if (!mg_list_copy(src, dst)) {
		mg_list_destroy(dst);
		return NULL;
	}
	return dst;
}
