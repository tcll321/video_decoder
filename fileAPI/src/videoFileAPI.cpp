// fileAPI.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "videoFileAPI.h"
#include "IDManager.h"
#include "FFmpegReader.h"

typedef CIDAutoBusy<CIDManager<FILE_ID>, FILE_ID> ChannelIDAutoBusy;
static CIDManager<FILE_ID> s_fileIDManager;

#define FILE_GETOBJECT(pObj, ClassName, findID, Parameter) \
	IDInfo<FILE_ID> IDInfo; \
	if(!s_fileIDManager.Find(findID, &IDInfo)) \
	return FILE_API_ERR_NO_HANDLE; \
	ClassName* pObj = (ClassName*)IDInfo.Parameter; \
	if(!pObj) \
	return FILE_API_ERR_NULL_OBJECT

FILEAPI_API FileApiErr FILEAPI_METHOD Video_Open(FILE_ID *id, const char* file)
{
	FileApiErr err = FILE_API_OK;
	CFFmpegReader* reader = new CFFmpegReader();
	if (NULL == reader)
		return FILE_API_ERR_NULL_PTR;
	err = reader->Init(file);
	if (FILE_API_OK == err)
	{
		*id = s_fileIDManager.New("ffreader", (void*)reader);
		if (NULL == *id)
		{
			delete reader;
			return FILE_API_ERR_NULL_PTR;
		}
	}
	else
	{
		delete reader;
	}
	return err;
}

FILEAPI_API FileApiErr FILEAPI_METHOD Video_Read(FILE_ID id, char** data, int* datalen)
{
	ChannelIDAutoBusy autoBusy(s_fileIDManager, id);
	FILE_GETOBJECT(pReader, CFFmpegReader, id, param);
	return pReader->Read(*data, datalen);
}

FILEAPI_API FileApiErr FILEAPI_METHOD Video_DataFree(char* ptr)
{
	if (ptr)
		delete ptr;
	return FILE_API_OK;
}

FILEAPI_API FileApiErr FILEAPI_METHOD Video_Close(FILE_ID id)
{
	FileApiErr err = FILE_API_OK;
	ChannelIDAutoBusy autoBusy(s_fileIDManager, id);
	FILE_GETOBJECT(pReader, CFFmpegReader, id, param);
	delete pReader;
	pReader = NULL;
	return err;
}