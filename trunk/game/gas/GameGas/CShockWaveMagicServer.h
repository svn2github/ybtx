#pragma once
#include "CMagicServer.h"
#include "PatternCOR.h"
#include "CDistortedTick.h"
#include "CVector2.h"

namespace sqr
{
	class CCoreObjectDictator;
}

class CShockWaveMagicCfgServerSharedPtr;
class CShockWaveMagicServer;

class CShockWaveMagicHandlerServer
	: public CMagicServer
{
public:
	CShockWaveMagicHandlerServer(CShockWaveMagicServer* pShockWaveMagic,CSkillInstServer* pInst, CFighterDictator* pFrom, const CFPos& pos);
	~CShockWaveMagicHandlerServer();
	virtual EMagicGlobalType	GetMagicType() const		{return eMGT_ShockWave;}
	CShockWaveMagicCfgServerSharedPtr&	GetCfgSharedPtr()const;
	void OnCaughtViewSightOf(uint32 uObjGlobalId, uint32 uDimension);
	void OnAoiMsgFromSelf(const void* pContext);
	void OnMoveEnded(EMoveEndedType eMEType, uint32 uMoveID);
	void DoMagicEff(CFighterDictator* pTo);
	void SetDestPos(const CFPos& pos);
	const CFPos& GetDestPos()const;

private:
	typedef set<CShockWaveMagicHandlerServer*, less<CShockWaveMagicHandlerServer*>,
		TMagicAllocator<CShockWaveMagicHandlerServer*> > SetShockWaveHandler_t;

	 CShockWaveMagicServer* m_pShockWaveMagic;
	 float					m_fRange;
	 uint32					m_uKillMaxNum;
	 CFPos					m_posDest;	//Œª“∆÷’µ„
};

class CShockWaveMagicServer
	: public CPtCORHandler
	, public CDistortedTick
	, public CMagicMallocObject
{
public:
	typedef set<CShockWaveMagicHandlerServer*, less<CShockWaveMagicHandlerServer*>,
		TMagicAllocator<CShockWaveMagicHandlerServer*> > SetShockWaveHandler_t;
	typedef set<CFighterDictator*, less<CFighterDictator*>,
		TMagicAllocator<CFighterDictator*> > SetCompleteObject_t;

	CShockWaveMagicServer(CSkillInstServer* pInst, const CShockWaveMagicCfgServerSharedPtr& pCfg, CFighterDictator* pFrom, const CFPos& pos);
	~CShockWaveMagicServer();
	void OnCOREvent(CPtCORSubject* pSubject, uint32 uEvent,void* pArg);
	void DeleteAllObj(CCoreSceneServer* pScene);
	void RegTick( uint32 uTime );
	void UnRegTick();
	void OnTick();
	void RemoveHandler(CShockWaveMagicHandlerServer* pHandler);
	CFighterDictator*	GetFighter()				{return m_pFighter;}
	EFSTargetType		GetTargetType()const		{return m_eSelectTargetType;}
	SetShockWaveHandler_t&	GetShockWaveMagicSet()	{return m_setShockWaveMagic;}
	SetCompleteObject_t&		GetFinishObjSet()		{return m_setCompleteObject;}
	bool IsSuitOfTheArea(CFighterDictator* pFighter);
	const CFPos& GetBeginPos()const;
	uint32 GetKillCount() const		{return m_uKillCount;}
	void AddKillCount()				{++m_uKillCount;}
	CShockWaveMagicCfgServerSharedPtr& GetCfgSharedPtr()const;

private:
	SetShockWaveHandler_t				m_setShockWaveMagic;
	SetCompleteObject_t					m_setCompleteObject;
	CShockWaveMagicCfgServerSharedPtr*	m_pCfg;
	CFighterDictator*					m_pFighter;			
	CFPos								m_Pos;
	CFPos								m_BeginPos;
	CVector2f							m_vecCur2f;
	float								m_fAngle;
	EFSTargetType						m_eSelectTargetType;
	uint32								m_uKillCount;
};
