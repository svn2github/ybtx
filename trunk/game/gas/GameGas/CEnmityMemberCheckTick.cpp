#include "stdafx.h"
#include "CEnmityMemberCheckTick.h"
#include "CNpcEnmityMember.h"
#include "CEnmityMgr.h"
#include "ErrLogHelper.h"

CEnmityMemberCheckTick::CEnmityMemberCheckTick(CEnmityMgr* pEnmityMgr, CNpcEnmityMember* pMember)
:m_pEnmityMgr(pEnmityMgr)
,m_pMember(pMember)
{
	
}

CEnmityMemberCheckTick::~CEnmityMemberCheckTick()
{

}

void CEnmityMemberCheckTick::OnTick()
{
	SQR_TRY
	{
		m_pEnmityMgr->GetOtherNearstEnemy();
	}
	SQR_CATCH(exp)
	{
		m_pMember->DetachAttackMsg();
		LogExp(exp);
	}
	SQR_TRY_END;
}
