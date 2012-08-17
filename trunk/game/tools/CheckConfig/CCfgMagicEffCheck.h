#pragma once
#include "CCfgMagicOp.h"
#include "CBaseMagicOp.h"
#include "TConfigAllocator.h"

class CCfgMagicEffCheck
{
public:
	~CCfgMagicEffCheck();
	static bool Check(const TCHAR* cfgFile, bool bFirstFile);
	static void EndCheck();
	static bool CheckMagicEffExist(const string& sValue);
	static bool CheckMagicOpAvail();
	static void BuildMOPMap();

	EValueMagicOpType GetFirstOpValueMagicOpType();
	string GetMagicEffName() { return m_strName; }
	bool CheckMagicOpRelationAvail(MOP_MATCH_CHECKER pFunChecker);
	bool CheckNpcSkillFilterObj2MagicEff(const string sRuleName, string sSkillName, ESRTargetType eSRTargetType);
	bool CheckMustCancelEffOfState();

	typedef map<string, CCfgMagicEffCheck*>	MapMagicEff;
	static MapMagicEff	ms_mapMagicEff;

	typedef map<string,	CBaseMagicOp*, less<string>, TConfigAllocator<pair<string, CBaseMagicOp* > > >	MapMagicOp;
	static MapMagicOp	ms_mapMagicOp;

private:
	typedef vector<CCfgMagicOp*>	VecChild;
	VecChild	m_vecChild;
	string		m_strName;					// 魔法效果名
	static uint32 ms_uMaxLineNum;
};
