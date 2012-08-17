#include "stdafx.h"
#include "CCpuUsageMgr.h"
#include "CCpuUsage.h"
#include "CAppServer.h"
#include "ThreadHelper.h"


CCpuUsageMgr* CCpuUsageMgr::ms_pInst = NULL;
CThreadCpuUsageMgr* CThreadCpuUsageMgr::ms_pInst = NULL;

static const  uint32 MAX_SAMPLE_COUNT=60; 

CCpuUsageMgr::CCpuUsageMgr()
{
	m_pCpuUsegae = new CSystemCpuUsage();
	CAppServer::Inst()->RegisterTick(this,1000);
}

CCpuUsageMgr::~CCpuUsageMgr()
{
	CAppServer::Inst()->UnRegisterTick(this);
	delete m_pCpuUsegae;
}

void CCpuUsageMgr::Init()
{
	ms_pInst = new CCpuUsageMgr();
}

void CCpuUsageMgr::Unit()
{
	delete ms_pInst;
	ms_pInst = NULL;
}

CCpuUsageMgr* CCpuUsageMgr::Inst()
{
	return ms_pInst;
}

void CCpuUsageMgr::OnTick()
{
	if (m_deqSample.size()>MAX_SAMPLE_COUNT)
	{
		m_deqSample.pop_back();
		m_deqSample.push_front(m_pCpuUsegae->CpuUsage());
	}
	else
	{
		m_deqSample.push_front(m_pCpuUsegae->CpuUsage());
	}
}

int32 CCpuUsageMgr::GetSystemCpuUsage(uint32 uCount)
{
	deque<int>& deqSample = ms_pInst->m_deqSample;
	deque<int>::const_iterator it = deqSample.begin();
	uint32 Count;
	if (deqSample.size()<uCount)
	{
		Count=deqSample.size();
	}
	int32 TopUsage = 0;
	for (uint32 index =0;index<=uCount&&it!=deqSample.end();it++,index++)
	{
		TopUsage += (*it);
	}

	return TopUsage/uCount;
}

class CThreadCpuUsageData
{
public:
	CThreadCpuUsageData(const string& strThreadName,HTHREADID tid)
		:m_strThreadName(strThreadName),m_tid(tid)
	{
		m_pCpuUseage = new CThreadCpuUsage(tid);
	}
	~CThreadCpuUsageData()
	{
		delete m_pCpuUseage;
	}
	void Refresh()
	{
		if (m_deqSample.size()>MAX_SAMPLE_COUNT)
		{
			m_deqSample.pop_back();
			m_deqSample.push_front( m_pCpuUseage->CpuUsageCurProcessbyTID(m_tid));
		}
		else
		{
			m_deqSample.push_front( m_pCpuUseage->CpuUsageCurProcessbyTID(m_tid));
		}
	}
	int32 GetCpuUsage(uint32 uCount)const
	{
		deque<int>::const_iterator it = m_deqSample.begin();
		uint32 Count;
		if (m_deqSample.size()<uCount)
		{
			Count=m_deqSample.size();
		}
		int32 TopUsage = 0;
		for (uint32 index =0;index<=uCount&&it!=m_deqSample.end();it++,index++)
		{
			TopUsage += (*it);
		}

		return TopUsage/uCount;
	}
private:
	string m_strThreadName;
	HTHREADID m_tid;
	deque<int> m_deqSample;
	CThreadCpuUsage* m_pCpuUseage;
};

CThreadCpuUsageMgr::CThreadCpuUsageMgr()
{
	CThreadCpuUsageData* pData = new CThreadCpuUsageData("MainThread",GetCurTID());
	m_mapThread.insert(make_pair(make_pair("MainThread",GetCurTID()),pData));
	Refresh();
	CAppServer::Inst()->RegisterTick(this,1000);
}

CThreadCpuUsageMgr::~CThreadCpuUsageMgr()
{
	CAppServer::Inst()->UnRegisterTick(this);
	map_Thread::iterator it = m_mapThread.begin();
	for (;it!=m_mapThread.end();++it)
	{
		delete it->second;
	}
}

void CThreadCpuUsageMgr::Init()
{
	ms_pInst = new CThreadCpuUsageMgr();
}

void CThreadCpuUsageMgr::Unit()
{
	delete ms_pInst;
	ms_pInst = NULL;
}

CThreadCpuUsageMgr* CThreadCpuUsageMgr::Inst()
{
	return ms_pInst;
}

void CThreadCpuUsageMgr::OnTick()
{
	Refresh();
}

void CThreadCpuUsageMgr::Refresh()
{
	vector<pair<string,HTHREADID> > vecThreadID = 	CWatchCpuThreadID::GetInst()->GetThreadIDVec();
	vector<pair<string,HTHREADID> >::const_iterator it = vecThreadID.begin();
	for (;it!=vecThreadID.end();++it)
	{
		map_Thread::iterator it_map = m_mapThread.find(*it);
		if (it_map==m_mapThread.end())
		{
			CThreadCpuUsageData* pData = new CThreadCpuUsageData((*it).first,(*it).second);
			m_mapThread.insert(make_pair(*it,pData));
		}
		else
		{
			it_map->second->Refresh();
		}
	}

	map_Thread::iterator it_map = m_mapThread.find(make_pair("MainThread",GetCurTID()));
	if (it_map!=m_mapThread.end())
	{
		it_map->second->Refresh();
	}
}

int32 CThreadCpuUsageMgr::GetTopThreadCpuUsage(uint32 uCount)
{
	map_Thread& mapThread = ms_pInst->m_mapThread;
	map_Thread::iterator it = mapThread.begin();
	int32 TopThreadCpuUsage = 0;
	for(;it!=mapThread.end();++it)
	{
		CThreadCpuUsageData* pData = it->second;
		int32 ThreadCpuUsage = pData->GetCpuUsage(uCount); 
		if(ThreadCpuUsage > TopThreadCpuUsage)
			TopThreadCpuUsage = ThreadCpuUsage;
	}

	return TopThreadCpuUsage;
}
