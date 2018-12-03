#pragma once
#include "get_bits.h"
#include "intmath.h"

#define INVALID_VLC           0x80000000

extern const uint8_t ff_golomb_vlc_len[512];
extern const uint8_t ff_ue_golomb_vlc_code[512];
extern const  int8_t ff_se_golomb_vlc_code[512];
extern const uint8_t ff_ue_golomb_len[256];

extern const uint8_t ff_interleaved_golomb_vlc_len[256];
extern const uint8_t ff_interleaved_ue_golomb_vlc_code[256];
extern const  int8_t ff_interleaved_se_golomb_vlc_code[256];
extern const uint8_t ff_interleaved_dirac_golomb_vlc_code[256];

/**
* Read an unsigned Exp-Golomb code in the range 0 to 8190.
*/
static inline int get_ue_golomb(GetBitContext *gb)
{
	unsigned int buf;

#if CACHED_BITSTREAM_READER
	buf = show_bits_long(gb, 32);

	if (buf >= (1 << 27)) {
		buf >>= 32 - 9;
		skip_bits_long(gb, ff_golomb_vlc_len[buf]);

		return ff_ue_golomb_vlc_code[buf];
	}
	else {
		int log = 2 * av_log2(buf) - 31;
		buf >>= log;
		buf--;
		skip_bits_long(gb, 32 - log);

		return buf;
	}
#else
	OPEN_READER(re, gb);
	UPDATE_CACHE(re, gb);
	buf = GET_CACHE(re, gb);

	if (buf >= (1 << 27)) {
		buf >>= 32 - 9;
		LAST_SKIP_BITS(re, gb, ff_golomb_vlc_len[buf]);
		CLOSE_READER(re, gb);

		return ff_ue_golomb_vlc_code[buf];
	}
	else {
		int log = 2 * av_log2(buf) - 31;
		LAST_SKIP_BITS(re, gb, 32 - log);
		CLOSE_READER(re, gb);
		if (log < 7) {
			return AVERROR_INVALIDDATA;
		}
		buf >>= log;
		buf--;

		return buf;
	}
#endif
}

/**
* read signed exp golomb code.
*/
static inline int get_se_golomb(GetBitContext *gb)
{
	unsigned int buf;

#if CACHED_BITSTREAM_READER
	buf = show_bits_long(gb, 32);

	if (buf >= (1 << 27)) {
		buf >>= 32 - 9;
		skip_bits_long(gb, ff_golomb_vlc_len[buf]);

		return ff_se_golomb_vlc_code[buf];
	}
	else {
		int log = 2 * av_log2(buf) - 31;
		buf >>= log;

		skip_bits_long(gb, 32 - log);

		if (buf & 1)
			buf = -(buf >> 1);
		else
			buf = (buf >> 1);

		return buf;
	}
#else
	OPEN_READER(re, gb);
	UPDATE_CACHE(re, gb);
	buf = GET_CACHE(re, gb);

	if (buf >= (1 << 27)) {
		buf >>= 32 - 9;
		LAST_SKIP_BITS(re, gb, ff_golomb_vlc_len[buf]);
		CLOSE_READER(re, gb);

		return ff_se_golomb_vlc_code[buf];
	}
	else {
		int log = av_log2(buf);
		int sign;
		LAST_SKIP_BITS(re, gb, 31 - log);
		UPDATE_CACHE(re, gb);
		buf = GET_CACHE(re, gb);

		buf >>= log;

		LAST_SKIP_BITS(re, gb, 32 - log);
		CLOSE_READER(re, gb);

		sign = (buf & 1);
		buf = ((buf >> 1) ^ sign) - sign;

		return buf;
	}
#endif
}

/**
* Read an unsigned Exp-Golomb code in the range 0 to UINT32_MAX-1.
*/
static inline unsigned get_ue_golomb_long(GetBitContext *gb)
{
	unsigned buf, log;

	buf = show_bits_long(gb, 32);
	log = 31 - av_log2(buf);
	skip_bits_long(gb, log);

	return get_bits_long(gb, log + 1) - 1;
}

/**
* read unsigned exp golomb code, constraint to a max of 31.
* the return value is undefined if the stored value exceeds 31.
*/
static inline int get_ue_golomb_31(GetBitContext *gb)
{
	unsigned int buf;

#if CACHED_BITSTREAM_READER
	buf = show_bits_long(gb, 32);

	buf >>= 32 - 9;
	skip_bits_long(gb, ff_golomb_vlc_len[buf]);
#else

	OPEN_READER(re, gb);
	UPDATE_CACHE(re, gb);
	buf = GET_CACHE(re, gb);

	buf >>= 32 - 9;
	LAST_SKIP_BITS(re, gb, ff_golomb_vlc_len[buf]);
	CLOSE_READER(re, gb);
#endif

	return ff_ue_golomb_vlc_code[buf];
}
