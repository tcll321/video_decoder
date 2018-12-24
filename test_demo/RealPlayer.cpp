#include "stdafx.h"
#include "RealPlayer.h"


CRealPlayer::CRealPlayer(const char* url, HWND hwnd)
	:CPlayer(url, hwnd)
{
}


CRealPlayer::~CRealPlayer()
{
}

int CRealPlayer::Play()
{
	RtspOpen(m_strUrl.c_str(), "", "", true, RtspCallback, this);
	return 0;
}

bool CRealPlayer::RtspCallback(void * userParam, unsigned char * data, unsigned int dataLen, const char * mimeType, bool isKeyFrame, unsigned long pts)
{
	CRealPlayer* pPlayer = (CRealPlayer*)userParam;
	if (pPlayer)
	{
		pPlayer->DataCallback(data, dataLen, mimeType, isKeyFrame, pts);
		return true;
	}
	return false;
}

void CRealPlayer::DataCallback(unsigned char * data, unsigned int dataLen, const char * mimeType, bool isKeyFrame, unsigned long pts)
{
	if (dataLen > 0)
	{
		if ((data[0] == 0x00 && data[1] == 0x00 && data[2] == 0x00 && data[3] == 0x01)
			|| (data[0] == 0x00 && data[1] == 0x00 && data[2] == 0x01))
		{
			if (m_deqDataPacket.size() > 512)
			{
				// 			LOG(WARNING) << "slice buf is full!!";
				Sleep(1);
				return;
			}
			DATAPACKET *pDataPacket = new DATAPACKET();
			pDataPacket->data = new BYTE[dataLen];
			pDataPacket->dataLen = dataLen;
			memcpy(pDataPacket->data, data, dataLen);
			CARAutoLock lock(m_lock);
			m_deqDataPacket.push_back(pDataPacket);
		}
	}
}
