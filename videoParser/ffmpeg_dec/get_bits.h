#pragma once
#include "common.h"
#include "error.h"
#include "utils.h"
#include "intreadwrite.h"

#define MB_LEN_MAX    5             // max. # bytes in multibyte char
#define SHRT_MIN    (-32768)        // minimum (signed) short value
#define SHRT_MAX      32767         // maximum (signed) short value
#define USHRT_MAX     0xffff        // maximum unsigned short value
#define INT_MIN     (-2147483647 - 1) // minimum (signed) int value
#define INT_MAX       2147483647    // maximum (signed) int value
#define UINT_MAX      0xffffffff    // maximum unsigned int value
#define LONG_MIN    (-2147483647L - 1) // minimum (signed) long value
#define LONG_MAX      2147483647L   // maximum (signed) long value
#define ULONG_MAX     0xffffffffUL  // maximum unsigned long value
#define LLONG_MAX     9223372036854775807i64       // maximum signed long long int value
#define LLONG_MIN   (-9223372036854775807i64 - 1)  // minimum signed long long int value
#define ULLONG_MAX    0xffffffffffffffffui64       // maximum unsigned long long int value

#ifndef UNCHECKED_BITSTREAM_READER
#define UNCHECKED_BITSTREAM_READER !CONFIG_SAFE_BITSTREAM_READER
#endif

#ifndef CACHED_BITSTREAM_READER
#define CACHED_BITSTREAM_READER 0
#endif

#ifndef NEG_USR32
#   define NEG_USR32(a,s) (((uint32_t)(a))>>(32-(s)))
#endif

typedef struct GetBitContext {
	const uint8_t *buffer, *buffer_end;
#if CACHED_BITSTREAM_READER
	uint64_t cache;
	unsigned bits_left;
#endif
	int index;
	int size_in_bits;
	int size_in_bits_plus8;
} GetBitContext;

static inline unsigned int get_bits(GetBitContext *s, int n);
static inline void skip_bits(GetBitContext *s, int n);
static inline unsigned int show_bits(GetBitContext *s, int n);

/* Bitstream reader API docs:
* name
*   arbitrary name which is used as prefix for the internal variables
*
* gb
*   getbitcontext
*
* OPEN_READER(name, gb)
*   load gb into local variables
*
* CLOSE_READER(name, gb)
*   store local vars in gb
*
* UPDATE_CACHE(name, gb)
*   Refill the internal cache from the bitstream.
*   After this call at least MIN_CACHE_BITS will be available.
*
* GET_CACHE(name, gb)
*   Will output the contents of the internal cache,
*   next bit is MSB of 32 or 64 bits (FIXME 64 bits).
*
* SHOW_UBITS(name, gb, num)
*   Will return the next num bits.
*
* SHOW_SBITS(name, gb, num)
*   Will return the next num bits and do sign extension.
*
* SKIP_BITS(name, gb, num)
*   Will skip over the next num bits.
*   Note, this is equivalent to SKIP_CACHE; SKIP_COUNTER.
*
* SKIP_CACHE(name, gb, num)
*   Will remove the next num bits from the cache (note SKIP_COUNTER
*   MUST be called before UPDATE_CACHE / CLOSE_READER).
*
* SKIP_COUNTER(name, gb, num)
*   Will increment the internal bit counter (see SKIP_CACHE & SKIP_BITS).
*
* LAST_SKIP_BITS(name, gb, num)
*   Like SKIP_BITS, to be used if next call is UPDATE_CACHE or CLOSE_READER.
*
* BITS_LEFT(name, gb)
*   Return the number of bits left
*
* For examples see get_bits, show_bits, skip_bits, get_vlc.
*/

#if CACHED_BITSTREAM_READER
#   define MIN_CACHE_BITS 64
#elif defined LONG_BITSTREAM_READER
#   define MIN_CACHE_BITS 32
#else
#   define MIN_CACHE_BITS 25
#endif

#if !CACHED_BITSTREAM_READER

#define OPEN_READER_NOSIZE(name, gb)            \
    unsigned int name ## _index = (gb)->index;  \
    unsigned int av_unused name ## _cache

#if UNCHECKED_BITSTREAM_READER
#define OPEN_READER(name, gb) OPEN_READER_NOSIZE(name, gb)

#define BITS_AVAILABLE(name, gb) 1
#else
#define OPEN_READER(name, gb)                   \
    OPEN_READER_NOSIZE(name, gb);               \
    unsigned int name ## _size_plus8 = (gb)->size_in_bits_plus8

#define BITS_AVAILABLE(name, gb) name ## _index < name ## _size_plus8
#endif

#define CLOSE_READER(name, gb) (gb)->index = name ## _index

# ifdef LONG_BITSTREAM_READER

# define UPDATE_CACHE_LE(name, gb) name ## _cache = \
      AV_RL64((gb)->buffer + (name ## _index >> 3)) >> (name ## _index & 7)

# define UPDATE_CACHE_BE(name, gb) name ## _cache = \
      AV_RB64((gb)->buffer + (name ## _index >> 3)) >> (32 - (name ## _index & 7))

#else

# define UPDATE_CACHE_LE(name, gb) name ## _cache = \
      AV_RL32((gb)->buffer + (name ## _index >> 3)) >> (name ## _index & 7)

# define UPDATE_CACHE_BE(name, gb) name ## _cache = \
      AV_RB32((gb)->buffer + (name ## _index >> 3)) << (name ## _index & 7)

#endif


#ifdef BITSTREAM_READER_LE

# define UPDATE_CACHE(name, gb) UPDATE_CACHE_LE(name, gb)

# define SKIP_CACHE(name, gb, num) name ## _cache >>= (num)

#else

# define UPDATE_CACHE(name, gb) UPDATE_CACHE_BE(name, gb)

# define SKIP_CACHE(name, gb, num) name ## _cache <<= (num)

#endif

#if UNCHECKED_BITSTREAM_READER
#   define SKIP_COUNTER(name, gb, num) name ## _index += (num)
#else
#   define SKIP_COUNTER(name, gb, num) \
    name ## _index = FFMIN(name ## _size_plus8, name ## _index + (num))
#endif

#define BITS_LEFT(name, gb) ((int)((gb)->size_in_bits - name ## _index))

#define SKIP_BITS(name, gb, num)                \
    do {                                        \
        SKIP_CACHE(name, gb, num);              \
        SKIP_COUNTER(name, gb, num);            \
    } while (0)

#define LAST_SKIP_BITS(name, gb, num) SKIP_COUNTER(name, gb, num)

#define SHOW_UBITS_LE(name, gb, num) zero_extend(name ## _cache, num)
#define SHOW_SBITS_LE(name, gb, num) sign_extend(name ## _cache, num)

#define SHOW_UBITS_BE(name, gb, num) NEG_USR32(name ## _cache, num)
#define SHOW_SBITS_BE(name, gb, num) NEG_SSR32(name ## _cache, num)

#ifdef BITSTREAM_READER_LE
#   define SHOW_UBITS(name, gb, num) SHOW_UBITS_LE(name, gb, num)
#   define SHOW_SBITS(name, gb, num) SHOW_SBITS_LE(name, gb, num)
#else
#   define SHOW_UBITS(name, gb, num) SHOW_UBITS_BE(name, gb, num)
#   define SHOW_SBITS(name, gb, num) SHOW_SBITS_BE(name, gb, num)
#endif

#define GET_CACHE(name, gb) ((uint32_t) name ## _cache)

#endif

static inline int get_bits_count(const GetBitContext *s)
{
#if CACHED_BITSTREAM_READER
	return s->index - s->bits_left;
#else
	return s->index;
#endif
}

static inline unsigned int get_bits1(GetBitContext *s)
{
#if CACHED_BITSTREAM_READER
	if (!s->bits_left)
		refill_64(s);

#ifdef BITSTREAM_READER_LE
	return get_val(s, 1, 1);
#else
	return get_val(s, 1, 0);
#endif
#else
	unsigned int index = s->index;
	uint8_t result = s->buffer[index >> 3];
#ifdef BITSTREAM_READER_LE
	result >>= index & 7;
	result &= 1;
#else
	result <<= index & 7;
	result >>= 8 - 1;
#endif
#if !UNCHECKED_BITSTREAM_READER
	if (s->index < s->size_in_bits_plus8)
#endif
		index++;
	s->index = index;

	return result;
#endif
}

/**
* Skips the specified number of bits.
* @param n the number of bits to skip,
*          For the UNCHECKED_BITSTREAM_READER this must not cause the distance
*          from the start to overflow int32_t. Staying within the bitstream + padding
*          is sufficient, too.
*/
static inline void skip_bits_long(GetBitContext *s, int n)
{
#if CACHED_BITSTREAM_READER
	skip_bits(s, n);
#else
#if UNCHECKED_BITSTREAM_READER
	s->index += n;
#else
	s->index += av_clip(n, -s->index, s->size_in_bits_plus8 - s->index);
#endif
#endif
}

/**
* Read 1-25 bits.
*/
static inline unsigned int get_bits(GetBitContext *s, int n)
{
	register int tmp;
#if CACHED_BITSTREAM_READER

	av_assert2(n>0 && n <= 32);
	if (n > s->bits_left) {
		refill_32(s);
		if (s->bits_left < 32)
			s->bits_left = n;
	}

#ifdef BITSTREAM_READER_LE
	tmp = get_val(s, n, 1);
#else
	tmp = get_val(s, n, 0);
#endif
#else
	OPEN_READER(re, s);
	av_assert2(n>0 && n <= 25);
	UPDATE_CACHE(re, s);
	tmp = SHOW_UBITS(re, s, n);
	LAST_SKIP_BITS(re, s, n);
	CLOSE_READER(re, s);
#endif
	return tmp;
}

/**
* Show 1-25 bits.
*/
static inline unsigned int show_bits(GetBitContext *s, int n)
{
	register int tmp;
#if CACHED_BITSTREAM_READER
	if (n > s->bits_left)
		refill_32(s);

	tmp = show_val(s, n);
#else
	OPEN_READER_NOSIZE(re, s);
	av_assert2(n>0 && n <= 25);
	UPDATE_CACHE(re, s);
	tmp = SHOW_UBITS(re, s, n);
#endif
	return tmp;
}

static inline void skip_bits(GetBitContext *s, int n)
{
#if CACHED_BITSTREAM_READER
	if (n < s->bits_left)
		skip_remaining(s, n);
	else {
		n -= s->bits_left;
		s->cache = 0;
		s->bits_left = 0;

		if (n >= 64) {
			unsigned skip = (n / 8) * 8;

			n -= skip;
			s->index += skip;
		}
		refill_64(s);
		if (n)
			skip_remaining(s, n);
	}
#else
	OPEN_READER(re, s);
	LAST_SKIP_BITS(re, s, n);
	CLOSE_READER(re, s);
#endif
}

/**
* Read 0-32 bits.
*/
static inline unsigned int get_bits_long(GetBitContext *s, int n)
{
	av_assert2(n >= 0 && n <= 32);
	if (!n) {
		return 0;
#if CACHED_BITSTREAM_READER
	}
	return get_bits(s, n);
#else
}
	else if (n <= MIN_CACHE_BITS) {
		return get_bits(s, n);
	}
	else {
#ifdef BITSTREAM_READER_LE
		unsigned ret = get_bits(s, 16);
		return ret | (get_bits(s, n - 16) << 16);
#else
		unsigned ret = get_bits(s, 16) << (n - 16);
		return ret | get_bits(s, n - 16);
#endif
	}
#endif
}

/**
* Show 0-32 bits.
*/
static inline unsigned int show_bits_long(GetBitContext *s, int n)
{
	if (n <= MIN_CACHE_BITS) {
		return show_bits(s, n);
	}
	else {
		GetBitContext gb = *s;
		return get_bits_long(&gb, n);
	}
}

static inline unsigned int show_bits1(GetBitContext *s)
{
	return show_bits(s, 1);
}

static inline void skip_bits1(GetBitContext *s)
{
	skip_bits(s, 1);
}

/**
* Initialize GetBitContext.
* @param buffer bitstream buffer, must be AV_INPUT_BUFFER_PADDING_SIZE bytes
*        larger than the actual read bits because some optimized bitstream
*        readers read 32 or 64 bit at once and could read over the end
* @param bit_size the size of the buffer in bits
* @return 0 on success, AVERROR_INVALIDDATA if the buffer_size would overflow.
*/
static inline int init_get_bits(GetBitContext *s, const uint8_t *buffer,
	int bit_size)
{
	int buffer_size;
	int ret = 0;

	if (bit_size >= INT_MAX - FFMAX(7, AV_INPUT_BUFFER_PADDING_SIZE * 8) || bit_size < 0 || !buffer) {
		bit_size = 0;
		buffer = NULL;
		ret = AVERROR_INVALIDDATA;
	}

	buffer_size = (bit_size + 7) >> 3;

	s->buffer = buffer;
	s->size_in_bits = bit_size;
	s->size_in_bits_plus8 = bit_size + 8;
	s->buffer_end = buffer + buffer_size;
	s->index = 0;

#if CACHED_BITSTREAM_READER
	refill_64(s);
#endif

	return ret;
}

/**
* Initialize GetBitContext.
* @param buffer bitstream buffer, must be AV_INPUT_BUFFER_PADDING_SIZE bytes
*        larger than the actual read bits because some optimized bitstream
*        readers read 32 or 64 bit at once and could read over the end
* @param byte_size the size of the buffer in bytes
* @return 0 on success, AVERROR_INVALIDDATA if the buffer_size would overflow.
*/
static inline int init_get_bits8(GetBitContext *s, const uint8_t *buffer,
	int byte_size)
{
	if (byte_size > INT_MAX / 8 || byte_size < 0)
		byte_size = -1;
	return init_get_bits(s, buffer, byte_size * 8);
}

static inline int get_bits_left(GetBitContext *gb)
{
	return gb->size_in_bits - get_bits_count(gb);
}
