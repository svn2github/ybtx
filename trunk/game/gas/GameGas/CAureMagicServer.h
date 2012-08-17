#pragma once
#include "CMagicServer.h"
#include "GameDef.h"
#include "ITeamServerManagerHandler.h"
#include "CDistortedTick.h"

class CAureMagicCfgServerSharedPtr;
class CEffDataByGlobalIDMgr;

class CAureMagicServer
	:public CMagicServer
	,public ITeamServerManagerHandler
	,public CDistortedTick
{
public:
	CAureMagicServer(CSkillInstServer* pInst,const CAureMagicCfgServerSharedPtr& pCfg, CFighterDictator* pFrom, CFighterDictator* pTo);
	~CAureMagicServer();
	virtual void*					GetTag() const			{ return (void*)eCOHT_Magic; }
	virtual EMagicGlobalType		GetMagicType() const	{ return eMGT_Aure; }
	void OnCOREvent(CPtCORSubject* pSubject, uint32 uEvent,void* pArg);
	void OnTransfered(CCoreSceneServer* pOldScene, const CFPos& OldPos);
	void Cancel();
	virtual void OnCaughtViewSightOf(uint32 uObjGlobalId, uint32 uDimension);
	virtual void OnLostViewSightOf(uint32 uObjGlobalId, uint32 uDimension);
	CAureMagicCfgServerSharedPtr& GetCfgSharedPtr()const;

	static bool CancelTheExistedStance(const CAureMagicCfgServerSharedPtr& pCfg, CFighterDictator* pFrom);
	static bool HasAureMagic(const CAureMagicCfgServerSharedPtr& pCfg, const CFighterDictator* pFrom);
	static void CancelAllAureMagic(CFighterDictator* pFrom);
	static void CancelAllInvisibleAureMagic(CFighterDictator* pFrom);
	static void CancelAllAureMagicByType(CFighterDictator* pFrom, EStanceType eStanceType);

private:
	void DoMagicEff(CFighterDictator* pTo);
	void CancelMagicEff(CFighterDictator* pTo);
	void OnAddTeamMember(uint32 uTeamID, ETeamType eTeamType, uint32 uEntityID);
	void OnRemoveTeamMember(uint32 uTeamID, ETeamType eTeamType, uint32 uEntityID);
	void OnCreateTeam(uint32 uTeamID, ETeamType eTeamType, uint32 uEntityID);
	void OnDestroyTeam(uint32 uTeamID, ETeamType eTeamType);
	void OnUpdate(uint32 uTeamID);
	bool IsAure()const;
	//void OnTick();

private:
	typedef map<EStanceType, CAureMagicServer*, less<EStanceType>, 
		TMagicAllocator<pair<const EStanceType, CAureMagicServer*> > >	MapAureMagic_t;
	typedef set<uint32, less<uint32>, TMagicAllocator<uint32> > SetEntityID_t;

	CAureMagicCfgServerSharedPtr*	m_pCfg;
	EAgileType						m_eHoldAglie;
	SetEntityID_t					m_setEntityID;
	CEffDataByGlobalIDMgr*			m_pCancelEffData;
	bool							m_bCancelling;
	bool							m_bInTeam;
};
