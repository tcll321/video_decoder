#ifndef AudioDataSink_h__
#define AudioDataSink_h__

#include "MediaSink.hh"
#include "ARRTSPClient.h"

class AudioDataSink: public MediaSink
{
public:
	AudioDataSink(UsageEnvironment& env, ARRTSPClient* client, DWORD bufferSize);
	virtual ~AudioDataSink();

protected:
	static void afterGettingFrame(void* clientData, unsigned frameSize
		, unsigned numTruncatedBytes
		, struct timeval presentationTime
		, unsigned durationInMicroseconds);
	void OnAfterGettingFrame(unsigned frameSize, struct timeval presentationTime);

	BYTE* m_buffer;
	DWORD m_bufferSize;

public:
	virtual void stopPlaying();

private: // redefined virtual functions:
	virtual Boolean continuePlaying();
	ARRTSPClient* m_client;
};

#endif // AudioDataSink_h__

