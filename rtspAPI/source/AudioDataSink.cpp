#include "StdAfx.h"
#include "AudioDataSink.h"

AudioDataSink::AudioDataSink(UsageEnvironment& env, ARRTSPClient* client, DWORD bufferSize)
: MediaSink(env)
, m_bufferSize(bufferSize)
, m_client(client)
{
	m_buffer = new BYTE[m_bufferSize];	
}

AudioDataSink::~AudioDataSink() 
{
	delete[] m_buffer;
}

Boolean AudioDataSink::continuePlaying()
{
	if(fSource == NULL)
		return False;

	fSource->getNextFrame(m_buffer, m_bufferSize, afterGettingFrame, this, onSourceClosure, this);
	return True;
}

void AudioDataSink::afterGettingFrame(void* clientData, unsigned frameSize, unsigned /*numTruncatedBytes*/
									, struct timeval presentationTime, unsigned /*durationInMicroseconds*/)
{
	AudioDataSink* thisObj = (AudioDataSink*)clientData;
	if(thisObj)
		thisObj->OnAfterGettingFrame(frameSize, presentationTime);
}
void AudioDataSink::stopPlaying()
{
	//m_client->OnData(NULL, 0, "", false, 0);
}

void AudioDataSink::OnAfterGettingFrame(unsigned frameSize, struct timeval presentationTime)
{
	DWORD pts = DWORD((INT64)presentationTime.tv_sec*1000 + (presentationTime.tv_usec+500)/1000);

	if (!CARRTSPClientCallbacks::IsExist(m_client))
	{
// 		NPLogWarning((_T("%s : context is not exist!"),  __FUNCTION__));
	}
	else
	{
		m_client->OnData(m_buffer, frameSize, fSource->MIMEtype(), false, pts);
	}

	continuePlaying();
	return;
}

