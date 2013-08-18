/**
 *  This file is part of Gem.
 *
 *  Gem is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Gem is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Gem.  If not, see <http://www.gnu.org/licenses/\>.
 */

/**
 * list.c
 *
 * A simple list structure
 */

#include <runite/util/list.h>

/**
 * Creates a new list_t
 */
static void list_init(list_t* list)
{
	list->top = list->bottom = (list_node_t*)NULL;
}

/**
 * Properly frees a list_t
 */
static void list_free(list_t* list)
{

}

/**
 * Checks whether a list is empty or not
 */
bool list_empty(list_t* list)
{
	return list->top == (list_node_t*)NULL && list->bottom == (list_node_t*)NULL;
}

/**
 * Puts a node to the back of the list
 */
void list_push_back(list_t* list, list_node_t* node)
{
	if (list_empty(list)) {
		list->top = list->bottom = node;
		node->next = node->prev = (list_node_t*)NULL;
	} else {
		list_insert_after(list, list->bottom, node);
	}
}

/**
 * Puts a node to the front of the list
 */
void list_push_front(list_t* list, list_node_t* node)
{
	if (list_empty(list)) {
		list->top = list->bottom = node;
		node->next = node->prev = (list_node_t*)NULL;
	} else {
		list_insert_before(list, list->top, node);
	}
}

/**
 * Inserts one node before another
 *  - other: The node to insert before
 *  - node: The node to insert
 */
void list_insert_before(list_t* list, list_node_t* other, list_node_t* node)
{
	list_node_t* prev = other->prev;
	other->prev = node;
	if (prev != (list_node_t*)NULL) {
		prev->next = node;
	}
	node->prev = prev;
	node->next = other;
	if (other == list->top) {
		list->top = node;
	}
}

/**
 * Inserts one node after another
 *  - other: The node to insert after
 *  - node: The node to insert
 */
void list_insert_after(list_t* list, list_node_t* other, list_node_t* node)
{
	list_node_t* next = other->next;
	other->next = node;
	if (next != (list_node_t*)NULL) {
		next->prev = node;
	}
	node->prev = other;
	node->next = next;
	if (other == list->bottom) {
		list->bottom = node;
	}
}

/**
 * Returns the node at the back of the list
 */
list_node_t* list_back(list_t* list)
{
	return list->bottom;
}

/**
 * Returns the node at the front of the list
 */
list_node_t* list_front(list_t* list)
{
	return list->top;
}

/**
 * Returns the count of the elements in the list
 */
int list_count(list_t* list)
{
	list_node_t* node = list_front(list);
	int count = 0;
	while (node != 0) {
		count++;
		node = node->next;
	}
	return count;
}

/**
 * Erases a node from the list
 */
void list_erase(list_t* list, list_node_t* node)
{
	list_node_t* prev = node->prev;
	list_node_t* next = node->next;
	if (node == list->top) {
		list->top = node->next;
	}
	if (node == list->bottom) {
		list->bottom = node->prev;
	}
	if (prev != NULL) {
		prev->next = next;
	}
	if (next != NULL) {
		next->prev = prev;
	}
	node->prev = node->next = (list_node_t*)NULL;
}

object_proto_t list_proto = {
	.init = (object_init_t)list_init,
	.free = (object_free_t)list_free
};
