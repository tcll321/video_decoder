#ifndef H264MediaSink_h__
#define H264MediaSink_h__

#include "MediaSink.hh"
#include "ARRTSPClient.h"

class MediaBuffInfo
{
private:
	unsigned char *m_pBuff;
	unsigned int m_dwLen;
public:
	MediaBuffInfo(unsigned int len = 0)	
	{
		if(len == 0)
		{
			m_pBuff = NULL;
			m_dwLen = 0;
			return;
		}
		m_pBuff = new unsigned char[len];
		if(m_pBuff != NULL)
		{
			m_dwLen = len;
		}
	}
	~MediaBuffInfo()
	{
		if(m_pBuff != NULL)
		{
			delete []m_pBuff;
			m_pBuff = NULL;
		}
		m_dwLen = 0;
	}
	int Resize(unsigned int len)
	{
		if(m_dwLen == len) return m_dwLen;
		
		if(m_pBuff != NULL)
		{
			delete []m_pBuff;
			m_pBuff = NULL;
		}
		m_pBuff = new unsigned char[len];
		if(m_pBuff != NULL)
		{
			m_dwLen = len;
			return m_dwLen;
		}
		else
		{
			m_dwLen = 0;
		}

		return m_dwLen;
	}
	unsigned char *Get()
	{
		return m_pBuff;
	}
	unsigned int Length()
	{
		return m_dwLen;
	}
	bool IsEmpty()
	{
		return (m_dwLen > 0)?false:true;
	}
};

class H264MediaSink: public MediaSink
{
public:
	//H264MediaSink(UsageEnvironment& env, const char* sPropParameterSetsStr, NPRTSPClient* client);
	H264MediaSink(UsageEnvironment& env, MediaSubsession* subsession, ARRTSPClient* client);
	virtual ~H264MediaSink();

protected:
	static void afterGettingFrame(void* clientData, unsigned frameSize
		, unsigned numTruncatedBytes
		, struct timeval presentationTime
		, unsigned durationInMicroseconds);
	void OnAfterGettingFrame(unsigned frameSize, struct timeval presentationTime);

public:
	virtual void stopPlaying();

private: // redefined virtual functions:
	virtual Boolean continuePlaying();

	char* mimeType;
	
	MediaBuffInfo m_spsHeader;
	MediaBuffInfo m_ppsHeader;
	
	MediaBuffInfo m_RecvBuffer;
	BYTE* m_recvPtr;
	
	MediaSubsession* subsession_ptr;

	BOOL m_spsSet;
	BOOL m_ppsSet;
	BOOL m_isFirst;
	ARRTSPClient* m_client;
};

#endif // H264MediaSink_h__

