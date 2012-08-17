#pragma once
#include "CMagicServer.h"
#include "ITeamServerManagerHandler.h"
#include "CDistortedTick.h"

namespace sqr
{
	class CCoreSceneServer;
}

class CBattleArrayMagicCfgServerSharedPtr;
class CBattleArrayMagicServer;

class CBattleArrayStationServer
	: public CMagicServer
	, public CDistortedTick
{
	friend class CBattleArrayMagicServer;
public:
	CBattleArrayStationServer(CBattleArrayMagicServer* pArrayMgr,uint32 uID, uint32 uEntityID,const CFPos& pos);
	~CBattleArrayStationServer();
	EMagicGlobalType	GetMagicType() const				{return eMGT_BattleArray;}
	void OnCaughtViewSightOf(uint32 uObjGlobalId, uint32 uDimension);
	void OnLostViewSightOf(uint32 uObjGlobalId, uint32 uDimension);
	void BeginSyncInfoTo(uint32 uObjID,uint32 uSyncInfoToObjID);
	void OnCOREvent(CPtCORSubject* pSubject, uint32 uEvent,void* pArg);
	void ChangeArrayState(EBattleArrayState eState);
	void FinishArray();
	void NotifyClientArrayChange();

	CBattleArrayMagicCfgServerSharedPtr& GetCfgSharedPtr()const;

private:
	CBattleArrayMagicServer* m_pArrayMgr;
	uint8		m_uID;
	uint32		m_uEntityID;
	CFPos		m_Pos;	
	bool		m_bInStation;
	EBattleArrayState m_eBattleArrayState;
};


class CBattleArrayMagicServer
	: public ITeamServerManagerHandler
	, public CDistortedTick
	, public CMagicMallocObject
{	
	friend class CBattleArrayStationServer;
public:
	typedef vector<pair<uint32, CFPos>, TMagicAllocator<pair<uint32, CFPos> > > VecBattleArray_t;

	CBattleArrayMagicServer(CSkillInstServer* pInst, const CBattleArrayMagicCfgServerSharedPtr& pCfg, CFighterDictator* pFrom, const VecBattleArray_t& vecBattleArray);
	static void Transfer(const string& strPos,const string& strEntityID,VecBattleArray_t& vecBattleArray);
	static void PositionTransfer(VecBattleArray_t& vecBattleArray,const CFPos& pos);
	void SetInStationCount(uint8 uCount);
	void SetOutArrayStateCount(uint8 uCount);
	void OnTick();
	void OnRemoveTeamMember(uint32 uTeamID,ETeamType eTeamType,uint32 uEntityID);
	void NotifyClientArrayValueChange();
	uint32 CalcArrayLevel();

	CBattleArrayMagicCfgServerSharedPtr& GetCfgSharedPtr()const;
private:
	~CBattleArrayMagicServer();

	typedef vector<CBattleArrayStationServer*, TMagicAllocator<CBattleArrayStationServer*> > VecBAStation_t;
	typedef vector<uint32, TMagicAllocator<uint32> > VecMakeArray_t;

	CSkillInstServer*		m_pInst;
	CBattleArrayMagicCfgServerSharedPtr*	m_pCfg;
	CFighterDictator*		m_pFrom;
	CCoreSceneServer*		m_pScene;
	uint8					m_uInStationCount;
	uint8					m_uOutArrayStateCount;
	int32					m_iValue;
	uint32					m_uValue;
	VecBattleArray_t		m_vecBattleArray;
	VecBAStation_t			m_vecBAStation;
	static VecMakeArray_t	ms_vecMakeArraySpeed;
};
