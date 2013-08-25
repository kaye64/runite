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

#ifndef _LIST_H_
#define _LIST_H_

#include <stdbool.h>
#include <stdlib.h>

#include <runite/util/object.h>
#include <runite/util/container_of.h>

typedef struct list_node list_node_t;
typedef struct list list_t;

struct list_node {
	list_node_t* prev;
	list_node_t* next;
};

struct list {
	object_t object;
	list_node_t* top;
	list_node_t* bottom;
};

extern object_proto_t list_proto;

bool list_empty(list_t* list);

void list_push_back(list_t* list, list_node_t* node);
void list_push_front(list_t* list, list_node_t* node);
void list_insert_before(list_t* list, list_node_t* other, list_node_t* node);
void list_insert_after(list_t* list, list_node_t* other, list_node_t* node);


list_node_t* list_back(list_t* list);
list_node_t* list_front(list_t* list);
int list_count(list_t* list);

void list_erase(list_t* list, list_node_t* node);

#define list_for_each(list)										\
	for (list_node_t* node_iter = list_front(list); node_iter != NULL; node_iter = node_iter->next)

#define list_for_get(item) item = container_of(node_iter, typeof(*item), node)

#endif /* _LIST_H_ */
