// rtspAPI.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "rtspAPI.h"
#include "ARRTSPClient.h"

RTSP_API void* RtspOpen(const char* streamUrl, const char* userName, const char* password, bool useTcp,
	RtspDataCallBack fCallBack, void* cbParam, bool sendActiveCmd /*= true*/)
{
	CRTSPClient* client = new CRTSPClient(streamUrl, userName, password, useTcp, fCallBack, cbParam, sendActiveCmd);
	if (client->Open())
		return (void*)client;
	delete client;
	return 0;
}

RTSP_API void RtspClose(void* hd)
{
	if (0 == hd)
		return;
	CRTSPClient* client = (CRTSPClient*)hd;
	client->Close();
	delete client;
	return;
}