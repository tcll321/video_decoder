#include "stdafx.h"
#include "FFmpegReader.h"


CFFmpegReader::CFFmpegReader()
	:m_avFormatContext(NULL)
	, m_formatOpts(NULL)
	, m_pBsfc(NULL)
	, m_videoStreamIdx(-1)
{
}


CFFmpegReader::~CFFmpegReader()
{
	if (m_pBsfc)
		av_bitstream_filter_close(m_pBsfc);

	if (m_avFormatContext)
		avformat_close_input(&m_avFormatContext);
}

FileApiErr CFFmpegReader::Init(const char* file)
{
	int err = 0;
	av_register_all();
	m_fileName = file;

	m_avFormatContext = avformat_alloc_context();

	err = avformat_open_input(&m_avFormatContext, file, NULL, &m_formatOpts);
	if (err)
	{
		printf("FFMPEG: Could not open input container\n");
		return FILE_API_ERR_OPEN;
	}

	err = avformat_find_stream_info(m_avFormatContext, &m_formatOpts);
	if (err < 0)
	{
		printf("FFMPEG: Couldn't find stream information\n");
		return FILE_API_ERR_OPEN;
	}

	av_dump_format(m_avFormatContext, 0, file, 0);

	for (unsigned int i = 0; i < m_avFormatContext->nb_streams; i++)
	{
		if (AVMEDIA_TYPE_VIDEO == m_avFormatContext->streams[i]->codecpar->codec_type)
		{
			// Retrieve required h264_mp4toannexb filter
			m_pBsfc = av_bitstream_filter_init("h264_mp4toannexb");
			if (!m_pBsfc) {
				printf("FFMPEG: Could not aquire h264_mp4toannexb filter\n");
				return FILE_API_ERR_OPEN;
			}
			m_videoStreamIdx = i;
			break;
		}
	}
	if (m_pBsfc)
		return FILE_API_OK;
	return FILE_API_ERR_OPEN;
}

FileApiErr CFFmpegReader::Read(char*& data, int* len)
{
	AVPacket packet;
	while (true)
	{
		if (av_read_frame(m_avFormatContext, &packet) == 0)
		{
			if (packet.stream_index == m_videoStreamIdx)
			{
				//
				// Apply MP4 to H264 Annex B filter on buffer
				//
				int nRet = 0;
				int isKeyFrame = packet.flags & AV_PKT_FLAG_KEY;
				nRet = av_bitstream_filter_filter(m_pBsfc,
					m_avFormatContext->streams[m_videoStreamIdx]->codec,
					NULL,
					&packet.data,
					&packet.size,
					packet.data,
					packet.size,
					isKeyFrame);

				// Current approach leads to a duplicate SPS and PPS....., does not seem to be an issue!

				if (packet.data != NULL && packet.size>0)
				{
					data = new char[packet.size];
					memcpy(data, packet.data, packet.size);
					*len = packet.size;
					av_free(packet.data);
				}
			}

			// Free the packet that was allocated by av_read_frame
			av_free_packet(&packet);
			break;
		}
		else {
			av_free_packet(&packet);
			return FILE_API_ERR_READ;  // Indicates that we reached end of container and to stop video decode
		}
	}

	return FILE_API_OK;
}
