#pragma once

#include "Player.h"
#include "videoFileAPI.h"

class CLocalPlayer :
	public CPlayer
{
public:
	CLocalPlayer(const char* url, HWND hwnd);
	~CLocalPlayer();

	int Play();

	int StartReadThread();
	void StopReadThread();
	static DWORD WINAPI threadRead(void* param);
	void ReadWorker();
private:
	FILE_ID	m_fileID;

	bool	m_bRunning;
	HANDLE	m_hThread;
};

