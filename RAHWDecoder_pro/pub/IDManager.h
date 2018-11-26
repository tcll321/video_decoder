// IDManager.h

#ifndef _IDManager_h_
#define _IDManager_h_

#if defined(_MSC_VER) && (_MSC_VER > 1000)
	#pragma once
#endif

#include <map>
#include <string>
#include "locker.h"
#include "types.h"
#include "thread.h"

template<typename TYPE>
struct IDInfo
{
	TYPE id;
	void* param;
	void* param2;
	void* param3;
	OSThreadID busyThreadID;
	std::string desc;

	IDInfo()
	{
		param = NULL;
		param2 = NULL;
		param3 = NULL;
		busyThreadID = INVALID_OSTHREADID;
 	}
};

template<typename TYPE>
struct IDLess
{
	bool operator()(const TYPE& left, const TYPE& right) const
	{
		return ((INT_PTR)left < (INT_PTR)right);
	}
};

template<typename IDType>
class CIDManager
{
public:
	CIDManager();
	~CIDManager();

	/// 生成一个ID，最多可以保存三个参数
	/// 返回 (IDType)0 表失败
	IDType New(const TCHAR* desc = _T(""), void* param = NULL, void* param2 = NULL, void* param3 = NULL);

	/// 删除一个已有的ID
	bool Delete(IDType id);

	/// 删除与参数相同的ID
	bool DeleteByParam(void* param, INT index);
	
	/// 查找ID，返回其对应的参数
	bool Find(IDType id, IDInfo<IDType>* idInfo = NULL) const;

	/// 跟据参数查找,返回第一个相匹配的信息
	bool Find(void* param, INT index, IDInfo<IDType>* idInfo = NULL);

	/// 重新设置ID所对应的参数
	bool Set(IDType id, const IDInfo<IDType>& idInfo);

	/// 检查ID与描述是否匹配
	bool Check(IDType id, const TCHAR* desc = NULL) const;

	/// ID个数
	INT Size() const;

	/// 清除所有ID
	void Clear();

	/// 设置ID为busy状态
	bool Busy(IDType id);

	/// 设置ID为空闲状态
	bool Unbusy(IDType id);

private:
	typedef std::map<
				IDType,
				IDInfo<IDType>,
				IDLess<IDType>
				> IDMap;
	LONG m_nextID;
	IDMap m_idMap;
	mutable CLocker m_locker;
};

template<typename IDType>
CIDManager<IDType>::CIDManager()
	: m_nextID(0)
{
}

template<typename IDType>
CIDManager<IDType>::~CIDManager()
{
	Clear();
}

template<typename IDType>
void CIDManager<IDType>::Clear()
{
	m_idMap.clear();
	m_nextID = 0;
}

template<typename IDType>
IDType CIDManager<IDType>::New(const TCHAR* desc /*= _T("")*/, void* param /*= NULL*/, void* param2 /*= NULL*/, void* param3 /*= NULL*/)
{
	CARAutoLock autoLock(m_locker);
	LONG oldID = m_nextID;
	while(true)
	{
		m_nextID++;
		if(oldID == m_nextID)
		{
			break;
		}

		if(0 == m_nextID)
			m_nextID = 1;
		if(!Find((IDType)m_nextID))
		{
			IDInfo<IDType> info;
			info.id = (IDType)m_nextID;
			info.param = param;
			info.param2 = param2;
			info.param3 = param3;
			if(desc)
				info.desc = desc;
			m_idMap.insert(std::make_pair((IDType)m_nextID, info));
			return (IDType)m_nextID;
		}
	}
	return (IDType)0;
}

template<typename IDType>
bool CIDManager<IDType>::Delete(IDType id)
{
	CARAutoLock autoLock(m_locker);
	bool ok = (1==m_idMap.erase(id));
	return ok;
}

template<typename IDType>
bool CIDManager<IDType>::DeleteByParam(void* param, INT index)
{
	CARAutoLock autoLock(m_locker);
	typename IDMap::iterator it;
	for(it=m_idMap.begin(); it!=m_idMap.end();)
	{
		const IDInfo<IDType>& idInfo = it->second;
		if(0==index && idInfo.param==param)
		{
			m_idMap.erase(it);
			it = m_idMap.begin();
		}
		else if(1==index && idInfo.param2==param)
		{
			m_idMap.erase(it);
			it = m_idMap.begin();
		}
		else if(2==index && idInfo.param3==param)
		{
			m_idMap.erase(it);
			it = m_idMap.begin();
		}
		else
		{
			it++;
		}
	}
	return true;
}

template<typename IDType>
bool CIDManager<IDType>::Find(IDType id, IDInfo<IDType>* idInfo /*= NULL*/) const
{
	CARAutoLock autoLock(m_locker);
	typename IDMap::const_iterator it = m_idMap.find(id);
	if(it == m_idMap.end())
		return false;
	if(idInfo)
	{
		const IDInfo<IDType>& rinfo = it->second;
		idInfo->id = rinfo.id;
		idInfo->param = rinfo.param;
		idInfo->param2 = rinfo.param2;
		idInfo->param3 = rinfo.param3;
		idInfo->desc = rinfo.desc;
		idInfo->busyThreadID = rinfo.busyThreadID;
	}
	return true;
}

template<typename IDType>
bool CIDManager<IDType>::Find(void* param, INT index, IDInfo<IDType>* idInfo /*= NULL*/)
{
	CARAutoLock autoLock(m_locker);
	typename IDMap::const_iterator it;
	for(it=m_idMap.begin(); it!=m_idMap.end(); ++it)
	{
		if(0==index && it->second.param==param)
			break;
		else if(1==index && it->second.param2==param)
			break;
		else if(2==index && it->second.param3==param)
			break;
	}
	if(it != m_idMap.end())
	{
		if(idInfo)
		{
			idInfo->id = it->second.id;
			idInfo->param = it->second.param;
			idInfo->param2 = it->second.param2;
			idInfo->param3 = it->second.param3;
			idInfo->desc = it->second.desc;
		}
		return true;
	}
	return false;
}

template<typename IDType>
bool CIDManager<IDType>::Set(IDType id, const IDInfo<IDType>& idInfo)
{
	CARAutoLock autoLock(m_locker);
	typename IDMap::iterator it = m_idMap.find(id);
	if(it == m_idMap.end())
		return false;
	IDInfo<IDType>& info = it->second;
	info.param = idInfo.param;
	info.param2 = idInfo.param2;
	info.param3 = idInfo.param3;
	info.desc = idInfo.desc;
	return true;
}

template<typename IDType>
bool CIDManager<IDType>::Check(IDType id, const TCHAR* desc /*= NULL*/) const
{
	if(id == (IDType)0)
		return false;
	CARAutoLock autoLock(m_locker);
	IDInfo<IDType> info;
	if(!Find(id, &info))
		return false;
	if(desc && info.desc!=desc)
		return false;
	return true;
}

template<typename IDType>
INT CIDManager<IDType>::Size() const
{
	CARAutoLock autoLock(m_locker);
	return (INT)m_idMap.size();
}

template<typename IDType>
bool CIDManager<IDType>::Busy(IDType id)
{
	for(;; Sleep(1))
	{
		CARAutoLock autoLock(m_locker);
		typename IDMap::iterator it = m_idMap.find(id);
		if(it == m_idMap.end())
			return false;
		if(it->second.busyThreadID != INVALID_OSTHREADID)
			continue;
		else
		{
			it->second.busyThreadID = OSThreadSelf();
			break;
		}
	}
	return true;
}

template<typename IDType>
bool CIDManager<IDType>:: Unbusy(IDType id)
{
	CARAutoLock autoLock(m_locker);
	typename IDMap::iterator it = m_idMap.find(id);
	if(it == m_idMap.end())
		return false;
// 	ASSERT(it->second.busyThreadID == OSThreadSelf());
	it->second.busyThreadID = INVALID_OSTHREADID;
	return true;
}

template<typename BusyType, typename BusyValue>
class CIDAutoBusy
{
public:
	explicit CIDAutoBusy(BusyType& busyType, BusyValue busyValue)
		:	busyType_(busyType), busyValue_(busyValue)
	{
		busyType_.Busy(busyValue_);
	}

	~CIDAutoBusy()
	{
		busyType_.Unbusy(busyValue_);
	}
private:
	CIDAutoBusy(const CIDAutoBusy&);
	CIDAutoBusy& operator= (const CIDAutoBusy&);

	BusyType& busyType_;
	BusyValue busyValue_;
};

#endif	// #ifndef _CNPIDManager_h_
