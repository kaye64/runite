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

#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <stdbool.h>

#include <runite/util/object.h>
#include <runite/util/container_of.h>
#include <runite/util/list.h>

typedef struct queue queue_t;

struct queue {
	object_t object;
	list_t list;
};

extern object_proto_t queue_proto;

void queue_push(queue_t* queue, list_node_t* item);
list_node_t* queue_pop(queue_t* queue);
list_node_t* queue_peek(queue_t* queue);
bool queue_empty(queue_t* queue);
void queue_clear(queue_t* queue);

#endif /* _QUEUE_H_ */
