#ifndef _PUB_DEFS_H_
#define _PUB_DEFS_H_

#if _MSC_VER > 1000
#pragma once
#endif

#define DECODER_METHOD __stdcall

typedef enum
{
	DECODER_OK = 0,	//成功
	DECODER_ERR_NULL_PTR = -1,	//空指针
	DECODER_ERR_NULL_OBJECT = -2,	//获取对象失败
	DECODER_ERR_NO_HANDLE = -3,	//空句柄
	DECODER_ERR_NOT_INITIALIZED = -4,	//初始化失败
	DECODER_ERR_FULL_BUFFER = -5,	//缓冲区满
	DECODER_ERR_NEEDMORE_DATA = -6,	//需要更多数据
	DECODER_ERR_DEVICE_FAILED = -7,	//设备加载失败
	DECODER_ERR_CREATEDIR_FAILED = -8,	//创建文件夹失败
	DECODER_ERR_GETDEVICEINFO_FAILED = -9, //获取设备信息失败
	DECODER_ERR_DECOD_FAILED		= -10	//解码失败
}DecoderError;

typedef enum VideoCodec_enum
{
	VideoCodec_MPEG1 = 0,                                       /**<  MPEG1             */
	VideoCodec_MPEG2,                                           /**<  MPEG2             */
	VideoCodec_MPEG4,                                           /**<  MPEG4             */
	VideoCodec_VC1,                                             /**<  VC1               */
	VideoCodec_H264,                                            /**<  H264              */
	VideoCodec_JPEG,                                            /**<  JPEG              */
	VideoCodec_H264_SVC,                                        /**<  H264-SVC          */
	VideoCodec_H264_MVC,                                        /**<  H264-MVC          */
	VideoCodec_HEVC,                                            /**<  HEVC              */
	VideoCodec_VP8,                                             /**<  VP8               */
	VideoCodec_VP9,                                             /**<  VP9               */
	VideoCodec_NumCodecs,                                       /**<  Max codecs        */
}VideoCodec;

typedef struct DeviceInfo_Struct
{
	DeviceInfo_Struct() {
		devId = 0;
	}
	int		devId;
	char	devName[256];
}DeviceInfo;

#endif
