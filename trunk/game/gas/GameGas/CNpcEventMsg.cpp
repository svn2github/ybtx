#include "stdafx.h"
#include "CNpcEventMsg.h"
#include "CCharacterDictator.h"
#include "CNpcEventMetaData.h"
#include "PatternRef.inl"

template class TPtRefer<CNpcEventMsg, CNpcAI>;

CNpcEventMsg::CNpcEventMsg(CNpcAI* pAI, ENpcEvent eNpcEvent, ENpcEventCodeIndex eCodeIndex, void* pTag, void* pArg)
:TStateEvent<ENpcEvent>(eNpcEvent)
,m_eEvent(eNpcEvent)
,m_pTag(pTag)
,m_pArg(pArg)
{
	m_RefOfAI.SetHolder(this);
	SetNpcAIHolder(pAI);
#ifdef NpcEventCheck
	m_eCodeIndex = eCodeIndex;
#endif
}

CNpcEventMsg::~CNpcEventMsg()
{
	CNpcAI* pAI = m_RefOfAI.Get();
	if (pAI)
		pAI->DispatchEventToNpcAI(this);
	//cout<<"Event  ：  "<<GetName()<<endl;
	SetNpcAIHolder(NULL);
}

void CNpcEventMsg::SetNpcAIHolder(CNpcAI* pAI)
{
	pAI ? m_RefOfAI.Attach(pAI->GetRefByEventMsg()) : m_RefOfAI.Detach();
}

CNpcEventMsg* CNpcEventMsg::Create(CNpcAI* pAI, ENpcEvent eID, ENpcEventCodeIndex eCodeIndex, void* pTag, void* pArg)
{
	//因为角色对象先设置成死亡状态，然后才删除对象并析构AI，所以死亡后有可能会发消息(比如删除状态机的消息)
	if (pAI)	
		return new CNpcEventMsg(pAI, eID, eCodeIndex, pTag, pArg);
	return NULL;
}

const TCHAR* CNpcEventMsg::GetName() const
{
	return CNpcEventMetaDataMgr::GetInst()->GetEntity(this->GetID())->GetName();
}

