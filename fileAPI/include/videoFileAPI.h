// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� FILEAPI_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// FILEAPI_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef FILEAPI_EXPORTS
#define FILEAPI_API __declspec(dllexport)
#else
#define FILEAPI_API __declspec(dllimport)
#endif

#include "file_pub_defs.h"

typedef struct FILE_ID_* FILE_ID;

FILEAPI_API FileApiErr FILEAPI_METHOD Video_Open(FILE_ID *id, const char* file);

FILEAPI_API FileApiErr FILEAPI_METHOD Video_Read(FILE_ID id, char** data, int* datalen);

FILEAPI_API FileApiErr FILEAPI_METHOD Video_DataFree(char* ptr);

FILEAPI_API FileApiErr FILEAPI_METHOD Video_Close(FILE_ID id);
