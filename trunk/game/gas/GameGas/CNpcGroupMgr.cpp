#include "stdafx.h"
#include "CNpcGroupMgr.h"
#include "CCharacterDictator.h"
#include "INpcAIHandler.h"
#include "PtrlGas2GacCPPDef.h"
#include "CFighterDictator.h"
#include "CDummyAI.h"
#include "TSqrAllocator.inl"

template class TPtRefer<CNpcGroupMgr,CCharacterDictator>;

CNpcGroupMgr::CNpcGroupMgr(CCharacterDictator* pCharacter)
{
	m_CharacterHandler.SetHolder(this);
	SetCharacterHolder(pCharacter);
}

void CNpcGroupMgr::SetCharacterHolder(CCharacterDictator* pCharacter)
{
	pCharacter ? m_CharacterHandler.Attach(pCharacter->GetRefByGroupMgr()) : m_CharacterHandler.Detach();
}

void CNpcGroupMgr::OnCOREvent(CPtCORSubject* pSubject, uint32 uEvent,void* pArg)
{
	CFighterDictator* pTargetFighter = (CFighterDictator*)(pSubject);
	if (NULL == pTargetFighter)
		return;
	CCharacterDictator* pTargetCharacter = pTargetFighter->GetCharacter();
	if (NULL == pTargetCharacter)
		return;
	switch(uEvent)
	{
	case eCE_Die:
		{
			RemoveMember(pTargetCharacter);
		}
		break;
	default:
		break;
	}
}
void CNpcGroupMgr::DoAttach(CFighterDictator* pFighter)
{
	if (NULL == pFighter)
		return;
	pFighter->Attach(this, eCE_Die);
}
void CNpcGroupMgr::DoDetach(CFighterDictator* pFighter)
{
	if (NULL == pFighter)
		return;
	pFighter->Detach(this, eCE_Die);
}

bool CNpcGroupMgr::AddMember( CCharacterDictator *pMember )
{
	if(!pMember)
		return false;
	if (pMember->GetNpcType() == ENpcType_None)
		return false;
	CFighterDictator* pMemFighter = pMember->GetFighter();
	if(!pMemFighter)
		return false;
	if (!m_mapMemberByID.insert(make_pair(pMember->GetEntityID(), pMember)).second)
		return false;
	DoAttach(pMemFighter);
	return true;
}

void CNpcGroupMgr::RemoveMember( CCharacterDictator* pMember )
{
	if(m_mapMemberByID.empty())
		return;
	if (pMember)
		m_mapMemberByID.erase(pMember->GetEntityID());
	CFighterDictator* pMemFighter = pMember->GetFighter();
	DoDetach(pMemFighter);
	if(m_mapMemberByID.empty())
	{
		CCharacterDictator* pCharacter = GetCharacter();
		INpcAIHandler* pAIHandle = pCharacter->GetNpcAIHandler();
		if(pAIHandle)
		{
			CDummyAI* pDummyAI = pAIHandle->CastToDummyAI();
			if(pDummyAI)
				pDummyAI->OnMemberAllDead();
		}
	}
	
}

void CNpcGroupMgr::RemoveAllMember()
{
	for(MemberIDMapType::iterator it = m_mapMemberByID.begin(); it != m_mapMemberByID.end();)
	{
		CCharacterDictator* pCharacter = CCharacterDictator::GetCharacterByID(it->first);
		if(pCharacter)
			pCharacter->SetGroupLeader(NULL);
		m_mapMemberByID.erase(it++);
	}
}

CCharacterDictator* CNpcGroupMgr::GetMemberByID( uint32 uGlobalID ) const
{
	MemberIDMapType::const_iterator it = m_mapMemberByID.find( uGlobalID );
	if (it == m_mapMemberByID.end())
		return NULL;
	return it->second;
}

bool CNpcGroupMgr::IsInMemberList(uint32 uGlobalID)
{
	if(m_mapMemberByID.empty())
		return false;
	MemberIDMapType::const_iterator it = m_mapMemberByID.find( uGlobalID );
	if (it == m_mapMemberByID.end())
		return false;
	else
		return true;
}

const MemberIDMapType& CNpcGroupMgr::GetMemberIDMapType() const
{
	return m_mapMemberByID;
}

CNpcGroupMgr::~CNpcGroupMgr()
{
	for(MemberIDMapType::iterator it = m_mapMemberByID.begin(); it != m_mapMemberByID.end();)
	{
		CCharacterDictator* pCharacter = CCharacterDictator::GetCharacterByID(it->first);
		if(pCharacter)
			pCharacter->OnGroupDestroyed();
		m_mapMemberByID.erase(it++);
	}
	SetCharacterHolder(NULL);
}
