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

#ifndef _ARCHIVE_H_
#define _ARCHIVE_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include <runite/hash.h>
#include <runite/file.h>
#include <runite/util/object.h>
#include <runite/util/list.h>

typedef struct archive archive_t;
typedef struct archive_file archive_file_t;

struct archive {
	object_t object;
	uint16_t num_files;
	list_t files;
};

struct archive_file {
	file_t file;
	jhash_t identifier;
	list_node_t node;
};

extern object_proto_t archive_proto;

#define ARCHIVE_COMPRESS_FILE 0
#define ARCHIVE_COMPRESS_WHOLE 1

bool archive_decompress(archive_t* archive, file_t* data);
bool archive_compress(archive_t* archive, file_t* out_file, uint8_t scheme);

archive_file_t* archive_add_file(archive_t* archive, jhash_t identifier, file_t* file);
void archive_remove_file(archive_t* archive, archive_file_t* file);
archive_file_t* archive_get_file(archive_t* archive, jhash_t identifier);

#endif /* _ARCHIVE_H_ */
