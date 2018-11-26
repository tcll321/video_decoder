#include "stdafx.h"
#include "Decoder.h"


CDecoder::CDecoder()
	:m_cuContext(NULL)
	, m_nvDevoder(NULL)
{
	cuInit(0);
}


CDecoder::~CDecoder()
{
	if (m_nvDevoder)
		delete m_nvDevoder;
}

DecoderError CDecoder::InitDecoder(int devId, int width, int height, VideoCodec codec)
{
	CUresult res = CUDA_SUCCESS;
	res = cuCtxCreate(&m_cuContext, 0, devId);
	if (res != CUDA_SUCCESS)
	{
		printf("cuCtxCreate failed!! errno=%d\n", res);
		return DECODER_ERR_NOT_INITIALIZED;
	}
	if (m_nvDevoder == NULL)
	{
		m_nvDevoder = new NvDecoder(m_cuContext, width, height, false, GetNvCodecID(codec));
	}
}

cudaVideoCodec CDecoder::GetNvCodecID(VideoCodec codec)
{
	switch (codec) {
	case VideoCodec_MPEG1: return cudaVideoCodec_MPEG1;
	case VideoCodec_MPEG2: return cudaVideoCodec_MPEG2;
	case VideoCodec_MPEG4: return cudaVideoCodec_MPEG4;
	case VideoCodec_VC1: return cudaVideoCodec_VC1;
	case VideoCodec_H264: return cudaVideoCodec_H264;
	case VideoCodec_HEVC: return cudaVideoCodec_HEVC;
	case VideoCodec_VP8: return cudaVideoCodec_VP8;
	case VideoCodec_VP9: return cudaVideoCodec_VP9;
	case VideoCodec_JPEG: return cudaVideoCodec_JPEG;
	default: return cudaVideoCodec_NumCodecs;
	}
}

DecoderError CDecoder::Decod(const unsigned char* data, int datalen, unsigned char** ppFrame, int* frameSize, int* framCount)
{
	if (m_nvDevoder == NULL)
		return DECODER_ERR_NULL_PTR;

	if (m_nvDevoder->Decode(data, datalen, ppFrame, framCount))
	{
		if (*framCount != 0)
		{
			*frameSize = m_nvDevoder->GetFrameSize();
		}
		return DECODER_OK;
	}

	return DECODER_ERR_DECOD_FAILED;
}
