#include "ColorUtils.h"
#include "NvCodecUtils.h"
#include "cuda.h"
#include "cuda_runtime.h"
#include "device_launch_parameters.h"


CColorUtils::CColorUtils(int deviceID)
	:m_deviceID(deviceID)
	, m_cuContext(NULL)
{
// 	cuCtxCreate(&m_cuContext, CU_CTX_SCHED_BLOCKING_SYNC, m_deviceID);
}


CColorUtils::~CColorUtils()
{
	if (m_cuContext)
	{
// 		cuCtxDestroy(m_cuContext);
		m_cuContext = NULL;
	}
}

int CColorUtils::cudaNv12ToBgr24(uint8_t *dpNv12, int nNv12Pitch, uint8_t **dpBgra, int nBgraPitch, int nWidth, int nHeight, int iMatrix)
{
	cudaError_t cudaStatus;
	CUdeviceptr dpFrame = 0;
	cudaStatus = cudaSetDevice(m_deviceID);
	if (cudaStatus != cudaSuccess)
	{
		fprintf(stderr, "cudaGetDevice failed: %s\n", cudaGetErrorString(cudaStatus));
		return -1;
	}
	CUresult ret = CUDA_SUCCESS;

	// 	if (!ck(cuMemAlloc(&dpFrame, nWidth * nHeight * 4)))
	// 		return -1;
	unsigned char* hostChar = NULL;
// 	cuCtxPushCurrent(m_cuContext);
	ret = cuMemAlloc(&dpFrame, nWidth * nHeight * 3);
	Nv12ToBgr32(dpNv12, nWidth, (uint8_t*)dpFrame, 3 * nWidth, nWidth, nHeight);
// 	cuCtxPopCurrent(NULL);
	cudaDeviceSynchronize();

	*dpBgra = (uint8_t*)dpFrame;
	return 0;
}

int CColorUtils::cudaNv12ToRgb24(uint8_t *dpNv12, int nNv12Pitch, uint8_t **dpRgb, int nRgbPitch, int nWidth, int nHeight, int iMatrix)
{
	cudaError_t cudaStatus;
	CUdeviceptr dpFrame = 0;
	cudaStatus = cudaSetDevice(m_deviceID);
	if (cudaStatus != cudaSuccess)
	{
		fprintf(stderr, "cudaGetDevice failed: %s\n", cudaGetErrorString(cudaStatus));
		return -1;
	}
	CUresult ret = CUDA_SUCCESS;

	// 	if (!ck(cuMemAlloc(&dpFrame, nWidth * nHeight * 4)))
	// 		return -1;
	unsigned char* hostChar = NULL;
// 	cuCtxPushCurrent(m_cuContext);
	ret = cuMemAlloc(&dpFrame, nWidth * nHeight * 3);
	Nv12ToBgr32(dpNv12, nWidth, (uint8_t*)dpFrame, 3 * nWidth, nWidth, nHeight);
// 	cuCtxPopCurrent(NULL);
	*dpRgb = (uint8_t*)dpFrame;
	return 0;
}

int CColorUtils::cudaNv12ToBgra32(uint8_t *dpNv12, int nNv12Pitch, uint8_t **dpBgra, int nBgraPitch, int nWidth, int nHeight, int iMatrix)
{
	cudaError_t cudaStatus;
	CUdeviceptr dpFrame = 0;
// 	cudaStatus = cudaSetDevice(m_deviceID);
// 	if (cudaStatus != cudaSuccess)
// 	{
// 		fprintf(stderr, "cudaGetDevice failed: %s\n", cudaGetErrorString(cudaStatus));
// 		return -1;
// 	}
	CUresult ret = CUDA_SUCCESS;

// 	if (!ck(cuMemAlloc(&dpFrame, nWidth * nHeight * 4)))
// 		return -1;
	unsigned char* hostChar = NULL;
// 	cuCtxPushCurrent(m_cuContext);
	ret = cuMemAlloc(&dpFrame, nWidth * nHeight * 4);
	Nv12ToBgra32(dpNv12, nWidth, (uint8_t*)dpFrame, 4 * nWidth, nWidth, nHeight);
// 	cuCtxPopCurrent(NULL);
	*dpBgra = (uint8_t*)dpFrame;
	return 0;
}

int CColorUtils::cudaNv12ToBgra64(uint8_t *dpNv12, int nNv12Pitch, uint8_t *dpBgra, int nBgraPitch, int nWidth, int nHeight, int iMatrix)
{
	cudaError_t cudaStatus;
	CUdeviceptr dpFrame = 0;
	cudaStatus = cudaSetDevice(m_deviceID);
	if (cudaStatus != cudaSuccess)
	{
		fprintf(stderr, "cudaGetDevice failed: %s\n", cudaGetErrorString(cudaStatus));
		return -1;
	}
	ck(cuMemAlloc(&dpFrame, nWidth * nHeight * 4));
	Nv12ToBgra64(dpNv12, nWidth, (uint8_t *)dpFrame, 4 * nWidth, nWidth, nHeight);
	cudaStatus = cudaDeviceSynchronize();
	if (cudaStatus != cudaSuccess)
	{
		fprintf(stderr, "memcpy host to device failed: %s\n", cudaGetErrorString(cudaStatus));
		return -1;
	}
	return 0;
}

int CColorUtils::cudaP016ToBgra32(uint8_t *dpP016, int nP016Pitch, uint8_t *dpBgra, int nBgraPitch, int nWidth, int nHeight, int iMatrix)
{
	return 0;
}

int CColorUtils::cudaP016ToBgra64(uint8_t *dpP016, int nP016Pitch, uint8_t *dpBgra, int nBgraPitch, int nWidth, int nHeight, int iMatrix)
{
	return 0;
}

int CColorUtils::Nv12ToBgrPlanar(uint8_t *dpNv12, int nNv12Pitch, uint8_t *dpBgrp, int nBgrpPitch, int nWidth, int nHeight, int iMatrix)
{
	return 0;
}

int CColorUtils::P016ToBgrPlanar(uint8_t *dpP016, int nP016Pitch, uint8_t *dpBgrp, int nBgrpPitch, int nWidth, int nHeight, int iMatrix)
{
	return 0;
}

int CColorUtils::Bgra64ToP016(uint8_t *dpBgra, int nBgraPitch, uint8_t *dpP016, int nP016Pitch, int nWidth, int nHeight, int iMatrix)
{
	return 0;
}

int CColorUtils::ConvertUInt8ToUInt16(uint8_t *dpUInt8, uint16_t *dpUInt16, int nSrcPitch, int nDestPitch, int nWidth, int nHeight)
{
	return 0;
}
int CColorUtils::ConvertUInt16ToUInt8(uint16_t *dpUInt16, uint8_t *dpUInt8, int nSrcPitch, int nDestPitch, int nWidth, int nHeight)
{
	return 0;
}

int CColorUtils::ResizeNv12(unsigned char *dpDstNv12, int nDstPitch, int nDstWidth, int nDstHeight, unsigned char *dpSrcNv12, int nSrcPitch, int nSrcWidth, int nSrcHeight, unsigned char *dpDstNv12UV)
{
	return 0;
}

int CColorUtils::ResizeP016(unsigned char *dpDstP016, int nDstPitch, int nDstWidth, int nDstHeight, unsigned char *dpSrcP016, int nSrcPitch, int nSrcWidth, int nSrcHeight, unsigned char *dpDstP016UV)
{
	return 0;
}

int CColorUtils::ScaleYUV420(unsigned char *dpDstY, unsigned char* dpDstU, unsigned char* dpDstV, int nDstPitch, int nDstChromaPitch, int nDstWidth, int nDstHeight,
	unsigned char *dpSrcY, unsigned char* dpSrcU, unsigned char* dpSrcV, int nSrcPitch, int nSrcChromaPitch, int nSrcWidth, int nSrcHeight, bool bSemiplanar)
{
	return 0;
}

int CColorUtils::MattingImage(void* srcImage, int srcWidth, int srcHeight, void* dstImage, int x, int y, int dstWidth, int dstHeight)
{
	if ((x+dstWidth)>srcWidth || (y+dstHeight)>srcHeight)
	{
		return -1;
	}
	cudaError_t cudaStatus;
	unsigned char* pDstImage = NULL;
	int	nDevId;
	int dstImageSize = dstWidth*dstHeight * 3;
	cudaStatus = cudaSetDevice(m_deviceID);
	if (cudaStatus != cudaSuccess)
	{
		fprintf(stderr, "cudaGetDevice failed: %s\n", cudaGetErrorString(cudaStatus));
		return -1;
	}
// 	cuCtxPushCurrent(m_cuContext);

	cudaStatus = cudaMalloc((void**)&pDstImage, dstImageSize);
	if (cudaStatus != cudaSuccess)
	{
		cudaFree(pDstImage);
		fprintf(stderr, "cudaMalloc failed: %s\n", cudaGetErrorString(cudaStatus));
		return -2;
	}
	int ret = MattingPicture(srcImage, srcWidth, srcHeight, pDstImage, x, y, dstWidth, dstHeight);
	if (ret != 0)
	{
		cudaFree(pDstImage);
		fprintf(stderr, "kernel_MattingPicture failed: %s\n", cudaGetErrorString(cudaStatus));
		return -3;
	}
	cudaStatus = cudaMemcpy(dstImage, pDstImage, dstWidth*dstHeight * 3, cudaMemcpyDeviceToHost);
	if (cudaStatus != cudaSuccess)
	{
		cudaFree(pDstImage);
		fprintf(stderr, "memcpy device to host failed: %s\n", cudaGetErrorString(cudaStatus));
		return -4;
	}
	cudaStatus = cudaDeviceSynchronize();
	if (cudaStatus != cudaSuccess)
	{
		cudaFree(pDstImage);
		fprintf(stderr, "cudaDeviceSynchronize failed: %s\n", cudaGetErrorString(cudaStatus));
		return -5;
	}
	cudaFree(pDstImage);
// 	cuCtxPopCurrent(NULL);
	return 0;
}

int CColorUtils::MemcpyHostToDev(void ** dstImage, int dstLen, void * srcImage, int srcLen)
{
	cudaError_t cudaStatus;
	cudaStatus = cudaSetDevice(m_deviceID);
	if (cudaStatus != cudaSuccess)
	{
		fprintf(stderr, "cudaGetDevice failed: %s\n", cudaGetErrorString(cudaStatus));
		return -1;
	}
	cudaStatus = cudaMalloc(dstImage, dstLen);
	if (cudaStatus != cudaSuccess)
	{
		fprintf(stderr, "cudaMalloc failed: %s\n", cudaGetErrorString(cudaStatus));
		return -1;
	}
	cudaStatus = cudaMemcpy(*dstImage, srcImage, dstLen, cudaMemcpyHostToDevice);
	if (cudaStatus != cudaSuccess)
	{
		cudaFree(dstImage);
		fprintf(stderr, "memcpy host to device failed: %s\n", cudaGetErrorString(cudaStatus));
		return -1;
	}
	cudaStatus = cudaDeviceSynchronize();
	if (cudaStatus != cudaSuccess)
	{
		cudaFree(dstImage);
		fprintf(stderr, "memcpy host to device failed: %s\n", cudaGetErrorString(cudaStatus));
		return -1;
	}
	return 0;
}

int CColorUtils::MemcpyDevToHost(void * dstImage, int dstLen, void * srcImage, int srcLen)
{
	cudaError_t cudaStatus;
// 	cudaStatus = cudaSetDevice(m_deviceID);
// 	if (cudaStatus != cudaSuccess)
// 	{
// 		fprintf(stderr, "cudaGetDevice failed: %s\n", cudaGetErrorString(cudaStatus));
// 		return -1;
// 	}
// 	cuCtxPushCurrent(m_cuContext);
	cudaStatus = cudaMemcpy(dstImage, srcImage, dstLen, cudaMemcpyDeviceToHost);
	if (cudaStatus != cudaSuccess)
	{
		fprintf(stderr, "memcpy host to device failed: %s\n", cudaGetErrorString(cudaStatus));
		return -1;
	}
	cudaStatus = cudaDeviceSynchronize();
	if (cudaStatus != cudaSuccess)
	{
		fprintf(stderr, "memcpy host to device failed: %s\n", cudaGetErrorString(cudaStatus));
		return -1;
	}
// 	cuCtxPopCurrent(NULL);
	return 0;
}

void CColorUtils::FreeGpuMem(void * p)
{
	if (p)
	{
		cudaFree(p);
		p = NULL;
	}
}

