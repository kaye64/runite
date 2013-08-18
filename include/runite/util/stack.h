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

#ifndef _STACK_H_
#define _STACK_H_

#include <stdbool.h>

#include <runite/util/object.h>

typedef struct int_stack int_stack_t;

struct int_stack {
	object_t object;
	int stack[32];
	int bottom;
};

extern object_proto_t int_stack_proto;

bool stack_push(int_stack_t* _stack, int i);
int stack_pop(int_stack_t* _stack);

#endif /* _STACK_H_ */
