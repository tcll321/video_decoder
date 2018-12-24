// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� RAHWDECODER_PRO_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// RAHWDECODER_PRO_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef RAHWDECODER_PRO_EXPORTS
#define RAHWDECODER_PRO_API __declspec(dllexport)
#else
#define RAHWDECODER_PRO_API __declspec(dllimport)
#endif

#include "pubDefs.h"

typedef struct CHANNEL_ID_* CHANNEL_ID;

RAHWDECODER_PRO_API DecoderError DECODER_METHOD HW_GetDeviceCount(int *count);

RAHWDECODER_PRO_API DecoderError DECODER_METHOD HW_GetDeviceInfo(int devNo, DeviceInfo* info);

RAHWDECODER_PRO_API DecoderError DECODER_METHOD HW_CreateDecoder(CHANNEL_ID *id);

RAHWDECODER_PRO_API DecoderError DECODER_METHOD HW_DeleteDecoder(CHANNEL_ID id);

RAHWDECODER_PRO_API DecoderError DECODER_METHOD HW_InitDecoder(CHANNEL_ID id, int devId, int width, int height, VideoCodec codec);

RAHWDECODER_PRO_API DecoderError DECODER_METHOD HW_Decod(CHANNEL_ID id, const unsigned char* data, int datalen,	unsigned char** ppFrame, int* frameSize, int* width, int* height, int* framCount);
