#ifndef _THREAD_H_
#define _THREAD_H_

#if _MSC_VER > 1000
#pragma once
#endif

#include <process.h>
#include "types.h"

// 线程的默认堆栈大小
// 注：若创建线程时堆栈大小参数传0值，则总使用下面的值作为默认大小（不使用操作系统的默认值）
#if defined(_NP_IS_MIPS_ARM)
#define THREAD_STACK_SIZE_DEFAULT (1024*128)
#else
#define THREAD_STACK_SIZE_DEFAULT (1024*512)
#endif

#if defined(WIN32)
#ifndef STACK_SIZE_PARAM_IS_A_RESERVATION
#define STACK_SIZE_PARAM_IS_A_RESERVATION		0x00010000
#endif
#endif

typedef int(*FOSThreadRoutine)(void*);
typedef enum OSThreadPriority__
{
	OS_BAD_THREAD = -1,
	OS_LOW_THREAD = 0,
	OS_NORMAL_THREAD,
	OS_HIGH_THREAD,
	OS_URGENT_THREAD,
} OSThreadPriority;

bool OSCreateThread(
	OSThread* thd,
	OSThreadID* tid /*= NULL*/,
	FOSThreadRoutine fnRoutine,
	void* param,
	unsigned long stackSize
);

void OSCloseThread(
	OSThread thd
);


typedef LPTHREAD_START_ROUTINE FPlatformThreadRoutine;

struct OSCreateThread_Param
{
	union
	{
		OSThreadID parentThreadID;
		void* parentThreadID_hex;
		INT_PTR parentThreadID_int;
	};
	FOSThreadRoutine fnRoutine;
	void* param;
};

static unsigned long WINAPI OSCreateThread_Routine(LPVOID param)
{
	int r;
	struct OSCreateThread_Param info = *(struct OSCreateThread_Param*)param;
	free(param);
	r = info.fnRoutine(info.param);
	//	DEBUG_EXP(NPCRT_API_LockFlagCheck(OSThreadSelf()));
	_endthreadex(r);
	return r;
}

inline bool OSCreateThread__(
	OSThread* thd,
	OSThreadID* tid,
	FPlatformThreadRoutine fnRoutine,
	void* param,
	long stackSize
)
{
	OSThreadID win_tid = INVALID_OSTHREADID;

	if (NULL == thd)
	{
		return false;
	}

	if ((int)stackSize <= 0)
		stackSize = THREAD_STACK_SIZE_DEFAULT;
	*thd = CreateThread(
		NULL,
		stackSize,
		fnRoutine,
		param,
		(GetWindowsVersion() >= Windows_XP ? STACK_SIZE_PARAM_IS_A_RESERVATION : 0),
		&win_tid
	);
	if (tid)
		*tid = win_tid;
	return *thd != NULL;
}

inline bool OSCreateThread(
	OSThread* thd,
	OSThreadID* tid,
	FOSThreadRoutine fnRoutine,
	void* param,
	unsigned long stackSize
)
{
	bool ok;
	struct OSCreateThread_Param* info = (struct OSCreateThread_Param*)malloc(sizeof(struct OSCreateThread_Param));
	if (NULL == info)
		return false;
	info->parentThreadID = OSThreadSelf();
	info->fnRoutine = fnRoutine;
	info->param = param;
	ok = OSCreateThread__(thd, tid, OSCreateThread_Routine, info, stackSize);
	if (!ok)
		free(info);
	return ok;
}

inline void OSCloseThread(
	OSThread thd
)
{
	WaitForSingleObject(thd, INFINITE);
	CloseHandle(thd);
}

#endif
