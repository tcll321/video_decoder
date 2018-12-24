// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 RAHWDECODER_PRO_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// RAHWDECODER_PRO_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
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
