#pragma once
#include <memory>

#ifdef COLORUTILSINTERFACE_EXPORTS
#define COLORUTILSINTERFACE_API __declspec(dllexport)
#else
#define COLORUTILSINTERFACE_API __declspec(dllimport)
#endif

class COLORUTILSINTERFACE_API ColorUtilsInterface {
public:
	ColorUtilsInterface(void);
	static std::shared_ptr<ColorUtilsInterface> Create(int deviceID);

	// dpBgr GPU显存地址  需要调用FreeGpuMem释放显存
	virtual int cudaNv12ToBgr24(uint8_t *dpNv12, int nNv12Pitch, uint8_t **dpBgr, int nBgrPitch, int nWidth, int nHeight, int iMatrix = 0) = 0;
	// dpRgb GPU显存地址  需要调用FreeGpuMem释放显存
	virtual int cudaNv12ToRgb24(uint8_t *dpNv12, int nNv12Pitch, uint8_t **dpRgb, int nRgbPitch, int nWidth, int nHeight, int iMatrix = 0) = 0;

	// dpBgra GPU显存地址  需要调用FreeGpuMem释放显存
	virtual int cudaNv12ToBgra32(uint8_t *dpNv12, int nNv12Pitch, uint8_t **dpBgra, int nBgraPitch, int nWidth, int nHeight, int iMatrix = 0) = 0;

	//	srcImage GPU显存RGB数据
	//	dstImage 内存RGB数据
	virtual int MattingImage(void* srcImage, int srcWidth, int srcHeight, void* dstImage, int x, int y, int dstWidth, int dstHeight) = 0;

	// dstImage GPU显存地址  需要调用FreeGpuMem释放显存
	// srcImage CPU内存地址
	virtual	int MemcpyHostToDev(void** dstImage, int dstLen, void* srcImage, int srcLen) = 0;
	// dstImage CPU内存地址
	// srcImage GPU显存地址
	virtual	int MemcpyDevToHost(void* dstImage, int dstLen, void* srcImage, int srcLen) = 0;
	// 释放显存
	virtual void FreeGpuMem(void* p) = 0;

};