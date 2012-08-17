#include "stdafx.h"
#include "CNPCGazedAtMgr.h"
#include "CAppClient.h"
#include "CCharacterDirector.h"
#include "CCharacterFollower.h"

static const uint32 uGazedAtDis = 300;

CNPCGazedAtMgr::CNPCGazedAtMgr(CCharacterDirector* pCharacter)
: m_pCharacter(pCharacter)
, m_bStartUp(false)
{
}

CNPCGazedAtMgr::~CNPCGazedAtMgr()
{
	m_vectorGazedAtNPC.clear();
	CAppClient::Inst()->UnRegisterTick(this);
}

void CNPCGazedAtMgr::AddSyncNPC(CCharacterFollower* pCharacter)
{
	bool bGazedAt = InGazedAtZone(pCharacter) ? true : false;
	m_vectorGazedAtNPC.push_back(pCharacter);

	if (bGazedAt)	
		pCharacter->SetInterestedObj(m_pCharacter);

	if (!m_bStartUp)
	{
		m_bStartUp = true;
		CAppClient::Inst()->RegisterTick(this, 33);
	}	
}

void CNPCGazedAtMgr::DelSyncNPC(CCharacterFollower* pCharacter)
{
	VectorGazedAtNPC::iterator it = find(m_vectorGazedAtNPC.begin(), m_vectorGazedAtNPC.end(), pCharacter);
	if (it != m_vectorGazedAtNPC.end())
	{
		pCharacter->ClearInterested(false);
		m_vectorGazedAtNPC.erase(it);
		if (m_vectorGazedAtNPC.empty())
		{
			m_bStartUp = false;
			CAppClient::Inst()->UnRegisterTick(this);
		}	
	}
}

void CNPCGazedAtMgr::OnTick()
{
	VectorGazedAtNPC::iterator it = m_vectorGazedAtNPC.begin();
	for (; it != m_vectorGazedAtNPC.end(); ++it)
	{
		if (InGazedAtZone(*it) && !((*it)->CppGetCtrlState(eFCS_ForbitTurnAround)))
			(*it)->SetInterestedObj(m_pCharacter);
		else
			(*it)->ClearInterested(false);
	}
}

bool CNPCGazedAtMgr::InGazedAtZone(CCharacterFollower* pCharacter)
{
	CFPos posSelf = m_pCharacter->GetPixelPos();
	CFPos posNPC  = pCharacter->GetPixelPos();
	return posSelf.Dist(posNPC) <= uGazedAtDis; 
}
