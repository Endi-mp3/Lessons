#pragma once
#include "mg_types.h"

typedef bool (*MGListDataCmpFun)(void*, void*, size_t);
typedef bool (*MGListDataCopyFun)(void*, void*, size_t);
typedef bool (*MGListDataDelFun)(void*, size_t);

struct MGListNode_;
struct MGList_;

typedef struct MGListNode_ MGListNode;
typedef struct MGList_ MGList;

/**
 * @macro foreach
 * @brief helper to go thought list
 * @param it - MGListNode* Used as iterator.
 * @param lst - MGList* List.
 */
#define foreach(it, lst) \
	for ( it = mg_list_node_first( (lst) ); it != NULL; it = mg_list_node_next( (it) ) )

/**
 * @brief	Allocates MGList, init callbacks
 * @param	fun_cmp - pointer to compare function \a MGListDataCmpFun. If NULL - memcmp will be used.
 * @param	fun_del - pointer to remove function \a MGListDataDelFun. If NULL - no additional actions on remove.
 * @param	fun_cpy - pointer to copy function \a MGListDataCopyFun. If NULL - memcpy will be used.
 * @return	list object on success, or NULL on failure.
 */
MGList* mg_list_create(MGListDataCmpFun fun_cmp, MGListDataDelFun fun_del, MGListDataCopyFun fun_cpy);

/**
 * @brief	Change current callback in lst to fun.
 * If fun is NULL, default callback will be set.
 * @param	lst - pointer to list object
 * @param	fun - pointer to callback \a MGListDataCmpFun
 */
void mg_list_set_cmp_cb(MGList* lst, MGListDataCmpFun fun);

/**
 * @brief	Change current callback in lst to fun.
 * If fun is NULL, default callback will be set.
 * @param	lst - pointer to list object
 * @param	fun - pointer to callback \a MGListDataDelFun
 */
void mg_list_set_del_cb(MGList* lst, MGListDataDelFun fun);

/**
 * @brief	Change current callback in lst to fun.
 * If fun is NULL, default callback will be set.
 * @param	lst - pointer to list object
 * @param	fun - pointer to callback \a MGListDataCopyFun
 */
void mg_list_set_cpy_cb(MGList* lst, MGListDataCopyFun fun);
/**
 * @brief	Destroy list and all non-const data
 * @param	lst - pointer to list object
 */
void mg_list_destroy(MGList* lst);

/**
 * @brief	Allocates new node and add it to list.
 * @details Allocates data for node. Data in node filled with copy callback (default memcpy).
 * @param	lst - pointer to list object
 * @param	data - pointer to data
 * @param	len - data len
 * @return	true on success, false on failure
 */
bool mg_list_add(MGList *lst, void* data, size_t len);

/**
 * @brief	Detach node from list.
 * @details If node is first - change first to next.
 * @param	lst - pointer to list object
 * @param	node - pointer to node object
 * @return	true if node found in list, and was detached
 *			false on failure
 */
bool mg_list_detach(MGList *lst, MGListNode *node);

/**
 * @brief	Destroy de-attached allocated node.
 * @param	node - pointer to node object
 * @param	fun - pointer to delete function for node data. Nothing will be done if NULL.
 * @return	true if node found in list, and was detached
 *			false on failure
 */
bool mg_list_node_destroy(MGListNode *node, MGListDataDelFun fun);

/**
 * @brief	Remove node from list.
 * @details detach node object and destroy it after that.
 * @param	lst - pointer to list object
 * @param	node - pointer to node.
 */
void mg_list_remove(MGList* lst, MGListNode* node);


/**
 * @brief	Return pointer on node's const data
 * @param	node - pointer to node object
 * @return  pointer to const data
 */
const void* mg_list_data_const(MGListNode* node);

/**
 * @brief	Return pointer on node's data
 * @param	node - pointer to node object
 * @return  pointer to data
 */
void* mg_list_data(MGListNode* node);

/**
 * @brief	Return size of data in node.
 * @param	node - pointer to node object
 * @return	length of data
 */
size_t mg_list_data_len(MGListNode* node);

/**
 * @brief	Copy data from node to data with callback from list object.
 * @details Buffer for data allocated by MGList.
 * @param	lst - pointer to list object
 * @param	node - pointer to node object
 * @return	true on success, false on failure
 */
void* mg_list_data_copy(MGList* lst, MGListNode* node);

/**
 * @brief	Return count of nodes in list.
 * @param	lst - pointer to list object
 * @return	Count of node objects
 */
size_t mg_list_count(MGList* lst);


/**
 * @brief	Check if list contains data.
 * @param	lst - pointer to list object
 * @param	data - pointer to data object
 * @param	len - size of data object
 * @return	true if contains, false - not.
 */
bool mg_list_contains(MGList* lst, void* data, size_t len);

/**
 * @brief	Return next node.
 * @param	node - pointer to node object
 * @return	pointer to next node or NULL
 */
MGListNode* mg_list_node_next(MGListNode* node);

/**
 * @brief	Return first node of list.
 * @param	lst - pointer to list object
 * @return	pointer to the first node or NULL
 */
MGListNode* mg_list_node_first(MGList* lst);

/**
 * @brief	Return node which passed fun_cmp
 *
 * @param	lst - pointer to list object
 * @param	data - pointer to data
 * @param	len - length of the data
 * @return	pointer to node or NULL
 */
MGListNode* mg_list_search(MGList* lst, void* data, size_t len);

/**
 * @brief	Return node which passed fun_cmp
 *
 * @param	lst - pointer to list object
 * @param	data - pointer to data
 * @param	len - length of the data
 * @param	fun_cmp - pointer to compare function \a MGListDataCmpFun.
 *						If NULL - default will be used.
 * @return	pointer to node or NULL
 */
MGListNode* mg_list_search2(MGList* lst, void* data, size_t len, MGListDataCmpFun fun_cmp);

/**
 * @brief	Copy data from one list to another one
 * @param	src - source list
 * @param	dst - destination list
 * @return  true if everything is ok
 */
bool mg_list_copy(MGList* src, MGList* dst);

/**
 * @brief	Clone source list (params and data).
 * @param	src - source list
 * @return	pointer to cloned list or NULL on failure
 */
MGList* mg_list_clone(MGList* src);
