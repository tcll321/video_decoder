#ifndef RAWLOCK_H
#define RAWLOCK_H

#include <tchar.h>

#if defined(_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif

#include <windows.h>

typedef CRITICAL_SECTION ARRawMutex;

void ARRawMutexInit(ARRawMutex* mutex, unsigned long spinCount = 0);
void ARRawMutexDestroy(ARRawMutex* mutex);
void ARRawMutexLock(ARRawMutex* mutex);
bool ARRawMutexTryLock(ARRawMutex* mutex, unsigned long timeoutMSELs = 0);
void ARRawMutexUnlock(ARRawMutex* mutex);

#ifdef __cplusplus
class CARRawLock
{
	ARRawMutex m_mutex;

private:
	CARRawLock(const CARRawLock&);
	CARRawLock& operator= (const CARRawLock&);

public:
	explicit CARRawLock(unsigned long spinCount = 0)
	{
		ARRawMutexInit(&m_mutex, spinCount);
	}
	~CARRawLock()
	{
		ARRawMutexDestroy(&m_mutex);
	}

	void Lock()
	{
		ARRawMutexLock(&m_mutex);
	}
	bool TryLock(unsigned long timeoutMSELs = 0)
	{
		return ARRawMutexTryLock(&m_mutex, timeoutMSELs);
	}
	void Unlock()
	{
		ARRawMutexUnlock(&m_mutex);
	}

public:
	class AutoLock
	{
	private:
		AutoLock(const AutoLock&);
		AutoLock& operator= (const AutoLock&);

		CARRawLock& lock_;
		bool const lockIt_;

	public:
		explicit AutoLock(CARRawLock& mutex, bool lockIt = true)
			: lock_(mutex), lockIt_(lockIt)
		{
			if (lockIt_)
				lock_.Lock();
			else
				lock_.Unlock();
		}
		~AutoLock()
		{
			if (lockIt_)
				lock_.Unlock();
			else
				lock_.Lock();
		}
	};
};
#endif	/* #ifdef __cplusplus */

inline bool TryEnterCriticalSection__(LPCRITICAL_SECTION cs)
{
	typedef bool(WINAPI* FUNC)(CRITICAL_SECTION*);
	static FUNC fn = NULL;
	if (NULL == fn)
	{
		fn = (FUNC)GetProcAddress(
			GetModuleHandle(_T("kernel32.dll")),
			"TryEnterCriticalSection"
		);
	}
	return fn(cs);
}

inline bool InitializeCriticalSectionAndSpinCount__(LPCRITICAL_SECTION cs, DWORD spinCount)
{
	typedef bool(WINAPI* FUNC)(CRITICAL_SECTION*, DWORD);
	static FUNC fn = NULL;
	if (NULL == fn)
	{
		fn = (FUNC)GetProcAddress(
			GetModuleHandle(_T("kernel32.dll")),
			"InitializeCriticalSectionAndSpinCount"
		);
	}
	return fn(cs, spinCount);
}

inline bool SetCriticalSectionSpinCount__(LPCRITICAL_SECTION cs, DWORD spinCount)
{
	typedef bool(WINAPI* FUNC)(CRITICAL_SECTION*, DWORD);
	static FUNC fn = NULL;
	if (NULL == fn)
	{
		fn = (FUNC)GetProcAddress(
			GetModuleHandle(_T("kernel32.dll")),
			"SetCriticalSectionSpinCount"
		);
	}
	return fn(cs, spinCount);
}

inline void ARRawMutexInit(ARRawMutex* mutex, DWORD spinCount /*= 0*/)
{
	InitializeCriticalSectionAndSpinCount__(mutex, spinCount);
}

inline void ARRawMutexDestroy(ARRawMutex* mutex)
{
	DeleteCriticalSection(mutex);
}

inline void ARRawMutexLock(ARRawMutex* mutex)
{
	EnterCriticalSection(mutex);
}

inline BOOL ARRawMutexTryLock__(ARRawMutex* mutex)
{
	BOOL ok = TryEnterCriticalSection__(mutex);
	return ok;
}

inline bool ARRawMutexTryLock(ARRawMutex* mutex, DWORD timeoutMSELs /*= 0*/)
{
	if (INFINITE == timeoutMSELs)
	{
		ARRawMutexLock(mutex);
		return true;
	}
	else if (0 == timeoutMSELs)
	{
		return (ARRawMutexTryLock__(mutex)?true:false);
	}
	else
	{
		DWORD tick0;
		if (ARRawMutexTryLock__(mutex))
			return true;
		tick0 = GetTickCount();
		while (true)
		{
			Sleep(1);
			if (ARRawMutexTryLock__(mutex))
				return true;
			else if (GetTickCount() - tick0 >= timeoutMSELs)
				break;
		}
		return false;
	}
	return false;
}

inline void ARRawMutexUnlock(ARRawMutex* mutex)
{
	LeaveCriticalSection(mutex);
}

#endif //RAWLOCK_H
