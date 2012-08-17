#pragma once
#include "TSingleton.h"
#include "FightDef.h"
#include "CNpcAIDataMallocObject.h"
#include "TNpcAIDataAllocator.h"

class CNormalSkillServerSharedPtr;

typedef list<const CNormalSkillServerSharedPtr*, TNpcAIDataAllocator<const CNormalSkillServerSharedPtr*> > LstBornSkillType;
typedef list<pair<string, uint32>, TNpcAIDataAllocator<pair<string, uint32> > > LstSkillRule;

class CNpcFightBaseData
	: public CNpcAIDataMallocObject
{
	
	friend class CNpcFightBaseDataMgr;
public:
	CNpcFightBaseData( const string& sName);
	~CNpcFightBaseData();

	inline const string& GetName() const { return m_sName; }
	inline const LstBornSkillType& GetNpcBornSkill() const { return m_lstBornSkill;}
	void GetSkillRuleName(string& strSkillRuleName) const;
	inline const LstSkillRule& GetSkillRuleList() const {return m_lSkillRule;}
private:
	void CreateNpcBornSkill(string& strSkill);
	void CreateNpcSkillRule(string& strSkillRule);
public:
	string 				m_sNormalAttack;
	float				m_fAttackSpeed;	
	float				m_fAttackScope;
	EAttackType			m_eAttackType;
	EWeaponType			m_eWeaponType;
	uint32				m_uBornTriggerType;
	LstBornSkillType	m_lstBornSkill;
	bool				m_bOpenTargetPKState;
	bool				m_bLevelPress;
	bool				m_bChaosEnmity;
	bool				m_bRegulate;
	bool				m_bExclaimAlert;
	uint32				m_uRandMaxAttackSpeed;
	LstSkillRule		m_lSkillRule;
private:
	string				m_sName;
};

class CNpcFightBaseDataMgr 
	: public TSingleton< CNpcFightBaseDataMgr >
	, public CNpcAIDataMallocObject
{
	friend class TSingleton< CNpcFightBaseDataMgr >;
	CNpcFightBaseDataMgr();
	~CNpcFightBaseDataMgr();
public:
	const CNpcFightBaseData* GetEntity( const string& sName);
	bool AddEntity( CNpcFightBaseData* pEntity );
	bool LoadConfig( const string& sFileName );
	void DeleteAll();
private:
	typedef map<string, const CNpcFightBaseData*, less<string>, TNpcAIDataAllocator<pair<string, const CNpcFightBaseData*> > > IndexMapType;
	IndexMapType m_mapIndex;
};
