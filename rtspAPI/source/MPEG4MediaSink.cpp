#include "StdAfx.h"
#include "MPEG4MediaSink.h"
#include "MPEG4LATMAudioRTPSource.hh"

MPEG4MediaSink::MPEG4MediaSink(UsageEnvironment& env, const char* fmtpConfig, ARRTSPClient* client)
: MediaSink(env)
, m_bufferSize(1000000)
, m_isFirst(true)
, m_client(client)
{
	if(fmtpConfig)
	{
		unsigned len;
		m_sFmtpConfig = fmtpConfig;
		unsigned char* configData = parseGeneralConfigStr(fmtpConfig, len);
		m_buffer = new BYTE[len + m_bufferSize];
		BYTE* p = m_buffer;
		memcpy(p, configData, len);
		p += len;
		m_pData = p;
	}
	else
	{
		m_buffer = new BYTE[m_bufferSize];
		m_pData = m_buffer;
	}
}

MPEG4MediaSink::~MPEG4MediaSink() 
{
	delete[] m_buffer;
}

Boolean MPEG4MediaSink::continuePlaying()
{
	if(fSource == NULL)
		return False;

	fSource->getNextFrame(m_pData, m_bufferSize, afterGettingFrame, this, onSourceClosure, this);
	return True;
}

void MPEG4MediaSink::afterGettingFrame(void* clientData, unsigned frameSize, unsigned /*numTruncatedBytes*/
									, struct timeval presentationTime, unsigned /*durationInMicroseconds*/)
{
	MPEG4MediaSink* thisObj = (MPEG4MediaSink*)clientData;
	thisObj->OnAfterGettingFrame(frameSize, presentationTime);
}

void MPEG4MediaSink::stopPlaying()
{
	//m_client->OnData(NULL, 0, "", false, 0);
}

void MPEG4MediaSink::OnAfterGettingFrame(unsigned frameSize, struct timeval presentationTime)
{
	if (!CARRTSPClientCallbacks::IsExist(m_client))
	{
// 		NPLogWarning((_T("%s : context is not exist!"),  __FUNCTION__));
	}
	else
	{
		DWORD pts = DWORD((INT64)presentationTime.tv_sec*1000 + (presentationTime.tv_usec+500)/1000);
		if(m_isFirst && !m_sFmtpConfig.empty())
		{
			m_isFirst = false;
			m_client->OnData(m_buffer, (DWORD)(m_pData-m_buffer)+frameSize, fSource->MIMEtype(), true, pts);			
		}

		if(!m_isFirst)
		{
			BOOL isKeyFrame = (*(DWORD*)(m_pData)==0xB0010000);
			m_client->OnData(m_pData, frameSize, fSource->MIMEtype(), isKeyFrame, pts);
		}
	}	

	continuePlaying();
	return;
}



