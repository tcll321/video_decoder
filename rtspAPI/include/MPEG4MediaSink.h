#ifndef MPEG4MediaSink_h__
#define MPEG4MediaSink_h__

#include "MediaSink.hh"
#include "ARRTSPClient.h"
#include <string>

class MPEG4MediaSink: public MediaSink
{
public:
	MPEG4MediaSink(UsageEnvironment& env, const char* fmtpConfig, ARRTSPClient* client);
	virtual ~MPEG4MediaSink();

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
	
	std::string m_sFmtpConfig;
	BYTE* m_buffer;
	BYTE* m_pData;
	DWORD m_bufferSize;
	
 	BOOL m_isFirst;
	ARRTSPClient* m_client;
};

#endif // MPEG4MediaSink_h__

