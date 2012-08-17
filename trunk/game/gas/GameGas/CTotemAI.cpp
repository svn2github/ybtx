#include "stdafx.h"
#include "CTotemAI.h"
#include "CSkillInstServer.h"
#include "CTotemMagicCfgServer.h"
#include "CGenericTarget.h"
#include "CFighterDictator.h"
#include "CCharacterDictator.h"
#include "CEntityServerManager.h"
#include "CMagicEffDataMgrServer.h"
#include "CFilterOpServer.h"
#include "BaseHelper.h"
#include "TSqrAllocator.inl"

//----------------------------Í¼ÌÚ-----------------------------------
CTotemAI::CTotemAI(CCharacterDictator* pCharacter, const CNpcServerBaseData* pBaseData,const CNpcStateTransit* pStateTransit, ENpcType eRealNpcType, ENpcAIType eRealAIType, EObjectAoiType eObjAoiType)
:CNpcAI(pCharacter, pBaseData, pStateTransit, eRealNpcType, eRealAIType, eObjAoiType)
,m_uTime(0)
{
	m_pCancelEffData = new CEffDataByGlobalIDMgr;

	CCharacterDictator* pCharacterServant = GetCharacter();
	CFighterDictator* pFighterServant = pCharacterServant->GetFighter();
	CCharacterDictator* pMaster = pCharacterServant->GetMaster();
	CFighterDictator* pFighterMaster =pMaster?pMaster->GetFighter():NULL;

	pFighterMaster->Attach(this, eCE_Die);
	pFighterMaster->Attach(this, eCE_Offline);
	pFighterMaster->Attach(this, eCE_ChangeMapEnd);
	m_pCfg=new CTotemMagicCfgServerSharedPtr(CTotemMagicCfgServer::GetTotemMagic(GetName()));
	m_fRadian			= float(GetCfgSharedPtr()->GetRadius()->GetDblValue(pFighterMaster));
	m_uAllTime			= GetCfgSharedPtr()->GetTime()->GetIntValue(pFighterMaster);
	pCharacterServant->SetEyeSight(m_fRadian);
	pFighterServant->CppSetCamp(pFighterMaster->CppGetCamp());
	m_pSkillInst = CSkillInstServer::CreateOrigin(pCharacterServant->GetEntityID(),GetName(),pFighterMaster->CppGetLevel(),eATT_None);
	pFighterServant->RegistDistortedTick(this, 1000);
}

CTotemAI::~CTotemAI()
{
	ClearAllEnmity();
	if( this != NULL)
		GetCharacter()->UnRegistDistortedTick(this);
	CharacterSet::iterator it= m_setCharacter.begin();
	for(; it!= m_setCharacter.end(); ++it)
	{	
		CCharacterDictator* pCharacter=CEntityServerManager::GetInst()->GetCharacter(*it);
		if (pCharacter&&GetCfgSharedPtr()->GetTouchMagicEff()&&m_pCancelEffData)
		{
			GetCfgSharedPtr()->GetTouchMagicEff()->Cancel(m_pSkillInst,pCharacter->GetFighter(),m_pCancelEffData->GetEffData(pCharacter->GetEntityID()));
		}
	}

	m_pSkillInst->DestroyOrigin();
	m_setCharacter.clear();

	if(m_pCancelEffData)
	{
		m_pCancelEffData->Clear();
		delete m_pCancelEffData;
		m_pCancelEffData = NULL;
	}
	SafeDelete(m_pCfg);
}

CTotemMagicCfgServerSharedPtr& CTotemAI::GetCfgSharedPtr()const
{
	return *m_pCfg;
}

void CTotemAI::OnCOREvent(CPtCORSubject* pSubject, uint32 uEvent,void* pArg)
{
	CCharacterDictator* pTotem = GetCharacter();
	Ast(pTotem);
	Ast(pTotem->GetFighter());
	CCharacterDictator* pMaster = pTotem->GetMaster();
	CFighterDictator* pFighterMaster =pMaster?pMaster->GetFighter():NULL;
	if(pSubject == pFighterMaster)
	{
		switch(uEvent)
		{
		case eCE_Die:
		case eCE_Offline:
		case eCE_ChangeMapEnd:
			pTotem->SetOnDead();	
			break;
		}
	}
}

void CTotemAI::OnTick()
{
	CCharacterDictator* pTotem = GetCharacter();
	Ast(pTotem);
	CFighterDictator* pFighterTotem = pTotem->GetFighter();
	Ast(pFighterTotem);

	if (m_uTime < m_uAllTime)
	{
		if (GetCfgSharedPtr()->GetDotMagicEff() != NULL)
		{
			CFPos pos;
			GetCharacter()->GetPixelPos(pos);
			CGenericTarget genericTarget(pos);
			GetCfgSharedPtr()->GetDotMagicEff()->Do(m_pSkillInst, pFighterTotem, &genericTarget);
		}
		++m_uTime;
		return;
	}
	if (GetCfgSharedPtr()->GetFinalMagicEff() != NULL)
	{
		GetCfgSharedPtr()->GetFinalMagicEff()->Do(m_pSkillInst, pFighterTotem, pFighterTotem);
	}
	GetCharacter()->UnRegistDistortedTick(this);
	GetCharacter()->SetOnDead();
}

void CTotemAI::OnMasterDestroyed()
{
}

void CTotemAI::MakeFilterLimit(FilterLimit& filterLimit)
{
	CCharacterDictator* pTotem = GetCharacter();
	Ast(pTotem);
	Ast(pTotem->GetFighter());
	CCharacterDictator* pMaster = pTotem->GetMaster();
	CFighterDictator* pFighterMaster =pMaster?pMaster->GetFighter():NULL;
	filterLimit.fFilterRadius=float(GetCfgSharedPtr()->GetRadius()->GetDblValue(pFighterMaster,m_pSkillInst->GetSkillLevel()-1));
	filterLimit.bIsValueMagicOp		= false;
	filterLimit.bTargetAliveCheckIsIgnored=m_pSkillInst->GetTargetAliveCheckIsIgnored();
}

//-------------------------- Totem×´Ì¬º¯Êý¶¨Òå -------------------------

CTotemAI* CNpcAI::CNpcStateTotemBase::GetOwner() const
{
	return class_cast<CTotemAI*>(CNpcStateBase::GetOwner());
}

DEFINE_NPCSTATE_METHOD(TotemCaughtCharacter, EnterState)
{
	CTotemAI* pTotemAI = GetOwner();
	Ast(GetOwner()->GetCharacter());
	const CMagicEffServerSharedPtr& pTouchEffect	= pTotemAI->GetCfgSharedPtr()->GetTouchMagicEff();
	CEffDataByGlobalIDMgr* pEffDataMgr=pTotemAI->GetCancelEffData();
	Ast(pEffDataMgr);
	if (pTouchEffect)
	{
		size_t uEntityID = reinterpret_cast<size_t>(pEvent->GetTag());

		CEntityServer* pEntity = CEntityServerManager::GetInst()->GetEntityByID(uEntityID);
		if (!pEntity)
			return pEvent;

		CCharacterDictator* pTo = CCharacterDictator::GetCharacterByID(uEntityID);

		CFighterDictator* pFromFighter = pTotemAI->GetCharacter()->GetFighter();
		CFighterDictator* pToFighter = pTo->GetFighter();
		CGenericTarget Target(pToFighter);
		CMagicOpTreeServer::VecEntityID vecEntityID;
		FilterLimit filterLimit;
		GetOwner()->MakeFilterLimit(filterLimit);
		CFilterOpServer::FilterOperateObject(pTotemAI->m_pSkillInst, vecEntityID, GetOwner()->GetCfgSharedPtr()->GetOperateObject(), filterLimit, pFromFighter, &Target);
		CMagicOpTreeServer::VecEntityID::iterator it_obj = vecEntityID.begin();
		//cout << "Totem caught TCHAR num = " << vecEntityID.size() << endl;
		for (; it_obj != vecEntityID.end(); ++it_obj)
		{
			CCharacterDictator* pCharacter = CEntityServerManager::GetInst()->GetCharacter(*it_obj);
			if (NULL==pCharacter || !pFromFighter->CppIsAlive())
			{
				continue;
			}
			CFighterDictator* pToFighter=pCharacter->GetFighter();
			//cout << "Totem touchEffect do " << endl;
			pTouchEffect->Do(pTotemAI->m_pSkillInst, pFromFighter, pToFighter,pEffDataMgr->AddEffData(pTo->GetEntityID(), pFromFighter));
			pTotemAI->m_setCharacter.insert(pCharacter->GetEntityID());
		}

	}
}
END_DEFINE_NPCSTATE_METHOD

DEFINE_NPCSTATE_METHOD(TotemLostCharacter, EnterState)
{
	CTotemAI* pTotemAI = GetOwner();
	const CMagicEffServerSharedPtr& pTouchEffect	= pTotemAI->GetCfgSharedPtr()->GetTouchMagicEff();
	CEffDataByGlobalIDMgr* pEffDataMgr=pTotemAI->GetCancelEffData();
	Ast(pEffDataMgr);
	if (pTouchEffect)
	{
		size_t uEntityID = reinterpret_cast<size_t>(pEvent->GetTag());

		CEntityServer* pEntity = CEntityServerManager::GetInst()->GetEntityByID(uEntityID);
		if (!pEntity)
		{
			pTotemAI->m_setCharacter.erase(uEntityID);
			return pEvent;
		}
	
		CCharacterDictator* pTo =  CCharacterDictator::GetCharacterByID(uEntityID);
		CFighterDictator* pToFighter = pTo->GetFighter();

		if (pTotemAI->m_setCharacter.find(uEntityID) != pTotemAI->m_setCharacter.end())
		{
			pTouchEffect->Cancel(pTotemAI->m_pSkillInst, pToFighter,
				pEffDataMgr->GetEffData(uEntityID));	
			pTotemAI->GetCancelEffData()->DelEffData(uEntityID);
			pTotemAI->m_setCharacter.erase(uEntityID);
		}
	}
}
END_DEFINE_NPCSTATE_METHOD
