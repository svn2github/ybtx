#pragma once
#include "CMagicServer.h"
#include "CDistortedTick.h"
#include "TConfigAllocator.h"

class CEffDataByGlobalIDMgr;
class CReplacePositionMagicMOP;
class CPositionMagicCfgServerSharedPtr;

class CPositionMagicServer
	:public CMagicServer
	,public CDistortedTick
{
	friend class CReplacePositionMagicMOP;
public:
	CPositionMagicServer(CSkillInstServer* pInst, const CPositionMagicCfgServerSharedPtr& pCfg, CFighterDictator* pFrom, const CFPos& pos);	
	void OnCOREvent(CPtCORSubject* pSubject, uint32 uEvent,void* pArg);
	void Cancel();
	const string& GetPostionMagicName();
	virtual void OnCaughtViewSightOf(uint32 uObjGlobalId, uint32 uDimension);
	virtual void OnLostViewSightOf(uint32 uObjGlobalId, uint32 uDimension);
	virtual EMagicGlobalType			GetMagicType() const				{return eMGT_Position;}
	virtual void OnAoiMsgFromSelf(const void* pContext);
	typedef vector<uint32, TConfigAllocator<uint32> > VecEntityID_t;
	void GetTargetVector(VecEntityID_t& vecTarget);
	CPositionMagicCfgServerSharedPtr& GetCfgSharedPtr()const;

private:
	~CPositionMagicServer();
	void DoMagicEff(CFighterDictator* pTo);
	void CancelMagicEff(CFighterDictator* pTo);
	void RegTick( uint32 uTime );
	void UnRegTick();
	void OnTick();

	typedef set<uint32, less<uint32>, TMagicAllocator<uint32> > SetEntityID_t;
	
	SetEntityID_t				m_setEntityID;
	CPositionMagicCfgServerSharedPtr*	m_pCfg;
	CFPos						m_Pos;
	float						m_fRadian;
	uint32						m_uTouchCount;
	uint32						m_uCount;
	uint32						m_uAllTime;
	uint32						m_uTime;
	CPositionMagicServer*		m_PLastPosMagic;
	CEffDataByGlobalIDMgr*		m_pCancelEffData;
	bool						m_bSingleObjCount;
};

