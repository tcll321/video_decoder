#pragma once

#include <string>
#include <deque>
#include "locker.h"
#include "RAHWDecoder_pro.h"

class CPlayer
{
public:
	CPlayer(const char* url, HWND hwnd);
	~CPlayer();

	typedef struct DataPacket
	{
		BYTE* data;
		int dataLen;
	}DATAPACKET;

	virtual int Play() = 0;
	
	int CreatePlayer(int deviceID);
	void ClosePlayer();

	int StartPlayThread();
	void StopPlayThread();
	static DWORD WINAPI threadPlay(void* param);
	void PlayWorker();

private:
	void SaveYUVToFile(const unsigned char* data, int len);

protected:
	std::string m_strUrl;
	HWND	m_hwndWindow;
	typedef std::deque<DataPacket*>	DEQDATAPACKET;
	DEQDATAPACKET		m_deqDataPacket;
	CLocker		m_lock;

	bool	m_bIsPlay;
	HANDLE	m_hPlayThread;
	CHANNEL_ID	m_decoderID;

	FILE *m_pfYuv;
};

