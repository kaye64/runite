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
 * sorted_list.c
 *
 * A sorted list structure
 */

#include <runite/util/sorted_list.h>

/**
 * Creates a new sorted_list_t
 */
static void sorted_list_init(sorted_list_t* list)
{
	object_init(list, &list->list);
}

/**
 * Properly frees a sorted_list_t
 */
static void sorted_list_free(sorted_list_t* list)
{
	object_free(&list->list);
}

/**
 * Inserts a node to a sorted list
 */
void sorted_list_insert(sorted_list_t* list, list_node_t* node)
{
	if (list_empty(&list->list)) {
		list_push_front(&list->list, node);
		return;
	}

	list_node_t* other_node = list_front(&list->list);	
	while (other_node != NULL) {
		int cmp_result = list->compare_func(node, other_node);
		if (cmp_result == 0) {
			list_insert_after(&list->list, other_node, node);
			return;
		}
	
		if (cmp_result < 0) {
			if (other_node->prev == NULL) {
				list_insert_before(&list->list, other_node, node);
				return;
			}
			cmp_result = list->compare_func(node, other_node->prev);
			if (cmp_result > 0) {
				list_insert_before(&list->list, other_node, node);
				return;
			}
			other_node = other_node->prev;
			continue;
		}
	
		if (cmp_result > 0) {
			if (other_node->next == NULL) {
				list_insert_after(&list->list, other_node, node);
				return;
			}
			cmp_result = list->compare_func(node, other_node->next);
			if (cmp_result < 0) {
				list_insert_after(&list->list, other_node, node);
				return;
			}
			other_node = other_node->next;
			continue;
		}
	}
}

object_proto_t sorted_list_proto = {
	.init = (object_init_t)sorted_list_init,
	.free = (object_free_t)sorted_list_free
};










