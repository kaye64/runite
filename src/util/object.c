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
 * object.c
 *
 * Defines our object system
 */
#include <runite/util/object.h>

/**
 * Allocates and initializes an object
 */
object_t* _object_new(object_proto_t proto, size_t size)
{
	object_t* object = (object_t*)malloc(size);
	_object_init(proto, object);
	object->must_free = true;
	return object;
}

/**
 * Initializes an object
 */
void _object_init(object_proto_t proto, object_t* object)
{
	proto.init(object);
	object->prototype.init = proto.init;
	object->prototype.free = proto.free;
	object->must_free = false;
}

/**
 * Frees an object
 */
void _object_free(object_t* object)
{
	object->prototype.free(object);
	if (object->must_free) {
		free(object);
	}
}
