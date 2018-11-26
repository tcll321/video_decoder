#include "stdafx.h"
#include "ARRTSPClient.h"
#include "RTSPCommon.hh"
#include "AudioDataSink.h"
#include "H264MediaSink.h"
#include "MPEG4MediaSink.h"
#include "DataSink.h"
#include "GroupsockHelper.hh"
#include "locker.h"

#define CMD_TIMEOUT 10000000                 // 单位是 us
#define CHECKALIVE_TIMOUT 1000000   

#define RECV_TIMEOUT 10000                   // 单位是 ms
#define THREADEXIT_TIMEOUT 5000

#define RTSPUDP_BASEBEGINPORT (15000)
#define RTSPUDP_PORTNUM (5000)

#define SockValid(sock)			((INT_PTR)(sock) >= 0)

std::vector<ARRTSPClient*> CARRTSPClientCallbacks::ms_cbVec;
CLocker CARRTSPClientCallbacks::ms_lock;

ARRTSPClient* ARRTSPClient::createNew(UsageEnvironment &env, const char *rtspURL, int verbosityLevel, char const *applicationName, portNumBits tunnelOverHTTPPortNum, int socketNumToServer)
{
	return new ARRTSPClient(env, rtspURL, verbosityLevel, applicationName, tunnelOverHTTPPortNum, socketNumToServer);
}

ARRTSPClient::ARRTSPClient(UsageEnvironment &env, const char *rtspURL, int verbosityLevel, char const *applicationName, portNumBits tunnelOverHTTPPortNum, int socketNumToServer)
	: RTSPClient(env, rtspURL, verbosityLevel, applicationName, tunnelOverHTTPPortNum, socketNumToServer)
	, serverSupportsGetParameter(false)
	, m_useTcp(false)
	, m_cseq(0)
	, m_session(NULL)
	, authenticator(NULL)
	, m_watchVariable(0)
	, m_resultCode(0)
	, m_resultString(NULL)
	, waitResponseTask(NULL)
	, checkAliveTask(NULL)
	, m_callback(NULL)
	, m_callbackParam(NULL)
	, m_sendActiveCmd(TRUE)
	, live_recv_thread(INVALID_OSTHREAD)
	, liveRecvthreadID(INVALID_OSTHREADID)
{
}

ARRTSPClient::~ARRTSPClient()
{
}

BOOL ARRTSPClient::Open(const char* userName, const char* password, BOOL useTcp, fDataCallBack callback, void* cbParam, BOOL sendActiveCmd)
{
// 	ASSERT(m_session == NULL);

	m_useTcp = useTcp;
	m_sendActiveCmd = sendActiveCmd;

	if ((userName != NULL && userName[0] != '\0')
		|| (password != NULL && password[0] != '\0'))
		authenticator = new Authenticator(userName, password);
	else
		authenticator = NULL;

// 	ASSERT(waitResponseTask == NULL);
	m_watchVariable = 0;
	waitResponseTask = envir().taskScheduler().scheduleDelayedTask(CMD_TIMEOUT, ResponseTimeOut, this);
	m_cseq = sendOptionsCommand(ResponseHandler, authenticator);
	envir().taskScheduler().doEventLoop(&m_watchVariable);
	if (m_resultCode != 0)
	{
		delete[] m_resultString;
		return false;
	}
	serverSupportsGetParameter = RTSPOptionIsSupported("GET_PARAMETER", m_resultString);
	delete[] m_resultString;

	m_watchVariable = 0;
	waitResponseTask = envir().taskScheduler().scheduleDelayedTask(CMD_TIMEOUT, ResponseTimeOut, this);
	m_cseq = sendDescribeCommand(ResponseHandler);
	envir().taskScheduler().doEventLoop(&m_watchVariable);
	if (m_resultCode != 0)
	{
		delete[] m_resultString;
		return false;
	}

	MediaSession* session = MediaSession::createNew(envir(), m_resultString);
	if (session == NULL)
	{
		delete[] m_resultString;
		return false;
	}

	if (!session->hasSubsessions())
	{
		Medium::close(session);
		delete[] m_resultString;
		return false;
	}
	delete[] m_resultString;

	MediaSubsessionIterator setupIter(*session);
	MediaSubsession *subsession;
	while ((subsession = setupIter.next()) != NULL)
	{
		if (!((strcmp(subsession->mediumName(), "audio") == 0)
			|| (strcmp(subsession->mediumName(), "video") == 0)))
			continue;

		if (!m_useTcp)
		{
			WORD port = GetIdlePort();
			if (port != 0)
			{
				subsession->setClientPortNum(port);
			}
			else
			{
				return false;
			}
		}

		if (!subsession->initiate()) continue;
		if (!subsession->rtpSource()) continue;

		m_watchVariable = 0;
		waitResponseTask = envir().taskScheduler().scheduleDelayedTask(CMD_TIMEOUT, ResponseTimeOut, this);
		bool useTcp = m_useTcp ? true : false;
		m_cseq = sendSetupCommand(*subsession, ResponseHandler, False, useTcp, False/*, authenticator*/);
		envir().taskScheduler().doEventLoop(&m_watchVariable);
		if (m_resultCode != 0)
		{
			Medium::close(session);
			delete[] m_resultString;
			return false;
		}
		delete[] m_resultString;
	}

	m_watchVariable = 0;
	waitResponseTask = envir().taskScheduler().scheduleDelayedTask(CMD_TIMEOUT, ResponseTimeOut, this);
	m_cseq = sendPlayCommand(*session, ResponseHandler/*, 0.0f, 0.0f, 1.0f, authenticator*/);
	envir().taskScheduler().doEventLoop(&m_watchVariable);
	if (m_resultCode != 0)
	{
		//sendTeardownCommand(*session, NULL );
		Medium::close(session);
		delete[] m_resultString;
		return false;
	}

	m_session = session;
	m_callback = callback;
	m_callbackParam = cbParam;

	CARRTSPClientCallbacks::Add(this);

	m_watchVariable = 0;
	OSCreateThread(&live_recv_thread, &liveRecvthreadID/*NULL*/, THWorker, this, 0);

	lastRecvTime = GetTickCount();
	m_sendActiveCmdTime = GetTickCount();
	if (m_sendActiveCmd)
		checkAliveTask = envir().taskScheduler().scheduleDelayedTask(CHECKALIVE_TIMOUT, (TaskFunc*)CheckAliveHandler, (void*)this);

	setupIter.reset();
	while ((subsession = setupIter.next()) != NULL)
	{
		if (subsession->clientPortNum() == 0)
			continue; // port # was not set

		if (subsession->readSource() == NULL)
			continue; // was not initiated

		if (strcmp(subsession->mediumName(), "audio") == 0)
		{
			subsession->sink = new AudioDataSink(envir(), this, 10000);
		}
		else if (strcmp(subsession->mediumName(), "video") == 0)
		{
			if (strcmp(subsession->codecName(), "H264") == 0)
			{
				//subsession->sink = H264VideoFileSink::createNew(envir(), "f:\\test1.h264", subsession->fmtp_spropparametersets(), 200000, false);
				//subsession->sink = new H264MediaSink(envir(), subsession->fmtp_spropparametersets(), this);
				subsession->sink = new H264MediaSink(envir(), subsession, this);
			}
			else if (strcmp(subsession->codecName(), "MP4V-ES") == 0)
			{
				subsession->sink = new MPEG4MediaSink(envir(), subsession->fmtp_config(), this);
			}
			else
			{
				subsession->sink = new DataSink(envir(), this, 1000000);
			}
		}

		if (subsession->sink)
		{
			subsession->sink->startPlaying(*(subsession->readSource()), NULL, this);
		}
	}

	return true;
}

int ARRTSPClient::THWorker(void* thisObj)
{
	if (thisObj)
	{
		ARRTSPClient* p = (ARRTSPClient*)(thisObj);
		if (p)
		{
			return p->Worker();
		}
	}

	return 0;
}

int ARRTSPClient::Worker()
{
	lastRecvTime = GetTickCount();
	envir().taskScheduler().doEventLoop(&m_watchVariable);
// 	m_semAck.Post();
	return 0;
}

void ARRTSPClient::ResponseTimeOut(void* clientData)
{
	ARRTSPClient* thisObj = (ARRTSPClient*)clientData;
	thisObj->OnResponseTimeOut();
}

void ARRTSPClient::OnResponseTimeOut()
{
	changeResponseHandler(m_cseq, NULL);

	CARAutoLock locker(m_scheduleLock);

	if (waitResponseTask != NULL)
	{
		envir().taskScheduler().unscheduleDelayedTask(waitResponseTask);
		waitResponseTask = NULL;
	}

	// Fill in 'negative' return values:
	m_resultCode = ~0;
	char *resultstr = new char[32];
	sprintf_s(resultstr, 32, "Response Timeout...");
	m_resultString = resultstr;

	// Signal a break from the event loop (thereby returning from the blocking command):
	m_watchVariable = ~0;
}

bool ARRTSPClient::CanbindPort(unsigned short port)
{
	SOCKET s = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
	if (!SockValid(s) && WSANOTINITIALISED == WSAGetLastError())
	{
		WORD wVersionRequested;
		WSADATA wsaData;
		int high, low;
		static BOOL isInited = false;
		if (isInited)
			return true;
		for (high = 2; high >= 1; --high)
		{
			for (low = 2; low >= 0; --low)
			{
				wVersionRequested = MAKEWORD(low, high);
				if (0 == WSAStartup(wVersionRequested, &wsaData))
				{
					if (LOBYTE(wsaData.wVersion) == (BYTE)(low)
						&& HIBYTE(wsaData.wVersion) == (BYTE)(high))
					{
						isInited = true;
						return true;
					}
				}
				WSACleanup();
			}
		}
		s = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
	}

	if (s == INVALID_SOCKET)
	{
		return false;
	}

	struct sockaddr_in sinAddr;
	memset(&sinAddr, 0, sizeof(sinAddr));
	sinAddr.sin_family = AF_INET;
	sinAddr.sin_addr.s_addr = ADDR_ANY;
	sinAddr.sin_port = htons((WORD)port);
	if (0 != bind(s, (struct sockaddr*)&sinAddr, sizeof(sinAddr)))
	{
		closesocket(s);
		return false;
	}

	closesocket(s);
	return true;
}

unsigned short ARRTSPClient::GetIdlePort()
{
	unsigned short getport = 0;
	unsigned short port = RTSPUDP_BASEBEGINPORT;
	unsigned short endport = port + RTSPUDP_PORTNUM;

	for (; port <= endport; port += 2)
	{
		if (CanbindPort(port) && CanbindPort(port + 1))
		{
			getport = port;
			break;
		}
	}
	return getport;
}

void ARRTSPClient::ResponseHandler(RTSPClient* rtspClient, int resultCode, char* resultString)
{
	ARRTSPClient* thisObj = (ARRTSPClient*)rtspClient;
	thisObj->OnResponseHandler(resultCode, resultString);
}

void ARRTSPClient::OnResponseHandler(int resultCode, char* resultString)
{
	CARAutoLock locker(m_scheduleLock);

	if (waitResponseTask)
	{
		envir().taskScheduler().unscheduleDelayedTask(waitResponseTask);
		waitResponseTask = NULL;
	}

	m_resultCode = resultCode;
	m_resultString = resultString;
	m_watchVariable = ~0;
}

void ARRTSPClient::CheckAliveHandler(void* clientData)
{
	ARRTSPClient* thisObj = (ARRTSPClient*)clientData;
	thisObj->OnCheckAliveHandler();
}

void ARRTSPClient::OnCheckAliveHandler()
{
	INT64 nowMsel = GetTickCount();

	if ((nowMsel - lastRecvTime) > (RECV_TIMEOUT))
	{
		if (checkAliveTask != NULL)
		{
			envir().taskScheduler().unscheduleDelayedTask(checkAliveTask);
			checkAliveTask = NULL;
		}

		if (m_callback)
		{
			m_callback(m_callbackParam, NULL, 0, "", false, 0);
		}
	}
	else
	{
		if (checkAliveTask != NULL)
		{
			envir().taskScheduler().unscheduleDelayedTask(checkAliveTask);
			checkAliveTask = NULL;
		}
		checkAliveTask = envir().taskScheduler().scheduleDelayedTask(CHECKALIVE_TIMOUT, (TaskFunc*)CheckAliveHandler, (void*)this);
	}

	if (m_sendActiveCmd)
	{
		SendActiveCmd();
	}
}

void ARRTSPClient::SendActiveCmd()
{
	if (m_sendActiveCmd)
	{
		INT64 nowMsel = GetTickCount();
		// Delay a random time before sending another 'liveness' command.
		unsigned delayMax = sessionTimeoutParameter(); // if the server specified a maximum time between 'liveness' probes, then use that
		if (delayMax == 0) {
			delayMax = 10;
		}

		// Choose a random time from [delayMax/2,delayMax-1) seconds:
		unsigned const sec_1stPart = delayMax * 500;
		unsigned secondsToDelay;
		if (sec_1stPart <= 1000) {
			secondsToDelay = sec_1stPart;
		}
		else {
			unsigned const sec_2ndPart = sec_1stPart - 1000;
			secondsToDelay = sec_1stPart + (sec_2ndPart*our_random()) % sec_2ndPart;
		}

		if ((nowMsel - m_sendActiveCmdTime) >= secondsToDelay)
		{
			if (serverSupportsGetParameter)
			{
				sendGetParameterCommand(*m_session, ActiveCmdResponseHandler, NULL/*, authenticator*/);
			}
			else
			{
				sendOptionsCommand(ActiveCmdResponseHandler/*, authenticator*/);
			}

			m_sendActiveCmdTime = nowMsel;
		}
	}
}

void ARRTSPClient::ActiveCmdResponseHandler(RTSPClient* /*rtspClient*/, int /*resultCode*/, char* resultString)
{
	//暂时不处理
	if (resultString != NULL)
		delete[]resultString;
}

void ARRTSPClient::Close()
{
	if (checkAliveTask != NULL)
	{
		envir().taskScheduler().unscheduleDelayedTask(checkAliveTask);
		checkAliveTask = NULL;
	}

	if (waitResponseTask != NULL)
	{
		envir().taskScheduler().unscheduleDelayedTask(waitResponseTask);
		waitResponseTask = NULL;
	}

	m_watchVariable = ~0;

	if (!m_semAck.Wait(THREADEXIT_TIMEOUT))
	{
		printf((_T("%s : m_semAck.Wait %dms timeout, url = %s"), __FUNCTION__, THREADEXIT_TIMEOUT, url()));
	}
	else
	{
		printf((_T("%s : m_semAck.Wait ok, url = %s"), __FUNCTION__, url()));
	}

	// 	// 强行杀死线程
	// 	NPCRT_API_LockFlagCheck(liveRecvthreadID);
	// #if defined(WIN32)
	/*VERIFY*/(TerminateThread(live_recv_thread, DWORD(-1)));
	// #elif defined(__linux__)
	// 	VERIFY(0 == pthread_cancel(live_recv_thread));
	// #endif

	OSCloseThread(live_recv_thread);
	live_recv_thread = INVALID_OSTHREAD;
	liveRecvthreadID = INVALID_OSTHREADID;

	if (m_session != NULL)
	{
		bool someSubsessionsWereActive = False;

		MediaSubsessionIterator subIter(*m_session);
		MediaSubsession* subsession = NULL;
		subsession = NULL;
		while ((subsession = subIter.next()) != NULL)
		{
			if (subsession->sink != NULL)
			{
				subsession->sink->stopPlaying();
				Medium::close(subsession->sink);
				subsession->sink = NULL;
			}
			if (subsession->rtcpInstance() != NULL)
			{
				subsession->rtcpInstance()->setByeHandler(NULL, NULL);
			}
			someSubsessionsWereActive = true;
		}

		if (someSubsessionsWereActive)
		{
			sendTeardownCommand(*m_session, NULL);
		}

		Medium::close(m_session);
		m_session = NULL;
	}

	if (authenticator)
	{
		delete authenticator;
		authenticator = NULL;
	}

	CARRTSPClientCallbacks::Del(this);

	m_callback = NULL;
	m_callbackParam = NULL;
}

BOOL ARRTSPClient::OnData(BYTE* data, DWORD dataLen, const char* mimeType, BOOL isKeyFrame, DWORD pts)
{
	if (m_callback)
	{
		m_callback(m_callbackParam, data, dataLen, mimeType, isKeyFrame, pts);

		lastRecvTime = GetTickCount();
	}
	return TRUE;
}

CRTSPClient::CRTSPClient(const char* streamUrl,
	const char* userName, const char* passWord,
	BOOL bTcp,
	fDataCallBack cb, void* param,
	BOOL bSendActiveCmd)
{
	strcpy_s(m_url, streamUrl);
	strcpy_s(m_username, userName);
	strcpy_s(m_password, passWord);

	m_useTcp = bTcp;
	m_sendActiveCmd = bSendActiveCmd;
	m_rtpDataHandler = cb;
	m_cbParam = param;
}

CRTSPClient::~CRTSPClient()
{
}

BOOL CRTSPClient::Open()
{
	CARAutoLock locker(m_lock);

#ifdef WIN32
	m_scheduler = BasicTaskScheduler::createNew();
#else
	m_scheduler = BasicTaskSchedulerEpoll::createNew();
#endif

	m_env = BasicUsageEnvironment::createNew(*m_scheduler);

	m_rtspClient = ARRTSPClient::createNew(*m_env, m_url, RTSP_CLIENT_VERBOSITY_LEVEL, "ARClient", 0);

	if (m_rtspClient->Open(m_username, m_password, m_useTcp, m_rtpDataHandler, m_cbParam, m_sendActiveCmd))
		return true;

	Medium::close(m_rtspClient);
	m_rtspClient = NULL;
	m_env->reclaim();
	delete m_scheduler;

	return false;
}

void CRTSPClient::Close()
{
	CARAutoLock locker(m_lock);

	m_rtspClient->Close();
	Medium::close(m_rtspClient);
	m_rtspClient = NULL;
	m_env->reclaim();
	delete m_scheduler;
}