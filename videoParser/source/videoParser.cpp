// videoParser.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "videoParser.h"
#include "AVCodecParser.h"

extern AVCodecParser ff_h264_parser;

// 这是导出函数的一个示例。
VIDEOPARSER_API void* Init(AVCodecType type)
{
	switch (type)
	{
	case AV_CODEC_TYPE_H264:
		return &ff_h264_parser;
	}
    return NULL;
}

VIDEOPARSER_API void Clear(void * context)
{
}

VIDEOPARSER_API int Parser(void * context, const char * data, int len, VideoInfo * info)
{
	AVCodecParser* parser = (AVCodecParser*)context;
	parser->parser_parse((const uint8_t*)data, len, info);
	return 0;
}
