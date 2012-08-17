#pragma once
#include "CMagicServer.h"
#include "CMoveServerManager.h"
#include "ActorDef.h"
#include "CDistortedTick.h"
#include "CMoveMagicDefs.h"

class CGenericTarget;
class CMagicEffDataMgrServer;
class CMagicEffServerSharedPtr;

class CBaseMoveMagicServer : public IMoveManager
{
public:
	CBaseMoveMagicServer(CSkillInstServer* pInst, CFighterDictator* pFighter, CGenericTarget* pTarget, const MoveInfo& moveInfo, const string& strName, const CMagicEffServerSharedPtr& pMagicEff, EMoveActionType eMoveActionType, uint32& uMoveMagicMoveID);
	void OnCOREvent(CPtCORSubject* pSubject, uint32 uEvent, void* pArg);
	CSkillInstServer*	GetSkillInst()			{return m_pInst;}
	const string&		GetName(){return m_strMoveMagicName;}
private:
	~CBaseMoveMagicServer();
	CSkillInstServer*			m_pInst;
	CMagicEffServerSharedPtr*	m_pMagicEff;
	CGenericTarget*				m_pTarget;
	string						m_strMoveMagicName;
	EMoveActionType				m_eMoveActionType;
};

class CMoveMagicCfgServerSharedPtr;

class CMoveMagicServer
	: public CMagicServer
	, public CDistortedTick
{
public:
	CMoveMagicServer(CSkillInstServer* pInst, const CMoveMagicCfgServerSharedPtr& pCfg, CFighterDictator* pFrom, CFighterDictator* pTo);
	CMoveMagicServer(CSkillInstServer* pInst, const CMoveMagicCfgServerSharedPtr& pCfg, CFighterDictator* pFrom, const CFPos& pos);
	~CMoveMagicServer();
	void OnCOREvent(CPtCORSubject* pSubject, uint32 uEvent,void* pArg);
	virtual EMagicGlobalType GetMagicType() const	{return eMGT_Move;}
	void OnCaughtViewSightOf(uint32 uObjGlobalId, uint32 uDimension);
	void OnAoiMsgFromSelf(const void* pContext);
	CMoveMagicCfgServerSharedPtr& GetCfgSharedPtr()const;
	void Do();
	//void OnTick();
private:
	void MakeMoveInfo(MoveInfo& moveInfo, EMoveType MoveType);
	bool Immune(CFighterDictator* pFrom, CFighterDictator* pTo, CSkillInstServer* pSkillInst);

	typedef set<uint32, less<uint32>, TMagicAllocator<uint32> > SetTarget_t;
	CMoveMagicCfgServerSharedPtr*	m_pCfg;
	CGenericTarget*			m_pTo;
	SetTarget_t				m_setTarget;
	CFighterDictator*		m_pObjOwner;
	uint32					m_uMoveID;
	CMagicEffDataMgrServer*		m_pCancelEffData;
	CFighterDictator*					m_pMagicOwner;
	CFPos m_BeginPos;
};
