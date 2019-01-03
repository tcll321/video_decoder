#pragma once
#include <stdint.h>"
#include "ColorUtilsInterface.h"
#include <cuda.h>

class CColorUtils
	:public ColorUtilsInterface
{
public:
	CColorUtils(int deviceID);
	~CColorUtils();

	int cudaNv12ToBgr24(uint8_t *dpNv12, int nNv12Pitch, uint8_t **dpBgra, int nBgraPitch, int nWidth, int nHeight, int iMatrix = 0);
	int cudaNv12ToRgb24(uint8_t *dpNv12, int nNv12Pitch, uint8_t **dpRgb, int nRgbPitch, int nWidth, int nHeight, int iMatrix = 0);

	int cudaNv12ToBgra32(uint8_t *dpNv12, int nNv12Pitch, uint8_t **dpBgra, int nBgraPitch, int nWidth, int nHeight, int iMatrix = 0);
	int cudaNv12ToBgra64(uint8_t *dpNv12, int nNv12Pitch, uint8_t *dpBgra, int nBgraPitch, int nWidth, int nHeight, int iMatrix = 0);

	int cudaP016ToBgra32(uint8_t *dpP016, int nP016Pitch, uint8_t *dpBgra, int nBgraPitch, int nWidth, int nHeight, int iMatrix = 4);
	int cudaP016ToBgra64(uint8_t *dpP016, int nP016Pitch, uint8_t *dpBgra, int nBgraPitch, int nWidth, int nHeight, int iMatrix = 4);

	int Nv12ToBgrPlanar(uint8_t *dpNv12, int nNv12Pitch, uint8_t *dpBgrp, int nBgrpPitch, int nWidth, int nHeight, int iMatrix = 0);
	int P016ToBgrPlanar(uint8_t *dpP016, int nP016Pitch, uint8_t *dpBgrp, int nBgrpPitch, int nWidth, int nHeight, int iMatrix = 4);

	int Bgra64ToP016(uint8_t *dpBgra, int nBgraPitch, uint8_t *dpP016, int nP016Pitch, int nWidth, int nHeight, int iMatrix = 4);

	int ConvertUInt8ToUInt16(uint8_t *dpUInt8, uint16_t *dpUInt16, int nSrcPitch, int nDestPitch, int nWidth, int nHeight);
	int ConvertUInt16ToUInt8(uint16_t *dpUInt16, uint8_t *dpUInt8, int nSrcPitch, int nDestPitch, int nWidth, int nHeight);

	int ResizeNv12(unsigned char *dpDstNv12, int nDstPitch, int nDstWidth, int nDstHeight, unsigned char *dpSrcNv12, int nSrcPitch, int nSrcWidth, int nSrcHeight, unsigned char *dpDstNv12UV = nullptr);
	int ResizeP016(unsigned char *dpDstP016, int nDstPitch, int nDstWidth, int nDstHeight, unsigned char *dpSrcP016, int nSrcPitch, int nSrcWidth, int nSrcHeight, unsigned char *dpDstP016UV = nullptr);

	int ScaleYUV420(unsigned char *dpDstY, unsigned char* dpDstU, unsigned char* dpDstV, int nDstPitch, int nDstChromaPitch, int nDstWidth, int nDstHeight,
		unsigned char *dpSrcY, unsigned char* dpSrcU, unsigned char* dpSrcV, int nSrcPitch, int nSrcChromaPitch, int nSrcWidth, int nSrcHeight, bool bSemiplanar);

	int MattingImage(void* srcImage, int srcWidth, int srcHeight, void* dstImage, int x, int y, int dstWidth, int dstHeight);
	// dstImage GPUœ‘¥Êµÿ÷∑
	// srcImage CPUƒ⁄¥Êµÿ÷∑
	virtual	int MemcpyHostToDev(void** dstImage, int dstLen, void* srcImage, int srcLen);
	// dstImage CPUƒ⁄¥Êµÿ÷∑
	// srcImage GPUœ‘¥Êµÿ÷∑
	virtual	int MemcpyDevToHost(void* dstImage, int dstLen, void* srcImage, int srcLen);
	//  Õ∑≈œ‘¥Ê
	virtual void FreeGpuMem(void* p);

private:
	int m_deviceID;
	CUcontext m_cuContext;
};

