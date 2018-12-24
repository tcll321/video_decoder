#pragma once
#include "Player.h"
#include "rtspAPI.h"

class CRealPlayer :
	public CPlayer
{
public:
	CRealPlayer(const char* url, HWND hwnd);
	~CRealPlayer();

	int Play();

	static bool RtspCallback(void* userParam, unsigned char* data, unsigned int dataLen, const char* mimeType, bool isKeyFrame, unsigned long pts);
	void DataCallback(unsigned char* data, unsigned int dataLen, const char* mimeType, bool isKeyFrame, unsigned long pts);

private:
};

