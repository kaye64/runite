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

#ifndef _FILE_H_
#define _FILE_H_

#include <stdlib.h>
#include <stdbool.h>

typedef struct file file_t;
struct file {
	size_t length;
	unsigned char* data;
};

bool file_read(file_t* file, const char* path);
void file_path_join(char* path_a, char* path_b, char* out);

#endif /* _FILE_H_ */
