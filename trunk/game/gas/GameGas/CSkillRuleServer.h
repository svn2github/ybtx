#pragma once
#include "CDynamicObject.h"
#include "CCfgColChecker.h"
#include "CSkillRuleDef.h"
#include "TNpcAIDataAllocator.h"

struct CSkillRuleData;
struct CRuleCondData;
class TypeName2SRTarget;

typedef list<CSkillRuleData*, TNpcAIDataAllocator<CSkillRuleData*> > LstRuleDataType;
typedef list<CRuleCondData*, TNpcAIDataAllocator<CRuleCondData*> > RuleCondDataList;
typedef list<string, TNpcAIDataAllocator<string> > SkillNameLst;

class CSkillRuleServer 
	: public virtual CDynamicObject
{
public:
	static bool LoadSkillRuleCfg(const string& strFileName);
	static void UnloadSkillRuleCfg();
	static bool IsHaveSkillRule(const string &strNpcRuleName);
	static LstRuleDataType* GetSkillRuleDataByName(string strName) { return m_mapRuleDataByName[strName];}
private:
	static void CreateRuleCondData(CSkillRuleData* pData,string &strBegin,string &strContiune);
	static void CreateRuleTargetType(CSkillRuleData* pData,string& strTarget);
	static void CreateRuleDistance(CSkillRuleData* pData,string& strDistace);
	static void ParseRuleCond(CSkillRuleData* pData,string& strCond,ERuleCondKind eRuleKind);
	static void ParseRuleCondArg(CRuleCondData* pRuleCondData,string& strArg);
	static bool IsHaveSkillArg(const string &strSkillName);
	static void CreateMultiSkill(CSkillRuleData* pData,string &strMultiSkill);
	static void CreateSkillTypeName(CSkillRuleData* pData, string& strSkillRuleName);
	static void CreateRuleDataSkillCfg(CSkillRuleData* pData);

	template<class MapType, class EnumType>
	static void CreateMap(MapType& mapEnum, uint32 uCount, ...);
private:
	typedef map<string,ESkillRuleTypeName, less<string>, TNpcAIDataAllocator<pair<string, ESkillRuleTypeName> > > MapSRTypeName;
	typedef	MapSRTypeName::const_iterator		MapSRTypeNamrIter;
	typedef map<string,ERCondType, less<string>, TNpcAIDataAllocator<pair<string, ERCondType> > > MapRuleCondType;
	typedef map<string,LstRuleDataType*, less<string>, TNpcAIDataAllocator<pair<string, LstRuleDataType*> > > MapSRDataType;
	static MapSRTypeName		m_mapSkillTypeName;
	static MapSRDataType		m_mapRuleDataByName;
	static string				m_strTempRuleName;
	static string				m_strTempSkillName;
	static TypeName2SRTarget	m_mapSRTargetType;
};

