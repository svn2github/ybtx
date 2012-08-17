#pragma once
#include "CMagicEffServer.h"

class CSkillInstServer;
class CFighterDictator;
class CMagicOpArg;
class CGenericTarget;
class CCfgCalc;

struct  FilterLimit
	:public CConfigMallocObject
{
	float fFilterRadius;
	bool bTargetAliveCheckIsIgnored;
	bool bIsValueMagicOp;
};

class CFilterOpServer
	:public CConfigMallocObject
{
public:
	// 筛选管道(魔法效果使用)
	static void FilterByPipe(CSkillInstServer* pSkillInst, CMagicOpTreeServer::VecEntityID& vecEntityID, vector<MagicEffFilter*> vecFilterPipe, const FilterLimit& pFilterLimit, CFighterDictator* pFrom, CGenericTarget* pTo);
	// 筛选作用对象(魔法使用)
	static void FilterOperateObject(CSkillInstServer* pSkillInst, CMagicOpTreeServer::VecEntityID& vecObj, EOperateObjectType eOperaterObjectType, const FilterLimit& pFilterLimit, CFighterDictator* pFrom, CGenericTarget* pTo);

private:
	static void FilterEntity(CSkillInstServer* pSkillInst, CMagicOpTreeServer::VecEntityID& vecObj, MagicEffFilter* pMagicEffFilter, const FilterLimit& pFilterLimit, CFighterDictator* pFrom, CGenericTarget* pTo);
	static void QueryObj(CMagicOpTreeServer::VecEntityID& vecObj, const FilterLimit& pFilterLimit, float uFilterRadius, CFighterDictator* pFrom, CGenericTarget* pTo, ESimpleRelation eRelation);
	static void QueryObj(vector<CFighterDictator*>& vecObj, const FilterLimit& pFilterLimit, float uFilterRadius, CFighterDictator* pFrom, CGenericTarget* pTo, ESimpleRelation eRelation);
};
