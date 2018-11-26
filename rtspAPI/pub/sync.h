#pragma once
#include "rawlock.h"
#include <mmsyscom.h>

#define INVALID_OSTHREAD			((OSThread)0)
#define INVALID_OSTHREADID			((unsigned long)0)

typedef struct
{
	unsigned long syncDWORD1;
	CRITICAL_SECTION rawMutex;
	__int64 lockedCount;
	union
	{
		unsigned long lockedThreadID;	/* 拥有该锁的线程，INVALID_OSTHREADID表示没有线程占用此锁 */
		void* lockedThreadID_hex;	/* 调试时，在调试器中总显示为16进制 */
		int lockedThreadID_int;	/* 调试时，在调试器中总显示为有符号数 */
	};
	unsigned long syncDWORD2;
} OSMutex;

typedef HANDLE OSSema;

void OSMutexInit(OSMutex* mutex, unsigned long spinCount = 0);
void OSMutexDestroy(OSMutex* mutex);
void OSMutexLock(OSMutex* mutex);
bool OSMutexTryLock(OSMutex* mutex, unsigned long timeoutMSELs = 0);
void OSMutexUnlock(OSMutex* mutex);
/* 信号量 */
BOOL OSSemaInit(OSSema* sema, int initValue = 0);
void OSSemaDestroy(OSSema* sema);
BOOL OSSemaPost(OSSema* sema, int n = 1);
BOOL OSSemaWait(OSSema* sema, DWORD timeoutMSELs = INFINITE);

/* 同步对象的简单C++类包装 */
#ifdef __cplusplus

#if defined(_MSC_VER)
#pragma warning(disable : 4201) /* nonstandard extension used : nameless struct/union */
#endif
class COSSema
{
	COSSema(const COSSema&);
	COSSema& operator= (const COSSema&);
	OSSema sema_;

public:
	explicit COSSema(int initValue = 0)
	{
		OSSemaInit(&sema_, initValue);
	}
	~COSSema()
	{
		OSSemaDestroy(&sema_);
	}
	BOOL Post(int n = 1)
	{
		return OSSemaPost(&sema_, n);
	}
	BOOL Wait(DWORD timeoutMSELs = INFINITE)
	{
		return OSSemaWait(&sema_, timeoutMSELs);
	}
};
#endif /* #ifdef __cplusplus */

inline void OSMutexInit(OSMutex* mutex, unsigned long spinCount /*= 0*/)
{
	ARRawMutexInit(&mutex->rawMutex, spinCount);
	mutex->lockedCount = 0;
	mutex->lockedThreadID = INVALID_OSTHREADID;
	mutex->syncDWORD1 = MAKEFOURCC('S', 'Y', 'N', 'C');
	mutex->syncDWORD2 = MAKEFOURCC('S', 'Y', 'N', 'C');
}

inline void OSMutexDestroy(OSMutex* mutex)
{
	ARRawMutexDestroy(&mutex->rawMutex);
	memset(mutex, 0xcd, sizeof(mutex));
}

inline void OSMutexLock(OSMutex* mutex)
{
	ARRawMutexLock(&mutex->rawMutex);
	mutex->lockedCount++;
	if (1 == mutex->lockedCount)
	{
		mutex->lockedThreadID = GetCurrentThreadId();
	}
}

inline bool OSMutexTryLock(OSMutex* mutex, unsigned long timeoutMSELs /*= 0*/)
{
	if (!ARRawMutexTryLock(&mutex->rawMutex, timeoutMSELs))
		return false;
	mutex->lockedCount++;
	if (1 == mutex->lockedCount)
	{
		mutex->lockedThreadID = GetCurrentThreadId();
	}
	return true;
}

inline void OSMutexUnlock(OSMutex* mutex)
{
	mutex->lockedCount--;
	if (0 == mutex->lockedCount)
		mutex->lockedThreadID = INVALID_OSTHREADID;
	ARRawMutexUnlock(&mutex->rawMutex);
}

inline BOOL OSSemaInit(OSSema* sema, int initValue /*= 0*/)
{
	return NULL != (*sema = CreateSemaphore(NULL, initValue, 0x7FFFFFFF, NULL));
}

inline void OSSemaDestroy(OSSema* sema)
{
	CloseHandle(*sema);
	memset(sema, 0xcd, sizeof(sema));
}

inline BOOL OSSemaPost(OSSema* sema, int n /*= 1*/)
{
	BOOL ok;
	while (true)
	{
		ok = ReleaseSemaphore(*sema, n, NULL);
		if (!ok && ERROR_TOO_MANY_POSTS == GetLastError())
		{
			Sleep(1);
			continue;
		}
		break;
	}
	return ok;
}

inline BOOL OSSemaWait(OSSema* sema, DWORD timeoutMSELs /*= INFINITE*/)
{
	return (WAIT_OBJECT_0 == WaitForSingleObject(*sema, timeoutMSELs));
}
