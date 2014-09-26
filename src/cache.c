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

#include <runite/cache.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <netinet/in.h>
#include <zlib.h>

#include <runite/util/sorted_list.h>
#include <runite/util/container_of.h>
#include <runite/util/codec.h>

#define DATA_BLOCK_SIZE 520
#define INDEX_ENTRY_SIZE 6

static void cache_fs_get(codec_t* data_indices, codec_t* data_blocks, int index_id, int file_id, file_t* cache_file);

typedef struct index_list_node index_list_node_t;
struct index_list_node {
	list_node_t node;
	char index[256];
};

/**
 * Initializes a new cache_t
 */
static void cache_init(cache_t* cache)
{
	cache->num_files = 0;
	cache->files = 0;
}

/**
 * Cleans up a cache_t
 */
static void cache_free(cache_t* cache)
{
	if (cache->num_files != 0) {
		free(cache->num_files);
	}
	if (cache->files != 0) {
		for (int i = 0; i < cache->num_indices; i++) {
			for (int x = 0; x < cache->num_files[i]; x++) {
				file_t* file = &cache->files[i][x];
				if (file->data != NULL) {
					free(file->data);
				}
			}
			free(cache->files[i]);
		}
		free(cache->files);
	}
}

/**
 * A wrapper around strcmp that operates on 'list_node_t's
 */
static int strcmp_wrap(list_node_t* a, list_node_t* b) {
	index_list_node_t* nodeA = container_of(a, index_list_node_t, node);
	index_list_node_t* nodeB = container_of(b, index_list_node_t, node);
	return strcmp((const char*)nodeA->index, (const char*)nodeB->index);
}

/**
 * Opens a directory in cache fs form (ie. client cached index + data files)
 */
int cache_open_fs_dir(cache_t* cache, const char* directory)
{
	DIR *dir = opendir(directory);
	struct dirent *entry;
	int num_indices = 0;

	sorted_list_t* index_list = object_new(sorted_list);
	index_list->compare_func = strcmp_wrap;
	if (dir == NULL) {
		return 1;
	}

	char data_file[256];
	while ((entry = readdir(dir)) != NULL) {
		if (strstr(entry->d_name, "idx")) {
			index_list_node_t* node = (index_list_node_t*)malloc(sizeof(index_list_node_t));
			strcpy(node->index, entry->d_name);
			sorted_list_insert(index_list, &node->node);
			num_indices++;
		} else if (strstr(entry->d_name, "dat")) {
			sprintf(data_file, "%s/%s", directory, entry->d_name);
		}
	}

	if (data_file == NULL || num_indices == 0) {
		return 1;
	}

	char** index_files = (char**)malloc(sizeof(char*)*num_indices);
	int i = 0;
	while (!list_empty(&index_list->list)) {
		list_node_t* node = list_front(&index_list->list);
		index_list_node_t* index_node = container_of(node, index_list_node_t, node);
		index_files[i] = (char*)malloc(sizeof(char)*256);
		sprintf(index_files[i++], "%s/%s", directory, (char*)index_node->index);
		list_erase(&index_list->list, node);
		free(index_node);
	}
	object_free(index_list);
	closedir(dir);

	cache_open_fs(cache, num_indices, (const char**)index_files, data_file);

	for (int i = 0; i < num_indices; i++) {
		free(index_files[i]);
	}
	free(index_files);

	return 0;
}

/**
 * Opens a cache fs from memory (ie. client cached index + data files)
 */
void cache_open_fs(cache_t* cache, int num_indices, const char** index_files, const char* data_file)
{
	cache->num_indices = num_indices;

	codec_t* data_indices;
	codec_t data_blocks;
	int num_blocks;

	/* Read the data file into memory */
	FILE *data_fd = fopen(data_file, "r");
	fseek(data_fd, 0, SEEK_END);
	int data_size = ftell(data_fd);

	object_init(codec, &data_blocks);
	codec_resize(&data_blocks, data_size);
	num_blocks = data_size / DATA_BLOCK_SIZE;

	fseek(data_fd, 0, SEEK_SET);
	fread(data_blocks.data, DATA_BLOCK_SIZE, num_blocks, data_fd);
	fclose(data_fd);

	/* Read the indices into memory */
	cache->num_files = (int*)calloc(sizeof(int), num_indices);
	cache->files = (file_t**)malloc(sizeof(file_t*)*num_indices);
	data_indices = (codec_t*)malloc(sizeof(codec_t)*num_indices);
	for (int i = 0; i < num_indices; i++) {
		FILE* index_fd = fopen(index_files[i], "r");
		fseek(index_fd, 0, SEEK_END);
		int index_size = ftell(index_fd);

		object_init(codec, &data_indices[i]);
		codec_resize(&data_indices[i], index_size);
		cache->num_files[i] = index_size / INDEX_ENTRY_SIZE;
		cache->files[i] = (file_t*)malloc(sizeof(file_t)*cache->num_files[i]);

		fseek(index_fd, 0, SEEK_SET);
		fread(data_indices[i].data, INDEX_ENTRY_SIZE, cache->num_files[i], index_fd);
		fclose(index_fd);

		for (int x = 0; x < cache->num_files[i]; x++) {
			cache_fs_get(&data_indices[i], &data_blocks, i, x, &cache->files[i][x]);
		}

		object_free(&data_indices[i]);
	}

	object_free(&data_blocks);
}

/**
 * Generates a checksum file for a given index and stores it in a file_t
 *  - file: Where to store the checksum file
 */
void cache_gen_crc(cache_t* cache, int index, file_t* file)
{
	int num_files = cache->num_files[index];
	size_t num_crcs = (num_files+1);
	size_t buf_len = num_crcs*4;
	uint32_t crc_buf[num_crcs];
	/* calculate the crc table */
	crc_buf[num_files] = 1234;
	for (int i = 0; i < num_files; i++) {
		file_t* file = cache_get_file(cache, index, i);
		crc_buf[i] = crc32(0L, Z_NULL, 0);
		crc_buf[i] = crc32(crc_buf[i], (const unsigned char*)file->data, file->length);
		crc_buf[num_files] = (crc_buf[num_files] << 1) + crc_buf[i];
		crc_buf[i] = htonl(crc_buf[i]);
	}
	crc_buf[num_files] = htonl(crc_buf[num_files]);
	file->data = (unsigned char*)malloc(buf_len);
	memcpy(file->data, (char*)&crc_buf, buf_len);
	file->length = buf_len;
}

/**
 * Accesses a file within the cache
 */
file_t* cache_get_file(cache_t* cache, int index, int file)
{
	if (index > cache->num_indices || file > cache->num_files[index]) {
		return NULL;
	}
	return &cache->files[index][file];
}

/**
 * Extracts the cached file from a cache fs
 */
static void cache_fs_get(codec_t* data_indices, codec_t* data_blocks, int index_id, int file_id, file_t* cache_file)
{
	int num_files = data_indices->length/INDEX_ENTRY_SIZE;
	if (file_id < 0 || file_id > num_files) {
		goto error;
	}

	codec_seek(data_indices, file_id*INDEX_ENTRY_SIZE);

	cache_file->length = codec_get24fp(data_indices, NULL, 0);
	int current_block = codec_get24fp(data_indices, NULL, 0);
	int write_caret = 0;
	int to_read = cache_file->length;
	int file_part = 0;
	int num_blocks = data_blocks->length/DATA_BLOCK_SIZE;

	cache_file->data = (unsigned char*)malloc(cache_file->length);

	while (current_block != 0) {
		if (current_block <= 0 || current_block > num_blocks) {
			free(cache_file->data);
			return;
		}
		codec_seek(data_blocks, current_block*DATA_BLOCK_SIZE);

		int block_file_id = codec_get16(data_blocks);
		int block_file_pos = codec_get16(data_blocks);
		int next_block = codec_get24(data_blocks);
		int block_cache_id = codec_get8(data_blocks);

		int read_this_block = to_read;
		if (read_this_block > 512) {
			read_this_block = 512;
		}
		if (block_file_id != file_id || block_file_pos != file_part || block_cache_id-1 != index_id) {
			free(cache_file->data);
			goto error;
		}
		codec_getn(data_blocks, cache_file->data+(write_caret), read_this_block);

		write_caret += read_this_block;
		to_read -= read_this_block;
		current_block = next_block;
		file_part++;
	}
	if (to_read > 0) {
		free(cache_file->data);
		goto error;
	}
	goto exit;
error:
	cache_file->length = 0;
	cache_file->data = NULL;
exit:
	return;
}

object_proto_t cache_proto = {
	.init = (object_init_t)cache_init,
	.free = (object_free_t)cache_free
};
