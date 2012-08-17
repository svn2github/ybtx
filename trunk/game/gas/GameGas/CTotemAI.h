#pragma once

#include "CNpcAI.h"

class CEffDataByGlobalIDMgr;
struct FilterLimit;
class CTotemMagicCfgServerSharedPtr; 

class CTotemAI
	:public CNpcAI
	,public CDistortedTick
{
public:
	typedef set<uint32, less<uint32>, TNpcAIAllocator<uint32> >		CharacterSet;
	CTotemAI(CCharacterDictator* pCharacter, const CNpcServerBaseData *pData,const CNpcStateTransit* pStateTransit, ENpcType eRealNpcType, ENpcAIType eRealAIType, EObjectAoiType eObjAoiType);
	virtual void OnMasterDestroyed();
	void MakeFilterLimit(FilterLimit& filterLimit);
	CEffDataByGlobalIDMgr* GetCancelEffData()	{return m_pCancelEffData;}
	CTotemMagicCfgServerSharedPtr*	m_pCfg;
	CSkillInstServer*				m_pSkillInst;
	CharacterSet					m_setCharacter;
	CTotemMagicCfgServerSharedPtr& GetCfgSharedPtr()const;
private:
	virtual ~CTotemAI();
	void OnCOREvent(CPtCORSubject* pSubject, uint32 uEvent,void* pArg);
	void OnTick();
	float							m_fRadian;
	uint32							m_uTouchCount;
	uint32							m_uAllTime;
	uint32							m_uTime;
	CEffDataByGlobalIDMgr*			m_pCancelEffData;
};
