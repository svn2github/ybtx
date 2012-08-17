#pragma once
#include "CMagicServer.h"

namespace sqr
{
	class CCoreObjTracer;
}

class CTransferableMagicCfgServerSharedPtr;

class CTransferableMagicServer
	:public CMagicServer
{
public:
	CTransferableMagicServer(CSkillInstServer* pInst,const CTransferableMagicCfgServerSharedPtr& pCfg, CFighterDictator* pFrom, CFighterDictator* pTo);
	~CTransferableMagicServer();
	void OnCOREvent(CPtCORSubject* pSubject, uint32 uEvent,void* pArg);
	virtual void OnMoveEnded(EMoveEndedType eMEType, uint32 uMoveID);
	virtual void OnTraceEnded(ETraceEndedType eTEType);
	virtual EMagicGlobalType			GetMagicType() const		{return eMGT_Transferable;}
	CTransferableMagicCfgServerSharedPtr&		GetCfgSharedPtr()const;

private:
	typedef set<uint32, less<uint32>, TMagicAllocator<uint32> > SetEntityID_t;

	CTransferableMagicCfgServerSharedPtr*		m_pCfg;
	uint32								m_uCount;
	uint32								m_uAmountLimit;
	SetEntityID_t						m_setEntityID;	
};
