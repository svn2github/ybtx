#include "stdafx.h"
#include "CTriggerEvent.h"

uint32 CTriggerEvent::GetID(EHurtResult eArg1, bool bArg2, ESkillType eArg3, EAttackType eArg4)
{
	return eSET_End * 2
		+ eArg1 * 2 * eST_End * eATT_End
		+ bArg2 * eST_End * eATT_End
		+ eArg3 * eATT_End
		+ eArg4;
}

uint32 CTriggerEvent::GetID(ESpecialEventType eArg1, bool bArg2)
{
	return eArg1 + bArg2 * eSET_End;
}

CTriggerEvent::MapHurtResult		CTriggerEvent::m_smapEHurtResult;
CTriggerEvent::MapSkillType			CTriggerEvent::m_smapESkillType;
CTriggerEvent::MapAttackType		CTriggerEvent::m_smapAttackType;
CTriggerEvent::MapSpecialEventType	CTriggerEvent::m_smapESpecialEventType;
bool CTriggerEvent::__init = CTriggerEvent::BuildMap();

bool CTriggerEvent::BuildMap()
{
	//伤害结果类型
	AddEvent("失败", eHR_Fail);
	AddEvent("成功", eHR_Success);
	AddEvent("未命中", eHR_Miss);
	AddEvent("物理闪避", eHR_PhyDodge);
	AddEvent("魔法闪避", eHR_MagDodge);
	AddEvent("招架", eHR_Parry);
	AddEvent("格挡", eHR_Block);
	AddEvent("完全抵抗", eHR_ComResist);
	AddEvent("抵抗", eHR_Resist);
	AddEvent("暴击", eHR_Strike);
	AddEvent("命中", eHR_Hit);
	AddEvent("伤害", eHR_Hurt);
	AddEvent("非DOT伤害", eHR_HurtExptDOT);
	AddEvent("攻击", eSET_Attack);
	AddEvent("免疫", eHR_Immune);
	AddEvent("吸收前伤害", eSET_BeforeChangeHurt);

	//技能类型
	AddEvent("", eST_None);
	AddEvent("物理", eST_Physical);
	AddEvent("魔法", eST_Magic);

	//攻击类型
	AddEvent("", eATT_None);
	AddEvent("穿刺", eATT_Puncture);
	AddEvent("砍斫", eATT_Chop);
	AddEvent("钝击", eATT_BluntTrauma);
	AddEvent("自然", eATT_Nature);
	AddEvent("破坏", eATT_Destroy);
	AddEvent("暗黑", eATT_Evil);

	//特殊类型
	AddEvent("减蓝", eSET_SubMP);
	AddEvent("减能量", eSET_SubEP);
	AddEvent("减怒气", eSET_SubRP);
	AddEvent("减连击", eSET_SubCP);
	AddEvent("治疗", eSET_Heal);
	AddEvent("吸血", eSET_SuckBlood);
	AddEvent("吸收前治疗", eSET_BeforeChangeHeal);
	AddEvent("施放技能", eSET_DoSkill);
	AddEvent("施放战斗技能", eSET_DoFightSkill);
	AddEvent("施放自然系法术", eSET_DoNatureMagic);
	AddEvent("施放破坏系法术", eSET_DoDestructionMagic);
	AddEvent("施放暗黑系法术", eSET_DoEvilMagic);
	AddEvent("生命值改变", eSET_HPChanged);
	AddEvent("近程普通攻击", eSET_ShortNormalAttack);
	AddEvent("近程普通攻击伤害", eSET_ShortNormalAttackDamage);
	AddEvent("普通攻击暴击", eSET_NormalAttackStrike);
	AddEvent("近程普通攻击单体伤害", eSET_ShortNormalAttackSingletonDamage);
	AddEvent("远程普通攻击伤害", eSET_LongNormalAttackDamage);
	AddEvent("主手普通攻击伤害", eSET_MainHandNADamage);
	AddEvent("安装者死亡", eSET_InstallerDie);
	AddEvent("杀死前", eSET_Kill);
	AddEvent("杀死后", eSET_Killed);
	AddEvent("杀死NPC", eSET_KillNPC);
	AddEvent("杀死Player", eSET_KillPlayer);
	AddEvent("被安装者杀死", eSET_KillByInstaller);
	AddEvent("离开战斗", eSET_LeftBattle);
	AddEvent("进入战斗", eSET_EnterBattle);
	AddEvent("指定的吟唱无时间用完", eSET_CancelSpecifiedNoSingTime);
	AddEvent("任何吟唱无时间用完", eSET_CancelAnyNoSingTime);
	AddEvent("满怒气", eSET_RagePointIsFull);
	AddEvent("致死伤害", eSET_DeadlyHurt);
	AddEvent("消失前", eSET_Clear);
	AddEvent("与主人解除关系", eSET_RemoveFromMaster);
	AddEvent("单攻技能攻击", eSET_AttackByEnemyObjectSkill);
	AddEvent("开始移动", eSET_MoveBegin);
	AddEvent("停止移动", eSET_MoveEnd);
	AddEvent("武器模型改变", eSET_WeaponModChange);
	AddEvent("装备武器", eSET_SetupWeapon);
	AddEvent("撤销武器", eSET_RemoveWeapon);
	AddEvent("更换装备", eSET_ChangeEquip);
	AddEvent("安装控制状态", eSET_SetupControlState);
	AddEvent("安装定身状态", eSET_SetupPauseState);
	AddEvent("安装减速状态", eSET_SetupCripplingState);
	AddEvent("安装特殊状态", eSET_SetupSpecialState);
	AddEvent("打断技命中", eSET_InterruptMopHit);

	return true;
}

