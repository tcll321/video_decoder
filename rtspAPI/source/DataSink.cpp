#include "StdAfx.h"
#include "DataSink.h"

DataSink::DataSink(UsageEnvironment& env, ARRTSPClient* client, DWORD bufferSize)
: MediaSink(env)
, m_bufferSize(bufferSize)
, m_client(client)
{
	m_buffer = new BYTE[m_bufferSize];	
}

DataSink::~DataSink() 
{
	delete[] m_buffer;
}

Boolean DataSink::continuePlaying()
{
	if(fSource == NULL)
		return False;

	fSource->getNextFrame(m_buffer, m_bufferSize, afterGettingFrame, this, onSourceClosure, this);
	return True;
}

void DataSink::afterGettingFrame(void* clientData, unsigned frameSize, unsigned /*numTruncatedBytes*/
									, struct timeval presentationTime, unsigned /*durationInMicroseconds*/)
{
	DataSink* thisObj = (DataSink*)clientData;
	thisObj->OnAfterGettingFrame(frameSize, presentationTime);
}
void DataSink::stopPlaying()
{
	//m_client->OnData(NULL, 0, "", false, 0);
}

void DataSink::OnAfterGettingFrame(unsigned frameSize, struct timeval presentationTime)
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

