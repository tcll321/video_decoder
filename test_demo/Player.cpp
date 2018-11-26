#include "stdafx.h"
#include "Player.h"


CPlayer::CPlayer(const char* url, HWND hwnd)
	:m_strUrl(url)
	, m_hwndWindow(hwnd)
	, m_bIsPlay(false)
	, m_hPlayThread(NULL)
	, m_decoderID(NULL)
	, m_pfYuv(NULL)
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
			int frameSize;
			HW_Decod(m_decoderID, pDataPacket->data, pDataPacket->dataLen, &pFrame, &frameSize, &count);
			delete pDataPacket->data;
			delete pDataPacket;
// 			for (int i=0; i<count; i++)
// 			{
// 				SaveYUVToFile(pFrame, frameSize);
// 			}
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
		m_pfYuv = fopen("d:\\out.yuv", "ab");
	}
	if (m_pfYuv)
	{
		fwrite(data, 1, len, m_pfYuv);
	}
}
