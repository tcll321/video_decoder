#ifndef _FILE_PUB_DEFS_
#define _FILE_PUB_DEFS_

#if _MSC_VER > 1000
#pragma once
#endif

#define FILEAPI_METHOD __stdcall

typedef enum
{
	FILE_API_OK			= 0,	// 成功
	FILE_API_ERR_OPEN	= -1,	// 打开文件失败
	FILE_API_ERR_READ	= -2,	// 读取文件失败
	FILE_API_ERR_CLOSE	= -3,	// 文件关闭失败
	FILE_API_ERR_NO_HANDLE	= -4,
	FILE_API_ERR_NULL_OBJECT =-5, 
	FILE_API_ERR_NULL_PTR	= -6
}FileApiErr;

#endif
