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

#include <runite/hash.h>

#define MULTIPLIER 61

/**
 * Implements the hashing function used by Jagex archive containers
 * This is just a variant of the function given in Brian W. Kernighan's
 * book 'The Practice of Programming'.
 */
uint32_t jagex_hash(char* string)
{
	uint32_t h = 0;
	for (uint8_t* p = (uint8_t*)string; *p != '\0'; p++) {
		h = (MULTIPLIER * h + *p) - 32;
	}
	return h;
}
