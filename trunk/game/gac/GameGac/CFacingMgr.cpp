#include "stdafx.h"
#include "CFacingMgr.h"
#include "CAppClient.h"
#include "CCharacterDirector.h"
#include "CCharacterFollower.h"

CFacingMgr::MultiMapDstBeLookedOnSrc CFacingMgr::m_multiMapDstBeLookedOnSrc;		// pSrcFol 关注的 pDstFol 未进入视野时用

CFacingMgr::CFacingMgr()
	: m_uSrcEntityID(0)
	, m_uDstEntityID(0) 
	, m_fNADistance(0.0f)
{
}

CFacingMgr::~CFacingMgr()
{
	m_uSrcEntityID = 0;
	m_uDstEntityID = 0;
	m_fNADistance = 0.0f;
	CAppClient::Inst()->UnRegisterTick(this);
	m_multiMapDstBeLookedOnSrc.clear();
}

void CFacingMgr::CreateFacingPair(CCharacterFollower* pCharSrc, CCharacterFollower* pCharDst)
{
	if (!pCharSrc || !pCharSrc->CppIsAlive() || pCharSrc->CppGetCtrlState(eFCS_FeignDeath) || !pCharSrc->GetIsPlayer() || pCharSrc == pCharDst)
		return;
	
	m_uSrcEntityID = pCharSrc->GetEntityID();	
	if (pCharDst)
	{
		m_uDstEntityID = pCharDst->GetEntityID();
		SetInterestObj(pCharSrc, pCharDst);
	}
	else
	{
		if (m_uDstEntityID != 0)
			m_multiMapDstBeLookedOnSrc.insert(pair<uint32, uint32>(m_uDstEntityID, m_uSrcEntityID));
	}
}

void CFacingMgr::TouchOffFacingPair(CCharacterFollower* pCharDst)
{
	uint32 uDstEntityID = pCharDst->GetEntityID();
	typedef MultiMapDstBeLookedOnSrc::iterator it_BeLookedOn;
	pair<it_BeLookedOn, it_BeLookedOn> pos = m_multiMapDstBeLookedOnSrc.equal_range(uDstEntityID);
	for (; pos.first != pos.second; ++pos.first)
	{
		CCharacterFollower* pCharSrc = CCharacterFollower::GetCharacterByID(pos.first->second);
		if (pCharSrc)
		{
			pCharSrc->GetFacingMgr()->CreateFacingPair(pCharSrc, pCharDst);
			m_multiMapDstBeLookedOnSrc.erase(pos.first);
		}
	}
}

void CFacingMgr::RemoveFacingPair()
{
	ClearInterested();

	typedef MultiMapDstBeLookedOnSrc::iterator it_BeLookedOn;
	pair<it_BeLookedOn, it_BeLookedOn> pos = m_multiMapDstBeLookedOnSrc.equal_range(m_uDstEntityID);
	for (; pos.first != pos.second; ++pos.first)
	{
		if (m_uSrcEntityID == pos.first->second)
		{
			m_multiMapDstBeLookedOnSrc.erase(pos.first);
		}
	}
}

void CFacingMgr::SetInterestObj(CCharacterFollower* pCharSrc, CCharacterFollower* pCharDst)
{
	Ast(pCharDst->GetRenderObject() && "CFacingMgr::SetInterestObj");
	pCharSrc->SetInterestedObj(pCharDst);
	CAppClient::Inst()->UnRegisterTick(this);
	CAppClient::Inst()->RegisterTick(this, 33);
}

void CFacingMgr::ClearInterested()
{
	CCharacterFollower* pCharSrc = CCharacterFollower::GetCharacterByID(m_uSrcEntityID);
	if(pCharSrc)
	{
		if (!pCharSrc->ClearInterested(false))
		{
			m_uSrcEntityID		= 0;
			CAppClient::Inst()->UnRegisterTick(this);
		}
	}

	m_uDstEntityID = 0;
}

void CFacingMgr::OnTick()
{
	bool result = Do();
	if(!result)
		RemoveFacingPair();
}

bool CFacingMgr::Do()
{
	CCharacterFollower* pCharSrc = CCharacterFollower::GetCharacterByID(m_uSrcEntityID);
	CCharacterFollower* pCharDst = CCharacterFollower::GetCharacterByID(m_uDstEntityID);
	if(!pCharSrc || !pCharDst)
		return false;

	if (!pCharSrc->CppIsAlive() || pCharSrc->CppGetCtrlState(eFCS_FeignDeath))
		return false;

	CRenderObject* pDstRednerObj	= pCharDst->GetRenderObject();

	if (pDstRednerObj)
	{
		//if (pCharSrc->CppGetCtrlState(eFCS_ForbitTurnAround))
		//	return true;

		pCharSrc->SetInterestedObj(pCharDst);

		if (pCharSrc->CppGetCtrlState(eFCS_InNormalHorse))
			return true;

		m_fNADistance = pCharSrc->GetActor()->GetWalkBackRadius();
		
		CFPos posSelf	= pCharSrc->GetPixelPos();
		CFPos posTarget	= pCharDst->GetPixelPos();
		float x = posTarget.x - posSelf.x;
		float y = posTarget.y - posSelf.y;
		float fDistance = pCharSrc->GetEntityDistInGrid(pCharDst);

		if (pCharSrc->IsDirLockedTarget())
		{
			if (pCharSrc->CppGetCtrlState(eFCS_InBattle) && fDistance > m_fNADistance)
				return true;

			CDir dirTarget;
			CVector2f vecDir;
			vecDir.x = x;
			vecDir.y = y;
			dirTarget.Set(vecDir);
			uint8 uDirectorDir = pCharSrc->GetActor()->GetActionDir();
			uint8 diffDir = dirTarget.uDir - uDirectorDir;

			if ( pCharSrc->IsMoving())
			{
				if (pCharSrc->GetActor()->GetCurOnceActionLevel() == eAPP_AttackAction)
				{
					if ( uDirectorDir != dirTarget.uDir )
					{
						pCharSrc->DoActionDir(dirTarget.uDir);
					}

					if ( diffDir > 64 && diffDir < 192)
					{
						if ( eMM_Run == pCharSrc->GetMoveMode(pCharSrc->GetActor()->GetWalkBackRadius()) )
							pCharSrc->GetActor()->DoActionState(eAS_Run_Battle);
						else
							pCharSrc->GetActor()->DoActionState(eAS_Walk_Battle);
					}
				}
				else
				{
					if ( diffDir > 64 && diffDir < 192)
					{
						uint8 uMoveDir = pCharSrc->GetMoveDir();
						uint8 uActionDir;
						if (diffDir < 128)
							uActionDir = uMoveDir + 125;		// 目标在右，顺时针转
						else
							uActionDir = uMoveDir - 125;		// 目标在左，逆时针转

						pCharSrc->GetActor()->SetActionDir(uActionDir);
						if ( eMM_Run == pCharSrc->GetMoveMode(pCharSrc->GetActor()->GetWalkBackRadius()) )
							pCharSrc->GetActor()->DoActionState(eAS_Run_Battle, uActionDir);
						else
							pCharSrc->GetActor()->DoActionState(eAS_Walk_Battle, uActionDir);
					}
				}
			}
			else
			{
				if ( uDirectorDir != dirTarget.uDir )
				{
					pCharSrc->DoActionDir(dirTarget.uDir);
				}
			}	
		}
		return true;
	}

	return false;
}
