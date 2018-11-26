// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 FILEAPI_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// FILEAPI_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
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
