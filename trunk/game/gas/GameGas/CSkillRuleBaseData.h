#pragma once
#include "CNpcAIDataMallocObject.h"
#include "TNpcAIDataAllocator.h"
enum ERCondType;
enum ERuleCondKind;
enum ESRTargetType;
class CNormalSkillServerSharedPtr;

struct CRuleCondData
	: public CNpcAIDataMallocObject 
{
	float	m_fOneVal;
	float	m_fTwoVal;
	ERCondType m_eRCType;
	ERuleCondKind m_eRCKType;
};

struct CSkillRuleData
	: public CNpcAIDataMallocObject 
{
	ESRTargetType			m_eSRTargetType;
	uint32					m_uMinDistance;
	uint32					m_uMaxDistance;
	string					m_strSkillName;
	ESkillRuleTypeName		m_eSRTypeName;
	int32					m_iSkillPage;
	uint32					m_uTargetArg1;
	uint32					m_uTargetArg2;
	uint32					m_uCoolDownTime;
	bool					m_bCheckDistanceAgain;
	const CNormalSkillServerSharedPtr* m_pSkillServerCfg;
	list<CRuleCondData*, TNpcAIDataAllocator<CRuleCondData*> >	m_lstRuleCond;
	list<string, TNpcAIDataAllocator<string> >			m_lstMultiArgs;
};

