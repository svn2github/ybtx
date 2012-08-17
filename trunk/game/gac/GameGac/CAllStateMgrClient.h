#pragma once
#include "CTplStateCfgClient.h"
#include "TMagicStateAllocator.h"

class CPlayerFX;
class CFighterFollower;

typedef map<uint32, CPlayerFX*, less<uint32>, TMagicStateAllocator<pair<uint32, CPlayerFX*> > >			MapFxPlayer;

class CAllStateMgrClient;

class CBaseStateClient
	: public CMagicStateMallocObject
{
public:
	virtual CBaseStateCfg*	GetCfg() = 0;
	virtual CFighterFollower* GetOwner() {return NULL;}
	//virtual uint32			GetDynamicID() = 0;
	virtual uint32			GetID() = 0;
	virtual uint32			GetCount() = 0;
	int32					GetTime() {return m_iTime;}
	float					GetRemainTime();
	//void					SetRemainTime(int32 iTime)	{m_iRemainTime = iTime;}
	void					UpdateTime(int32 iTime);
	void					UpdateRemainTime(int32 iTime);
	virtual					~CBaseStateClient();
	uint32					GetDynamicID() {return m_uDynamicId;}
	uint32					GetSkillLevel()	{return m_uSkillLevel;}
	uint32					GetInstallerID() {return m_uInstallerID;}
	uint32					GetSetupOrder() {return m_uSetupOrder;}

protected:
	CBaseStateClient(uint32 m_uDynamicId, int32 iTime, int32 iRemainTime, uint32 uSkillLevel,
		uint32 uInstallerID, CAllStateMgrClient* pAllStateMgr);
	uint32					m_uDynamicId;						//动态ID
	int32					m_iTime;							//总时间
	int32					m_iRemainTime;						//剩余时间，-1为无限
	uint64					m_uUpdateTime;
	uint32					m_uSkillLevel;
	uint32					m_uInstallerID;
	uint32					m_uSetupOrder;
	CAllStateMgrClient*		m_pAllStateMgr;

};

typedef map<uint32, CBaseStateClient*, less<uint32>, TMagicStateAllocator<pair<uint32, CBaseStateClient*> > >		MapSetupOrder;

class CAllStateMgrClient
	: public CMagicStateMallocObject
{
	friend class CBaseStateClient;
public:
	CAllStateMgrClient(CFighterFollower* pFighter);
	~CAllStateMgrClient();
	void	OnStateCategoryBegin(uint32 uCategoryId, uint32 uDynamicId, uint32 uCascade);
	void	OnStateCascadeChange(uint32 uCategoryId, uint32 uDynamicId, uint32 uCascade, uint32 uOldCascade, bool bFromAdd = false);
	void	OnStateCategoryEnd(uint32 uCategoryId, uint32 uDynamicId, uint32 uOldCascade);
	void	OnStateEntityAdd(uint32 uCategoryId, uint32 uDynamicId, uint32 uCascade);
	void	OnStateEntityDelete(uint32 uCategoryId, uint32 uDynamicId, uint32 uOldCascade, uint32 uLeftDynamicId, uint32 uLeftCascade);
	void	ReplayFx();
	void	OnBuffUpdate(uint32 uDynamicId, const TCHAR* szName, uint32 uCount, int32 iTime, float fRemainTime, bool bDecrease, uint32 uSkillLevel, float fDotInterval, uint32 uInstallerID, int32 iCalcValue = 0);
	void	SetTargetBuff();
	MapSetupOrder			m_mapSetupOrder;
private:
	MapFxPlayer				m_mapFxPlayer;
	CFighterFollower*		m_pFighter;
	double					m_dScale;							//当前缩放比例
	uint32					m_uMaxSetupOrder;
};