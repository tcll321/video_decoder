#pragma once

#include "pubDefs.h"
#include "NvDecoder.h"

class CDecoder
{
public:
	CDecoder();
	~CDecoder();

	DecoderError InitDecoder(int devId, int width, int height, VideoCodec codec);
	DecoderError Decod(const unsigned char* data, int datalen, unsigned char** ppFrame, int* frameSize, int* framCount);

protected:
	cudaVideoCodec GetNvCodecID(VideoCodec codec);

private:
	CUcontext m_cuContext;
	NvDecoder* m_nvDevoder;

};

