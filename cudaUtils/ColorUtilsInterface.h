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

	// dpBgr GPU�Դ��ַ  ��Ҫ����FreeGpuMem�ͷ��Դ�
	virtual int cudaNv12ToBgr24(uint8_t *dpNv12, int nNv12Pitch, uint8_t **dpBgr, int nBgrPitch, int nWidth, int nHeight, int iMatrix = 0) = 0;
	// dpRgb GPU�Դ��ַ  ��Ҫ����FreeGpuMem�ͷ��Դ�
	virtual int cudaNv12ToRgb24(uint8_t *dpNv12, int nNv12Pitch, uint8_t **dpRgb, int nRgbPitch, int nWidth, int nHeight, int iMatrix = 0) = 0;

	// dpBgra GPU�Դ��ַ  ��Ҫ����FreeGpuMem�ͷ��Դ�
	virtual int cudaNv12ToBgra32(uint8_t *dpNv12, int nNv12Pitch, uint8_t **dpBgra, int nBgraPitch, int nWidth, int nHeight, int iMatrix = 0) = 0;

	//	srcImage GPU�Դ�RGB����
	//	dstImage �ڴ�RGB����
	virtual int MattingImage(void* srcImage, int srcWidth, int srcHeight, void* dstImage, int x, int y, int dstWidth, int dstHeight) = 0;

	// dstImage GPU�Դ��ַ  ��Ҫ����FreeGpuMem�ͷ��Դ�
	// srcImage CPU�ڴ��ַ
	virtual	int MemcpyHostToDev(void** dstImage, int dstLen, void* srcImage, int srcLen) = 0;
	// dstImage CPU�ڴ��ַ
	// srcImage GPU�Դ��ַ
	virtual	int MemcpyDevToHost(void* dstImage, int dstLen, void* srcImage, int srcLen) = 0;
	// �ͷ��Դ�
	virtual void FreeGpuMem(void* p) = 0;

};