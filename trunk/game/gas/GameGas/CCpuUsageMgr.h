#pragma once
#include "CTick.h"
#include "TSingleton.h"
#include "ThreadTypes.h"
#include "CEntityMallocObject.h"
#include "TEntityAllocator.h"

namespace sqr
{
	class CSystemCpuUsage;
	class CThreadCpuUsage;
}

class CThreadCpuUsageData;

class CCpuUsageMgr: public CTick, public CEntityMallocObject
{
public:
	static void Init();
	static void Unit();
	static CCpuUsageMgr* Inst();
	static int32 GetSystemCpuUsage(uint32 uCount);
private:
	static CCpuUsageMgr*	ms_pInst;
	CCpuUsageMgr();
	~CCpuUsageMgr();
	virtual void OnTick();
	CSystemCpuUsage* m_pCpuUsegae;
	deque<int> m_deqSample;
};


class CThreadCpuUsageDataComp
{
public:
	bool operator()(const CThreadCpuUsageData* pLeft, const CThreadCpuUsageData* pRight) const;
};

class CThreadCpuUsageMgr: public CTick, public CEntityMallocObject
{
public:
	static void Init();
	static void Unit();
	static CThreadCpuUsageMgr* Inst();
	static int GetTopThreadCpuUsage(uint32 uCount);
private:
	static CThreadCpuUsageMgr*	ms_pInst;
	typedef map<pair<string,HTHREADID>,CThreadCpuUsageData* > map_Thread;
	CThreadCpuUsageMgr();
	~CThreadCpuUsageMgr();
	virtual void OnTick();
	void Refresh();
	map_Thread m_mapThread;
};

