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

#ifndef _CACHE_H_
#define _CACHE_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include <runite/util/object.h>
#include <runite/file.h>

typedef struct cache cache_t;

struct cache {
	object_t object;
	int num_indices;
	int* num_files;
	bool must_free;
	file_t** files;
};

extern object_proto_t cache_proto;

int cache_open_fs_dir(cache_t* cache, const char* directory);
void cache_open_fs(cache_t* cache, int num_indices, const char** index_files, const char* data_file);

file_t* cache_get_file(cache_t* cache, int index, int file);
void cache_gen_crc(cache_t* cache, int index, file_t* file);

#endif /* _CACHE_H_ */
