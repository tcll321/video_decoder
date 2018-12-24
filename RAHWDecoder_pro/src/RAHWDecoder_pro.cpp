// RAHWDecoder_pro.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "RAHWDecoder_pro.h"
#include "IDManager.h"
#include "Decoder.h"
#include "cuda.h"
#include "deviceInfo.h"


typedef CIDAutoBusy<CIDManager<CHANNEL_ID>, CHANNEL_ID> ChannelIDAutoBusy;
static CIDManager<CHANNEL_ID> s_channelIDManager;

#define DECODER_GETOBJECT(pObj, ClassName, findID, Parameter) \
	IDInfo<CHANNEL_ID> IDInfo; \
	if(!s_channelIDManager.Find(findID, &IDInfo)) \
	return DECODER_ERR_NO_HANDLE; \
	ClassName* pObj = (ClassName*)IDInfo.Parameter; \
	if(!pObj) \
	return DECODER_ERR_NULL_OBJECT

RAHWDECODER_PRO_API DecoderError DECODER_METHOD HW_GetDeviceCount(int *count)
{
	CUresult res = cuInit(0);
	if (res != CUDA_SUCCESS)
	{
		printf("cuInit err!! errno=%d", res);
		return DECODER_ERR_NOT_INITIALIZED;
	}
	*count = GetDeviceCount();
	return DECODER_OK;
}

RAHWDECODER_PRO_API DecoderError DECODER_METHOD HW_GetDeviceInfo(int devNo, DeviceInfo* info)
{
	int err = 0;
	err = GetDeviceInfo(devNo, info);
	if (err != 0)
	{
		return DECODER_ERR_GETDEVICEINFO_FAILED;
	}
	return DECODER_OK;
}

RAHWDECODER_PRO_API DecoderError DECODER_METHOD HW_CreateDecoder(CHANNEL_ID *id)
{
	DecoderError err = DECODER_OK;
	CDecoder* decoder = new CDecoder();
	if (NULL == decoder)
		return DECODER_ERR_NULL_PTR;
	*id = s_channelIDManager.New("cudaDecoder", (void*)decoder);
	if (NULL == *id)
	{
		delete decoder;
		return DECODER_ERR_NULL_PTR;
	}

	return err;
}

RAHWDECODER_PRO_API DecoderError DECODER_METHOD HW_DeleteDecoder(CHANNEL_ID id)
{
	DecoderError err = DECODER_OK;
	ChannelIDAutoBusy autoBusy(s_channelIDManager, id);
	DECODER_GETOBJECT(pDecoder, CDecoder, id, param);
	delete pDecoder;
	pDecoder = NULL;
	return err;
}

RAHWDECODER_PRO_API DecoderError DECODER_METHOD HW_InitDecoder(CHANNEL_ID id, int devId, int width, int height, VideoCodec codec)
{
	DecoderError err = DECODER_OK;
	ChannelIDAutoBusy autoBusy(s_channelIDManager, id);
	DECODER_GETOBJECT(pDecoder, CDecoder, id, param);
	err = pDecoder->InitDecoder(devId, width, height, codec);
	return err;
}

RAHWDECODER_PRO_API DecoderError DECODER_METHOD HW_Decod(CHANNEL_ID id, const unsigned char* data, int datalen, unsigned char** ppFrame, int* frameSize, int* width, int* height, int* framCount)
{
	DecoderError err = DECODER_OK;
	ChannelIDAutoBusy autoBusy(s_channelIDManager, id);
	DECODER_GETOBJECT(pDecoder, CDecoder, id, param);
// 	err = pDecoder->Decod(data, datalen, ppFrame, frameSize, framCount);
	err = pDecoder->Decod(data, datalen, ppFrame, frameSize, width, height, framCount);
	return err;
}
