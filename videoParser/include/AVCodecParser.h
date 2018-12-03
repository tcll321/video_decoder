#pragma once
#include <stdint.h>

typedef struct AVCodecParser {
	int codec_ids[5]; /* several codec IDs are permitted */
	int(*parser_init)();
	/* This callback never returns an error, a negative value means that
	* the frame start was in a previous packet. */
	int(*parser_parse)(const uint8_t *buf, int buf_size, VideoInfo * info);
	void(*parser_close)();
	int(*split)(const uint8_t *buf, int buf_size);
} AVCodecParser;
