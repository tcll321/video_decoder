#include "stdafx.h"
#include "Player.h"
#include "ColorUtilsInterface.h"

CPlayer::CPlayer(const char* url, HWND hwnd)
	:m_strUrl(url)
	, m_hwndWindow(hwnd)
	, m_bIsPlay(false)
	, m_hPlayThread(NULL)
	, m_decoderID(NULL)
	, m_pfYuv(NULL)
	, m_pfRgb(NULL)
{
}

CPlayer::~CPlayer()
{
	CARAutoLock lock(m_lock);
	if (m_deqDataPacket.size()>0)
	{
		DATAPACKET* pdata = NULL;
		pdata = m_deqDataPacket.front();
		m_deqDataPacket.pop_front();
		delete[] pdata->data;
		delete pdata;
	}

	if (m_pfYuv)
	{
		fclose(m_pfYuv);
		m_pfYuv = NULL;
	}
	if (m_pfRgb)
	{
		fclose(m_pfRgb);
		m_pfRgb = NULL;
	}
}

int CPlayer::CreatePlayer(int deviceID)
{
	DecoderError err = DECODER_OK;
	err = HW_CreateDecoder(&m_decoderID);
	if (DECODER_OK != err)
		return err;
	err = HW_InitDecoder(m_decoderID, deviceID, 1920, 1080, VideoCodec_H264);
	StartPlayThread();
	return err;
}

void CPlayer::ClosePlayer()
{
	if (m_decoderID)
	{
		HW_DeleteDecoder(m_decoderID);
	}
	StopPlayThread();
}

int CPlayer::StartPlayThread()
{
	m_bIsPlay = true;
	m_hPlayThread = CreateThread(NULL, 0, threadPlay, this, 0, NULL);
	return 0;
}

void CPlayer::StopPlayThread()
{
	m_bIsPlay = false;
	if (m_hPlayThread)
	{
		CloseHandle(m_hPlayThread);
		m_hPlayThread = NULL;
	}
}

DWORD CPlayer::threadPlay(void * param)
{
	CPlayer *pPlayer = (CPlayer*)param;
	if (pPlayer)
	{
		pPlayer->PlayWorker();
	}
	return 0;
}

void CPlayer::PlayWorker()
{
	while (m_bIsPlay)
	{
		DATAPACKET* pDataPacket = NULL;
		if (m_deqDataPacket.size() > 0)
		{
			CARAutoLock lock(m_lock);
			pDataPacket = m_deqDataPacket.front();
			m_deqDataPacket.pop_front();
		}
		if (pDataPacket)
		{
			unsigned char* pFrame = NULL;
			int count;
			int frameSize, width, height;
			HW_Decod(m_decoderID, pDataPacket->data, pDataPacket->dataLen, &pFrame, &frameSize, &width, &height, &count);
// 			delete pDataPacket->data;
// 			delete pDataPacket;
			for (int i=0; i<count; i++)
			{
// 				SaveYUVToFile(pFrame, frameSize);
				SaveRgbToFile(pFrame, frameSize, width, height);
			}
		}
		else
			Sleep(1);
	}
}

void CPlayer::SaveYUVToFile(const unsigned char * data, int len)
{
	if (len <= 0)
	{
		return;
	}
	if (m_pfYuv == NULL)
	{
		m_pfYuv = fopen("d:\\out.yuv", "wb");
	}
	if (m_pfYuv)
	{
		std::shared_ptr<ColorUtilsInterface> p = ColorUtilsInterface::Create(0);
		unsigned char* dstMemData = new unsigned char[len];
		p->MemcpyDevToHost(dstMemData, len, (void*)data, len);
		fwrite(dstMemData, 1, len, m_pfYuv);
	}
}

void CPlayer::SaveRgbToFile(const unsigned char* data, int len, int width, int height)
{
	if (len <= 0)
	{
		return;
	}
	if (m_pfRgb == NULL)
	{
		m_pfRgb = fopen("d:\\newDecod.rgb", "wb");
	}
	if (m_pfRgb)
	{
		std::shared_ptr<ColorUtilsInterface> p = ColorUtilsInterface::Create(0);
		void* dstRgbData = NULL;
		unsigned char* dstMemData = new unsigned char[width*height*4];
		unsigned char* dstFace = new unsigned char[width * height * 4];
		p->cudaNv12ToBgra32((uint8_t*)data, width, (uint8_t**)&dstRgbData, width, width, height);
		p->MemcpyDevToHost(dstMemData, width*height * 4, dstRgbData, width*height * 4);
		p->MattingImage(dstRgbData, width, height, dstFace, 800, 500, 600, 300);
		p->FreeGpuMem(dstRgbData);
// 		fwrite(dstMemData, 1, width*height * 4, m_pfRgb);
		fwrite(dstFace, 1, width*height * 4, m_pfRgb);
		delete dstMemData;
	}
}
