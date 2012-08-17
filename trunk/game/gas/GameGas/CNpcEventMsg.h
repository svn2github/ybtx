#pragma once

#include "CAppEvent.h"
#include "TStateEvent.h"
#include "PatternRef.h"
#include "CNpcEvent.h"

#define NpcEventCheck
class CNpcAI;

class CNpcEventMsg
	:public CAppEvent
	,public TStateEvent<ENpcEvent>
{
public:
	CNpcEventMsg(CNpcAI* pAI, ENpcEvent eNpcEvent, ENpcEventCodeIndex eCodeIndex, void* pTag, void* pArg);
	
	void SetNpcAIHolder(CNpcAI* pAI);
	inline void* GetTag() const{ return m_pTag;}
	inline void* GetArg() const {return m_pArg;}
	const TCHAR* GetName() const;
#ifdef NpcEventCheck
	ENpcEventCodeIndex GetCodeIndex() const {return m_eCodeIndex;}
#endif
	static CNpcEventMsg* Create(CNpcAI* pAI, ENpcEvent eID, ENpcEventCodeIndex eCodeIndex = eNECI_Zero, void* pTag = NULL, void* pArg = NULL);
private:
	friend class TPtRefer<CNpcEventMsg,CNpcAI>;
	~CNpcEventMsg();
	
	ENpcEvent						m_eEvent;
	void*							m_pTag;
	void*							m_pArg;
#ifdef NpcEventCheck
	ENpcEventCodeIndex				m_eCodeIndex;
#endif
	TPtRefer<CNpcEventMsg,CNpcAI>	m_RefOfAI;
};
