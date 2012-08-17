#pragma once
#include "CFunctionMagicOpServer.h"
#include "PatternCOR.h"
#include "CDistortedTick.h"
#include "MagicOps_ChangeValue.h"
#include "CCfgArg.h"

class CMagicEffServer;
class CBulletMagicCfgServerSharedPtr;

class CCreateNPCMOP
	:public CFunctionMagicOpServer
{
public:
	bool CanExecuteWithAttackerIsDead()const
	{
		return true;
	}
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

class CDestoryNPCMOP
	:public CFunctionMagicOpServer
{
public:
	bool CanExecuteWithAttackerIsDead()const
	{
		return true;
	}
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

class CCreatePassiveNPCMOP
	:public CFunctionMagicOpServer
{
public:
	bool CanExecuteWithAttackerIsDead()const
	{
		return true;
	}
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};
class CCreateNPCOnTargetPosMOP
	:public CFunctionMagicOpServer
{
public:
	bool CanExecuteWithAttackerIsDead()const
	{
		return true;
	}
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};


//安装或清除触发器状态
class CSetupOrEraseTriggerStateMOP
	:public CFunctionCancelableMagicOp
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo);
	void Cancel(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo);
	CCfgArg* InitArg(const string& szArg);
};

//安装或清除魔法状态
class CSetupOrEraseMagicStateMOP
	:public CFunctionCancelableMagicOp
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo);
	void Cancel(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo);
	CCfgArg* InitArg(const string& szArg);
	virtual bool IsSetupNestedObj() const
	{
		return true;
	}
};

class CSetupMultiMagicStateMOP
	:public CFunctionCancelableMagicOp
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo);
	void Cancel(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo);
	CCfgArg* InitArg(const string& szArg);
	virtual bool IsSetupNestedObj() const
	{
		return true;
	}
};

//安装或清除姿态，同一姿态重复使用时，装一次清一次
class CSetupOrEraseAureMagicMOP
	:public CFunctionMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo);
	CCfgArg* InitArg(const string& szArg);
};

//安装姿态，可撤销，可多个，例如定时姿态
class CSetupAureMagicMOP
	:public CFunctionCancelableMagicOp
{
public:
	virtual bool CanExecuteWithAttackerIsDead()const
	{
		return false;
	}
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo);
	CCfgArg* InitArg(const string& szArg);
	void Cancel(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo);
	virtual bool IsSetupNestedObj() const
	{
		return true;
	}
};

//切换姿态，首先清除其他姿态，再安装，互斥
class CChangeAureMagicMOP
	:public CFunctionMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo);
	CCfgArg* InitArg(const string& szArg);
};

//清除姿态
class CEraseAureMagicMOP
	:public CFunctionMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo);
	CCfgArg* InitArg(const string& szArg);
};

class CEraseAureMagicOnCancellingMOP
	:public CFunctionCancelableMagicOp
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo);
	void Cancel(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo);
	CCfgArg* InitArg(const string& szArg);
};

//无视免疫安装魔法状态
class CIgnoreImmuneSetupSpecialStateMOP
	: public CFunctionCancelableMagicOp
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo);
	virtual void Cancel(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo);
	CCfgArg* InitArg(const string& szArg);
	virtual bool CanExecuteWithAttackerIsDead() const { return true; }
	virtual bool IsSetupNestedObj() const { return true; }	
};

//被动安装魔法状态
class CPassiveSetupMagicStateMOP
	: public CFunctionCancelableMagicOp
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo);
	virtual void Cancel(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo);
	CCfgArg* InitArg(const string& szArg);
	virtual bool CanExecuteWithAttackerIsDead() const { return true; }
	virtual bool IsSetupNestedObj() const { return true; }	
};

//安装魔法状态
class CSetupMagicStateMOP
	:public CPassiveSetupMagicStateMOP
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo);
	CCfgArg* InitArg(const string& szArg);
};

//安装触发器状态
class CSetupTriggerStateMOP
	:public CFunctionCancelableMagicOp
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo);
	void Cancel(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo);
	CCfgArg* InitArg(const string& szArg);
	virtual bool IsSetupNestedObj() const
	{
		return true;
	}
};

//安装伤害变更状态
class CSetupDamageChangeStateMOP
	:public CFunctionCancelableMagicOp
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo);
	void Cancel(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo);
	CCfgArg* InitArg(const string& szArg);
	virtual bool IsSetupNestedObj() const
	{
		return true;
	}
};

//安装积累触发状态
class CSetupCumuliTriggerStateMOP
	:public CFunctionCancelableMagicOp
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo);
	void Cancel(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo);
	CCfgArg* InitArg(const string& szArg);
	virtual bool IsSetupNestedObj() const
	{
		return true;
	}
};

//安装特殊状态
class CSetupSpecialStateMOP
	:public CFunctionCancelableMagicOp
{
public:
	virtual bool CanExecuteWithTargetIsDead()const
	{
		return true;
	}
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo);
	void Cancel(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo);
	CCfgArg* InitArg(const string& szArg);
};

//只安装不撤销魔法状态
class CSetupUncancellableMagicStateMOP
	:public CSetupMagicStateMOP
{
public:
	void Cancel(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo) {}
	bool Cancelable()const
	{
		return false;
	}
};

//按最高技能等级安装魔法状态
class CSetupMagicStateByMaxSkillLevelMOP
	:public CFunctionCancelableMagicOp
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo);
	void Cancel(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo);
	CCfgArg* InitArg(const string& szArg);
	virtual bool CanExecuteWithAttackerIsDead() const { return true; }
	virtual bool IsSetupNestedObj() const { return true; }	
};

//撤销魔法状态叠加层数
class CCancelMagicStateCascadeMOP
	:public CFunctionMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo);
	CCfgArg* InitArg(const string& szArg);
};

//按效果撤销魔法状态叠加层数
class CCancelMagicStateCascadeByEffectMOP
	:public CFunctionMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo);
	CCfgArg* InitArg(const string& szArg);
};




//触发触发器状态
class CTriggerStateMessageEventMOP
	:public CFunctionMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

class CResetMagicStateTimeMOP
	:public CFunctionMagicOpServer
{
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo);
	CCfgArg* InitArg(const string& szArg);
};

class CAddMagicStateTimeMOP
	:public CFunctionMagicOpServer
{
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo);
	CCfgArg* InitArg(const string& szArg);
};


class CSetDamageChangeStateAccumulateMOP
	:public CFunctionMagicOpServer
{
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

class CSetCumuliTriggerStateAccumulateMOP
	:public CFunctionMagicOpServer
{
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};


//释放位移魔法操作
class CRelMoveMagicMOP
	:public CFunctionMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo);
	CCfgArg* InitArg(const string& szArg);
};

//释放可撤销位移魔法操作
class CCancelableRelMoveMagicMOP
	:public CFunctionCancelableMagicOp
{
public:
	virtual bool CanExecuteWithAttackerIsDead()const
	{
		return false;
	}
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo);
	void Cancel(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo);
	CCfgArg* InitArg(const string& szArg);
};

//释放可传递魔法操作
class CRelTransferMagicMOP
	:public CFunctionMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo);
	CCfgArg* InitArg(const string& szArg);
};

//释放子弹魔法
class CRelBulletMagicMOP
	:public CFunctionMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo);
	CCfgArg* InitArg(const string& szArg);
};

//延迟施放子弹魔法
class CDelayRelBulletMagicMOP
	:public CFunctionMagicOpServer
{
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo);
	CCfgArg* InitArg(const string& szArg);
};


class CEraseMoveMagicMOP
	:public CFunctionMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

class CErasePositionMagicMOP
	:public CFunctionMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

class CEraseAllPositionMagicMOP
	:public CFunctionMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

class CReplacePositionMagicMOP
	:public CFunctionMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo);
	CCfgArg* InitArg(const string& szArg);
};

// 清除所有负面状态
class CEraseAllDecreaseStateMOP
	:public CFunctionMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

// 清除负面状态
class CEraseMoveDecreaseStateMOP
	:public CFunctionMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};
class CEraseSpeedDecreaseStateMOP
	:public CFunctionMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};
// 清除正面状态
class CEraseIncreaseStateMOP
	:public CFunctionMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

// 随机清除所有负面状态
class CRandEraseAllDecreaseStateMOP
	:public CFunctionMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

// 随机清除正面状态
class CRandEraseIncreaseStateMOP
	:public CFunctionMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

// 清除坐骑状态
class CEraseRidingStateMOP
	:public CFunctionMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

// 清除所有状态
class CEraseAllBufferMOP
	:public CFunctionMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

class CEraseStateMOP
	:public CFunctionMagicOpServer
{
public:
	bool CanExecuteWithAttackerIsDead()const
	{
		return true;
	}
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo);
	CCfgArg* InitArg(const string& szArg);
};

class CEraseSelfStateMOP
	:public CFunctionMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo);
	CCfgArg* InitArg(const string& szArg);
};

class CEraseStateOnCancellingMOP
	:public CFunctionCancelableMagicOp
{
public:
	bool CanExecuteWithAttackerIsDead()const
	{
		return true;
	}
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
	void Cancel(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo);
	CCfgArg* InitArg(const string& szArg);
};



// 清除某种负面类型的状态
class CEraseStateByDecreaseTypeMOP
	:public CFunctionMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo);
	CCfgArg* InitArg(const string& szArg);
};

// 反射某种负面类型状态
class CReflectStateByDecreaseTypeMOP
	:public CFunctionMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

// 抵抗控制
class CResistControlMOP
	:public CFunctionCancelableMagicOp
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
	void Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

// 抵抗定身
class CResistPauseMOP
	:public CFunctionCancelableMagicOp
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
	void Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

// 抵抗减速
class CResistCripplingMOP
	:public CFunctionCancelableMagicOp
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
	void Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

// 抵抗DOT
class CResistDOTMOP
	:public CFunctionCancelableMagicOp
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
	void Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

// 重置冷却单一技能时间为零--多
class CResetSingleCoolDownTimeMOP
	:public CFunctionMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

// 所有技能冷却结束--矮人骑士
class CAllSkillReadyMOP
	:public CFunctionMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

class CAllFightSkillReadyMOP
	:public CFunctionMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};
// ------------------------------------------------------------------------------------------------------------
// CLEAN UP START
// ------------------------------------------------------------------------------------------------------------

// 播放全身动作
class CDoAllBodyActionMOP
	:public CFunctionCancelableMagicOp
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
	void Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

// 魔法状态触发战斗状态转换,不需要注册
class CMagicStateTouchBattleStateMOP
	:public CFunctionMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

// 目标脱离战斗状态
class CTargetLeftBattleStateMOP
	:public CFunctionMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

// 目标禁止移动
class CTargetForbitMoveMOP
	:public CFunctionCancelableMagicOp
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
	void Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

// 目标禁止移动
class CForTestTargetCancelForbitMoveMOP
	:public CFunctionMagicOpServer
{
public:
		uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

// 目标禁止转向
class CTargetForbitTurnAroundMOP
	:public CFunctionCancelableMagicOp
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
	void Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

// 目标撤销普攻
class CTargetCancelNAMOP
	:public CFunctionMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

// 目标暂停普攻
class CTargetSuspendNAMOP
	:public CFunctionCancelableMagicOp
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
	void Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

// 目标禁止普通攻击
class CTargetForbitNormalAttackMOP
	:public CFunctionCancelableMagicOp
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
	void Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

// 目标禁止施放技能
class CTargetForbitUseSkillMOP
	:public CFunctionCancelableMagicOp
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
	void Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

// 目标禁止施放自然系法术
class CTargetForbitUseNatureSkillMOP
	:public CFunctionCancelableMagicOp
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
	void Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};
// 目标禁止施放破坏系法术
class CTargetForbitUseDestructionSkillMOP
	:public CFunctionCancelableMagicOp
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
	void Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};
// 目标禁止施放暗黑系法术
class CTargetForbitUseEvilSkillMOP
	:public CFunctionCancelableMagicOp
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
	void Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};
// 目标禁止客户端操作
class CTargetForbitClientOperateMOP
	:public CFunctionCancelableMagicOp
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
	void Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

class CTargetForbitUseClassSkillMOP
	:public CFunctionCancelableMagicOp
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
	void Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

class CTargetForbitUseDrugItemSkillMOP
	:public CFunctionCancelableMagicOp
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
	void Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

class CTargetForbitUseNotFightSkillMOP
	:public CFunctionCancelableMagicOp
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
	void Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

class CTargetForbitUseMissionItemSkillMOP
	:public CFunctionCancelableMagicOp
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
	void Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

// 假死
class CFeignDeathMOP
	:public CFunctionCancelableMagicOp
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
	void Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

// 嘲讽
class CTauntMOP
	:public CFunctionMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

// 重生复活
class CRenascenceMOP
	:public CFunctionMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

// 原地重生
class CRebornWhereYouBeMOP
	:public CFunctionMagicOpServer
{
public:
	bool CanExecuteWithAttackerIsDead()const
	{
		return true;
	}
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

class CPermitRebornMOP
	:public CFunctionMagicOpServer
{
public:
	bool CanExecuteWithAttackerIsDead()const
	{
		return true;
	}
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

// 持续回蓝
class CContinueAddMPMOP
	:public CFunctionCancelableMagicOp
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
	void Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

// 停止运动
class CStopMoveMOP
	:public CFunctionCancelableMagicOp
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
	void Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

// 暴气
class CBreakOutMOP
	:public CFunctionCancelableMagicOp
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
	void Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

// 灵魂链接
class CSoulLinkMOP
	:public CFunctionCancelableMagicOp
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
	void Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

// 施放被动技能
class CRelPassiveSkillMOP
	:public CFunctionMagicOpServer
{
public:
	bool CanExecuteWithAttackerIsDead()const
	{
		return true;
	}
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

class CRelNPCSkillMOP
	:public CFunctionMagicOpServer
{
public:
	bool CanExecuteWithAttackerIsDead()const
	{
		return true;
	}
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

class CRelNPCDirSkillMOP
	:public CFunctionMagicOpServer
{
public:
	bool CanExecuteWithAttackerIsDead()const
	{
		return true;
	}
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

// 缴械
class CForbitUseWeaponMOP : public CFunctionCancelableMagicOp
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
	void Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

// 普攻产生怒气
class CNormalAttackProduceRagePoint : public CFunctionMagicOpServer
{
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

// 被伤害产生怒气
class CBeDamagePreduceRagePoint : public CFunctionMagicOpServer
{
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

class CBurstSoulPrizeToExpOwnerMOP
	:public CFunctionMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);

	virtual bool CanExecuteWithAttackerIsDead()const
	{
		return true;
	}
};

class CTargetSetupMagicStateToSelfMOP
	:public CSetupMagicStateMOP
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo);
	void Cancel(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

class CCBurstSoulExtraPrizeToExpOwnerMOP
	:public CFunctionMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);

	virtual bool CanExecuteWithAttackerIsDead()const
	{
		return true;
	}
};

//目标对经验拥有者奖励魂并带附加效果
class CAddBurstSoulTimesToExpOwnerMOP
	:public CFunctionMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);

	virtual bool CanExecuteWithAttackerIsDead()const
	{
		return true;
	}
};

class CTakeOverTRuckMop
	:public CFunctionMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);

	virtual bool CanExecuteWithAttackerIsDead()const
	{
		return true;
	}
};

class CTargetSetHiddenMOP
	:public CFunctionCancelableMagicOp
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
	void Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

class CMoveSelfToTargetPosMop
	:public CFunctionMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

// ------------------------------------------------------------------------------------------------------------
// CLEAN UP END
// ------------------------------------------------------------------------------------------------------------

// 下一技能无吟唱时间
class CSetNoSingTimeAboutNextSkill
	:public CFunctionCancelableMagicOp
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
	void Cancel(CSkillInstServer* pSkillInst,const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

// 技能无吟唱时间（可撤销）
class CSetNoSingTimeForever
	:public CFunctionCancelableMagicOp
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
	void Cancel(CSkillInstServer* pSkillInst,const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

class CLockAgileValueMOP
	:public CFunctionCancelableMagicOp
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
	void Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

class CChangeTargetToBarrierMOP
	:public CFunctionCancelableMagicOp
{
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
	void Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

class CNonCombatBehaviorMOP
	:public CFunctionMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

class CChangeSceneMOP
	:public CFunctionMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

//召唤兽 随机地点
class CCreateServantMOP
	:public CFunctionMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

class CCreateServantWithTargetLevelFollowMasterMOP
	: public CFunctionMagicOpServer
{
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

class CRemoveServantEnemyMOP
	:public CFunctionMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

class CSetMonsterCardSkillLevelByMasterSkillLevelMOP
	: public CFunctionMagicOpServer
{
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

class CControlServantMOP
	:public CFunctionMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

class CSetServantLeftTimeMOP
	:public CFunctionMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

class CDestroyServantMOP
	:public CFunctionMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

class CDestroyServantOnCancellingMOP
	:public CFunctionCancelableMagicOp
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
	void Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

class CRelTickMOP
	:public CFunctionMagicOpServer
{
public:
	bool CanExecuteWithAttackerIsDead()const
	{
		return true;
	}
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo);
	CCfgArg* InitArg(const string& szArg);
};

// 使用普通坐骑
class CUseNormalHorseMOP
	:public CFunctionCancelableMagicOp
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
	void ChangeRidingHorse(CFighterDictator* pFighter, const CCfgCalc& Arg);
	void Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

// 使用战斗坐骑
class CUseBattleHorseMOP
	:public CFunctionCancelableMagicOp
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
	void Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

// 召唤战斗坐骑
class CCallBattleHorseMOP
	:public CFunctionCancelableMagicOp
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
	void Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

// 收回骑乘坐骑
class CTakeBackRidingHorseMOP
	:public CFunctionMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

class CSetServantTargetMOP
	:public CFunctionMagicOpServer
{
public:
	bool CanExecuteWithAttackerIsDead()const
	{
		return true;
	}
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

class CLetServantAttackTargetMOP
	:public CFunctionMagicOpServer
{
public:
	bool CanExecuteWithAttackerIsDead()const
	{
		return true;
	}
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

class CRemoveEnemyFromServantEnmityListMOP
	:public CFunctionMagicOpServer
{
public:	
	bool CanExecuteWithAttackerIsDead()const
	{
		return true;
	}
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

class CSetServantAttackMOP
	:public CFunctionMagicOpServer
{
public:
	bool CanExecuteWithAttackerIsDead()const
	{
		return true;
	}
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};


//执行手部动作
class CHandActionMOP
	:public CFunctionMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

//执行脚部动作
class CFootActionMOP
	:public CFunctionMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

//奖励魂
class CBurstSoulPrizeMOP
	:public CFunctionMagicOpServer
{
public:
	bool CanExecuteWithAttackerIsDead()const
	{
		return true;
	}
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

class CTargetRelBulletMagicMOP
	:public CFunctionMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

class CBurstSoulFXMOP
	: public CFunctionMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo);
	CCfgArg* InitArg(const string& szArg);
};

class CBurstSoulBallFXMOP
	: public CFunctionMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo);
	CCfgArg* InitArg(const string& szArg);
};

class CTargetBurstSoulFXMOP
	: public CFunctionMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo);
	CCfgArg* InitArg(const string& szArg);
};

class CAddBurstSoulTimesMOP
	: public CFunctionMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

class CConsumeBurstSoulTimesMOP
	: public CFunctionMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

class CAddContinuousBurstTimesMOP
	: public CFunctionCancelableMagicOp
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
	void Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

// 归阵
class CReturnBattleArrayMOP
	:public CFunctionMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

// 改变阵营
class CChangeCampMOP
	:public CFunctionCancelableMagicOp
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
	void Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

class COnlyPlayFX
	:public CFunctionCancelableMagicOp
{
	bool CanExecuteWithAttackerIsDead()const
	{
		return true;
	}
	bool CanExecuteWithTargetIsDead()const
	{
		return false;
	}
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo);
	void Cancel(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo);
	CCfgArg* InitArg(const string& szArg);
};

//class CUseMagaicEff
//	:public CFunctionCancelableMagicOp
//{
//	bool CanExecuteWithAttackerIsDead()const
//	{
//		return true;
//	}
//	bool CanExecuteWithTargetIsDead()const
//	{
//		return true;
//	}
//	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
//	void Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
//};
//
//class CCancelMagicEff
//	:public CFunctionCancelableMagicOp
//{
//	bool CanExecuteWithAttackerIsDead()const
//	{
//		return true;
//	}
//	bool CanExecuteWithTargetIsDead()const
//	{
//		return true;
//	}
//	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
//	void Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
//};

// 改变普攻动作速率
class CChangeNAActionRhythmMOP
	:public CFunctionCancelableMagicOp
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
	void Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

// 天地同寿--牧师
class CSelfMurderBlastMOP
	:public CFunctionMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

class CDisTriggerEventMOP
	:public CFunctionCancelableMagicOp
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
	void Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

class CSetSameTargetMOP
	:public CFunctionMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

class CSetTargetSelfMOP
	:public CFunctionMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

class CSetTargetNULLMOP
	:public CFunctionMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

class CReplaceSkillMOP
	:public CFunctionCancelableMagicOp
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
	void Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

template<typename PropertyType>
class TImmuneMOP
	:public CFunctionCancelableMagicOp
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
	void Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

#define DefineImmuneMOP(Name) \
	typedef TImmuneMOP<CChange##Name##ImmuneRateMultiplierMOP> CImmune##Name##MOP;

DefineImmuneMOP(PunctureDamage);
DefineImmuneMOP(ChopDamage);
DefineImmuneMOP(BluntDamage);
DefineImmuneMOP(NatureDamage);
DefineImmuneMOP(DestructionDamage);
DefineImmuneMOP(EvilDamage);
DefineImmuneMOP(ControlDecrease);
DefineImmuneMOP(PauseDecrease);
DefineImmuneMOP(CripplingDecrease);
DefineImmuneMOP(DebuffDecrease);
DefineImmuneMOP(DOTDecrease);
DefineImmuneMOP(SpecialDecrease);
DefineImmuneMOP(Cure);
DefineImmuneMOP(MoveMagic);
DefineImmuneMOP(NonTypePhysicsDamage);
DefineImmuneMOP(NonTypeDamage);
DefineImmuneMOP(NonTypeCure);
DefineImmuneMOP(InterruptCastingProcess);

#undef DefineImmuneMOP

class CImmuneTaskTypeDamageMOP
	:public CFunctionCancelableMagicOp
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
	void Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

class CClosePrintInfoMOP
	:public CFunctionCancelableMagicOp
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
	void Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

class  CSwapPositionMOP
	:public CFunctionMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

class CInterruptCastingProcessMagicMOP
	:public CFunctionMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

class CFillHPMPMOP
	:public CFunctionMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

class CTalentChangeManaPointMOP
	:public CFunctionMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo);
	CCfgArg* InitArg(const string& szArg);
};

class CRelMoveMagicAtShockwaveMagicPosMOP
	:public CFunctionMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo);
	CCfgArg* InitArg(const string& szArg);
};

class CRelPositionMagicAtTargetPosMOP
	:public CFunctionMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo);
	CCfgArg* InitArg(const string& szArg);
};

class CRelShockWaveMagicAtTargetPosMOP
	:public CFunctionMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo);
	CCfgArg* InitArg(const string& szArg);
};

class CNonCombatStateMagicOp
	:public CFunctionCancelableMagicOp
{
public:
	uint32 Do(CSkillInstServer* pSkillInst,const CCfgArg* pArg,CFighterDictator* pFrom,CFighterDictator* pTo);
	void Cancel(CSkillInstServer* pSkillInst,const CCfgArg* pArg,CFighterDictator* pFrom,CFighterDictator* pTo);
	CCfgArg* InitArg(const string& szArg);
};

class CBecomeTargetShadowMOP
	:public CFunctionMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};
class CForceSetDOTDamageMOP
	:public CFunctionMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

class CBindTargetObjToSelfMOP
	:public CFunctionMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

class CTestTargetMoveMagicMOP
	:public CFunctionMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};

class CDisplayMsgMOP
	:public CFunctionMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo);
	CCfgArg* InitArg(const string& szArg);
};

class CDebaseEnmityMOP
	:public CFunctionMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
};


class CDoWorldSkillMOP
	:public CFunctionMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo);
	CCfgArg* InitArg(const string& szArg);
};


class CDoCampSkillMOP
	:public CFunctionMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo);
	CCfgArg* InitArg(const string& szArg);
};


class CDoArmyCorpsSkillMOP
	:public CFunctionMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo);
	CCfgArg* InitArg(const string& szArg);
};

class CDoTongSkillMOP
	:public CFunctionMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo);
	CCfgArg* InitArg(const string& szArg);
};
