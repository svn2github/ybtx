#pragma once
#include "CDynamicObject.h"
#include "TCfgCalcMallocAllocator.h"

class CCfgCalc;
class CCfgRelationDeal;
class CFighterDirector;
class CFighterDictator;
class CFighterNull;

//全局变量类
class CCfgGlobal
	: public virtual CDynamicObject
	, public CCfgCalcMallocObject
{
	friend class CCfgRelationDeal;
public:
	typedef map<string, CCfgGlobal*, less<string>, TCfgCalcMallocAllocator<pair<string, CCfgGlobal*> > >					MapGlobalSkillParam;
	typedef vector<CCfgGlobal*, TCfgCalcMallocAllocator<CCfgGlobal*> >							VecGlobalSkillParam;

private:
	static MapGlobalSkillParam					m_mapVar;
	static VecGlobalSkillParam					m_vecVar;
	static bool									m_bRandValueIsFixed;
	static int32								m_iRandFixedValue;
	static bool									m_bRandfValueIsFixed;
	static double								m_dRandfFixedValue;

public:
	CCfgGlobal() : m_pCfgCalc(NULL)
	{

	}
	~CCfgGlobal();
	static bool									LoadConfig(const TCHAR* cfgFile);	//读配置表
	static void									UnloadConfig();						//卸载配置表
	static pair<bool, CCfgGlobal*>				GetCfgValue(const string&name);		//获取全局变量是否存在以及值
	static bool									ExistCfgGlobal(const string& name);
	static const TCHAR*							GetCalcChars(const TCHAR* sName);
	static uint32								GetVarId(const string& name);
	static pair<bool, CCfgGlobal*>				GetCfgValue(uint32 uId);

	static void									SetRandFixedValue(int32 iValue);
	static void									SetRandfFixedValue(double dValue);
	static void									ClearRandFixedValue();
	static void									ClearRandfFixedValue();

	template<typename FighterType>
	double										Get(const FighterType* pFrom,const FighterType* pTo, uint32 index);			//获取全局变量的值

	uint32										GetId()		{return m_uId;}

private:
	//static bool									Init();
	//static bool									__init;

	CCfgCalc*									m_pCfgCalc;
	bool										m_bIsArrayConst;
	vector<double>								m_dVecResult;
	deque<bool>									m_bDeqCalculated;
	uint32										m_uId;

public:

	//private:
	//	string										strVarName;
	//	CCfgCalc*									pVarValue;
	//
	//public:
	//	string&										GetName();
	//	CCfgCalc*									GetValue();

	//扩展函数
	static int32 Rand(double fStart, double fEnd);										//随机函数，返回整数
	static double Randf(double fStart, double fEnd);										//随机函数，返回浮点数
	static double IfElse(double fIf, double fThen, double fElse);						//条件取值函数

	template<typename FighterType>
	static bool ExistMagicState(string& name, const FighterType* pFighter);				//检测是否存在某个普通魔法状态（客户端）

	template<typename FighterType>
	static bool ExistTriggerState(string& name, const FighterType* pFighter);			//检测是否存在某个触发器魔法状态（客户端）

	template<typename FighterType>
	static bool ExistDamageChangeState(string& name, const FighterType* pFighter);		//检测是否存在某个伤害变更魔法状态（客户端）

	template<typename FighterType>
	static bool ExistSpecialState(string& name, const FighterType* pFighter);			//检测是否存在某个特殊魔法状态（客户端）

	template<typename FighterType>
	static bool ExistState(string& name, const FighterType* pFighter);			//检测是否存在某个魔法状态（客户端）

	template<typename FighterType>
	static uint32 StateCount(string& name, const FighterType* pFighter);			//获取某个魔法状态的层数（客户端）

	template<typename FighterType>
	static uint32 CurRlserStateCount(string& name, const FighterType* pFrom, const FighterType* pFighter);			//获取某个魔法状态的层数（客户端）

	template<typename FighterType>
	static uint32 TriggerCount(string& name, const FighterType* pFighter);			//获取某个魔法状态的作用次数或伤害数值累计值（客户端）

	template<typename FighterType>
	static int32 StateLeftTime(string& name, const FighterType* pFrom, const FighterType* pTo);				//获取某个魔法状态的剩余时间（客户端）

	template<typename FighterType>
	static double Distance(const FighterType* pFrom, const FighterType* pTo);						//求pFrom和pTo间的距离

	template<typename FighterType>
	static bool TargetIsNPC(const FighterType* pTo);

	static bool IsActiveSkill(string& name, const CFighterDirector* pFighter);				//检测是否存在某个普通魔法状态（客户端）
	static bool IsActiveSkill(string& name, const CFighterDictator* pFighter);		//检测是否存在某个普通魔法状态（服务器）
	static bool IsActiveSkill(string& name, const CFighterNull* pFighter);					//检测是否存在某个普通魔法状态（空）

};

class CAttrVarMap
	: public CCfgCalcMallocObject
{
public:
	typedef map<string, uint32, less<string>, TCfgCalcMallocAllocator<pair<string, uint32> > >					MapAttrVarName2Id;

private:
	static MapAttrVarName2Id					m_mapVarId;
	static bool									InitMapVarId();
	static bool									__init;

public:
	static pair<bool, uint32>					GetVarId(const string& name);
};





