#pragma once
#include <stdint.h>

typedef struct ParseContext {
	uint8_t *buffer;
	int index;
	int last_index;
	unsigned int buffer_size;
	uint32_t state;             ///< contains the last few bytes in MSB order
	int frame_start_found;
	int overread;               ///< the number of bytes which where irreversibly read from the next frame
	int overread_index;         ///< the index into ParseContext.buffer of the overread bytes
	uint64_t state64;           ///< contains the last 8 bytes in MSB order
} ParseContext;

enum AVCodecType {
	AV_CODEC_TYPE_NONE,
	AV_CODEC_TYPE_H264 = 27,
};

#define END_NOT_FOUND (-100)

typedef struct VideoInfoStruct
{
	int width;
	int height;
	int codeWidth;
	int codeHeight;
	int64_t pts;     /* pts of the current frame */
}VideoInfo;