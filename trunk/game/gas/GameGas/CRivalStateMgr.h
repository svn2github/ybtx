#pragma once
#include "TMagicStateAllocator.h"

class CSkillInstServer;
class CCfgArg;
class CFighterDictator;
class CFighterDictator;

struct CStateFuncArg
{
	CStateFuncArg(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo)
	: m_pSkillInst(pSkillInst)
	, m_pArg(pArg)
	, m_pFrom(pFrom)
	, m_pTo(pTo)
	{

	}
	CSkillInstServer* m_pSkillInst;
	const CCfgArg* m_pArg;
	CFighterDictator* m_pFrom;
	CFighterDictator* m_pTo;
};

typedef pair<uint32, CSkillInstServer*>	RivalContent;

typedef map<uint32, RivalContent, greater<uint32>, TMagicStateAllocator<pair<uint32, RivalContent> > > MapRivalLevel;

class CRivalStateCategory
	:public CMagicStateMallocObject
{
public:
	~CRivalStateCategory();
	void AddState(uint32 uLevel, CStateFuncArg& aStateFuncArg);
	bool DelState(uint32 uLevel, CStateFuncArg& aStateFuncArg);
	void Clear();
	void SetupState(CSkillInstServer* pSkillInst, CStateFuncArg& aStateFuncArg);
	void CancelState(CSkillInstServer* pSkillInst, CStateFuncArg& aStateFuncArg);

private:
	MapRivalLevel			m_mapRivalLevel;
};

typedef map<string, CRivalStateCategory*, less<string>, TMagicStateAllocator<pair<string, CRivalStateCategory*> > >
	MapRivalCategory;

class CRivalStateMgr
	:public CMagicStateMallocObject
{
public:
	CRivalStateMgr() {};
	~CRivalStateMgr();
	void AddState(const string& name, CStateFuncArg& aStateFuncArg);
	void DelState(const string& name, CStateFuncArg& aStateFuncArg);
	void ClearAll();

private:
	MapRivalCategory		m_mapRivalCategory;
};
