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

#include <string.h>
#include <stdio.h>
#include <sys/stat.h>

/**
 * Read a file from disk into a file_t
 */
bool file_read(file_t* file, const char* path)
{
	struct stat fstat;
	if (stat(path, &fstat) != 0) {
		return false;
	}
	file->data = (unsigned char*)malloc(fstat.st_size);
	file->length = fstat.st_size;
	FILE* fd = fopen(path, "r");
	if (!fd) {
		free(file->data);
		return false;
	}
	if (fread(file->data, 1, file->length, fd) != file->length) {
		fclose(fd);
		free(file->data);
		return false;
	}
	fclose(fd);
	return true;
}

/**
 * Write a file_t to disk
 */
bool file_write(file_t* file, const char* path)
{
	FILE* fd = fopen(path, "w+");
	if (!fd) {
		return false;
	}
	if (fwrite(file->data, 1, file->length, fd) != file->length) {
		fclose(fd);
		return false;
	}
	fclose(fd);
	return true;
}

/**
 * Cleanly joins two file paths together
 */
void file_path_join(char* path_a, char* path_b, char* out)
{
	sprintf(out, "%s/%s", path_a, path_b);
	/* remove any duplicate separators */
	for (char* p = out; *p != '\0'; p++) {
		if (*p == '/' && *(p+1) == '/') {
			strcpy(p, p+1);
		}
	}
}
