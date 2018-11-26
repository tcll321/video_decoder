#ifndef RTSPAPIEXPORT_H
#define RTSPAPIEXPORT_H

#if defined(_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif

/*
编译说明：
【WIN32工程】
1、如果把RTSPAPI_DECL描述的函数/类加入到工程一起编译使用（不使用dll），则需要在使用工程里定义全局宏RTSPAPI_DECL。
2、如果把RTSPAPI_DECL描述的函数/类放到某个dll里面导出使用，则在dll工程里面需要定义全局宏RTSPAPI_EXPORTS，使用dll的工程不需要定义任何宏。


RTSPAPI库里的所有导出C函数使用RTSPAPI_API修饰，
所有导出C++类使用RTSPAPI_DECL修饰
*/

#ifndef RTSPAPI_DECL
#ifdef WIN32
#ifdef RTSPAPI_EXPORTS
#define RTSPAPI_DECL __declspec(dllexport)
#else
#define RTSPAPI_DECL __declspec(dllimport)
#endif
#else
#define RTSPAPI_DECL __attribute__ ((visibility("default")))
#endif
#endif

#ifdef __cplusplus
#define RTSP_API extern "C" RTSPAPI_DECL
#else
#define RTSP_API RTSPAPI_DECL
#endif

#endif
