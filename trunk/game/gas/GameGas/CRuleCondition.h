#pragma once
#include "CSkillRule.h"
#include "CDistortedTick.h"

struct CRuleCondData;
class CCharacterDictator;

#define DECLARE_SKILLRULECOND(name) \
	class name : public IRuleCondHandler \
	{ \
	public: \
		name(CSkillRule* pSkillRule,CRuleCondData* pRuleData); \
		virtual	bool	CheckCondition(); \
		virtual bool AutoCheck(); \
		CCharacterDictator* GetCharacter() { return m_pSkillRule->GetCharacter();} \
		CSkillRule* GetSkillRule() { return m_pSkillRule;} \
		ERuleCondKind GetRuleCondKind() { return m_eRCKType;} \
	private: \
		CSkillRule* m_pSkillRule; \
		ERuleCondKind m_eRCKType; 
#define END_DECLARE_SKILLRULECOND   };

DECLARE_SKILLRULECOND(CHpGreaterCond);
	float m_fVergeHpValue;
	public:
	virtual void CreateEventHandler();
END_DECLARE_SKILLRULECOND;

DECLARE_SKILLRULECOND(CHpLesserCond)
	float m_fVergeHpValue;
	public:
	virtual void CreateEventHandler();
END_DECLARE_SKILLRULECOND

DECLARE_SKILLRULECOND(CMpGreaterCond)
	float m_fVergeMpValue;
	public:
	virtual void CreateEventHandler();
END_DECLARE_SKILLRULECOND

DECLARE_SKILLRULECOND(CMpLesserCond)
	float m_fVergeMpValue;
	public:
	virtual void CreateEventHandler();
END_DECLARE_SKILLRULECOND

DECLARE_SKILLRULECOND(CEnterBattleCond)	
	public:
	virtual void CreateEventHandler();
END_DECLARE_SKILLRULECOND

DECLARE_SKILLRULECOND(CBeHurtValueGreater)
	int32 m_iBeHurtValue;
	public:
	virtual void CreateEventHandler();
END_DECLARE_SKILLRULECOND

DECLARE_SKILLRULECOND(CRandomRate)
	int32 m_iRandNum;
END_DECLARE_SKILLRULECOND

DECLARE_SKILLRULECOND(CBattleTimerExcess)
	uint32 m_uBattleTime;
	CDistortedTick* m_pTick;
	public:
		virtual ~CBattleTimerExcess();
		struct CBattleTimeTick
			: public CDistortedTick
			, public CNpcAIMallocObject 
		{
			CBattleTimeTick(CBattleTimerExcess *pBattleTimer) : m_pBattleTimer(pBattleTimer) { }
			void OnTick();
			CBattleTimerExcess* m_pBattleTimer;
		};
	void ReInstallCond();
	void UnInstallCond();
	void CreateEventHandler();
END_DECLARE_SKILLRULECOND

DECLARE_SKILLRULECOND(CDeathCond)
	public:
	virtual void CreateEventHandler();
END_DECLARE_SKILLRULECOND

DECLARE_SKILLRULECOND(CTimerCond)
	uint32 m_uTime;
	CDistortedTick* m_pTick;
	public:
		virtual ~CTimerCond();
		struct CTimerTick
			: public CDistortedTick
			, public CNpcAIMallocObject 
		{
			CTimerTick(CTimerCond *pTimerCond) : m_pTimerCond(pTimerCond) { }
			void OnTick();
			CTimerCond* m_pTimerCond;
		};
		void ReInstallCond();
		void UnInstallCond();
END_DECLARE_SKILLRULECOND

DECLARE_SKILLRULECOND(CRangeTimerCond)
	uint32 m_uMinTime;
	uint32 m_uMaxTime;
	CDistortedTick* m_pTick;
	public:
		virtual ~CRangeTimerCond();
		struct CRangeTimerTick
			: public CDistortedTick
			, public CNpcAIMallocObject 
		{
			CRangeTimerTick(CRangeTimerCond *pTimerCond) : m_pRangeTimerCond(pTimerCond) {}
			void OnTick();
			CRangeTimerCond* m_pRangeTimerCond;
		};
		void ReInstallCond();
		void UnInstallCond();
END_DECLARE_SKILLRULECOND

DECLARE_SKILLRULECOND(CEnterPhaseCond)	
	public:
		virtual void CreateEventHandler();
END_DECLARE_SKILLRULECOND

DECLARE_SKILLRULECOND(CPhaseTimerExcess)
	uint32 m_uTime;
	CDistortedTick* m_pTick;
	public:
		virtual ~CPhaseTimerExcess();
		struct CPhaseTimerTick
			: public CDistortedTick
			, public CNpcAIMallocObject 
		{
			CPhaseTimerTick(CPhaseTimerExcess *pPhaseTimerCond) : m_pPhaseTimerCond(pPhaseTimerCond) {}
			void OnTick();
			CPhaseTimerExcess* m_pPhaseTimerCond;
		};
		void UnInstallCond();
		void CreateEventHandler();
END_DECLARE_SKILLRULECOND

DECLARE_SKILLRULECOND(CEnterWanderCond)
	public:
		void CreateEventHandler();
END_DECLARE_SKILLRULECOND

DECLARE_SKILLRULECOND(CWanderTimerExcess)
uint32 m_uTime;
CDistortedTick* m_pTick;
	public:
		virtual ~CWanderTimerExcess();
		struct CWanderTimerTick
			: public CDistortedTick
			, public CNpcAIMallocObject 
		{
			CWanderTimerTick(CWanderTimerExcess *pWanderTimerCond) : m_pWanderTimerCond(pWanderTimerCond) {}
			void OnTick();
			CWanderTimerExcess* m_pWanderTimerCond;
		};
		void UnInstallCond();
		void CreateEventHandler();
END_DECLARE_SKILLRULECOND

DECLARE_SKILLRULECOND(CEnemyDead)
	public:
		void CreateEventHandler();
END_DECLARE_SKILLRULECOND

#undef DECLARE_SKILLRULECOND
#undef END_DECLARE_SKILLRULECOND

class CHpFirstGreaterCond : public CHpGreaterCond
{
public:
	CHpFirstGreaterCond(CSkillRule* pSkillRule,CRuleCondData* pRuleData); 
	virtual bool AutoCheck();
private:
	bool m_bInEffect;
};

class CHpFirstLesserCond : public CHpLesserCond
{
public:
	CHpFirstLesserCond(CSkillRule* pSkillRule,CRuleCondData* pRuleData); 
	virtual bool AutoCheck();
private:
	bool m_bInEffect;
};

class CMpFirstGreaterCond : public CMpGreaterCond
{
public:
	CMpFirstGreaterCond(CSkillRule* pSkillRule,CRuleCondData* pRuleData);
	virtual bool AutoCheck();
private:
	bool m_bInEffect;
};

class CMpFirstLesserCond : public CMpLesserCond
{
public:
	CMpFirstLesserCond(CSkillRule* pSkillRule,CRuleCondData* pRuleData);
	virtual bool AutoCheck();
private:
	bool m_bInEffect;
};
