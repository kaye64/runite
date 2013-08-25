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

#include <runite/file.h>

#include <stdio.h>
#include <sys/stat.h>

bool file_read(file_t* file, const char* path)
{
	struct stat fstat;
	if (stat(path, &fstat) != 0) {
		return false;
	}
	file->data = (unsigned char*)malloc(fstat.st_size);
	file->length = fstat.st_size;
	FILE* fd = fopen(path, "r");
	if (fread(file->data, 1, file->length, fd) != file->length) {
		fclose(fd);
		free(file->data);
		return false;
	}
	fclose(fd);
	return true;
}
