#pragma once
#include "INpcEventHandler.h"
#include "CDistortedTick.h"
#include "PatternRef.h"
#include "TNpcAIDataAllocator.h"
#include "TNpcAIAllocator.h"
#include "CSkillRule.h"
#include "CNpcEvent.h"

class CNpcAI;
struct CRuleCondData;
class CNpcEventMsg;
class CTargetFilter;

class CSkillRuleMgr 
	: public INpcEventHandler
{
friend class CPhaseChangeRule;
friend class CTargetFilter;
typedef list<CSkillRuleData*, TNpcAIDataAllocator<CSkillRuleData*> > LstRuleDataType;
public:
	CSkillRuleMgr(CNpcAI* pAI);
	~CSkillRuleMgr();
	virtual void Handle(CNpcEventMsg* pEvent);
	void OnEvent(ENpcEvent eEvent);
	void InstallBattleSkillRule();
	void UnInstallBattleSkillRule();
	void InstallWanderSkillRule();												//安装非战斗状态技能释放规则
	void UnInstallWanderSkillRule();											//卸载非战斗状态技能释放规则
	void ExecuteDeadSkill();													//死亡时释放技能
	void InstallPhase(uint32 uPage);
	void UnInstallPhase(uint32 uPage);
	void ChangeNextPhase();
	float GetCurrentHp() const { return m_fCurrentHp;}
	float GetFrontHp() const { return m_fFrontHp;}
	float GetCurrentMp() const { return m_fCurrentMp;}
	float GetFrontMp() const { return m_fFrontMp;}
	void ClearPhasePreparedSkillList();
	void AddPreparedSkillFront(CSkillRule* pSkillRule);
	void AddPreparedSkillBack(CSkillRule* pSkillRule, bool bRepeat);
	void AddEventHandler(uint32 uEventID,CSkillRule* pSkillRule);
	CNpcAI* GetOwner()const;
	CCharacterDictator* GetCharacter()const;
	uint64 GetStartBattleTime() { return m_uStartBattleTime;}
	bool GetIsHavePreparedSkill() { return !m_lstPreparedSkill.empty();}
	bool ExecuteSkill();
	void CreateSkillAniTick();
	void DestorySkillAniTick();
	void ClearCurSkill();	
	string& GetCurSkillName(){return m_pCurrentSkill->m_strSkillName;}
	TPtRefee<CSkillRuleMgr, CNpcAI>& GetRefsByNpcAI() {return m_RefsByNpcAI;}
	bool BeActiveState() {return m_bSkillMgrActive;}
	void OnBeHurt(int32 iHpChange);
	int32 GetHpChange(){return m_iHpChange;}
	const string& GetSkillRuleName() {return m_strSkillRuleName;}
	CSkillRule* GetCurrentSkill() {return m_pCurrentSkill;}
protected:
	void CalculateHp();
	void CalculateMp();
private:
	void InitBattleSkillRule();
	CSkillRule* CreateSkillRule(ESkillRuleTypeName eRuleType,CSkillRuleData* pSkillRuleData);
	void InitWanderAndDeadSkillRule();
	void MakeSkillRuleData();
private:
	typedef list<CSkillRule*, TNpcAIAllocator<CSkillRule*> > ListSkillRule;
	typedef map<int32,ListSkillRule, less<int32>, TNpcAIAllocator<pair<int32, ListSkillRule> > > MapSkillRulePage;
	float		m_fCurrentHp;
	float		m_fCurrentMp;
	float		m_fFrontHp;
	float		m_fFrontMp;
	bool		m_bSkillMgrActive;
	int32		m_iHpChange;
	uint32 m_uCurrentPage;
	uint64 m_uStartBattleTime;
	CNpcAI*	m_pOwner;	
	CDistortedTick* m_pSkillAnimaionTick;
	CSkillRule* m_pCurrentSkill;
	ListSkillRule m_lstPreparedSkill;				
	ListSkillRule m_lstWanderSkill;
	ListSkillRule m_lstDeadSkill;
	string			m_strSkillRuleName;

	LstRuleDataType			m_lstBattleSkillRuleData;
	LstRuleDataType			m_lstPhaseBattleSkillRuleData;
	LstRuleDataType			m_lstWanderAndDeadSkillRuleData;

	MapSkillRulePage m_mapSkilRuleByPage;
	typedef map<uint32, MapSkillRulePage, less<uint32>, TNpcAIAllocator<pair<uint32, MapSkillRulePage> > > MapID2SkillRulePage;
	MapID2SkillRulePage	m_mapEvent2RulePage;
	typedef map<uint32, ListSkillRule, less<uint32>, TNpcAIAllocator<pair<uint32, ListSkillRule> > > MapID2SkillRule;
	MapID2SkillRule m_mapWanderRuleHandler;
	TPtRefee<CSkillRuleMgr, CNpcAI> m_RefsByNpcAI;
};

