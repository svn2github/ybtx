#include "stdafx.h"
#include "CNpcQueueMgr.h"
#include "CNpcQueueMember.h"
#include "CCharacterDictator.h"
#include "TSqrAllocator.inl"

/************************************************************************/
/* 构造阵型表：先左后右，再上再下，左上右下，右上左下*/
/************************************************************************/
CNpcQueueMgr::CNpcQueueMgr()
{
	m_lNpcQueue.push_front(new CNpcQueueMember(-64, 0));	//左
	m_lNpcQueue.push_back(new CNpcQueueMember(63, 0));		//右
	m_lNpcQueue.push_back(new CNpcQueueMember(0, 0));		//上
	m_lNpcQueue.push_back(new CNpcQueueMember(127, 0));		//下
	m_lNpcQueue.push_back(new CNpcQueueMember(-32, 0));		//左上
	m_lNpcQueue.push_back(new CNpcQueueMember(95, 0));		//右下
	m_lNpcQueue.push_back(new CNpcQueueMember(31, 0));		//右上
	m_lNpcQueue.push_back(new CNpcQueueMember(-96, 0));		//左下
}

CNpcQueueMgr::~CNpcQueueMgr()
{
	CNpcQueue::iterator iter = m_lNpcQueue.begin();
	for (;iter != m_lNpcQueue.end();)
	{
		delete (*iter);
		iter++;
	}
	m_lNpcQueue.clear();
}

int32 CNpcQueueMgr::AddMemberAndGetAngle(uint32 uEntityID)
{
	CCharacterDictator* pCharacter = CCharacterDictator::GetCharacterByID(uEntityID);
	if(NULL == pCharacter)
		return -1;
	CNpcQueue::iterator iter = m_lNpcQueue.begin();
	for (; iter != m_lNpcQueue.end();)
	{
		if ((*iter)->GetEntityID() == 0 || (*iter)->GetEntityID() == uEntityID)
		{
			(*iter)->SetEntityID(uEntityID);
			return (*iter)->GetAngle();
		}
		iter++;
	}
	return -1;		//加入队列失败
}

void CNpcQueueMgr::RemoveMember(uint32 uEntityID)
{
	CCharacterDictator* pCharacter = CCharacterDictator::GetCharacterByID(uEntityID);
	if(NULL == pCharacter)
		return;
	CNpcQueue::iterator iter = m_lNpcQueue.begin();
	for (; iter != m_lNpcQueue.end();)
	{
		if ((*iter)->GetEntityID() == uEntityID)
		{
			(*iter)->SetEntityID(0);
			return;
		}
		iter++;
	}
}
