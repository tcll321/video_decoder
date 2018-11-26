#ifndef _DEVICE_INFO_
#define _DEVICE_INFO_

#if _MSC_VER>1000
#pragma once
#endif

#include "cuda.h"

int GetDeviceCount();
int GetDeviceInfo(int devNo, DeviceInfo *info);

inline int GetDeviceCount()
{
	int nGpuCount = 0;
	cuDeviceGetCount(&nGpuCount);
	return nGpuCount;
}

inline int GetDeviceInfo(int devNo, DeviceInfo *info)
{
	int nGpuCount = 0;
	CUresult res = CUDA_SUCCESS;
	cuDeviceGetCount(&nGpuCount);
	if (devNo < 0 || devNo>=nGpuCount)
	{
		printf("input device number err!!\n");
		return -1;
	}
	res = cuDeviceGet(&info->devId, devNo);
	if (res != CUDA_SUCCESS)
	{
		printf("cuDeviceGet err!! errno=%d\n", res);
		return res;
	}
	res = cuDeviceGetName(info->devName, sizeof(info->devName), info->devId);
	if (res != CUDA_SUCCESS)
	{
		printf("cuDeviceGetName err!! errno=%d\n", res);
		return res;
	}
	return CUDA_SUCCESS;
}

#endif
