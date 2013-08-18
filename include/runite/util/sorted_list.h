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

#ifndef _SORTED_LIST_H_
#define _SORTED_LIST_H_

#include <runite/util/list.h>

typedef struct sorted_list sorted_list_t;
typedef int(*compare_func_t)(list_node_t*,list_node_t*);

struct sorted_list {
	object_t object;
	list_t list;
	compare_func_t compare_func;
};

extern object_proto_t sorted_list_proto;

void sorted_list_insert(sorted_list_t* list, list_node_t* node);

#endif /* _SORTED_LIST_H_ */
