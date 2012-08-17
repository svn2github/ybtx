#pragma once
#include "FightDef.h"
#include "CConfigMallocObject.h"
#include "TConfigAllocator.h"
#include "CFighterMallocObject.h"

class CSkillReplace
	: public CConfigMallocObject
{
public:
	CSkillReplace();
	~CSkillReplace();

public:
	typedef multimap<string, CSkillReplace*, less<string>, TConfigAllocator<pair<string, CSkillReplace*> > > MultiMapCSkillReplace;
	
	static bool LoadConfig(const string& szFileName);
	static void UnloadConfig();
	
	const string&	GetName()				{ return m_sSkillName; }
	const string&	GetStateName()			{ return m_sStateName; }
	const string&	GetReplaceSkillName()	{ return m_sReplaceSkillName; }
	uint32			GetPriority()			{ return m_uPriority; }
private:
	string 		m_sSkillName;			//技能名称	
	string		m_sStateName;			//条件状态名
	string		m_sReplaceSkillName;	//替换技能名
	uint32		m_uPriority;			//替换优先级
public:
	static	MultiMapCSkillReplace	ms_multimapSkillReplace;
};

class CFighterDictator;
class CSkillReplaceServer
	:public CFighterMallocObject
{
public:
	CSkillReplaceServer(CFighterDictator* pFighter);
	~CSkillReplaceServer();

	CFighterDictator* GetFighter() { return m_pFighter; }
	uint32 ReplaceSkill(const TCHAR* szStateName);
	void   CancelReplaceSkill(const TCHAR* szStateName);
private:
	CFighterDictator*					m_pFighter;
};
