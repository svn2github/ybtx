#pragma once
#include "CBaseRuleCondition.h"
#include "CPos.h"
#include "CNpcAIMallocObject.h"
#include "TNpcAIAllocator.h"
#include "TNpcAIDataAllocator.h"
#include "PatternRef.h"
#include "CDistortedTick.h"

class CNpcAI;
class CSkillRuleMgr;
class CCharacterDictator;
class CNpcEventMsg;
class CExclaimRule;
struct CSkillRuleData;
class CGoBackExclaimRule;
enum ESRTargetType;
enum ESkillRuleKind;
class CSkillRuleDoSkillMoveMgr;
class CSkillRuleCoolDownTick;
struct CRuleCondData;
class CNormalSkillServerSharedPtr;
class CTargetFilter;

typedef list<CRuleCondData*, TNpcAIDataAllocator<CRuleCondData*> > RuleCondDataList;
typedef list<string, TNpcAIDataAllocator<string> > MultiArgList;

class CSkillRule
	: public CNpcAIMallocObject
{
friend class CSkillRuleDoSkillMoveMgr;
friend class CSkillAroundRule;
friend class CSkillRuleMgr;
friend class CExclaimRule;
friend class CMultiSkillRule;
friend class CGoBackExclaimRule;
friend class CSkillRuleCoolDownTick;
friend class CTargetFilter;
public:
	CSkillRule(CSkillRuleMgr* pRuleMgr,CSkillRuleData* pSkillRuleData);
	virtual ~CSkillRule();
	CSkillRuleMgr* GetSkillRuleMgr() { return m_pSkillRuleMgr;}
	virtual bool CheckAndDoSkill();	
	virtual bool DoRuleSkill();		//走技能规则释放流程的技能
	virtual void DoNoRuleSkill();			//不走技能规则直接释放的技能
	virtual void OnSkillCondSatify(bool bRepeat = true);//能否添加重复的技能
	bool GetIsRuleSkill();
	virtual	void Handle(uint32 uEventID);
	virtual void CreateEventHandler();
	void CreateRuleCond(CSkillRuleData* pSkillRuleData); 
	bool CheckContinueCond(IRuleCondHandler* pRuleCond = NULL);
	virtual EDoSkillType CheckSkillDistance();
	void LoadBeginCond();
	void LoadContinueCond();
	void UnInstallBeginCond();
	void UnInstallContinueCond();
	CCharacterDictator* GetCharacter();
	CNpcAI* GetOwner();
	void AddEventHandler(uint32 uEventID,IRuleCondHandler* pRuleCond);
	virtual void StopDoMoveSkill();
	void SetSkillRuleKind(ESkillRuleKind eSRKType)	{ m_eSRKType = eSRKType;}				//涉及技能规则类型，战斗释放，待机释放，死亡释放三种不同类型
	ESkillRuleKind GetSkillRuleKind() const {return m_eSRKType;}
	void SkillRuleReferHandler(CSkillRuleDoSkillMoveMgr* pDoMoveMgr);
	bool SkillRuleCoolDown() {return m_pCoolDownTick == NULL;}
	void CheckCoolDown();
	CTargetFilter* GetTargetFilter() const {return m_pTargetFilter;} 
protected:
	string			m_strSkillName;
	const CNormalSkillServerSharedPtr* m_pSkillServerCfg;
private:
	typedef list<IRuleCondHandler*, TNpcAIAllocator<IRuleCondHandler*> > ListRuleCond;
	ESkillRuleKind	m_eSRKType;
	float m_fMinDistance;
	float m_fMaxDistance;
	int32 m_iSkillPage;
	uint32	m_uCoolDownTime;
	bool m_bLoadContinueCond;
	CSkillRuleMgr*  m_pSkillRuleMgr;
	CDistortedTick*	m_pCoolDownTick;
	ListRuleCond	m_listBeginCondition;
	ListRuleCond	m_listContinueCondition;
	CTargetFilter*	m_pTargetFilter;
	bool			m_bCheckDistanceAgain;
	map<uint32,ListRuleCond, less<uint32>, TNpcAIAllocator<pair<uint32, ListRuleCond> > > m_mapRuleCondByEvent;
	TPtRefer<CSkillRule, CSkillRuleDoSkillMoveMgr>  m_pSkillRuleReferHandler;
};

