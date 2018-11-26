#ifndef RTSPAPIEXPORT_H
#define RTSPAPIEXPORT_H

#if defined(_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif

/*
����˵����
��WIN32���̡�
1�������RTSPAPI_DECL�����ĺ���/����뵽����һ�����ʹ�ã���ʹ��dll��������Ҫ��ʹ�ù����ﶨ��ȫ�ֺ�RTSPAPI_DECL��
2�������RTSPAPI_DECL�����ĺ���/��ŵ�ĳ��dll���浼��ʹ�ã�����dll����������Ҫ����ȫ�ֺ�RTSPAPI_EXPORTS��ʹ��dll�Ĺ��̲���Ҫ�����κκꡣ


RTSPAPI��������е���C����ʹ��RTSPAPI_API���Σ�
���е���C++��ʹ��RTSPAPI_DECL����
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
