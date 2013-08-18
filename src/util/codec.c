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
 * codec.c
 *
 * Defines a protocol codec for parsing data streams
 * Assumes little endian native order.
 */

#include <runite/util/codec.h>

#include <assert.h>

/**
 * Initializes a new codec
 */
static void codec_init(codec_t* codec)
{
	codec->data = (unsigned char*)malloc(DEFAULT_BUFFER_SIZE);
	memset(codec->data, 0, DEFAULT_BUFFER_SIZE);
	codec->length = DEFAULT_BUFFER_SIZE;
	codec->caret = 0;
	codec->bit_access_mode = false;
}

/**
 * Properly frees a codec_t
 */
static void codec_free(codec_t* codec)
{
	free(codec->data);
}

/**
 * Resizes a codec_t
 * All data is lost upon resize, buffer is zero-initialized
 */
void codec_resize(codec_t* codec, size_t size)
{
	free(codec->data);
	codec->data = (unsigned char*)malloc(size);
	codec->length = size;
	memset(codec->data, 0, codec->length);
}

/**
 * Seek to a given position in the codec
 */
void codec_seek(codec_t* codec, size_t caret)
{
	if (caret > codec->length) {
		return;
	}
	codec->caret = caret;
}

/**
 * Returns the amount of valid data in the codec
 */
size_t codec_len(codec_t* codec)
{
	// We could do with a more reliable way to keep track of
	// the valid data in the buffer.
	return codec->caret;
}

/**
 * Enables or disables bit access mode on a codec
 *  - bit_mode: Whether to enable or disable bit access mode
 */
void codec_set_bit_access_mode(codec_t* codec, bool bit_mode)
{
	if (codec->bit_access_mode == bit_mode) {
		// nothing to do
		return;
	}
	if (bit_mode) {
		codec->bit_caret = 7;
	} else {
		if (codec->bit_caret != 7) {
			codec->caret++;
		}
	}
	codec->bit_access_mode = bit_mode;
}

/**
 * Puts a given number of bits to the codec
 *  - nbits: The number of bits to put
 *  - i: The value to put
 */
void codec_put_bits(codec_t* codec, int nbits, uint32_t i)
{
	assert(codec->bit_access_mode);
	for (int n = nbits-1; n >= 0; n--) {
		uint8_t bit_val = (i & (1 << n)) >> n;
		if (bit_val) {
			codec->data[codec->caret] |= (1 << codec->bit_caret);
		} else {
			codec->data[codec->caret] &= ~(1 << codec->bit_caret);
		}
		if (codec->bit_caret == 0) {
			codec->bit_caret = 7;
			codec->caret++;
		} else {
			codec->bit_caret--;
		}
	}
}

/**
 * Gets a given number of bits from the codec
 *  - nbits: The number of bits to get
 * returns: The value
 */
uint32_t codec_get_bits(codec_t* codec, int nbits)
{
	/* todo */
	return 0;
}

/**
 * Puts 8 bits to the codec
 */
void codec_put8(codec_t* codec, uint8_t i)
{
	codec_put8f(codec, i, 0);
}

/**
 * Puts 16 bits to the codec
 */
void codec_put16(codec_t* codec, uint16_t i)
{
	codec_put16f(codec, i, 0);
}

/**
 * Puts 24 bits to the codec
 */
void codec_put24(codec_t* codec, uint32_t i)
{
	codec_put24f(codec, i, 0);
}

/**
 * Puts 32 bits to the codec
 */
void codec_put32(codec_t* codec, uint32_t i)
{
	codec_put32f(codec, i, 0);
}

/**
 * Puts 64 bits to the codec
 */
void codec_put64(codec_t* codec, uint64_t i)
{
	codec_put64f(codec, i, 0);
}

/**
 * Puts a given number of bytes to the codec
 */
void codec_putn(codec_t* codec, unsigned char* data, size_t len)
{
	if (codec->caret+len > codec->length || len == 0) {
		return;
	}

	memcpy(&codec->data[codec->caret], data, len);
	codec->caret += len;
}

/**
 * Concatenates one codec onto another
 *  - codec: The destination codec
 *  - other: The source codec
 */
void codec_concat(codec_t* dest, codec_t* src)
{
	size_t src_len = codec_len(src);
	codec_putn(dest, src->data, src_len);
}

/**
 * Puts 8 bits to the codec
 *  - flags: Modifier flags
 */
void codec_put8f(codec_t* codec, uint8_t i, uint8_t flags)
{
	if (codec->caret+1 > codec->length) {
		return;
	}

	uint8_t* val = (uint8_t*)&i;
	if (flags & CODEC_NEGATIVE) {
		val[0] = -val[0];
	}
	if (flags & CODEC_INV128) {
		val[0] = 128 - val[0];
	}
	if (flags & CODEC_OFS128) {
		val[0] = val[0] + 128;
	}

	codec->data[codec->caret++] = val[0];
}

/**
 * Puts 16 bits to the codec
 *  - flags: Modifier flags
 */
void codec_put16f(codec_t* codec, uint16_t i, uint8_t flags)
{
	if (codec->caret+2 > codec->length) {
		return;
	}

	uint8_t* val = (uint8_t*)&i;
	if (flags & CODEC_NEGATIVE) {
		val[0] = -val[0];
	}
	if (flags & CODEC_INV128) {
		val[0] = 128 - val[0];
	}
	if (flags & CODEC_OFS128) {
		val[0] = val[0] + 128;
	}
	if (flags & CODEC_LITTLE) {
		uint16_t tmp = i;
		val[0] = ((unsigned char*)&tmp)[1];
		val[1] = ((unsigned char*)&tmp)[0];
	}

	codec->data[codec->caret++] = val[1];
	codec->data[codec->caret++] = val[0];
}

/**
 * codec_put24f
 * Puts 24 bits to the codec
 *  - flags: Modifier flags
 */
void codec_put24f(codec_t* codec, uint32_t i, uint8_t flags)
{
	if (codec->caret+3 > codec->length) {
		return;
	}

	uint8_t* val = (uint8_t*)&i;
	val[3] = 0;
	if (flags & CODEC_NEGATIVE) {
		val[0] = -val[0];
	}
	if (flags & CODEC_INV128) {
		val[0] = 128 - val[0];
	}
	if (flags & CODEC_OFS128) {
		val[0] = val[0] + 128;
	}
	if (flags & CODEC_LITTLE) {
		uint32_t tmp = i;
		val[0] = ((unsigned char*)&tmp)[2];
		val[1] = ((unsigned char*)&tmp)[1];
		val[2] = ((unsigned char*)&tmp)[0];
	}

	codec->data[codec->caret++] = val[2];
	codec->data[codec->caret++] = val[1];
	codec->data[codec->caret++] = val[0];
}

/**
 * Puts 32 bits to the codec
 *  - flags: Modifier flags
 */
void codec_put32f(codec_t* codec, uint32_t i, uint8_t flags)
{
	if (codec->caret+4 > codec->length) {
		return;
	}

	uint8_t* val = (uint8_t*)&i;
	if (flags & CODEC_NEGATIVE) {
		val[0] = -val[0];
	}
	if (flags & CODEC_INV128) {
		val[0] = 128 - val[0];
	}
	if (flags & CODEC_OFS128) {
		val[0] = val[0] + 128;
	}
	if (flags & CODEC_LITTLE) {
		uint32_t tmp = i;
		val[0] = ((unsigned char*)&tmp)[3];
		val[1] = ((unsigned char*)&tmp)[2];
		val[2] = ((unsigned char*)&tmp)[1];
		val[3] = ((unsigned char*)&tmp)[0];
	} else if (flags & CODEC_MIDDLE_A) {
		/* This is essentially PDP-endian */
		uint32_t tmp = i;
		val[0] = ((unsigned char*)&tmp)[1];
		val[1] = ((unsigned char*)&tmp)[0];
		val[2] = ((unsigned char*)&tmp)[3];
		val[3] = ((unsigned char*)&tmp)[2];
	} else if (flags & CODEC_MIDDLE_B) {
		/* The reverse of MIXED_A */
		uint32_t tmp = i;
		val[0] = ((unsigned char*)&tmp)[2];
		val[1] = ((unsigned char*)&tmp)[3];
		val[2] = ((unsigned char*)&tmp)[0];
		val[3] = ((unsigned char*)&tmp)[1];
	}

	codec->data[codec->caret++] = val[3];
	codec->data[codec->caret++] = val[2];
	codec->data[codec->caret++] = val[1];
	codec->data[codec->caret++] = val[0];
}

/**
 * Puts 64 bits to the codec
 *  - flags: Modifier flags
 */
void codec_put64f(codec_t* codec, uint64_t i, uint8_t flags)
{
	if (codec->caret+8 > codec->length) {
		return;
	}

	uint8_t* val = (uint8_t*)&i;
	if (flags & CODEC_NEGATIVE) {
		val[0] = -val[0];
	}
	if (flags & CODEC_INV128) {
		val[0] = 128 - val[0];
	}
	if (flags & CODEC_OFS128) {
		val[0] = val[0] + 128;
	}
	if (flags & CODEC_LITTLE) {
		uint64_t tmp = i;
		val[0] = ((unsigned char*)&tmp)[7];
		val[1] = ((unsigned char*)&tmp)[6];
		val[2] = ((unsigned char*)&tmp)[5];
		val[3] = ((unsigned char*)&tmp)[4];
		val[4] = ((unsigned char*)&tmp)[3];
		val[5] = ((unsigned char*)&tmp)[2];
		val[6] = ((unsigned char*)&tmp)[1];
		val[7] = ((unsigned char*)&tmp)[0];
	}

	codec->data[codec->caret++] = val[7];
	codec->data[codec->caret++] = val[6];
	codec->data[codec->caret++] = val[5];
	codec->data[codec->caret++] = val[4];
	codec->data[codec->caret++] = val[3];
	codec->data[codec->caret++] = val[2];
	codec->data[codec->caret++] = val[1];
	codec->data[codec->caret++] = val[0];
}

/**
 * Puts a string to the codec
 *  - flags: Can be CODEC_JSTRING for 0xA terminated string
 */
void codec_puts(codec_t* codec, char* s, int len, uint8_t flags)
{
	if (codec->caret+len > codec->length) {
		return;
	}

	codec_putn(codec, (unsigned char*)s, len);
	if (flags & CODEC_JSTRING) {
		codec_put8(codec, 10);
	} else {
		codec_put8(codec, 0);
	}
}

/**
 * Gets 8 bits from the codec
 */
uint8_t codec_get8(codec_t* codec)
{
	return codec_get8fp(codec, NULL, 0);
}

/**
 * Gets 16 bits from the codec
 */
uint16_t codec_get16(codec_t* codec)
{
	return codec_get16fp(codec, NULL, 0);
}

/**
 * Gets 24 bits from the codec
 */
uint32_t codec_get24(codec_t* codec)
{
	return codec_get24fp(codec, NULL, 0);
}

/**
 * Gets 32 bits from the codec
 */
uint32_t codec_get32(codec_t* codec)
{
	return codec_get32fp(codec, NULL, 0);
}


/**
 * Gets 64 bits from the codec
 */
uint64_t codec_get64(codec_t* codec)
{
	return codec_get64fp(codec, NULL, 0);
}

/**
 * Gets a given number of bytes from the codec. If data is NULL,
 * space is created on the heap. Caller is responsible for freeing
 * this memory.
 *  - data: A place to store the data, or NULL
 *  - len: The length of the data
 * returns: A pointer to the data read
 */
unsigned char* codec_getn(codec_t* codec, unsigned char* data, size_t len)
{
	if (codec->caret+len > codec->length || len == 0) {
		return 0;
	}

	if (data == NULL) {
		data = (unsigned char*)malloc(len); // Caller is responsible for freeing
	}

	memcpy(data, &codec->data[codec->caret], len);
	codec->caret += len;
	return data;
}

/**
 * Gets 8 bits from the codec
 *  - i: Location to store the value, or NULL
 *  - flags: Modifier flags
 */
uint8_t codec_get8fp(codec_t* codec, uint8_t* i, uint8_t flags)
{
	if (codec->caret+1 > codec->length) {
		return 0;
	}

	uint8_t x;
	if (i == NULL) {
		i = &x;
	}
	uint8_t* val = (uint8_t*)i;

	val[0] = codec->data[codec->caret++];

	if (flags & CODEC_NEGATIVE) {
		val[0] = -val[0];
	}
	if (flags & CODEC_INV128) {
		val[0] = 128 - val[0];
	}
	if (flags & CODEC_OFS128) {
		val[0] = val[0] - 128;
	}
	return *i;
}

/**
 * Gets 16 bits from the codec
 *  - i: Location to store the value, or NULL
 *  - flags: Modifier flags
 */
uint16_t codec_get16fp(codec_t* codec, uint16_t* i, uint8_t flags)
{
	if (codec->caret+2 > codec->length) {
		return 0;
	}

	uint16_t x;
	if (i == NULL) {
		i = &x;
	}
	uint8_t* val = (uint8_t*)i;

	val[1] = codec->data[codec->caret++];
	val[0] = codec->data[codec->caret++];

	if (flags & CODEC_LITTLE) {
		uint16_t tmp = *i;
		val[0] = ((unsigned char*)&tmp)[1];
		val[1] = ((unsigned char*)&tmp)[0];
	}
	if (flags & CODEC_NEGATIVE) {
		val[0] = -val[0];
	}
	if (flags & CODEC_INV128) {
		val[0] = 128 - val[0];
	}
	if (flags & CODEC_OFS128) {
		val[0] = val[0] - 128;
	}
	return *i;
}

/**
 * Gets 24 bits from the codec
 *  - i: Location to store the value, or NULL
 *  - flags: Modifier flags
 */
uint32_t codec_get24fp(codec_t* codec, uint32_t* i, uint8_t flags)
{
	if (codec->caret+3 > codec->length) {
		return 0;
	}

	uint32_t x;
	if (i == NULL) {
		i = &x;
	}
	uint8_t* val = (uint8_t*)i;

	val[3] = 0;
	val[2] = codec->data[codec->caret++];
	val[1] = codec->data[codec->caret++];
	val[0] = codec->data[codec->caret++];

	if (flags & CODEC_LITTLE) {
		uint32_t tmp = *i;
		val[0] = ((unsigned char*)&tmp)[2];
		val[1] = ((unsigned char*)&tmp)[1];
		val[2] = ((unsigned char*)&tmp)[0];
	}
	if (flags & CODEC_NEGATIVE) {
		val[0] = -val[0];
	}
	if (flags & CODEC_INV128) {
		val[0] = 128 - val[0];
	}
	if (flags & CODEC_OFS128) {
		val[0] = val[0] - 128;
	}
	return *i;
}

/**
 * Gets 32 bits from the codec
 *  - i: Location to store the value, or NULL
 *  - flags: Modifier flags
 */
uint32_t codec_get32fp(codec_t* codec, uint32_t* i, uint8_t flags)
{
	if (codec->caret+4 > codec->length) {
		return 0;
	}

	uint32_t x;
	if (i == NULL) {
		i = &x;
	}
	uint8_t* val = (uint8_t*)i;

	val[3] = codec->data[codec->caret++];
	val[2] = codec->data[codec->caret++];
	val[1] = codec->data[codec->caret++];
	val[0] = codec->data[codec->caret++];

	if (flags & CODEC_LITTLE) {
		uint32_t tmp = *i;
		val[0] = ((unsigned char*)&tmp)[3];
		val[1] = ((unsigned char*)&tmp)[2];
		val[2] = ((unsigned char*)&tmp)[1];
		val[3] = ((unsigned char*)&tmp)[0];
	} else if (flags & CODEC_MIDDLE_A) {
		uint32_t tmp = *i;
		val[0] = ((unsigned char*)&tmp)[1];
		val[1] = ((unsigned char*)&tmp)[0];
		val[2] = ((unsigned char*)&tmp)[3];
		val[3] = ((unsigned char*)&tmp)[2];
	} else if (flags & CODEC_MIDDLE_B) {
		uint32_t tmp = *i;
		val[0] = ((unsigned char*)&tmp)[2];
		val[1] = ((unsigned char*)&tmp)[3];
		val[2] = ((unsigned char*)&tmp)[0];
		val[3] = ((unsigned char*)&tmp)[1];
	}
	if (flags & CODEC_NEGATIVE) {
		val[0] = -val[0];
	}
	if (flags & CODEC_INV128) {
		val[0] = 128 - val[0];
	}
	if (flags & CODEC_OFS128) {
		val[0] = val[0] - 128;
	}
	return *i;
}


/**
 * Gets 64 bits from the codec
 *  - i: Location to store the value, or NULL
 *  - flags: Modifier flags
 */
uint64_t codec_get64fp(codec_t* codec, uint64_t* i, uint8_t flags)
{
	if (codec->caret+8 > codec->length) {
		return 0;
	}

	uint64_t x;
	if (i == NULL) {
		i = &x;
	}
	uint8_t* val = (uint8_t*)i;

	val[7] = codec->data[codec->caret++];
	val[6] = codec->data[codec->caret++];
	val[5] = codec->data[codec->caret++];
	val[4] = codec->data[codec->caret++];
	val[3] = codec->data[codec->caret++];
	val[2] = codec->data[codec->caret++];
	val[1] = codec->data[codec->caret++];
	val[0] = codec->data[codec->caret++];

	if (flags & CODEC_LITTLE) {
		uint64_t tmp = *i;
		val[0] = ((unsigned char*)&tmp)[7];
		val[1] = ((unsigned char*)&tmp)[6];
		val[2] = ((unsigned char*)&tmp)[5];
		val[3] = ((unsigned char*)&tmp)[4];
		val[4] = ((unsigned char*)&tmp)[3];
		val[5] = ((unsigned char*)&tmp)[2];
		val[6] = ((unsigned char*)&tmp)[1];
		val[7] = ((unsigned char*)&tmp)[0];
	}
	if (flags & CODEC_NEGATIVE) {
		val[0] = -val[0];
	}
	if (flags & CODEC_INV128) {
		val[0] = 128 - val[0];
	}
	if (flags & CODEC_OFS128) {
		val[0] = val[0] - 128;
	}
	return *i;
}

/**
 * Gets a string from the codec
 *  - s: The output buffer
 *  - len: The length of the output buffer
 *  - flags: Can be CODEC_JSTRING for 0xA terminated string
 */
char* codec_gets(codec_t* codec, char* s, int len, uint8_t flags)
{
	if (codec->caret+len > codec->length) {
		return NULL;
	}

	char terminator = 0;
	if (flags & CODEC_JSTRING) {
		terminator = 10;
	}
	bool valid = false;
	for (int i = 0; i < len-1; i++) {
		if (codec->data[codec->caret+i] == terminator) {
			len = i;
			valid = true;
			break;
		}
	}
	if (!valid) {
		return NULL;
	}
	codec_getn(codec, (unsigned char*)s, len);
	s[len] = 0;
	return s;
}

object_proto_t codec_proto = {
	.init = (object_init_t)codec_init,
	.free = (object_free_t)codec_free
};
