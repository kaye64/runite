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

#include <runite/archive.h>

#include <string.h>
#include <bzlib.h>

#include <runite/util/math.h>
#include <runite/util/codec.h>

#define BZ2_VERBOSITY 0 /* 0 = silent, 1-4 = verbose */ 
#define BZ2_WORK_FACTOR 30
#define BZ2_BUFFER_SIZE 1024*10 

/**
 * Initializes a new archive_t
 */
static void archive_init(archive_t* archive)
{
	object_init(list, &archive->files);
	archive->num_files = 0;
}

/**
 * Cleans up an archive_t
 */
static void archive_free(archive_t* archive)
{
	object_free(&archive->files);
}

/**
 * Compresses to a headerless bz2 block
 * Assumes 100k block size
 */
static bool bz2_headerless_compress(unsigned char* src, uint32_t src_len, unsigned char* dest, uint32_t dest_len)
{
	/* init bzlib */
    bz_stream stream;
	stream.bzalloc = NULL; 
	stream.bzfree = NULL;
	stream.opaque = NULL;
	int ret = BZ2_bzCompressInit(&stream, 1, BZ2_VERBOSITY, BZ2_WORK_FACTOR);
	if (ret != BZ_OK) {
		return false;
	}


	return true;
}

/**
 * Decompresses a headerless bz2 block
 * Assumes 100k block size
 *  - dest_len: The size of the dest buffer. Contains the amount of data decompressed on successful return
 */
static bool bz2_headerless_decompress(unsigned char* src, uint32_t src_len, unsigned char* dest, uint32_t* dest_len)
{
	/* init bzlib */
	bz_stream stream;
	stream.bzalloc = NULL; 
	stream.bzfree = NULL;
	stream.opaque = NULL;
	int ret = BZ2_bzDecompressInit(&stream, BZ2_VERBOSITY, 0);
	if (ret != BZ_OK) {
		return false;
	}

	unsigned char* bz2_data = (unsigned char*)malloc(src_len+4);
	/* tack the header on */
	bz2_data[0] = 'B';
	bz2_data[1] = 'Z';
	bz2_data[2] = 'h';
	bz2_data[3] = '1';
	memcpy(bz2_data+4, src, src_len);

	/* do the decompress */
	stream.next_in = (char*)bz2_data;
	stream.avail_in = src_len+4;
	stream.next_out = (char*)dest;
	stream.avail_out = *dest_len;

	ret = BZ2_bzDecompress(&stream);
	if (ret != BZ_OK && ret != BZ_STREAM_END) {
		goto error;
	}

	/* finish up */
	*dest_len = stream.total_out_lo32;
	
	goto success;
error:
	free(bz2_data);
	BZ2_bzDecompressEnd(&stream);
	return false;
success:
	free(bz2_data);
	BZ2_bzDecompressEnd(&stream);
	return true;
}

/**
 * Decompresses an archive and loads the contents into memory
 */
bool archive_decompress(archive_t* archive, file_t* data)
{
	codec_t* arc_codec = object_new(codec);
	codec_resize(arc_codec, data->length);
	codec_putn(arc_codec, data->data, data->length);
	codec_seek(arc_codec, 0);
	
	uint32_t final_len = codec_get24(arc_codec);
	uint32_t container_len = codec_get24(arc_codec);
	bool compressed = true;
	
	if (container_len != final_len) { /* The entire container is compressed */
		compressed = false;

		/* decompress it */
		uint32_t decompressed_len = final_len;
		codec_t* new_codec = object_new(codec);
		codec_resize(new_codec, final_len);
		bool success = bz2_headerless_decompress(data->data, data->length, new_codec->data, &decompressed_len);
		if (!success || decompressed_len != final_len) {
			object_free(new_codec);
			printf("bz decompress (whole) failed\n");
			goto error;
		}

		object_free(arc_codec);
		arc_codec = new_codec;
	}

	int num_files = codec_get16(arc_codec);
	int file_ofs = arc_codec->caret + (num_files * 10);
	for (int i = 0; i < num_files; i++) {
		/* gather file metadata */
		archive_file_t* file = (archive_file_t*)malloc(sizeof(archive_file_t));
		file->identifier = codec_get32(arc_codec);
		uint32_t final_file_len = codec_get24(arc_codec);
		uint32_t actual_file_len = codec_get24(arc_codec);
		if (!compressed && final_file_len != actual_file_len) {
			free(file);
			printf("file size mismatch: %i %i\n", final_file_len, actual_file_len);
			goto error;
		}
		file->file.length = final_file_len;
		file->file.data = (unsigned char*)malloc(final_file_len);

		/* locate file data */
		if (compressed) {
			uint32_t decompressed_len = final_file_len;
			bool success = bz2_headerless_decompress(data->data+file_ofs, actual_file_len, file->file.data, &decompressed_len);
			if (!success) {
				free(file->file.data);
				free(file);
				printf("bz decompress (file) failed\n");
				goto error;
			}
		} else {
			memcpy(file->file.data, data->data+file_ofs, final_file_len);
		}

		/* add it to our list */
		list_push_back(&archive->files, &file->node);
		archive->num_files++;
		file_ofs += actual_file_len;
	}

	goto success;
error:
	object_free(arc_codec);
	return false;
success:
	object_free(arc_codec);
	return true;
}

/**
 * Compresses an archive
 *  - out_file: a file_t to store the output in
 *  - scheme: one of ARCHIVE_COMPRESS_{FILE,WHOLE,OPTIMIZE}
 */
bool archive_compress(archive_t* archive, file_t* out_file, uint8_t scheme)
{
	return true;
}

/**
 * Adds a file_t to the archive with a given identifier
 * returns: The corresponding archive_file_t
 */
archive_file_t* archive_add_file(archive_t* archive, jhash_t identifier, file_t* file)
{
	/* check for collision */
	if (archive_get_file(archive, identifier) != NULL) {
		return NULL;
	}

	/* create the structures */
	archive_file_t* archive_file = (archive_file_t*)malloc(sizeof(archive_file_t));
	archive_file->identifier = identifier;
	archive_file->file.length = file->length;
	archive_file->file.data = (unsigned char*)malloc(file->length);
	memcpy(archive_file->file.data, file->data, file->length);
	
	/* add it */
	list_push_back(&archive->files, &archive_file->node);
	archive->num_files++;
}

/**
 * Removes an archive_file_t from the archive
 */
void archive_remove_file(archive_t* archive, archive_file_t* file)
{
	/* check it exists */
	if (archive_get_file(archive, file->identifier) == NULL) {
		return;
	}

	/* remove it */
	list_erase(&archive->files, &file->node);
	archive->num_files--;
	free(file->file.data);
	free(file);
}

/**
 * Locates an archive_file_t in an archive by identifier
 */
archive_file_t* archive_get_file(archive_t* archive, jhash_t identifier)
{
	archive_file_t* file;
	list_for_each(&archive->files) {
		list_for_get(file);
		if (file->identifier == identifier) {
			return file;
		}
	}
	return NULL;
}

object_proto_t archive_proto = {
	.init = (object_init_t)archive_init,
	.free = (object_free_t)archive_free
};
