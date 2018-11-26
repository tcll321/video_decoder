#ifndef LOCK_H
#define LOCK_H

#if defined(_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif

#include "sync.h"

// 通用线程锁
class CLocker
{
private:
	CLocker(const CLocker&);
	CLocker& operator= (const CLocker&);

protected:
	OSMutex lock_;

public:
	explicit CLocker(unsigned long spinCount = 0)
	{
		OSMutexInit(&lock_, spinCount);
	}
	~CLocker()
	{
		OSMutexDestroy(&lock_);
	}

	void Lock()
	{
		OSMutexLock(&lock_);
	}
	bool TryLock(unsigned long timeoutMSELs = 0)
	{
		return OSMutexTryLock(&lock_, timeoutMSELs);
	}
	void Unlock()
	{
		OSMutexUnlock(&lock_);
	}
	int GetLockCount() const
	{
		return (int)lock_.lockedCount;
	}
	bool IsLockedByThreadSelf() const
	{
		return (GetCurrentThreadId() == lock_.lockedThreadID);
	}
};

class CARAutoLock
{
private:
	CARAutoLock(const CARAutoLock&);
	CARAutoLock& operator= (const CARAutoLock&);

private:
	CLocker& lock_;

public:
	explicit CARAutoLock(CLocker& lock) : lock_(lock)
	{
		lock_.Lock();
	}
	~CARAutoLock()
	{
		lock_.Unlock();
	}
	CLocker& GetLock() const
	{
		return lock_;
	}
};

#endif