#include "stdafx.h"
#include "CSlowObjCmd.h"
#include "CFastObjCmd.h"
#include "ExpHelper.h"
#include "CTimeSystemClient.h"
#include "CCoreObjectFollower.h"
#include "CoreObjectCommon.h"
#include "CDelayedObjCmdMgr.h"
#include "CQuickRand.h"
#include "IDistortedTime.h"

CSlowObjCmd::CSlowObjCmd(CCoreObjectFollower *pObj,const void* pData,uint32 uSize)
:m_pObj(pObj),m_sData(static_cast<const char*>(pData),uSize)
{
	Ast(m_pObj);
	Ast(uSize);

	m_pObj->m_pDelayedCmdMgr->m_lstCmd.push_back(this);

	uint32 uDelay = m_pObj->MoveDelayEnabled() ? m_pObj->GetMoveDelay() : 0;
	//uDelay=RandRC1(0,uDelay);
	//—”≥Ÿ200∫¡√Î
	//cout << m_pObj->GetGlobalID() << " delay time = " << uDelay << endl;
	pObj->GetDistortedTime()->RegistDistortedTick(this,uDelay);
}

CSlowObjCmd::~CSlowObjCmd()
{
	m_pObj->GetDistortedTime()->UnregistDistortedTick(this);

	while(!m_lstNestedCmd.empty())
	{
		delete m_lstNestedCmd.front();
		m_lstNestedCmd.pop_front();
	}
	m_pObj->m_pDelayedCmdMgr->m_lstCmd.pop_front();
}

void CSlowObjCmd::AddNestedCmd(CFastObjCmd* pCmd)
{
	m_lstNestedCmd.push_back(pCmd);
}

CCoreObjectFollower* CSlowObjCmd::GetObj()const
{
	return m_pObj;
}

const void* CSlowObjCmd::GetData()const
{
	return m_sData.data();
}
