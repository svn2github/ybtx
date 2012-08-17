#include "stdafx.h"
#include "CShockWaveMagicServer.h"
#include "CShockWaveMagicCfgServer.h"
#include "CMagicServer.inl"
#include "CCoreSceneServer.h"
#include "CFighterDictator.h"
#include "CMagicEffServer.h"
#include "CoreCommon.h"
#include "CCoreObjectDictator.h"
#include "CCharacterDictator.h"
#include "CSkillServer.h"
#include "CEntityServerManager.h"
#include "CQuickRand.h"
#include "CFilterOpServer.h"
#include "CGenericTarget.h"
#include "CSkillInstServer.h"
#include "CDir.h"
#include "BaseHelper.h"
#include "TSqrAllocator.inl"
#include "CMagicMgrServer.h"

#define SIDELENGTH 100
CShockWaveMagicHandlerServer::CShockWaveMagicHandlerServer(CShockWaveMagicServer* pShockWaveMagic,CSkillInstServer* pInst, CFighterDictator* pFrom, const CFPos& pos)
:CMagicServer(pInst,pFrom,0)
,m_pShockWaveMagic(pShockWaveMagic)
{
	float fRadian			= float( GetCfgSharedPtr()->GetRadius()->GetDblValue(pFrom, pInst->GetSkillLevel() - 1) );
	m_fRange				= float( GetCfgSharedPtr()->GetRange()->GetDblValue(pFrom, pInst->GetSkillLevel() - 1) );	
	m_uKillMaxNum			= GetCfgSharedPtr()->GetAmountLimit()->GetIntValue(pFrom, pInst->GetSkillLevel() - 1);
	CreateCoreObj<CShockWaveMagicHandlerServer>(pos);
	SetEyeSight(fRadian);
	m_pSkillInst->SetMagicBeginPos(pos);
	SetDestPos(pos);
}

CShockWaveMagicHandlerServer::~CShockWaveMagicHandlerServer()
{
	DeleteObj(GetFighter()->GetScene());
	m_pShockWaveMagic->RemoveHandler(this);
}

void CShockWaveMagicHandlerServer::OnMoveEnded(EMoveEndedType eMEType, uint32 uMoveID)
{
	if (eBTT_Line == GetCfgSharedPtr()->GetBulletTrackType())
	{
		void* pContext = NULL;
		GetCoreObj()->PostAoiMsgToSelf(pContext);
	}
	else if (eBTT_Random == GetCfgSharedPtr()->GetBulletTrackType())
	{
		EMoveToResult eResult;
		CFPos posFrom, posBulletStart, posBulletEnd;

		// 这有问题！beingway
		if ((int32)GetCfgSharedPtr()->GetOperateObject() == (int32)eFSTT_SelfPosition)
		{
			posFrom = GetFighter()->GetPixelPos();
		}

		uint32 uDiff = uint32(m_fRange*eGridSpanForObj);
		uint32 uMinX = uint32(posFrom.x - uDiff);
		uint32 uMaxX = uint32(posFrom.x + uDiff);
		uint32 uMinY = uint32(posFrom.y - uDiff);
		uint32 uMaxY = uint32(posFrom.y + uDiff);
		if (uMinX < 0)
			uMinX = 0;
		if (uMinY < 0)
			uMinY = 0;

		uint32 uCount;
		bool bOk;

		SetShockWaveHandler_t::iterator it_coreObj= m_pShockWaveMagic->GetShockWaveMagicSet().begin();
		for(; it_coreObj!= m_pShockWaveMagic->GetShockWaveMagicSet().end(); ++it_coreObj)
		{	
			uCount = 0;
			bOk = false;

			posBulletStart = (*it_coreObj)->GetCoreObj()->GetPixelPos();

			while (!bOk)
			{
				posBulletEnd.x = CQuickRand::Rand1((posBulletStart.x - SIDELENGTH), (posBulletStart.x + SIDELENGTH));
				posBulletEnd.y = CQuickRand::Rand1((posBulletStart.y - SIDELENGTH), (posBulletStart.y + SIDELENGTH));

				if (posBulletEnd.x < int32(uMinX) || posBulletEnd.x > int32(uMaxX) 
					|| posBulletEnd.y < int32(uMinY) || posBulletEnd.y > int32(uMaxY))
				{
					++uCount;

					if (uCount > 5)
					{
						posBulletEnd.x = CQuickRand::Rand1((posFrom.x - SIDELENGTH), (posFrom.x + SIDELENGTH));
						posBulletEnd.y = CQuickRand::Rand1((posFrom.y - SIDELENGTH), (posFrom.y + SIDELENGTH));
						bOk = true;
					}
					continue;
				}

				bOk = true;
			}

			eResult = (*it_coreObj)->GetCoreObj()->MoveTo(posBulletEnd, GetCfgSharedPtr()->GetSpeed(), eFPT_HypoLine, eBT_MidBarrier,0);
			if(eResult == eMTR_Success)
			{
				(*it_coreObj)->SetDestPos(posBulletEnd);
			}
		}
	}	
}

CShockWaveMagicCfgServerSharedPtr&	CShockWaveMagicHandlerServer::GetCfgSharedPtr()const
{
	return m_pShockWaveMagic->GetCfgSharedPtr();
}

void CShockWaveMagicHandlerServer::OnCaughtViewSightOf(uint32 uObjGlobalId, uint32 uDimension)
{
	CCoreObjectServer* pCoreObj = CCoreObjectServer::GetObjectServer(uObjGlobalId);
	if (!pCoreObj || !pCoreObj->GetHandler() || m_uKillMaxNum <= 0)
		return;
	CCharacterDictator* pCharacter=NULL;
	size_t uType = (size_t)(pCoreObj->GetHandler()->GetTag());
	if(uType == (uint32)(eCOHT_Character))
	{
		pCharacter =CCharacterDictator::GetCharacterByCoreObj(pCoreObj);
	}
	CFighterDictator* pTo = pCharacter?pCharacter->GetFighter():NULL;
	CFighterDictator* pFrom = GetFighter();
	uint32 uCheckID = GetCheckID();
	DoMagicEff(pTo);
	if (!pFrom->GetMagicMgr()->IsValid(uCheckID))
		return;
	if (!pFrom->CppIsAlive())
	{
		delete this;
		return;
	}
	if (eBTT_Random == GetCfgSharedPtr()->GetBulletTrackType()&&GetCfgSharedPtr()->GetOperateObject()==eOOT_Enemy)
	{
		CFPos posTo, posObj;
		posTo = pTo->GetPixelPos();
		float fDistance = float(GetCfgSharedPtr()->GetRadius()->GetDblValue(pFrom, m_pSkillInst->GetSkillLevel() - 1));

		SetShockWaveHandler_t::iterator it_Obj = m_pShockWaveMagic->GetShockWaveMagicSet().begin();
		for(; it_Obj != m_pShockWaveMagic->GetShockWaveMagicSet().end(); ++it_Obj)
		{
			posObj = (*it_Obj)->GetCoreObj()->GetPixelPos();
			float x = posTo.x - posObj.x;
			float y = posTo.y - posObj.y;
			float fDis = sqrt(x*x + y*y)/eGridSpanForObj;
			if (fDistance >= fDis)
			{
				(*it_Obj)->DeleteObj(pFrom->GetScene());
				m_pShockWaveMagic->GetShockWaveMagicSet().erase(it_Obj);
				break;
			}
		}														
	}
	if (m_uKillMaxNum <= 0 && GetCfgSharedPtr()->IsDisappearWhenEnough())
		delete this;
}

void CShockWaveMagicHandlerServer::OnAoiMsgFromSelf(const void* pContext)
{
	delete this;
}

void CShockWaveMagicHandlerServer::SetDestPos(const CFPos& pos)
{
	m_posDest = pos;
}

const CFPos& CShockWaveMagicHandlerServer::GetDestPos()const
{
	return m_posDest;
}

void CShockWaveMagicHandlerServer::DoMagicEff(CFighterDictator* pTo)
{
	if (pTo==NULL||pTo == GetFighter() || !pTo->CppIsAlive()|| ! m_pShockWaveMagic->IsSuitOfTheArea(pTo))
		return;

	GetFighter()->Detach(m_pShockWaveMagic, eCE_Die);
	CGenericTarget Target(pTo);
	CMagicOpTreeServer::VecEntityID vecEntityID;
	FilterLimit filterLimit;
	MakeFilterLimit<CShockWaveMagicHandlerServer>(filterLimit);
	CFilterOpServer::FilterOperateObject(m_pSkillInst, vecEntityID, GetCfgSharedPtr()->GetOperateObject(), filterLimit, GetFighter(), &Target);
	CMagicOpTreeServer::VecEntityID::iterator it_obj = vecEntityID.begin();
	for (; it_obj != vecEntityID.end()&&m_uKillMaxNum>0; ++it_obj,--m_uKillMaxNum)
	{
		m_pSkillInst->SetShockWaveCounter(m_pShockWaveMagic->GetKillCount() + 1);
		CCharacterDictator* pCharacter = CEntityServerManager::GetInst()->GetCharacter(*it_obj);
		if (NULL==pCharacter||m_pShockWaveMagic->GetFinishObjSet().count(pCharacter->GetFighter()) == 1)
		{
			continue;
		}
		uint32 uCheckID = GetCheckID();
		CFighterDictator* pFrom = GetFighter();
		GetCfgSharedPtr()->GetMagicEff()->Do(m_pSkillInst, GetFighter(), pTo);
		if (!pFrom->GetMagicMgr()->IsValid(uCheckID))
			return;
		m_pShockWaveMagic->GetFinishObjSet().insert(pTo);		
		m_pShockWaveMagic->AddKillCount();
	}

	GetFighter()->Attach(m_pShockWaveMagic, eCE_Die);
}

CShockWaveMagicServer::CShockWaveMagicServer(CSkillInstServer* pInst, const CShockWaveMagicCfgServerSharedPtr& pCfg, CFighterDictator* pFrom, const CFPos& pos)
:m_pCfg(new CShockWaveMagicCfgServerSharedPtr(pCfg))
,m_pFighter(pFrom)
,m_uKillCount(0)
{	
	m_Pos = pos;

	m_fAngle			= float( GetCfgSharedPtr()->GetAngle()->GetDblValue(pFrom, pInst->GetSkillLevel()-1)*3.14159/180 );
	float fRange		= float( GetCfgSharedPtr()->GetRange()->GetDblValue(pFrom, pInst->GetSkillLevel() - 1) );	
	uint32 uAmount		= GetCfgSharedPtr()->GetAmount()->GetIntValue(pFrom, pInst->GetSkillLevel() - 1);
	float fSpeed		= GetCfgSharedPtr()->GetSpeed();
	if (1 == uAmount)	m_fAngle = 0;

	EMoveToResult eResult;
	CFPos posFrom, posTo, posBulletStart, posBulletEnd, posFromPixel;
	posFrom=pFrom->GetPixelPos();
	m_BeginPos=posFrom;
	pFrom->GetPixelPos(posFromPixel);
	uint32 uCount=0;
	if (eBTT_Line == GetCfgSharedPtr()->GetBulletTrackType())
	{	
		posTo = m_Pos;
		float fSinAngle = sin(m_fAngle/2);
		float fCosAngle = cos(m_fAngle/2);
		float fX = posTo.x - posFrom.x;
		float fY = posTo.y - posFrom.y;
		float fDistance = sqrt( fX*fX + fY*fY );
		float fSinDis,fCosDis;
		if (fDistance==0.0f)
		{
			CCharacterDictator* pCharacter=pFrom->GetCharacter();
			Ast(pCharacter);
			CDir dir = pCharacter->GetActionDir();
			CVector2f vector2Dir;
			dir.Get(vector2Dir);
			m_vecCur2f=vector2Dir;
			fSinDis = vector2Dir.y;
			fCosDis = vector2Dir.x;
		}
		else
		{
			m_vecCur2f.x = m_Pos.x - m_BeginPos.x;
			m_vecCur2f.y = m_Pos.y - m_BeginPos.y;
			fSinDis = fY/fDistance;
			fCosDis = fX/fDistance;
		}
		float fSinFirst = fSinDis*fCosAngle - fCosDis*fSinAngle;
		float fCosFirst = fCosDis*fCosAngle + fSinDis*fSinAngle;

		float fSin, fCos;

		for (uint32 i = 0; i != uAmount;++i)
		{
			if (1 == uAmount)
			{
				fSin = fSinFirst*cos(m_fAngle) + fCosFirst*sin(m_fAngle);
				fCos = fCosFirst*cos(m_fAngle) - fSinFirst*sin(m_fAngle);
			}
			else
			{
				fSin = fSinFirst*cos(m_fAngle*i/(uAmount-1)) + fCosFirst*sin(m_fAngle*i/(uAmount-1));
				fCos = fCosFirst*cos(m_fAngle*i/(uAmount-1)) - fSinFirst*sin(m_fAngle*i/(uAmount-1));
			}
			posBulletStart.x = posFrom.x + 1*fCos*eGridSpanForObj;
			posBulletStart.y = posFrom.y + 1*fSin*eGridSpanForObj;
			posBulletEnd.x = posFrom.x + fRange*fCos*eGridSpanForObj;
			posBulletEnd.y = posFrom.y + fRange*fSin*eGridSpanForObj;
			CShockWaveMagicHandlerServer* pHandlerServer =new CShockWaveMagicHandlerServer(this,pInst,pFrom,posBulletStart);
			m_setShockWaveMagic.insert(pHandlerServer);
			eResult = pHandlerServer->GetCoreObj()->MoveTo(posBulletEnd, fSpeed, eFPT_HypoLine, eBT_MidBarrier,0);
			if (eResult != eMTR_Success)
			{
				++uCount;	 
				void* pContext = NULL;
				pHandlerServer->GetCoreObj()->PostAoiMsgToSelf(pContext);
			}
			else
			{
				pHandlerServer->SetDestPos(posBulletEnd);
			}
		}
	}
	else if (eBTT_Random == GetCfgSharedPtr()->GetBulletTrackType())
	{
		const CNormalSkillServerSharedPtr& pSkill = pInst->GetSkillCfg();
		m_eSelectTargetType = pSkill->GetSelectTargetType(); 

		if (m_eSelectTargetType == eFSTT_EnemyObject)
		{
			if (pFrom->GetTarget()->CppIsAlive())
			{
				pFrom->Notify(eCE_BeInterrupted);
			}			

			m_Pos = pFrom->GetTargetPos();
		}
		else if (m_eSelectTargetType == eFSTT_SelfPosition)
		{
			m_Pos = posFrom;
		}
		
		for (uint32 i = 0; i != uAmount; ++i)
		{
			posBulletStart.x = CQuickRand::Rand1((m_Pos.x - SIDELENGTH), (m_Pos.x + SIDELENGTH));
			posBulletStart.y = CQuickRand::Rand1((m_Pos.y - SIDELENGTH), (m_Pos.y + SIDELENGTH));
			posBulletEnd.x = CQuickRand::Rand1((posBulletStart.x - SIDELENGTH), (posBulletStart.x + SIDELENGTH));
			posBulletEnd.y = CQuickRand::Rand1((posBulletStart.y - SIDELENGTH), (posBulletStart.y + SIDELENGTH));

			CShockWaveMagicHandlerServer* pHandlerServer =new CShockWaveMagicHandlerServer(this,pInst,pFrom,posBulletStart);
			m_setShockWaveMagic.insert(pHandlerServer);
			eResult = pHandlerServer->GetCoreObj()->MoveTo(posBulletEnd, fSpeed, eFPT_HypoLine, eBT_MidBarrier,0);
			if (eResult != eMTR_Success)
			{
				++uCount;	 
			}
			else
			{
				pHandlerServer->SetDestPos(posBulletEnd);
			}
		}
	}

	if (uCount != uAmount)
	{
		if (eBTT_Random == GetCfgSharedPtr()->GetBulletTrackType())
			RegTick(10000);
	}
	else
	{
		RegTick(0);
	}

	pFrom->Attach(this, eCE_Offline);
	pFrom->Attach(this, eCE_Die);
	pFrom->Attach(this, eCE_ChangeMapEnd);
	pFrom->Attach(this, eCE_FinishDuel);
}

CShockWaveMagicServer::~CShockWaveMagicServer()
{
	m_setCompleteObject.clear();
	SafeDelete(m_pCfg);
}

void CShockWaveMagicServer::OnCOREvent(CPtCORSubject* pSubject, uint32 uEvent,void* pArg)
{
	if(pSubject == GetFighter())
	{
		switch(uEvent)
		{
		case eCE_Die:
		case eCE_Offline:
		case eCE_FinishDuel:
			DeleteAllObj(GetFighter()->GetScene());
			break;
		case eCE_ChangeMapEnd:
			{
				CCoreSceneServer* pScene = reinterpret_cast<CCoreSceneServer*>(pArg);
				DeleteAllObj(pScene);
				break;
			}
		}
	}
}

void CShockWaveMagicServer::RegTick( uint32 uTime )
{
	GetFighter()->RegistDistortedTick(this, uTime);
}

void CShockWaveMagicServer::UnRegTick()
{
	GetFighter()->UnRegistDistortedTick(this);
}

void CShockWaveMagicServer::OnTick()
{
	UnRegTick();
	DeleteAllObj(GetFighter()->GetScene());
}

bool CShockWaveMagicServer::IsSuitOfTheArea(CFighterDictator * pFighter)
{
	CFPos posTo=pFighter->GetPixelPos();
	CVector2f vec2fTarget;
	vec2fTarget.x = posTo.x - m_BeginPos.x;
	vec2fTarget.y= posTo.y - m_BeginPos.y;
	float fAng=CMath::ACos(vec2fTarget.Normalize().Dot(m_vecCur2f.Normalize()));
	if ((m_fAngle>0&&fAng>m_fAngle/2.0f)||(m_fAngle==0&&fAng>MATH_PI_DIV_2))
	{
		return false;
	}
	return true;

}

void CShockWaveMagicServer::RemoveHandler(CShockWaveMagicHandlerServer* pHandler)
{
	m_setShockWaveMagic.erase(pHandler);
	if (m_setShockWaveMagic.empty())
	{
		delete this;
	}
}
void CShockWaveMagicServer::DeleteAllObj(CCoreSceneServer* pScene)
{
	SetShockWaveHandler_t setHandler = m_setShockWaveMagic;
	while(!setHandler.empty())
	{
		(*setHandler.begin())->DeleteObj(pScene);
		delete *setHandler.begin();
		setHandler.erase(setHandler.begin());
	}
}

const CFPos& CShockWaveMagicServer::GetBeginPos()const
{
	return m_BeginPos;
}

CShockWaveMagicCfgServerSharedPtr& CShockWaveMagicServer::GetCfgSharedPtr()const
{
	return *m_pCfg;
}
