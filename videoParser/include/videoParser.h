// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� VIDEOPARSER_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// VIDEOPARSER_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef VIDEOPARSER_EXPORTS
#define VIDEOPARSER_API __declspec(dllexport)
#else
#define VIDEOPARSER_API __declspec(dllimport)
#endif

#include "defines.h"

VIDEOPARSER_API void* Init(AVCodecType type);
VIDEOPARSER_API void Clear(void* context);
VIDEOPARSER_API int	 Parser(void * context, const char* data, int len, VideoInfo* info);