#pragma once
#include <stdint.h>
#include <vcruntime_string.h>
#include "mem.h"
#include "mem_internal.h"

/**
* @ingroup lavc_decoding
* Required number of additionally allocated bytes at the end of the input bitstream for decoding.
* This is mainly needed because some optimized bitstream readers read
* 32 or 64 bit at once and could read over the end.<br>
* Note: If the first 23 bits of the additional bytes are not 0, then damaged
* MPEG bitstreams could cause overread and segfault.
*/
#define AV_INPUT_BUFFER_PADDING_SIZE 64

void av_fast_padded_malloc(void *ptr, unsigned int *size, size_t min_size);

void inline av_fast_padded_malloc(void *ptr, unsigned int *size, size_t min_size)
{
	uint8_t **p = (uint8_t**)ptr;
	if (min_size > SIZE_MAX - AV_INPUT_BUFFER_PADDING_SIZE) {
		av_freep(p);
		*size = 0;
		return;
	}
	if (!ff_fast_malloc(p, size, min_size + AV_INPUT_BUFFER_PADDING_SIZE, 1))
		memset(*p + min_size, 0, AV_INPUT_BUFFER_PADDING_SIZE);
}