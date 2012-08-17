#pragma once
#include "CSkillMgrServer.h"
#include "TFighterAllocator.h"

class CTempSkill;
class CFighterMediator;
class CGenericTarget;

class CPlayerSkillMgrServer 
	: public CSkillMgrServer
{
public:
	typedef vector<CTempSkill*,TFighterAllocator<CTempSkill*> >	VecTempSkill;

	CPlayerSkillMgrServer(CFighterMediator* pFigter);
	virtual ~CPlayerSkillMgrServer(void);

	EDoSkillResult UseFightSkill(const CNormalSkillServerSharedPtr& pSkill, uint8 uSkillLevel, uint8 uPos, uint8 uDir, uint8 uMagicDir);
	EDoSkillResult DoPlayerSkill(const string& szName, uint8 uSkillLevel, uint8 uDir = 0, uint8 uMagicDir = 0);
	EDoSkillResult DoPlayerPassiveSkill(const CNormalSkillServerSharedPtr& pSkill, uint8 uSkillLevel, uint8 uDir = 0);
	EDoSkillResult PlayerDoPassiveFightSkill(const string& szName, uint8 uSkillLevel);
	bool AddTempSkill(const string& szName, uint8 uSkillLevel);
	bool IsExistTempSkill(const string& szName, uint8 uSkillLevel, uint8 uPos);
	void RemoveTempSkill(const string& szName, uint8 uSkillLevel, uint8 uPos);
	void ActiveTempSkill(const string& szName, uint8 uSkillLevel, uint8 uPos);
	void RemoveTempSkill(const string& szName, uint8 uSkillLevel);
	void RemoveAllTempSkill();
	CFighterMediator* GetFighterMediator()const;

private:
	EDoSkillResult CanPlayUseSkill(const CNormalSkillServerSharedPtr& pSkill, uint8 uSkillLevel, uint8 uPos = 0);
	EDoSkillResult DoPlayerSkill(const CNormalSkillServerSharedPtr& pSkill, uint8 uSkillLevel, uint8 uDir = 0, uint8 uMagicDir = 0);
	EDoSkillResult DoSkillCheck(const CNormalSkillServerSharedPtr& pSkill)const;
	EDoSkillResult InitTarget(CGenericTarget& target, const CNormalSkillServerSharedPtr& pSkill, uint8 uSkillLevel)const;
	void AdjustSkillLevel(uint8& uOutPutSkillLevel, uint8 uSkillLevel, const CNormalSkillServerSharedPtr& pSkill)const;
	CSkillInstServer* CreateSkillInst(const CNormalSkillServerSharedPtr& pSkill, uint8 uSkillLevel, uint8 uDir, uint8 uMagicDir = 0)const;

private:
	VecTempSkill			m_vecTempSkill;
};

class CTempSkill
	:public CDistortedTick
	,public CFighterMallocObject
{
	friend class CPlayerSkillMgrServer;
public:
	CTempSkill(CPlayerSkillMgrServer* pSkillMgr,const string& strSkillName,uint8 uSkillLevel,uint8 uID);
	bool IsSameTempSkill(const string& strName,uint8 uSkillLevel,uint8 uID);
	void Cancel();
private:
	~CTempSkill();
	void RegTick( uint32 uTime );
	void OnTick();
	void UnRegTick();
	const string				m_strSkillName;
	CPlayerSkillMgrServer*		m_pSkillMgr;
	const uint8					m_uSkillLevel;
	const uint8					m_uID;
	bool						m_bIsEndTime;
	int32						m_uCount;
};

