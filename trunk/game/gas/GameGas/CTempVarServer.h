#pragma once
#include <set>
#include "CFighterMallocObject.h"
#include "TFighterAllocator.h"

using namespace std;

class CFighterDictator;
union UUnitValue;

//临时变量管理器
class CTempVarMgrServer
	:public CFighterMallocObject
{
public:
	CTempVarMgrServer(const CFighterDictator* pHolder);
	typedef map<string, int32, less<string>, TFighterAllocator<pair<const string, int32> > >	MapImmuneTaskAttack;							//免疫任务攻击类型
	
	//类成员指针问题2010-12-21
	//typedef map<string, int32 CTempVarMgrServer::* , less<string>, TFighterAllocator<pair<string, int32 CTempVarMgrServer::* > > > MapTempVar;
	typedef map<string, uint32> MapTempVar;
	typedef vector<int32 CTempVarMgrServer::*> VecTempVar;

	//易变值相关函数
	int32				GetVarValue(const string& sVarName); 		//用名字获取临时变量
	int32				GetVarValue(UUnitValue pVarPtr); 			//用偏移获取临时变量
	void				SetVarValue(const string& sVarName, int32 iValue);//设置临时变量的值
	static string		GetPassVarName(const string& sVarName);			//取被动变量的名字（如果是被动变量则取其对应主动变量的名字）
	static bool			ExistVar(const string& sVarName) {return m_mapPVar.find(sVarName) != m_mapPVar.end();}
	int32				GetDamage()					{return m_uSubHP;}
	int32				GetBeDamaged()				{return m_uBeSubHP;}
	void				SetDamage(int32 iValue);
	void				SetBeDamaged(int32 iValue);
	int32				GetSubMP()					{return m_uSubMP;}
	int32				GetBeSubMP()				{return m_uBeSubMP;}
	void				SetSubMP(int32 iValue);
	void				SetBeSubMP(int32 iValue);

	//吟唱时间相关函数
	bool				GetNoSingTime()				{return m_iNoSingTime != 0;}
	void				SetNoSingTime(int32 iCount = 1)	{m_iNoSingTime = iCount;}
	void				SetEternalNoSingTime()		{m_iNoSingTime = -1;}
	void				CancelNoSingTime()			{m_iNoSingTime = 0;}
	void				CountNoSingTime()			{if(m_iNoSingTime > 0) m_iNoSingTime--;}
	bool				GetNoSingTimeSkill(const string & sSkill)	{return m_setNoSingTimeSkill.find(sSkill) != m_setNoSingTimeSkill.end();}
	void				SetNoSingTimeSkill(const string & sSkill)	{m_setNoSingTimeSkill.insert(sSkill);}
	const string&		GetDamageChangeStateName() {return m_strDamageChangeStateName;}
	void				SetDamageChangeStateName(const string & sName) {m_strDamageChangeStateName = sName;}
	void				ClearNoSingTimeSkill()	{m_setNoSingTimeSkill.clear();}
	void				CancelNoSingTimeSkill(const string & sSkill)	{m_setNoSingTimeSkill.erase(sSkill);}

	static bool			SetVarPtr(UUnitValue* pVarPtr, const string& sVarName);

	int32				m_iMopCalcValue;							//魔法操作计算值
	bool				m_bLockIncreaseHP;
	bool				m_bLockDecreaseHP;
	bool				m_bLockIncreaseAgile;
	bool				m_bLockDecreaseAgile;
	float				m_fDamageShareProportion;   
	CFighterDictator* m_pLinkFighter;

	//停止触发（触发器和伤害变更的）事件
	bool				DisTriggerEvent()			{return m_iDisTriggerEvent > 0;}
	int32				m_iDisTriggerEvent;

private:
	static MapTempVar	m_mapPVar;			//临时变量的名字映射f
	static VecTempVar	m_vecPVar;
	set<string>			m_setNoSingTimeSkill;							//下一次吟唱时间为0的技能

public:
	static set<string>	m_setImmuneTaskAttackName;
	MapImmuneTaskAttack m_iImmuneTaskAttack;
	int32				m_iPrintInfoOff;								// 不显示弹出和打印信息

private:
	//易变值相关变量
	int32				m_uSubHP;										//临时.伤害
	int32				m_uBeSubHP;										//临时.被伤害
	int32				m_uUnchangeSubHP;								//临时.未变更伤害
	int32				m_uBeUnchangeSubHP;								//临时.被未变更伤害
	int32				m_uSubMP;										//临时.减蓝
	int32				m_uBeSubMP;										//临时.被减蓝
	int32				m_uHeal;										//临时.治疗
	int32				m_uGetHeal;										//临时.受到治疗
	int32				m_uUnchangeHealHP;								//临时.未变更治疗
	int32				m_uBeUnchangeHealHP;							//临时.被未变更治疗
	
	//吟唱时间相关变量
	int32				m_iNoSingTime;									//下一次所有吟唱时间为0

	string				m_strDamageChangeStateName;						//一次伤害被伤害变更吸收的状态名



	const CFighterDictator*	m_pHolder;	//临时变量管理器的拥有者

	//初始化专用
public:
	static bool			Init();
	static bool			__init;

};
