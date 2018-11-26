#include "stdafx.h"
#include "LocalPlayer.h"

CLocalPlayer::CLocalPlayer(const char* url, HWND hwnd)
	:CPlayer(url, hwnd)
	, m_fileID(NULL)
	, m_bRunning(false)
	, m_hThread(NULL)
{
}

CLocalPlayer::~CLocalPlayer()
{
	StopReadThread();
	if (m_fileID)
	{
		Video_Close(m_fileID);
		m_fileID = NULL;
	}
}

int CLocalPlayer::Play()
{
	FileApiErr err = FILE_API_OK;
	err = Video_Open(&m_fileID, m_strUrl.c_str());
	if (err == FILE_API_OK)
	{
		StartReadThread();
	}
	return err;
}

int CLocalPlayer::StartReadThread()
{
	m_bRunning = true;
	m_hThread = CreateThread(NULL, 0, threadRead, this, 0, NULL);
	return 0;
}

void CLocalPlayer::StopReadThread()
{
	m_bRunning = false;
	if (m_hThread)
	{
		CloseHandle(m_hThread);
		m_hThread = NULL;
	}
}

DWORD CLocalPlayer::threadRead(void * param)
{
	CLocalPlayer* pPlayer = (CLocalPlayer*)param;
	if (pPlayer)
	{
		pPlayer->ReadWorker();
	}
	return 0;
}

void CLocalPlayer::ReadWorker()
{
	while (m_bRunning)
	{
		char *data;
		int len;
		if (m_deqDataPacket.size() > 128)
		{
			Sleep(5);
			continue;
		}
		if (FILE_API_OK == Video_Read(m_fileID, &data, &len))
		{
			if (len > 0)
			{
				DATAPACKET *pDataPacket = new DATAPACKET();
				pDataPacket->data = new BYTE[len];
				pDataPacket->dataLen = len;
				memcpy(pDataPacket->data, data, len);
				Video_DataFree(data);
				CARAutoLock lock(m_lock);
				m_deqDataPacket.push_back(pDataPacket);
			}
		}
		else
		{
			Sleep(1);
		}
	}
}
