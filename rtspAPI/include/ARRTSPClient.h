#pragma once
#include "BasicUsageEnvironment.hh"
#include "NetAddress.hh"
#include "RTSPClient.hh"
#include <vector>
#include "locker.h"
#include "thread.h"

#define RTSP_CLIENT_VERBOSITY_LEVEL 0

typedef bool(*fDataCallBack)(void* param, unsigned char* data, unsigned int dataLen, const char* mimeType, bool isKeyFrame, unsigned long pts);

class ARRTSPClient : public RTSPClient
{
public:
	static ARRTSPClient* createNew(UsageEnvironment& env, char const* rtspURL,
		int verbosityLevel = 0,
		char const* applicationName = NULL,
		portNumBits tunnelOverHTTPPortNum = 0,
		int socketNumToServer = -1);

protected:
	ARRTSPClient(UsageEnvironment& env, char const* rtspURL,
		int verbosityLevel, char const* applicationName, portNumBits tunnelOverHTTPPortNum, int socketNumToServer);
	~ARRTSPClient();

public:
	BOOL Open(const char* userName, const char* password, BOOL useTcp, fDataCallBack callback, void* cbParam, BOOL sendActiveCmd);
	void Close();
	BOOL OnData(BYTE* data, DWORD dataLen, const char* mimeType, BOOL isKeyFrame, DWORD pts);

private:
	static void ActiveCmdResponseHandler(RTSPClient* rtspClient, int resultCode, char* resultString);

	static void ResponseHandler(RTSPClient* rtspClient, int resultCode, char* resultString);
	void OnResponseHandler(int resultCode, char* resultString);

	static void ResponseTimeOut(void* clientData);
	void OnResponseTimeOut();

	static void CheckAliveHandler(void* clientData);
	void OnCheckAliveHandler();

	void SendActiveCmd();

	bool CanbindPort(unsigned short port);
	unsigned short GetIdlePort();

private:
	Authenticator* authenticator;
	BOOL m_useTcp;
	unsigned m_cseq;
	MediaSession* m_session;
	char m_watchVariable;
	int m_resultCode;
	char* m_resultString;
	BOOL serverSupportsGetParameter;

	BOOL m_sendActiveCmd;
	TaskToken waitResponseTask;
	TaskToken checkAliveTask;
	fDataCallBack m_callback;
	void* m_callbackParam;
	INT64 m_sendActiveCmdTime;
	INT64 lastRecvTime;

	CLocker m_scheduleLock;
	OSThread live_recv_thread;		// 线程句柄
	OSThreadID liveRecvthreadID;   		// 线程的ID号
	COSSema m_semAck;

public:
	static int THWorker(void* thisObj);
	int Worker();
};

class CARRTSPClientCallbacks
{
public:
	static void Add(ARRTSPClient* client)
	{
		CARAutoLock locker(ms_lock);
		std::vector<ARRTSPClient*>::iterator it = find(ms_cbVec.begin(), ms_cbVec.end(), client);
		if (ms_cbVec.end() != it)
		{
			return;
		}
		ms_cbVec.push_back(client);
	}

	static void Del(ARRTSPClient* client)
	{
		CARAutoLock locker(ms_lock);
		std::vector<ARRTSPClient*>::iterator it = find(ms_cbVec.begin(), ms_cbVec.end(), client);
		if (ms_cbVec.end() != it)
		{
			ms_cbVec.erase(it);
		}
	}

	static BOOL IsExist(ARRTSPClient* client)
	{
		CARAutoLock locker(ms_lock);
		std::vector<ARRTSPClient*>::iterator it = find(ms_cbVec.begin(), ms_cbVec.end(), client);
		return ms_cbVec.end() != it;
	}

protected:
private:
	static std::vector<ARRTSPClient*> ms_cbVec;
	static CLocker ms_lock;
};

class CRTSPClient
{
public:
	CRTSPClient(const char* streamUrl,
		const char* userName, const char* passWord,
		BOOL bTcp,
		fDataCallBack cb, void* param,
		BOOL bSendActiveCmd);
	~CRTSPClient();
public:
	BOOL Open();
	void Close();

private:
	char m_url[600];
	BOOL m_useTcp;
	char m_username[255];
	char m_password[255];

	BOOL m_sendActiveCmd;

	fDataCallBack m_rtpDataHandler;
	void* m_cbParam;

	ARRTSPClient* m_rtspClient;

	CLocker m_lock;

#ifdef WIN32
	BasicTaskScheduler* m_scheduler;
#else
	BasicTaskSchedulerEpoll* m_scheduler;
#endif

	BasicUsageEnvironment* m_env;

};

