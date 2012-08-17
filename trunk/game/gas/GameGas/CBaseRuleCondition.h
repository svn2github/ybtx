#pragma once

#include "CSkillRuleDef.h"
#include "CNpcAIMallocObject.h"

class CSkillRule;
enum ERuleCondKind;

class IRuleCondHandler
	: public CNpcAIMallocObject
{
public:
	virtual ~IRuleCondHandler() {}
	virtual	bool	CheckCondition() = 0;
	virtual bool AutoCheck() { return false;}
	virtual ERuleCondKind GetRuleCondKind() = 0;
	virtual void CreateEventHandler() {}
	virtual void ReInstallCond() {}
	virtual void UnInstallCond() {}
};

