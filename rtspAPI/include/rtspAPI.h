#ifndef _RTSPAPI_H
#define _RTSPAPI_H

#if defined(_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif

#include "RtspAPIExport.h"

typedef bool(*RtspDataCallBack)(void* param, unsigned char* data, unsigned int dataLen, const char* mimeType, bool isKeyFrame, unsigned long long pts);


// 成功返回句柄，失败返回0
RTSP_API void* RtspOpen(const char* streamUrl, const char* userName, const char* password, bool useTcp,
	RtspDataCallBack fCallBack, void* cbParam, bool sendActiveCmd = true);

RTSP_API void RtspClose(void* hd);

#endif