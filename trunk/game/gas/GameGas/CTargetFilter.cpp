#include "stdafx.h"
#include "CTargetFilter.h"
#include "CSkillRuleMgr.h"
#include "CEnmityMgr.h"
#include "CCharacterDictator.h"
#include "CDir.h"
#include "CSceneCommon.h"
#include "Random.h"
#include "CNpcEnmityMember.h"
#include "CCoreSceneServer.h"
#include "ICoreObjectServerHandler.h"
#include "CRelationMgrServer.h"
#include "NpcInfoMgr.h"

CTargetFilter::CharacterVecType CTargetFilter::ms_vecCharachter;

CTargetFilter::CTargetFilter(CSkillRule* pSkillRule, ESRTargetType eTargetType, uint32 uTargetArg1, uint32 uTargetArg2)
:m_pSkillRule(pSkillRule)
,m_eTargetType(eTargetType)
,m_uTargetArg1(uTargetArg1)
,m_uTargetArg2(uTargetArg2)
{
	switch(m_eTargetType)
	{
	case eSRTT_P_EnemyPostion:
	case eSRTT_P_SelfPosition:
	case eSRTT_P_RandomEnemyPostion:
	case eSRTT_P_SelfDirection:
	case eSRTT_P_SelfReverseDirection:
	case eSRTT_P_RandomFriendPos:
	case eSRTT_P_MaxHpEnemyPos:
	case eSRTT_P_MinHpEnemyPos:
	case eSRTT_P_MaxMpEnemyPos:
	case eSRTT_P_MinMpEnemyPos:
	case eSRTT_P_RandomDirection:
	case eSRTT_P_NearestEnemyPos:
	case eSRTT_P_FarthestEnemyPos:
	case eSRTT_P_NearestFriendPos:
	case eSRTT_P_FarthestFriendPos:
	case eSRTT_P_RandomUnEnmityPos:
	case eSRTT_P_RandomUnServantPos:
	case eSRTT_P_RamdomUnEnmityUnServantPos:
	case eSRTT_P_MaxHpFriendPos:
	case eSRTT_P_MinHpFriendPos:
	case eSRTT_P_MaxMpFriendPos:
	case eSRTT_P_MinMpFriendPos:
	case eSRTT_P_AroundPos:
	case eSRTT_P_RandomPos:
		{
			m_eSkillType = ERuleSkillType_Pos;
		}
		break;
	default:
		{
			m_eSkillType = ERuleSkillType_Target;
		} 
		break;
	}
}

CTargetFilter::~CTargetFilter()
{

}

CNpcAI* CTargetFilter::GetOwnerAI()
{
	return GetSkillRule()->GetOwner();
}

CSkillRule* CTargetFilter::GetSkillRule()
{
	return m_pSkillRule;
}

ERuleSkillType CTargetFilter::GetRuleSkillType()
{
	return m_eSkillType;
}

uint32 CTargetFilter::GetTargetID()
{
	return m_uTargetID;
}

void CTargetFilter::GetTargetPos(CFPos &pos)
{
	pos = m_TargetPos;
}

bool CTargetFilter::SetSkillRuleTarget()
{
	switch(m_eSkillType)
	{
	case ERuleSkillType_Target:
		{
			CCharacterDictator* pTarget = NULL;
			if (GetCharacterTarget(pTarget) && pTarget)
			{
				m_uTargetID = pTarget->GetEntityID();
				return true;
			}
		}
		break;
	case ERuleSkillType_Pos:
		{
			if (GetPositionTarget(m_TargetPos))
			{
				return true;
			}
		}
		break;

	default:
		break;
	}
	return false;
}

bool CTargetFilter::GetPositionTarget(CFPos &pos)
{
	CCharacterDictator* pOwnChar = GetSkillRule()->GetCharacter();
	if(!pOwnChar)
		return false;
	switch(m_eTargetType)
	{
	case eSRTT_P_EnemyPostion:
		{
			CCharacterDictator* pTarget = GetOwnerAI()->GetFirstEnmityEnemy();
			if(!pTarget)
				return false;
			else
			{
				pTarget->GetPixelPos(pos);
				return true;
			}
		}
	case eSRTT_P_SelfPosition:
		{
			pOwnChar->GetPixelPos(pos);
			return true;
		}
	case eSRTT_P_RandomEnemyPostion:
		{
			CEnmityMgr* pEnmityMgr = GetOwnerAI()->GetEnmityMgr();
			CCharacterDictator* pTargetCharacter = pEnmityMgr->GetRandomEnemy();
			if (!pTargetCharacter)
				return false;
			else
			{
				pTargetCharacter->GetPixelPos(pos);
				return true;
			}
		}
	case eSRTT_P_SelfDirection:
		{
			CFPos ownerPos;
			pOwnChar->GetPixelPos(ownerPos);
			float dDistance = (float)(m_uTargetArg1);
			CDir dirCur;
			dirCur.SetDir(pOwnChar->GetActionDir());
			CVector2f vecDirf;
			dirCur.Get(vecDirf);
			pos.x = ownerPos.x + dDistance *eGridSpanForObj* vecDirf.x;
			pos.y = ownerPos.y + dDistance *eGridSpanForObj* vecDirf.y;
			return true;
		}
	case eSRTT_P_SelfReverseDirection:
		{
			CFPos ownerPos;
			pOwnChar->GetPixelPos(ownerPos);
			float dDistance = (float)(m_uTargetArg1);
			CDir dirCur;
			dirCur.SetDir(pOwnChar->GetActionDir()+128);
			CVector2f vecDirf;
			dirCur.Get(vecDirf);
			pos.x = ownerPos.x + dDistance *eGridSpanForObj* vecDirf.x;
			pos.y = ownerPos.y + dDistance *eGridSpanForObj* vecDirf.y;
			return true;
		}
	case eSRTT_P_RandomFriendPos:
		{
			float fDist = (float) m_uTargetArg1;
			CCharacterDictator* pCharacter = GetRandomFriend(fDist);
			if (pCharacter)
			{
				pCharacter->GetPixelPos(pos);
				return true;
			}
			return false;
		}
	case eSRTT_P_MaxHpEnemyPos:
		{
			CCharacterDictator* pCharacter = GetMaxHpEnemy();
			if (pCharacter)
			{
				pCharacter->GetPixelPos(pos);
				return true;
			}
			return false;
		}
	case eSRTT_P_MinHpEnemyPos:
		{
			CCharacterDictator* pCharacter = GetMinHpEnemy();
			if (pCharacter)
			{
				pCharacter->GetPixelPos(pos);
				return true;
			}
			return false;
		}
	case eSRTT_P_MaxMpEnemyPos:
		{
			CCharacterDictator* pCharacter = GetMaxMpEnemy();
			if (pCharacter)
			{
				pCharacter->GetPixelPos(pos);
				return true;
			}
			return false;
		}
	case eSRTT_P_MinMpEnemyPos:
		{
			CCharacterDictator* pCharacter = GetMinMpEnemy();
			if (pCharacter)
			{
				pCharacter->GetPixelPos(pos);
				return true;
			}
			return false;
		}
	case eSRTT_P_RandomDirection:
		{
			CFPos ownerPos;
			pOwnChar->GetPixelPos(ownerPos);
			float dDistance = (float)(m_uTargetArg1);
			CDir dirCur;
			uint8 uRandomDir = Random::Rand(0, 255);
			dirCur.SetDir(uRandomDir);
			CVector2f vecDirf;
			dirCur.Get(vecDirf);
			pos.x = ownerPos.x + dDistance *eGridSpanForObj* vecDirf.x;
			pos.y = ownerPos.y + dDistance *eGridSpanForObj* vecDirf.y;
			return true;
		}
	case eSRTT_P_NearestEnemyPos:
		{
			CCharacterDictator* pCharacter = GetNearestEnemy();
			if (pCharacter)
			{
				pCharacter->GetPixelPos(pos);
				return true;
			}
			return false;
		}
	case eSRTT_P_FarthestEnemyPos:
		{
			CCharacterDictator* pCharacter = GetFarthestEnemy();
			if (pCharacter)
			{
				pCharacter->GetPixelPos(pos);
				return true;
			}
			return false;
		}
	case eSRTT_P_NearestFriendPos:
		{
			float fDist = (float) m_uTargetArg1;
			CCharacterDictator* pCharacter = GetNearestFirend(fDist);
			if (pCharacter)
			{
				pCharacter->GetPixelPos(pos);
				return true;
			}
			return false;
		}
	case eSRTT_P_FarthestFriendPos:
		{
			float fDist = (float) m_uTargetArg1;
			CCharacterDictator* pCharacter = GetFarthestFriend(fDist);
			if (pCharacter)
			{
				pCharacter->GetPixelPos(pos);
				return true;
			}
			return false;
		}
	case eSRTT_P_RandomUnEnmityPos:
		{
			float fDist = (float) m_uTargetArg1;
			CCharacterDictator* pCharacter = GetRandomUnEnmity(fDist);
			if (pCharacter)
			{
				pCharacter->GetPixelPos(pos);
				return true;
			}
			return false;
		}
	case eSRTT_P_RandomUnServantPos:
		{
			float fDist = (float) m_uTargetArg1;
			CCharacterDictator* pCharacter = GetRandomUnServant(fDist);
			if (pCharacter)
			{
				pCharacter->GetPixelPos(pos);
				return true;
			}
			return false;
		}
	case eSRTT_P_RamdomUnEnmityUnServantPos:
		{
			float fDist = (float) m_uTargetArg1;
			CCharacterDictator* pCharacter = GetRandomUnEnmityUnServant(fDist);
			if (pCharacter)
			{
				pCharacter->GetPixelPos(pos);
				return true;
			}
			return false;
		}
	case eSRTT_P_MaxHpFriendPos:
		{
			float fDist = (float) m_uTargetArg1;
			CCharacterDictator* pCharacter = GetMaxHpFriend(fDist);
			if (pCharacter)
			{
				pCharacter->GetPixelPos(pos);
				return true;
			}
			return false;
		}
	case eSRTT_P_MinHpFriendPos:
		{
			float fDist = (float) m_uTargetArg1;
			CCharacterDictator* pCharacter = GetMinHpFriend(fDist);
			if (pCharacter)
			{
				pCharacter->GetPixelPos(pos);
				return true;
			}
			return false;
		}
	case eSRTT_P_MaxMpFriendPos:
		{
			float fDist = (float) m_uTargetArg1;
			CCharacterDictator* pCharacter = GetMaxMpFriend(fDist);
			if (pCharacter)
			{
				pCharacter->GetPixelPos(pos);
				return true;
			}
			return false;
		}
	case eSRTT_P_MinMpFriendPos:
		{
			float fDist = (float) m_uTargetArg1;
			CCharacterDictator* pCharacter = GetMinMpFriend(fDist);
			if (pCharacter)
			{
				pCharacter->GetPixelPos(pos);
				return true;
			}
			return false;
		}
	case eSRTT_P_AroundPos:
	case eSRTT_P_RandomPos:
		{
			CCharacterDictator* pCharacter = GetOwnerAI()->GetCharacter();
			if (!pCharacter)
			{
				return false;
			}
			else
			{

				CFPos ownpos;
				pCharacter->GetPixelPos(ownpos);
				uint8 uCurAngle = (uint8)Random::Rand(0,255);
				uint32 uRealDist = Random::Rand(m_uTargetArg1, m_uTargetArg2);
				CDir dirCur;
				dirCur.SetDir(uCurAngle);
				CVector2f vecDirf;
				dirCur.Get(vecDirf);
				pos.x = ownpos.x + uRealDist*eGridSpanForObj * vecDirf.x;
				pos.y = ownpos.y + uRealDist*eGridSpanForObj * vecDirf.y;
				return true;
			}
		}
		break;
	default:
		{
			const string& strNpcName = GetOwnerAI()->GetNpcName();
			string& strSkillRuleName = GetSkillRule()->GetSkillRuleMgr()->m_strSkillRuleName;
			string& strSkillName = GetSkillRule()->m_strSkillName;

			ostringstream strm;
			strm<<"NpcName :  "<<strNpcName.c_str()<<" SkillRuleName : "<<strSkillRuleName.c_str()<<" SkillName : "<<strSkillName.c_str()<<" 目标类型： "<<(uint32)m_eTargetType<<endl;
			LogErr("npc not exist skillrule pos target !", strm.str().c_str());
		}
		break;
	}
	return false;
}

bool CTargetFilter::GetCharacterTarget(CCharacterDictator* &pCharacter)
{
	CCharacterDictator* pOwnChar = GetSkillRule()->GetCharacter();
	if(!pOwnChar)
		return false;
	CEnmityMgr* pEnmityMgr = GetOwnerAI()->GetEnmityMgr();
	switch(m_eTargetType)
	{
	case eSRTT_T_EnemyObject:
		{
			CCharacterDictator* pEnemy = GetOwnerAI()->GetFirstEnmityEnemy();
			if (!pEnemy)
				return false; 
			else
			{
				pCharacter = pEnemy;
				return true;
			}
		}
	case eSRTT_T_Self:
		{
			pCharacter = GetOwnerAI()->GetCharacter();
			return true;
		}
	case eSRTT_T_NearestEnemy:
		{
			CCharacterDictator* pTarget = GetNearestEnemy();
			if (pTarget)
			{
				pCharacter = pTarget;
				return true;
			}
			return false;
		}
	case eSRTT_T_FarthestEnemy:
		{
			CCharacterDictator* pTarget = GetFarthestEnemy();
			if (pTarget)
			{
				pCharacter = pTarget;
				return true;
			}
			return false;
		}

	case eSRTT_T_RandomEnemy:
		{
			CCharacterDictator* pTargetCharacter = pEnmityMgr->GetRandomEnemy();
			if (!pTargetCharacter)
				return false;
			else
			{
				pCharacter = pTargetCharacter;
				return true;
			}
		}
	case eSRTT_T_RandomFriend:
		{
			float fDistance = (float)(m_uTargetArg1);
			CCharacterDictator* pFriend = GetRandomFriend(fDistance);
			if (pFriend)
			{
				pCharacter = pFriend;
				return true;
			}
			return false;
		}
	case eSRTT_T_MaxHpEnemy:
		{
			CCharacterDictator* pTarget = GetMaxHpEnemy();
			if (pTarget)
			{
				pCharacter = pTarget;
				return true;
			} 
			else
				return false;
		}
	case eSRTT_T_MinHpEnemy:
		{
			CCharacterDictator* pTarget = GetMinHpEnemy();
			if (pTarget)
			{
				pCharacter = pTarget;
				return true;
			} 
			else
				return false;
		}
	case eSRTT_T_MaxMpEnemy:
		{
			CCharacterDictator* pTarget = GetMaxMpEnemy();
			if (pTarget)
			{
				pCharacter = pTarget;
				return true;
			} 
			else
				return false;
		}
	case eSRTT_T_MinMpEnemy:
		{
			CCharacterDictator* pTarget = GetMinMpEnemy();
			if (pTarget)
			{
				pCharacter = pTarget;
				return true;
			} 
			else
				return false;
		}
	case eSRTT_T_NearestFriend:
		{
			float dDistance = (float)(m_uTargetArg1);
			CCharacterDictator* pTarget = GetNearestFirend(dDistance);
			if (pTarget)
			{
				pCharacter = pTarget;
				return true;
			}
			return false;
		}
	case eSRTT_T_FarthestFriend:
		{
			float fDist = (float) m_uTargetArg1;
			CCharacterDictator* pTarget = GetFarthestFriend(fDist);
			if (pTarget)
			{
				pCharacter = pTarget;
				return true;
			}
			return false;
		}
	case eSRTT_T_RandomUnEnmity:
		{
			float fDist = (float) m_uTargetArg1;
			CCharacterDictator* pTarget = GetRandomUnEnmity(fDist);
			if (pTarget)
			{
				pCharacter = pTarget;
				return true;
			}
			return false;
		}
	case eSRTT_T_RandomUnServant:
		{
			float fDist = (float) m_uTargetArg1;
			CCharacterDictator* pTarget = GetRandomUnServant(fDist);
			if (pTarget)
			{
				pCharacter = pTarget;
				return true;
			}
			return false;
		}
	case eSRTT_T_RamdomUnEnmityUnServant:
		{
			float fDist = (float) m_uTargetArg1;
			CCharacterDictator* pTarget = GetRandomUnEnmityUnServant(fDist);
			if (fDist)
			{
				pCharacter = pTarget;
				return true;
			}
			return false;
		}
	case eSRTT_T_MaxHpFriend:
		{
			float fDist = (float) m_uTargetArg1;
			CCharacterDictator* pTarget = GetMaxHpFriend(fDist);
			if (pTarget)
			{
				pCharacter = pTarget;
				return true;
			}
			return false;
		}
	case eSRTT_T_MinHpFriend:
		{
			float fDist = (float) m_uTargetArg1;
			CCharacterDictator* pTargte = GetMinHpFriend(fDist);
			if (pTargte)
			{
				pCharacter = pTargte;
				return true;
			}
			return false;
		}
	case eSRTT_T_MaxMpFriend:
		{
			float fDist = (float) m_uTargetArg1;
			CCharacterDictator* pTarget = GetMaxMpFriend(fDist);
			if (pTarget)
			{
				pCharacter = pTarget;
				return true;
			}
			return false;
		}
	case eSRTT_T_MinMpFriend:
		{
			float fDist = (float) m_uTargetArg1;
			CCharacterDictator* pTarget = GetMinMpFriend(fDist);
			if (pTarget)
			{
				pCharacter = pTarget;
				return true;
			}
			return false;
		}
	default:
		{
			const string& strNpcName = GetOwnerAI()->GetNpcName();
			string& strSkillRuleName = GetSkillRule()->GetSkillRuleMgr()->m_strSkillRuleName;
			string& strSkillName = GetSkillRule()->m_strSkillName;
			
			ostringstream strm;
			strm<<"NpcName :  "<<strNpcName.c_str()<<" SkillRuleName : "<<strSkillRuleName.c_str()<<" SkillName : "<<strSkillName.c_str()<<" 目标类型： "<<(uint32)m_eTargetType<<endl;
			LogErr("npc not exist skillrule character target !", strm.str().c_str());
		}
		break;
	}
	return false;
}

CCharacterDictator* CTargetFilter::GetMaxHpEnemy()
{
	const CEnmityMgr::EnemyListType& lstEnemy = GetOwnerAI()->GetEnmityMgr()->GetEnemyList();
	if(lstEnemy.empty())
		return NULL;

	CCharacterDictator* pCharacter = NULL;	
	float fHpRate = 0.0f;
	for (EnemyListType::const_iterator iter = lstEnemy.begin();iter != lstEnemy.end();++iter)
	{
		CCharacterDictator* pTempChar = CCharacterDictator::GetCharacterByID((*iter)->GetEnemyEntityID());
		float fTempRate = pTempChar->CppGetHP()/pTempChar->CppGetMaxHP();
		if (fTempRate>fHpRate)
		{
			fHpRate = fTempRate;
			pCharacter = pTempChar;
		}
	}
	return pCharacter;
}

CCharacterDictator* CTargetFilter::GetMinHpEnemy()
{
	const CEnmityMgr::EnemyListType& lstEnemy = GetOwnerAI()->GetEnmityMgr()->GetEnemyList();
	if(lstEnemy.empty())
		return NULL;

	CCharacterDictator* pCharacter = NULL;	
	float fHpRate = 1.0f;
	for (EnemyListType::const_iterator iter = lstEnemy.begin();iter != lstEnemy.end();++iter)
	{
		CCharacterDictator* pTempChar = CCharacterDictator::GetCharacterByID((*iter)->GetEnemyEntityID());
		float fTempRate = pTempChar->CppGetHP()/pTempChar->CppGetMaxHP();
		if (fTempRate < fHpRate)
		{
			fHpRate = fTempRate;
			pCharacter = pTempChar;
		}
	}
	return pCharacter;
}

CCharacterDictator* CTargetFilter::GetMaxMpEnemy()
{
	const CEnmityMgr::EnemyListType& lstEnemy = GetOwnerAI()->GetEnmityMgr()->GetEnemyList();
	if(lstEnemy.empty())
		return NULL;

	CCharacterDictator* pCharacter = NULL;	
	float fMpRate = 0.0f;
	for (EnemyListType::const_iterator iter = lstEnemy.begin();iter != lstEnemy.end();++iter)
	{
		CCharacterDictator* pTempChar = CCharacterDictator::GetCharacterByID((*iter)->GetEnemyEntityID());
		float fTempRate = pTempChar->CppGetMP()/pTempChar->CppGetMaxMP();
		if (fTempRate > fMpRate)
		{
			fMpRate = fTempRate;
			pCharacter = pTempChar;
		}
	}
	return pCharacter;
}

CCharacterDictator* CTargetFilter::GetMinMpEnemy()
{
	const CEnmityMgr::EnemyListType& lstEnemy = GetOwnerAI()->GetEnmityMgr()->GetEnemyList();
	if(lstEnemy.empty())
		return NULL;

	CCharacterDictator* pCharacter = NULL;	
	float fMpRate = 1.0f;
	for (EnemyListType::const_iterator iter = lstEnemy.begin();iter != lstEnemy.end();++iter)
	{
		CCharacterDictator* pTempChar = CCharacterDictator::GetCharacterByID((*iter)->GetEnemyEntityID());
		float fTempRate = pTempChar->CppGetMP()/pTempChar->CppGetMaxMP();
		if (fTempRate < fMpRate)
		{
			fMpRate = fTempRate;
			pCharacter = pTempChar;
		}
	}
	return pCharacter;
}

CCharacterDictator* CTargetFilter::GetMaxHpFriend(float fRadius)
{
	vector<CCoreObjectServer* > vecObject;
	CCharacterDictator* pCharacter = GetOwnerAI()->GetCharacter();
	Ast(pCharacter);
	CFighterDictator* pFighter = pCharacter->GetFighter();
	CFPos pixelPos = pCharacter->GetPixelPos();
	pCharacter->GetScene()->QueryObject(vecObject, pixelPos, fRadius);
	if (vecObject.empty())
		return NULL;

	CCharacterDictator* pFriend = NULL;
	float fHpRate = 0.0f;

	for (vector<CCoreObjectServer* >::iterator it = vecObject.begin(); it != vecObject.end(); ++it)
	{	
		if ( !(*it)) 
			continue;
		ICoreObjectServerHandler* pHandler = (*it)->GetHandler();
		if ( pHandler == NULL )
			continue;
		size_t eType = (size_t)(pHandler->GetTag());
		if (eType == eCOHT_Character)
		{
			CCharacterDictator *pOther = CCharacterDictator::GetCharacterByCoreObj(*it);
			if (!pOther || pOther == pCharacter)
				continue;
			CFighterDictator* pOtherFighter = pOther->GetFighter();
			bool bFriend = CRelationMgrServer::Inst().IsFriend(pFighter, pOtherFighter);
			if (bFriend)
			{
				float fOtherHpRate = pOther->CppGetHP()/pOther->CppGetMaxHP();
				if (fOtherHpRate > fHpRate)
				{
					pFriend = pOther;
					fHpRate = fOtherHpRate;
				}
			}
		}
	}
	return pFriend;
}

CCharacterDictator* CTargetFilter::GetMinHpFriend(float fRadius)
{
	vector<CCoreObjectServer* > vecObject;
	CCharacterDictator* pCharacter = GetOwnerAI()->GetCharacter();
	Ast(pCharacter);
	CFighterDictator* pFighter = pCharacter->GetFighter();
	CFPos pixelPos = pCharacter->GetPixelPos();
	pCharacter->GetScene()->QueryObject(vecObject, pixelPos, fRadius);
	if (vecObject.empty())
		return NULL;

	CCharacterDictator* pFriend = NULL;
	float fHpRate = 1.0f;

	for (vector<CCoreObjectServer* >::iterator it = vecObject.begin(); it != vecObject.end(); ++it)
	{	
		if ( !(*it)) 
			continue;
		ICoreObjectServerHandler* pHandler = (*it)->GetHandler();
		if ( pHandler == NULL )
			continue;
		size_t eType = (size_t)(pHandler->GetTag());
		if (eType == eCOHT_Character)
		{
			CCharacterDictator *pOther = CCharacterDictator::GetCharacterByCoreObj(*it);
			if (!pOther || pOther == pCharacter)
				continue;
			CFighterDictator* pOtherFighter = pOther->GetFighter();
			bool bFriend = CRelationMgrServer::Inst().IsFriend(pFighter, pOtherFighter);
			if (bFriend)
			{
				float fOtherHpRate = pOther->CppGetHP()/pOther->CppGetMaxHP();
				if (fOtherHpRate < fHpRate)
				{
					pFriend = pOther;
					fHpRate = fOtherHpRate;
				}
			}
		}
	}
	return pFriend;
}

CCharacterDictator* CTargetFilter::GetMaxMpFriend(float fRadius)
{
	vector<CCoreObjectServer* > vecObject;
	CCharacterDictator* pCharacter = GetOwnerAI()->GetCharacter();
	Ast(pCharacter);
	CFighterDictator* pFighter = pCharacter->GetFighter();
	CFPos pixelPos = pCharacter->GetPixelPos();
	pCharacter->GetScene()->QueryObject(vecObject, pixelPos, fRadius);
	if (vecObject.empty())
		return NULL;

	CCharacterDictator* pFriend = NULL;
	float fMpRate = 0.0f;

	for (vector<CCoreObjectServer* >::iterator it = vecObject.begin(); it != vecObject.end(); ++it)
	{	
		if ( !(*it)) 
			continue;
		ICoreObjectServerHandler* pHandler = (*it)->GetHandler();
		if ( pHandler == NULL )
			continue;
		size_t eType = (size_t)(pHandler->GetTag());
		if (eType == eCOHT_Character)
		{
			CCharacterDictator *pOther = CCharacterDictator::GetCharacterByCoreObj(*it);
			if (!pOther || pOther == pCharacter)
				continue;
			CFighterDictator* pOtherFighter = pOther->GetFighter();
			bool bFriend = CRelationMgrServer::Inst().IsFriend(pFighter, pOtherFighter);
			if (bFriend)
			{
				float fOtherMpRate = pOther->CppGetMP()/pOther->CppGetMaxMP();
				if (fOtherMpRate > fMpRate)
				{
					pFriend = pOther;
					fMpRate = fOtherMpRate;
				}
			}
		}
	}
	return pFriend;
}

CCharacterDictator* CTargetFilter::GetMinMpFriend(float fRadius)
{
	vector<CCoreObjectServer* > vecObject;
	CCharacterDictator* pCharacter = GetOwnerAI()->GetCharacter();
	Ast(pCharacter);
	CFighterDictator* pFighter = pCharacter->GetFighter();
	CFPos pixelPos = pCharacter->GetPixelPos();
	pCharacter->GetScene()->QueryObject(vecObject, pixelPos, fRadius);
	if (vecObject.empty())
		return NULL;

	CCharacterDictator* pFriend = NULL;
	float fMpRate = 1.0f;

	for (vector<CCoreObjectServer* >::iterator it = vecObject.begin(); it != vecObject.end(); ++it)
	{	
		if ( !(*it)) 
			continue;
		ICoreObjectServerHandler* pHandler = (*it)->GetHandler();
		if ( pHandler == NULL )
			continue;
		size_t eType = (size_t)(pHandler->GetTag());
		if (eType == eCOHT_Character)
		{
			CCharacterDictator *pOther = CCharacterDictator::GetCharacterByCoreObj(*it);
			if (!pOther || pOther == pCharacter)
				continue;
			CFighterDictator* pOtherFighter = pOther->GetFighter();
			bool bFriend = CRelationMgrServer::Inst().IsFriend(pFighter, pOtherFighter);
			if (bFriend)
			{
				float fOtherMpRate = pOther->CppGetMP()/pOther->CppGetMaxMP();
				if (fOtherMpRate < fMpRate)
				{
					pFriend = pOther;
					fMpRate = fOtherMpRate;
				}
			}
		}
	}
	return pFriend;
}

CCharacterDictator* CTargetFilter::GetRandomFriend(float fRadius)
{
	vector<CCoreObjectServer* > vecObject;
	CCharacterDictator* pCharacter = GetOwnerAI()->GetCharacter();
	Ast(pCharacter);
	CFighterDictator* pFighter = pCharacter->GetFighter();
	CFPos pixelPos = pCharacter->GetPixelPos();
	pCharacter->GetScene()->QueryObject(vecObject, pixelPos, fRadius);

	ms_vecCharachter.clear();
	for (vector<CCoreObjectServer* >::iterator it = vecObject.begin(); it != vecObject.end(); ++it)
	{	
		if ( !(*it)) 
			continue;
		ICoreObjectServerHandler* pHandler = (*it)->GetHandler();
		if ( pHandler == NULL )
			continue;
		size_t eType = (size_t)(pHandler->GetTag());
		if (eType == eCOHT_Character)
		{
			CCharacterDictator *pChar = CCharacterDictator::GetCharacterByCoreObj(*it);
			if (!pChar || pChar == pCharacter)
				continue;
			CFighterDictator* pOtherFighter = pChar->GetFighter();
			bool bFriend = CRelationMgrServer::Inst().IsFriend(pFighter, pOtherFighter);
			if (bFriend)
			{
				ms_vecCharachter.push_back(pChar);
			}
		}
	}
	if (ms_vecCharachter.size() > 0)
	{
		uint32 uRandomIndex = Random::Rand(0,ms_vecCharachter.size()-1);
		return ms_vecCharachter[uRandomIndex];
	}
	return NULL;
}

CCharacterDictator* CTargetFilter::GetNearestFirend(float fRadius)
{
	vector<CCoreObjectServer* > vecObject;
	CCharacterDictator* pCharacter = GetOwnerAI()->GetCharacter();
	Ast(pCharacter);
	CFighterDictator* pFighter = pCharacter->GetFighter();
	CFPos pixelPos = pCharacter->GetPixelPos();
	pCharacter->GetScene()->QueryObject(vecObject, pixelPos, fRadius);

	CCharacterDictator* pFriend = NULL;
	float fDist = 0.0f;
	for (vector<CCoreObjectServer* >::iterator it = vecObject.begin(); it != vecObject.end(); ++it)
	{	
		if ( !(*it)) 
			continue;
		ICoreObjectServerHandler* pHandler = (*it)->GetHandler();
		if ( pHandler == NULL )
			continue;
		size_t eType = (size_t)(pHandler->GetTag());
		if (eType == eCOHT_Character)
		{
			CCharacterDictator *pChar = CCharacterDictator::GetCharacterByCoreObj(*it);
			if (!pChar || pChar == pCharacter)
				continue;
			CFighterDictator* pOtherFighter = pChar->GetFighter();
			bool bFriend = CRelationMgrServer::Inst().IsFriend(pFighter, pOtherFighter);
			if (bFriend)
			{
				CFPos otherpos;
				pChar->GetPixelPos(otherpos);
				float flocaldis = pixelPos.Dist(otherpos);
				if (fDist == 0.0f)
				{
					pFriend = pChar;
					fDist = flocaldis;
					continue;
				}
				if (flocaldis < fDist)
				{
					pFriend = pChar;
					fDist = flocaldis;
				}
			}
		}
	}
	return pFriend;
}

CCharacterDictator* CTargetFilter::GetFarthestFriend(float fRadius)
{
	vector<CCoreObjectServer* > vecObject;
	CCharacterDictator* pCharacter = GetOwnerAI()->GetCharacter();
	Ast(pCharacter);
	CFighterDictator* pFighter = pCharacter->GetFighter();
	CFPos pixelPos = pCharacter->GetPixelPos();
	pCharacter->GetScene()->QueryObject(vecObject, pixelPos, fRadius);

	CCharacterDictator* pFriend = NULL;
	float fDist = 0.0f;
	for (vector<CCoreObjectServer* >::iterator it = vecObject.begin(); it != vecObject.end(); ++it)
	{	
		if ( !(*it)) 
			continue;
		ICoreObjectServerHandler* pHandler = (*it)->GetHandler();
		if ( pHandler == NULL )
			continue;
		size_t eType = (size_t)(pHandler->GetTag());
		if (eType == eCOHT_Character)
		{
			CCharacterDictator *pChar = CCharacterDictator::GetCharacterByCoreObj(*it);
			if (!pChar || pChar == pCharacter)
				continue;
			CFighterDictator* pOtherFighter = pChar->GetFighter();
			bool bFriend = CRelationMgrServer::Inst().IsFriend(pFighter, pOtherFighter);
			if (bFriend)
			{
				CFPos otherpos;
				pChar->GetPixelPos(otherpos);
				float flocaldis = pixelPos.Dist(otherpos);
				if (fDist == 0.0f)
				{
					pFriend = pChar;
					fDist = flocaldis;
					continue;
				}
				if (flocaldis > fDist)
				{
					pFriend = pChar;
					fDist = flocaldis;
				}
			}
		}
	}
	return pFriend;
}

CCharacterDictator* CTargetFilter::GetNearestEnemy()
{
	const CEnmityMgr::EnemyListType& lstEnemy = GetOwnerAI()->GetEnmityMgr()->GetEnemyList();
	if(lstEnemy.empty())
		return NULL;
	
	CCharacterDictator* pCharacter = GetOwnerAI()->GetCharacter();
	Ast (pCharacter);
	EnemyListType::const_iterator iter = lstEnemy.begin();
	CCharacterDictator* pNearestEnemy = CCharacterDictator::GetCharacterByID((*iter)->GetEnemyEntityID());
	iter++;
	float fDist = pCharacter->GetEntityDist(pNearestEnemy);

	for (;iter != lstEnemy.end();++iter )
	{
		CCharacterDictator* pOther = CCharacterDictator::GetCharacterByID((*iter)->GetEnemyEntityID());
		if (!pOther)
			continue;
		float fDistOther = pCharacter->GetEntityDist(pOther);
		if (fDistOther < fDist)
		{
			pNearestEnemy = pOther;
			fDist = fDistOther;
		}
	}
	return pNearestEnemy;
}

CCharacterDictator* CTargetFilter::GetFarthestEnemy()
{
	const CEnmityMgr::EnemyListType& lstEnemy = GetOwnerAI()->GetEnmityMgr()->GetEnemyList();
	if(lstEnemy.empty())
		return NULL;

	CCharacterDictator* pCharacter = GetOwnerAI()->GetCharacter();
	Ast (pCharacter);
	EnemyListType::const_iterator iter = lstEnemy.begin();
	CCharacterDictator* pNearestEnemy = CCharacterDictator::GetCharacterByID((*iter)->GetEnemyEntityID());
	iter++;
	float fDist = pCharacter->GetEntityDist(pNearestEnemy);

	for (;iter != lstEnemy.end();++iter )
	{
		CCharacterDictator* pOther = CCharacterDictator::GetCharacterByID((*iter)->GetEnemyEntityID());
		if (!pOther)
			continue;
		float fDistOther = pCharacter->GetEntityDist(pOther);
		if (fDistOther > fDist)
		{
			pNearestEnemy = pOther;
			fDist = fDistOther;
		}
	}
	return pNearestEnemy;
}

CCharacterDictator* CTargetFilter::GetRandomUnEnmity(float fRadius)
{
	vector<CCoreObjectServer* > vecObject;
	CCharacterDictator* pCharacter = GetOwnerAI()->GetCharacter();
	Ast(pCharacter);
	CFPos pixelPos = pCharacter->GetPixelPos();
	pCharacter->GetScene()->QueryObject(vecObject, pixelPos, fRadius);

	CEnmityMgr* pEnmityMgr = GetOwnerAI()->GetEnmityMgr();
	Ast (pEnmityMgr);

	ms_vecCharachter.clear();
	for (vector<CCoreObjectServer* >::iterator it = vecObject.begin(); it != vecObject.end(); ++it)
	{	
		if ( !(*it)) 
			continue;
		ICoreObjectServerHandler* pHandler = (*it)->GetHandler();
		if ( pHandler == NULL )
			continue;
		size_t eType = (size_t)(pHandler->GetTag());
		if (eType == eCOHT_Character)
		{
			CCharacterDictator* pOther = CCharacterDictator::GetCharacterByCoreObj(*it);
			if (!pOther || pOther == pCharacter)
				continue;
			bool bCanFight = GetOwnerAI()->CanFight(pOther);
			if (pEnmityMgr->GetFirstEnemy() != pOther && bCanFight)
			{
				ms_vecCharachter.push_back(pOther);
			}
		}
	}
	if (ms_vecCharachter.size() > 0)
	{
		uint32 uRandomIndex = Random::Rand(0,ms_vecCharachter.size()-1);
		return ms_vecCharachter[uRandomIndex];
	}
	return pEnmityMgr->GetFirstEnemy();
}

CCharacterDictator* CTargetFilter::GetRandomUnServant(float fRadius)
{
	vector<CCoreObjectServer* > vecObject;
	CCharacterDictator* pCharacter = GetOwnerAI()->GetCharacter();
	Ast(pCharacter);
	CFPos pixelPos = pCharacter->GetPixelPos();
	pCharacter->GetScene()->QueryObject(vecObject, pixelPos, fRadius);

	CEnmityMgr* pEnmityMgr = GetOwnerAI()->GetEnmityMgr();
	Ast (pEnmityMgr);

	ms_vecCharachter.clear();
	for (vector<CCoreObjectServer* >::iterator it = vecObject.begin(); it != vecObject.end(); ++it)
	{	
		if ( !(*it)) 
			continue;
		ICoreObjectServerHandler* pHandler = (*it)->GetHandler();
		if ( pHandler == NULL )
			continue;
		size_t eType = (size_t)(pHandler->GetTag());
		if (eType == eCOHT_Character)
		{
			CCharacterDictator* pOther = CCharacterDictator::GetCharacterByCoreObj(*it);
			if (!pOther || pOther == pCharacter)
				continue;
			bool bCanFight = GetOwnerAI()->CanFight(pOther);
			bool bServant = NpcInfoMgr::BeServantType(pOther->GetNpcType());
			if (bCanFight && !bServant)
			{
				ms_vecCharachter.push_back(pOther);
			}
		}
	}
	if (ms_vecCharachter.size() > 0)
	{
		uint32 uRandomIndex = Random::Rand(0,ms_vecCharachter.size()-1);
		return ms_vecCharachter[uRandomIndex];
	}
	return NULL;
}

CCharacterDictator* CTargetFilter::GetRandomUnEnmityUnServant(float fRadius)
{
	vector<CCoreObjectServer* > vecObject;
	CCharacterDictator* pCharacter = GetOwnerAI()->GetCharacter();
	Ast(pCharacter);
	CFPos pixelPos = pCharacter->GetPixelPos();
	pCharacter->GetScene()->QueryObject(vecObject, pixelPos, fRadius);

	CEnmityMgr* pEnmityMgr = GetOwnerAI()->GetEnmityMgr();
	Ast (pEnmityMgr);

	ms_vecCharachter.clear();
	for (vector<CCoreObjectServer* >::iterator it = vecObject.begin(); it != vecObject.end(); ++it)
	{	
		if ( !(*it)) 
			continue;
		ICoreObjectServerHandler* pHandler = (*it)->GetHandler();
		if ( pHandler == NULL )
			continue;
		size_t eType = (size_t)(pHandler->GetTag());
		if (eType == eCOHT_Character)
		{
			CCharacterDictator* pOther = CCharacterDictator::GetCharacterByCoreObj(*it);
			if (!pOther || pOther == pCharacter)
				continue;
			bool bCanFight = GetOwnerAI()->CanFight(pOther);
			bool bServant = NpcInfoMgr::BeServantType(pOther->GetNpcType());
			if (bCanFight && !bServant && pEnmityMgr->GetFirstEnemy() != pOther)
			{
				ms_vecCharachter.push_back(pOther);
			}
		}
	}
	if (ms_vecCharachter.size() > 0)
	{
		uint32 uRandomIndex = Random::Rand(0,ms_vecCharachter.size()-1);
		return ms_vecCharachter[uRandomIndex];
	}
	return pEnmityMgr->GetFirstEnemy();
}

