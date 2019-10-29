#include "StdAfx.h"
#include "H264MediaSink.h"
#include "H264VideoRTPSource.hh"
#include "H264VideoStreamFramer.hh"

#define MAX_RECV_BUFLEN (10*1024*1024)

H264MediaSink::H264MediaSink(UsageEnvironment& env, MediaSubsession* subsession, ARRTSPClient* client)
: MediaSink(env)
, subsession_ptr(subsession)
, m_spsSet(false)
, m_isFirst(true)
, mimeType(NULL)
, m_client(client)
{
	//if(sPropParameterSetsStr)
	if(subsession->fmtp_spropparametersets())
	{
		// If we have PPS/SPS NAL units encoded in a "sprop parameter string", prepend these to the file:
		unsigned numSPropRecords;
		//SPropRecord* sPropRecords = parseSPropParameterSets(sPropParameterSetsStr, numSPropRecords);
		SPropRecord* sPropRecords = parseSPropParameterSets(subsession->fmtp_spropparametersets(), numSPropRecords);
		
		for(unsigned i = 0; i < numSPropRecords; ++i) 
		{			
			BYTE* data = sPropRecords[i].sPropBytes;
			unsigned dataLen = sPropRecords[i].sPropLength;
			if(i == 0)
			{
				m_spsHeader.Resize(dataLen);
				memcpy(m_spsHeader.Get(), data, dataLen);
			}
			else if(i == 1)
			{
				m_ppsHeader.Resize(dataLen);
				memcpy(m_ppsHeader.Get(), data, dataLen);
			}
		}		
		delete[] sPropRecords;
	}

	DWORD len = 1024+1024 + MAX_RECV_BUFLEN;
 	
	m_RecvBuffer.Resize(len);
 	m_recvPtr = m_RecvBuffer.Get() + (m_RecvBuffer.Length() - MAX_RECV_BUFLEN);

// 	mimeType = new char[strlen(subsession_ptr->mediumName()) + strlen(subsession_ptr->codecName()) + 2];  
// 	sprintf(mimeType, "%s/%s", subsession_ptr->mediumName(), subsession_ptr->codecName());
}

H264MediaSink::~H264MediaSink() 
{
// 	delete[] mimeType;
}

Boolean H264MediaSink::continuePlaying()
{
	if(fSource == NULL)
		return False;

	fSource->getNextFrame(m_recvPtr, MAX_RECV_BUFLEN, afterGettingFrame, this, onSourceClosure, this);
	return True;
}

void H264MediaSink::afterGettingFrame(void* clientData, unsigned frameSize, unsigned /*numTruncatedBytes*/
									, struct timeval presentationTime, unsigned /*durationInMicroseconds*/)
{
	H264MediaSink* thisObj = (H264MediaSink*)clientData;
	if(thisObj != NULL)
	{
		thisObj->OnAfterGettingFrame(frameSize, presentationTime);
	}
	
}
/*
char const* nal_unit_type_description[32] = {
	"Unspecified", //0
	"Coded slice of a non-IDR picture", //1
	"Coded slice data partition A", //2
	"Coded slice data partition B", //3
	"Coded slice data partition C", //4
	"Coded slice of an IDR picture", //5
	"Supplemental enhancement information (SEI)", //6
	"Sequence parameter set", //7
	"Picture parameter set", //8
	"Access unit delimiter", //9
	"End of sequence", //10
	"End of stream", //11
	"Filler data", //12
	"Sequence parameter set extension", //13
	"Prefix NAL unit", //14
	"Subset sequence parameter set", //15
	"Reserved", //16
	"Reserved", //17
	"Reserved", //18
	"Coded slice of an auxiliary coded picture without partitioning", //19
	"Coded slice extension", //20
	"Reserved", //21
	"Reserved", //22
	"Reserved", //23
	"Unspecified", //24
	"Unspecified", //25
	"Unspecified", //26
	"Unspecified", //27
	"Unspecified", //28
	"Unspecified", //29
	"Unspecified", //30
	"Unspecified" //31
};
*/
void H264MediaSink::stopPlaying()
{
	//m_client->OnData(NULL, 0, "", false, 0);
}

void H264MediaSink::OnAfterGettingFrame(unsigned frameSize, struct timeval presentationTime)
{
	if (!CARRTSPClientCallbacks::IsExist(m_client))
	{
// 		NPLogWarning((_T("%s : context is not exist!"),  __FUNCTION__));
	}
	else
	{
		do {
			if (frameSize <= 0) break;
			
			BYTE start_code[4] = {0x00, 0x00, 0x00, 0x01};
			INT64 pts = ((INT64)presentationTime.tv_sec*1000 + (presentationTime.tv_usec+500)/1000);

			if (frameSize >= 4 && m_recvPtr[0] == 0 && m_recvPtr[1] == 0 && ((m_recvPtr[2] == 0 && m_recvPtr[3] == 1) || m_recvPtr[2] == 1))
			{
				if (m_recvPtr[2] == 1)
				{
					m_recvPtr += 3;
					frameSize -= 3;
				} 
				else
				{
					m_recvPtr += 4;
					frameSize -= 4;
				}
			}

			BYTE flag = *(m_recvPtr) & 0x1f;

// 			if(frameSize >= 4 && flag == 13) // Sequence parameter set extension
// 			{
// 				int i = 0;
// 				while (
// 					(m_recvPtr[i] != 0 || m_recvPtr[i+1] != 0 || m_recvPtr[i+2] != 0x01) && 
// 					(m_recvPtr[i] != 0 || m_recvPtr[i+1] != 0 || m_recvPtr[i+2] != 0 || m_recvPtr[i+3] != 0x01) 
// 					)
// 				{
// 					i++; 
// 					if (i+4 >= frameSize) 
// 					{
// 						i = -1;
// 						break; 
// 					} 
// 				}
// 				if (i == -1) break;
// 
// 				if  (m_recvPtr[i] != 0 || m_recvPtr[i+1] != 0 || m_recvPtr[i+2] != 0x01) 
// 				{
// 					i++;
// 				}
// 
// 				if  (m_recvPtr[i] != 0 || m_recvPtr[i+1] != 0 || m_recvPtr[i+2] != 0x01) break;
// 				
// 				i+= 3;
// 				m_recvPtr += i;
// 				flag = *(m_recvPtr) & 0x1f;
// 				frameSize -= i;
// 			}

			//NPLogWarning((_T("%d : %d"),  flag, frameSize));
			if(flag == 7) // Sequence parameter set
			{
				m_spsSet = true;
				if(frameSize > m_spsHeader.Length() + 4)
				{
					m_isFirst = false;
					m_spsSet = false;
					DWORD bufLen = frameSize;
					BYTE* p = m_recvPtr;
					p -= sizeof(start_code);
					bufLen += sizeof(start_code);
					memcpy(p, start_code, sizeof(start_code));
					m_client->OnData(p, bufLen, /*mimeType*/fSource->MIMEtype(), true, pts);
				}
				else
				{
					if(frameSize > 0 && frameSize < 1020)
					{
						m_spsHeader.Resize(frameSize);
						memcpy(m_spsHeader.Get(), m_recvPtr, frameSize);
					}				
				}
			}
			else if(flag == 8) // Picture parameter set
			{		
				if(frameSize > 0 && frameSize < 1020)
				{
					m_ppsHeader.Resize(frameSize);
					memcpy(m_ppsHeader.Get(), m_recvPtr, frameSize);
				}
			}
			else if(flag == 5) // IDR 
			{
				m_isFirst = false;
				m_spsSet = false;
				DWORD bufLen = frameSize;
				BYTE* p = m_recvPtr;
				p -= sizeof(start_code);
				bufLen += sizeof(start_code);
				memcpy(p, start_code, sizeof(start_code));

				if(!m_ppsHeader.IsEmpty())
				{
					p -= m_ppsHeader.Length();
					bufLen += m_ppsHeader.Length();
					memcpy(p, m_ppsHeader.Get(), m_ppsHeader.Length());
					p -= sizeof(start_code);
					bufLen += sizeof(start_code);
					memcpy(p, start_code, sizeof(start_code));
				}
				if(!m_spsHeader.IsEmpty())
				{
					p -= m_spsHeader.Length();
					bufLen += m_spsHeader.Length();
					memcpy(p, m_spsHeader.Get(), m_spsHeader.Length());
					p -= sizeof(start_code);
					bufLen += sizeof(start_code);
					memcpy(p, start_code, sizeof(start_code));
				}

				m_client->OnData(p, bufLen, /*mimeType*/fSource->MIMEtype(), true, pts);
			}
			else if(flag == 1) // non-IDR 
			{
				BOOL isKeyFrame = false;
				if(m_spsSet)
				{
					m_spsSet = false;
					isKeyFrame = true;
				}
				if(!m_isFirst)
				{
					DWORD bufLen = frameSize;
					BYTE* p = m_recvPtr;
					p -= sizeof(start_code);
					bufLen += sizeof(start_code);
					memcpy(p, start_code, sizeof(start_code));
					if(isKeyFrame)
					{
						if(!m_ppsHeader.IsEmpty())
						{
							p -= m_ppsHeader.Length();
							bufLen += m_ppsHeader.Length();
							memcpy(p, m_ppsHeader.Get(), m_ppsHeader.Length());
							p -= sizeof(start_code);
							bufLen += sizeof(start_code);
							memcpy(p, start_code, sizeof(start_code));
						}
						if(!m_spsHeader.IsEmpty())
						{
							p -= m_spsHeader.Length();
							bufLen += m_spsHeader.Length();
							memcpy(p, m_spsHeader.Get(), m_spsHeader.Length());
							p -= sizeof(start_code);
							bufLen += sizeof(start_code);
							memcpy(p, start_code, sizeof(start_code));
						}	
					}		
					m_client->OnData(p, bufLen, /*mimeType*/fSource->MIMEtype(), isKeyFrame, pts);
				}		
			}
			else if(flag == 9)
			{
				if(frameSize > 2)
				{
					DWORD bufLen = frameSize-2;
					BYTE* sendPtr = m_recvPtr + 2;
					m_client->OnData(sendPtr, bufLen, /*mimeType*/fSource->MIMEtype(), false, pts);
				}		
			}
// 			else
// 			{
// 				DWORD bufLen = frameSize;
// 				BYTE* p = m_recvPtr;
// 				p -= sizeof(start_code);
// 				bufLen += sizeof(start_code);
// 				memcpy(p, start_code, sizeof(start_code));
// 				m_client->OnData(p, bufLen, /*mimeType*/fSource->MIMEtype(), true, pts);
// 			}
		} while (false);
	}

	continuePlaying();
	return;
}


