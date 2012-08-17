#pragma once
#include "FightDef.h"
#include "CCfgCalc.h"
#include "CBaseMagicOp.h"
#include "CSkillRuleDef.h"

class CCfgMagicOp;

typedef bool (*MOP_MATCH_CHECKER)(CCfgMagicOp*);

struct MagicEffFilter
{
	MagicEffFilter():m_eObjectFilter(eOF_None),m_FilterPara(NULL) {}
	~MagicEffFilter()	{delete m_FilterPara;}
	EObjFilterType	m_eObjectFilter;	// 目标筛选类型
	CCfgCalc*		m_FilterPara;		// 筛选参数
};

class CCfgMagicOp
{
public:
	CCfgMagicOp(int32 nID);
	~CCfgMagicOp();

	static vector<string> Split(const string& src, string delimit);

	bool CheckMagicOpAvail();													// 检测魔法操作的有效性
	bool CheckRadiusMaxNum2Object();											// 检测筛选半径和目标上限对于再次筛选目标的有效性
	bool CheckMagicOpRelationAvail(MOP_MATCH_CHECKER pFunChecker);				// 检测魔法操作关系约束的有效性
	bool CheckFilterObj2MOPType();												// 临时操作的目标再次筛选类型只能为自身
	bool CheckNpcSkillFilterObj2MagicEff(const string sRuleName, string sSkillName, ESRTargetType eSRTargetType);
	EValueMagicOpType GetOpValueMagicOpType();

	static bool CheckFilterObj2CentralMS(CCfgMagicOp* pMagicOp);	// 检测叠加类型为集中的魔法状态关系约束的有效性
	static bool CheckCascade2CancelableMagicEff(CCfgMagicOp* pMagicOp);	// 检测 (可撤销效果)里的安装状态必须为唯一
	static bool CheckMOPType2CancelableMagicEff(CCfgMagicOp* pMagicOp);	// 检测 (触发器和伤害变更状态可撤销效果)里的非条件操作必须为功能型
	static bool CheckFilterObj2CancelableMagicEff(CCfgMagicOp* pMagicOp);
	static bool CheckFilterObj2CalcNeedTargetMOPInMS(CCfgMagicOp* pMagicOp);
	static bool CheckNoCond(CCfgMagicOp* pMagicOp);
	static bool CheckHasNoCond(CCfgMagicOp* pMagicOp);
	static bool CheckHasNoValueChangedOp(CCfgMagicOp* pMagicOp);
	static bool CheckCanExecuteWithTargetIsDead(CCfgMagicOp* pMagicOp);
	static bool CheckTempMopForState(CCfgMagicOp* pMagicOp);		// 检查临时操作
	static bool CheckForbidCancellableValueChangedMop(CCfgMagicOp* pMagicOp);
	static bool CheckNonPersistentObj(CCfgMagicOp* pMagicOp);
	static bool CheckPingPongObjPersistent(CCfgMagicOp* pMagicOp);
	static bool CheckNoSingleFilterSelf2Eff(CCfgMagicOp* pMagicOp);	// 检测可撤销魔法操作的筛选有效性
	static bool CheckNoSingleFilterObj2Eff(CCfgMagicOp* pMagicOp);	// 检测叠加类型为集中的魔法状态关系约束的有效性
	static bool CheckNoSingleAndSelfFilterObj2Eff(CCfgMagicOp* pMagicOp);	// 检测叠加类型为集中的魔法状态关系约束的有效性
	static bool CheckCancellableMopArgIsNotConst(CCfgMagicOp* pMagicOp);	// 检测可撤销魔法操作的参数不能为可变值
	static bool CheckCancelableMopSetupAureMagicNotSave(CCfgMagicOp* pMagicOp);	// 检测可撤销魔法操作的里安装姿态必须是强制不存的
	static bool CheckCancelableMopSetupStateNotSave(CCfgMagicOp* pMagicOp);		// 检测可撤销魔法操作的里安装的状态必须是强制不存的
	static bool CheckCancelEffOfState(CCfgMagicOp* pMagicOp);					// 检测特定可撤销魔法效果是否是状态的可撤销效果
	static bool CheckPosMagicSetupStateType(CCfgMagicOp* pMagicOp);				// 检测位置魔法的触发效果如安装状态类型是否正确
	static bool CheckMutexPosMagicSetupStateType(CCfgMagicOp* pMagicOp);		// 检测互斥位置魔法的触发效果如安装状态类型是否正确

	string GetMOPType() const {return m_strMOPType;}
	CBaseMagicOp* GetBaseMagicOp() const {return m_pMagicOp;}
	int32 GetID() const {return m_nID;}

private:
	typedef	vector<MagicEffFilter*>	VecMagicEffFilter;
	string						m_strMagicOpName;
	string						m_strMOPType;
	VecMagicEffFilter			m_vecFilterPipe;	// 筛选管道
	CCfgCalc*					m_MagicOpArg;
	CBaseMagicOp*				m_pMagicOp;
	int32						m_nID;
	bool						m_bIsCondition;
};
