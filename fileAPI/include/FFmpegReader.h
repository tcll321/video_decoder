#pragma once

#include <string>
#include "file_pub_defs.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus
#include "libavformat/avformat.h"
#include "libavformat/avio.h"
#include "libavcodec/avcodec.h"
#ifdef __cplusplus
}
#endif // __cplusplus


class CFFmpegReader
{
public:
	CFFmpegReader();
	~CFFmpegReader();

	FileApiErr Init(const char* file);
	FileApiErr Read(char*& data, int* len);

private:
	std::string m_fileName;

	AVFormatContext* m_avFormatContext;
	AVDictionary*	m_formatOpts;
	AVBitStreamFilterContext*	m_pBsfc;
	int	m_videoStreamIdx;

};

