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
 * stack.c
 *
 * Defines a simple integer stack
 */
#include <runite/util/stack.h>

#include <stdlib.h>

/**
 * Initializes a stack
 */
static void stack_init(int_stack_t* stack)
{
	stack->bottom = 0;
}

/**
 * Properly frees a stack
 */
static void stack_free(int_stack_t* stack)
{

}

/**
 * Pushes an item i to a stack
 * returns: Whether the push was successful
 */
bool stack_push(int_stack_t* stack, int i)
{
	if (stack->bottom < 32) {
		stack->stack[stack->bottom++] = i;
		return true;
	}
	return false;
}

/**
 * Pops an item from a stack
 */
int stack_pop(int_stack_t* stack)
{
	if (stack->bottom > 0) {
		return stack->stack[--stack->bottom];
	}
	return 0;
}

object_proto_t int_stack_proto = {
	.init = (object_init_t)stack_init,
	.free = (object_free_t)stack_free
};
