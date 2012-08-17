#include "stdafx.h"
#include "RegistShellCommon.h"
#include "GameDef.h"
#include "FightDef.h"
#include "ActorDef.h"
#include "ScriptX.h"
#include "CTalent.h"
#include "TSqrAllocator.inl"

void RegistShellCommon(CScript& Script)
{
	//========================== GameDef ==========================
	// 注册角色性别
	REGIST_ENUMTYPE( Script, ECharSex );
	REGIST_ENUMERATION( Script, ECharSex, eCS_NoSex );
	REGIST_ENUMERATION( Script, ECharSex, eCS_Male );
	REGIST_ENUMERATION( Script, ECharSex, eCS_Female );

	// 注册职业
	REGIST_ENUMTYPE( Script, EClass );
	REGIST_ENUMERATION( Script, EClass, eCL_NoneClass );
	REGIST_ENUMERATION( Script, EClass, eCL_Warrior );
	REGIST_ENUMERATION( Script, EClass, eCL_MagicWarrior );
	REGIST_ENUMERATION( Script, EClass, eCL_Paladin );
	REGIST_ENUMERATION( Script, EClass, eCL_NatureElf );
	REGIST_ENUMERATION( Script, EClass, eCL_EvilElf );
	REGIST_ENUMERATION( Script, EClass, eCL_ElfHunter );
	REGIST_ENUMERATION( Script, EClass, eCL_Priest );
	REGIST_ENUMERATION( Script, EClass, eCL_DwarfPaladin );
	REGIST_ENUMERATION( Script, EClass, eCL_OrcWarrior );
	REGIST_ENUMERATION( Script, EClass, eCL_Npc);
	REGIST_ENUMERATION( Script, EClass, eCL_Wizard );
	REGIST_ENUMERATION( Script, EClass, eCL_Assassin );
	REGIST_ENUMERATION( Script, EClass, eCL_Warlock );
	REGIST_ENUMERATION( Script, EClass, eCL_Archer );
	REGIST_ENUMERATION( Script, EClass, eCL_Soldier );
	REGIST_ENUMERATION( Script, EClass, eCL_Summoner);
	REGIST_ENUMERATION( Script, EClass, eCL_Beast );
	REGIST_ENUMERATION( Script, EClass, eCL_Summon );
	REGIST_ENUMERATION( Script, EClass, eCL_Lancer );
	REGIST_ENUMERATION( Script, EClass, eCL_Knight );
	REGIST_ENUMERATION( Script, EClass, eCL_NatureElement );
	REGIST_ENUMERATION( Script, EClass, eCL_EvilElement);
	REGIST_ENUMERATION( Script, EClass, eCL_DestructionElement );
	REGIST_ENUMERATION( Script, EClass, eCl_Building);
	REGIST_ENUMERATION( Script, EClass, eCL_Special);
	REGIST_ENUMERATION( Script, EClass, eCL_Count );


	// 注册势力
	REGIST_ENUMTYPE( Script, ECamp );
	REGIST_ENUMERATION( Script, ECamp, eCamp_None );
	REGIST_ENUMERATION( Script, ECamp, eCamp_AmyEmpire);
	REGIST_ENUMERATION( Script, ECamp, eCamp_WestEmpire);
	REGIST_ENUMERATION( Script, ECamp, eCamp_DevilIsland);
	REGIST_ENUMERATION( Script, ECamp, eCamp_Passerby);
	REGIST_ENUMERATION( Script, ECamp, eCamp_Monster);
	REGIST_ENUMERATION( Script, ECamp, eCamp_Chaos);
	REGIST_ENUMERATION( Script, ECamp, eCamp_NoQuestPasserby);
	REGIST_ENUMERATION( Script, ECamp, eCamp_Specail);
	REGIST_ENUMERATION( Script, ECamp, eCamp_Count);

	// 注册小地图类型
	REGIST_ENUMTYPE( Script, EMapRole );
	REGIST_ENUMERATION( Script, EMapRole, eMR_Npc );
	REGIST_ENUMERATION( Script, EMapRole, eMR_PlayerPKSafe );
	REGIST_ENUMERATION( Script, EMapRole, eMR_PlayerPKMatch );
	REGIST_ENUMERATION( Script, EMapRole, eMR_PlayerPKForce );
	REGIST_ENUMERATION( Script, EMapRole, eMR_TeamMate );
	REGIST_ENUMERATION( Script, EMapRole, eMR_Player );

	REGIST_ENUMTYPE( Script, EGameEntityType );
	REGIST_ENUMERATION( Script, EGameEntityType, eGET_GameEntity );
	REGIST_ENUMERATION( Script, EGameEntityType, eGET_CharacterDirector );
	REGIST_ENUMERATION( Script, EGameEntityType, eGET_CharacterFollower );
	REGIST_ENUMERATION( Script, EGameEntityType, eGET_CharacterDictator );
	REGIST_ENUMERATION( Script, EGameEntityType, eGET_CharacterMediator );
	REGIST_ENUMERATION( Script, EGameEntityType, eGET_IntObject );

	//========================== FightDef ==========================

	// 易变属性类型
	REGIST_ENUMTYPE( Script, EAgileType ); 
	REGIST_ENUMERATION( Script, EAgileType, eAT_HP);
	REGIST_ENUMERATION( Script, EAgileType, eAT_MP );
	REGIST_ENUMERATION( Script, EAgileType, eAT_EP );
	REGIST_ENUMERATION( Script, EAgileType, eAT_EP );
	REGIST_ENUMERATION( Script, EAgileType, eAT_CP );

	// 攻击类型
	REGIST_ENUMTYPE( Script, EAttackType );
	REGIST_ENUMERATION( Script, EAttackType, eATT_None);
	REGIST_ENUMERATION( Script, EAttackType, eATT_Puncture);
	REGIST_ENUMERATION( Script, EAttackType, eATT_Chop);
	REGIST_ENUMERATION( Script, EAttackType, eATT_BluntTrauma);
	REGIST_ENUMERATION( Script, EAttackType, eATT_Nature);
	REGIST_ENUMERATION( Script, EAttackType, eATT_Destroy);
	REGIST_ENUMERATION( Script, EAttackType, eATT_Evil);
	REGIST_ENUMERATION( Script, EAttackType, eATT_TaskNonBattle);
	REGIST_ENUMERATION( Script, EAttackType, eATT_TaskBomb);
	REGIST_ENUMERATION( Script, EAttackType, eATT_TaskSpecial);

	//// 武器类型
	//REGIST_ENUMTYPE( Script, EWeaponType );
	//REGIST_ENUMERATION( Script, EWeaponType, eWT_SHSword);
	//REGIST_ENUMERATION( Script, EWeaponType, eWT_SHAxe);
	//REGIST_ENUMERATION( Script, EWeaponType, eWT_SHHammer);
	//REGIST_ENUMERATION( Script, EWeaponType, eWT_SHKnife);
	//REGIST_ENUMERATION( Script, EWeaponType, eWT_PaladinGun);
	//REGIST_ENUMERATION( Script, EWeaponType, eWT_SHWand);
	//REGIST_ENUMERATION( Script, EWeaponType, eWT_THSword);
	//REGIST_ENUMERATION( Script, EWeaponType, eWT_THAxe);
	//REGIST_ENUMERATION( Script, EWeaponType, eWT_THHammer);
	//REGIST_ENUMERATION( Script, EWeaponType, eWT_THWand);
	//REGIST_ENUMERATION( Script, EWeaponType, eWT_Lance);
	//REGIST_ENUMERATION( Script, EWeaponType, eWT_LongKnife);
	//REGIST_ENUMERATION( Script, EWeaponType, eWT_LongStick);
	//REGIST_ENUMERATION( Script, EWeaponType, eWT_Bow);	
	//REGIST_ENUMERATION( Script, EWeaponType, eWT_CrossBow);

	// 战斗控制状态
	REGIST_ENUMTYPE( Script, EFighterCtrlState );
	REGIST_ENUMERATION( Script, EFighterCtrlState, eFCS_InNormalHorse );
	REGIST_ENUMERATION( Script, EFighterCtrlState, eFCS_InBattleHorse );
	REGIST_ENUMERATION( Script, EFighterCtrlState, eFCS_FeignDeath );
	REGIST_ENUMERATION( Script, EFighterCtrlState, eFCS_InBattle );
	REGIST_ENUMERATION( Script, EFighterCtrlState, eFCS_OnMission );
	REGIST_ENUMERATION( Script, EFighterCtrlState, eFCS_InDuel );
	REGIST_ENUMERATION( Script, EFighterCtrlState, eFCS_InWalkState );
	REGIST_ENUMERATION( Script, EFighterCtrlState, eFCS_InDrawWeaponMode );
	REGIST_ENUMERATION( Script, EFighterCtrlState, eFCS_AllowChangeDirection );
	REGIST_ENUMERATION( Script, EFighterCtrlState, eFCS_ForbitAutoTrack );
	REGIST_ENUMERATION( Script, EFighterCtrlState, eFCS_ForbitNormalAttack );
	REGIST_ENUMERATION( Script, EFighterCtrlState, eFCS_ForbitMove );
	REGIST_ENUMERATION( Script, EFighterCtrlState, eFCS_ForbitTurnAround );
	REGIST_ENUMERATION( Script, EFighterCtrlState, eFCS_ForbitUseWeapon );

	//---------------------------- 技能 ----------------------------
	// 释放技能目标类型
	REGIST_ENUMTYPE( Script, EFSTargetType );
	REGIST_ENUMERATION( Script, EFSTargetType, eFSTT_Position );
	REGIST_ENUMERATION( Script, EFSTargetType, eFSTT_EnemyObject );
	REGIST_ENUMERATION( Script, EFSTargetType, eFSTT_FriendObject );
	REGIST_ENUMERATION( Script, EFSTargetType, eFSTT_Self );
	REGIST_ENUMERATION( Script, EFSTargetType, eFSTT_SelfPosition );
	REGIST_ENUMERATION( Script, EFSTargetType, eFSTT_SelfDirection );

	// 选择地点类型
	REGIST_ENUMTYPE( Script, ESelectGroundType );
	REGIST_ENUMERATION( Script, ESelectGroundType, eSGT_GroundSelector );
	REGIST_ENUMERATION( Script, ESelectGroundType, eSGT_MouseSelector );

	// 伤害类型
	REGIST_ENUMTYPE( Script, EHurtResult );
	REGIST_ENUMERATION( Script, EHurtResult, eHR_Fail );
	REGIST_ENUMERATION( Script, EHurtResult, eHR_Success );
	REGIST_ENUMERATION( Script, EHurtResult, eHR_Miss );
	REGIST_ENUMERATION( Script, EHurtResult, eHR_PhyDodge );
	REGIST_ENUMERATION( Script, EHurtResult, eHR_MagDodge );
	REGIST_ENUMERATION( Script, EHurtResult, eHR_Immune );
	REGIST_ENUMERATION( Script, EHurtResult, eHR_Parry );
	REGIST_ENUMERATION( Script, EHurtResult, eHR_Block );	
	REGIST_ENUMERATION( Script, EHurtResult, eHR_ComResist );
	REGIST_ENUMERATION( Script, EHurtResult, eHR_Resist );
	REGIST_ENUMERATION( Script, EHurtResult, eHR_Strike );	
	REGIST_ENUMERATION( Script, EHurtResult, eHR_Hit );
	REGIST_ENUMERATION( Script, EHurtResult, eHR_EmptyAni);

	// 消耗类型
	REGIST_ENUMTYPE( Script, EConsumeType ); 
	REGIST_ENUMERATION( Script, EConsumeType, eCT_None);
	REGIST_ENUMERATION( Script, EConsumeType, eCT_HP );
	REGIST_ENUMERATION( Script, EConsumeType, eCT_MP );
	REGIST_ENUMERATION( Script, EConsumeType, eCT_RG );
	REGIST_ENUMERATION( Script, EConsumeType, eCT_EG );
	REGIST_ENUMERATION( Script, EConsumeType, eCT_CP );

	// 流程类型
	REGIST_ENUMTYPE( Script, ECastingProcessType ); 
	REGIST_ENUMERATION( Script, ECastingProcessType, eCPT_Wink);
	REGIST_ENUMERATION( Script, ECastingProcessType, eCPT_Sing );
	REGIST_ENUMERATION( Script, ECastingProcessType, eCPT_Channel );
	REGIST_ENUMERATION( Script, ECastingProcessType, eCPT_GradeSing );
	REGIST_ENUMERATION( Script, ECastingProcessType, eCPT_ImmovableSing );

	// 姿态类型
	REGIST_ENUMTYPE( Script, EStanceType );
	REGIST_ENUMERATION( Script, EStanceType, eSS_Stance );
	REGIST_ENUMERATION( Script, EStanceType, eSS_Aure );
	REGIST_ENUMERATION( Script, EStanceType, eSS_Form );

	// 魔法状态总类别
	REGIST_ENUMTYPE( Script, EStateGlobalType );
	REGIST_ENUMERATION( Script, EStateGlobalType, eSGT_Undefined );
	REGIST_ENUMERATION( Script, EStateGlobalType, eSGT_MagicState );
	REGIST_ENUMERATION( Script, EStateGlobalType, eSGT_TriggerState );
	REGIST_ENUMERATION( Script, EStateGlobalType, eSGT_DamageChangeState );
	REGIST_ENUMERATION( Script, EStateGlobalType, eSGT_CumuliTriggerState );
	REGIST_ENUMERATION( Script, EStateGlobalType, eSGT_SpecialState );

	// 魔法总类别
	REGIST_ENUMTYPE( Script, EMagicGlobalType );
	REGIST_ENUMERATION( Script, EMagicGlobalType, eMGT_Bullet );
	REGIST_ENUMERATION( Script, EMagicGlobalType, eMGT_ShockWave );
	REGIST_ENUMERATION( Script, EMagicGlobalType, eMGT_Transferable );
	REGIST_ENUMERATION( Script, EMagicGlobalType, eMGT_Position );
	REGIST_ENUMERATION( Script, EMagicGlobalType, eMGT_Move );
	REGIST_ENUMERATION( Script, EMagicGlobalType, eMGT_Barrier);
	REGIST_ENUMERATION( Script, EMagicGlobalType, eMGT_BattleArray);

	// 子弹轨迹类型
	REGIST_ENUMTYPE( Script, EBulletTrackType );
	REGIST_ENUMERATION( Script, EBulletTrackType , eBTT_Line );
	REGIST_ENUMERATION( Script, EBulletTrackType , eBTT_Parabola );

	//技能施放结果返回值
	REGIST_ENUMTYPE( Script, EDoSkillResult );
	REGIST_ENUMERATION( Script, EDoSkillResult , eDSR_Success );
	REGIST_ENUMERATION( Script, EDoSkillResult , eDSR_ToTargetHaveBarrier );
	REGIST_ENUMERATION( Script, EDoSkillResult , eDSR_OutOfDistance );
	//---------------------------- 动作 ----------------------------
	// 角色行为状态
	REGIST_ENUMTYPE( Script, EActionState );
	REGIST_ENUMERATION( Script, EActionState, eAS_Die );
	REGIST_ENUMERATION( Script, EActionState, eAS_Dead );
	REGIST_ENUMERATION( Script, EActionState, eAS_Idle_BackWpn );
	REGIST_ENUMERATION( Script, EActionState, eAS_Walk_BackWpn );
	REGIST_ENUMERATION( Script, EActionState, eAS_Attack_Right );
	REGIST_ENUMERATION( Script, EActionState, eAS_Cast );
	REGIST_ENUMERATION( Script, EActionState, eAS_Sleep );

	REGIST_ENUMERATION( Script, EActionState, eAS_Take );
	REGIST_ENUMERATION( Script, EActionState, eAS_Fire );
	REGIST_ENUMERATION( Script, EActionState, eAS_Kick );
	REGIST_ENUMERATION( Script, EActionState, eAS_Gather );
	REGIST_ENUMERATION( Script, EActionState, eAS_PickUp );
	REGIST_ENUMERATION( Script, EActionState, eAS_Item_Use );
	REGIST_ENUMERATION( Script, EActionState, eAS_Item_Sing );
	REGIST_ENUMERATION( Script, EActionState, eAS_Item_Cast );
	REGIST_ENUMERATION( Script, EActionState, eAS_Item_Channel );

	REGIST_ENUMERATION( Script, EActionState, eAS_Throw );
	REGIST_ENUMERATION( Script, EActionState, eAS_WaitBite );
	REGIST_ENUMERATION( Script, EActionState, eAS_Disturbed01 );
	REGIST_ENUMERATION( Script, EActionState, eAS_Disturbed02 );
	REGIST_ENUMERATION( Script, EActionState, eAS_Disturbed03 );
	REGIST_ENUMERATION( Script, EActionState, eAS_Bite );
	REGIST_ENUMERATION( Script, EActionState, eAS_WaitPull );
	REGIST_ENUMERATION( Script, EActionState, eAS_Pull01 );
	REGIST_ENUMERATION( Script, EActionState, eAS_Pull02 );
	REGIST_ENUMERATION( Script, EActionState, eAS_Pull03 );
	REGIST_ENUMERATION( Script, EActionState, eAS_Succeed01 );
	REGIST_ENUMERATION( Script, EActionState, eAS_Succeed02 );

	REGIST_ENUMERATION( Script, EActionState, eAS_Birth );
	REGIST_ENUMERATION( Script, EActionState, eAS_Still_1 );
	REGIST_ENUMERATION( Script, EActionState, eAS_Still_2 );
	REGIST_ENUMERATION( Script, EActionState, eAS_Still_3 );
	REGIST_ENUMERATION( Script, EActionState, eAS_Respond_Enter );
	REGIST_ENUMERATION( Script, EActionState, eAS_Respond_Keep );
	REGIST_ENUMERATION( Script, EActionState, eAS_Respond_Leave );
	REGIST_ENUMERATION( Script, EActionState, eAS_Idle_Special );
	REGIST_ENUMERATION( Script, EActionState, eAS_Die_Special );
	REGIST_ENUMERATION( Script, EActionState, eAS_Dead_Special );

	// 武器动画播放类型
	REGIST_ENUMTYPE( Script, EWeaponPlayType );
	REGIST_ENUMERATION( Script, EWeaponPlayType, eWPT_None);
	REGIST_ENUMERATION( Script, EWeaponPlayType, eWPT_Always);
	REGIST_ENUMERATION( Script, EWeaponPlayType, eWPT_BattleChange);

	// 队伍类型
	REGIST_ENUMTYPE( Script, ETeamType );
	REGIST_ENUMERATION( Script, ETeamType, eTT_Team);
	REGIST_ENUMERATION( Script, ETeamType, eTT_Raid);


	REGIST_ENUMTYPE( Script, ESkillCoolDownType);
	REGIST_ENUMERATION(Script, ESkillCoolDownType, eSCDT_FightSkill);
	REGIST_ENUMERATION(Script, ESkillCoolDownType, eSCDT_UnrestrainedFightSkill);
	REGIST_ENUMERATION(Script, ESkillCoolDownType, eSCDT_NoComCDFightSkill);
	REGIST_ENUMERATION(Script, ESkillCoolDownType, eSCDT_PublicSkill);
	REGIST_ENUMERATION(Script, ESkillCoolDownType, eSCDT_DrugItemSkill);
	REGIST_ENUMERATION(Script, ESkillCoolDownType, eSCDT_SpecialItemSkill);
	REGIST_ENUMERATION(Script, ESkillCoolDownType, eSCDT_OtherItemSkill);
	REGIST_ENUMERATION(Script, ESkillCoolDownType, eSCDT_TempSkill);
	REGIST_ENUMERATION(Script, ESkillCoolDownType, eSCDT_UnrestrainedSkill);
	REGIST_ENUMERATION(Script, ESkillCoolDownType, eSCDT_OtherSkill);
	REGIST_ENUMERATION(Script, ESkillCoolDownType, eSCDT_NonFightSkill);
	REGIST_ENUMERATION(Script, ESkillCoolDownType, eSCDT_EquipSkill);
	REGIST_ENUMERATION(Script, ESkillCoolDownType, eSCDT_End);
	
	//  --------------------    非战斗行为  ----------------------
	REGIST_ENUMTYPE( Script, NonCombatBehavior );
	REGIST_ENUMERATION( Script, NonCombatBehavior, nCB_ItemSkill );
	REGIST_ENUMERATION( Script, NonCombatBehavior, nCB_ExpModulusChange );
	REGIST_ENUMERATION( Script, NonCombatBehavior, nCB_FetchModulusChange );
	REGIST_ENUMERATION( Script, NonCombatBehavior, nCB_ExpModulusInFBChange );

	//======================== 注册Npc类型 ================================
	REGIST_ENUMTYPE(Script, ENpcType);
	REGIST_ENUMERATION( Script, ENpcType, ENpcType_None);
	REGIST_ENUMERATION( Script, ENpcType, ENpcType_Totem);
	REGIST_ENUMERATION( Script, ENpcType, ENpcType_Pet);
	REGIST_ENUMERATION( Script, ENpcType, ENpcType_Summon);
	REGIST_ENUMERATION( Script, ENpcType, ENpcType_Normal);
	REGIST_ENUMERATION( Script, ENpcType, ENpcType_BattleHorse);
	REGIST_ENUMERATION( Script, ENpcType, ENpcType_Dummy);
	REGIST_ENUMERATION( Script, ENpcType, ENpcType_Member);
	REGIST_ENUMERATION( Script, ENpcType, ENpcType_OrdnanceMonster);
	REGIST_ENUMERATION( Script, ENpcType, ENpcType_MagicBuilding);
	REGIST_ENUMERATION( Script, ENpcType, ENpcType_Cannon);
	REGIST_ENUMERATION( Script, ENpcType, ENpcType_MonsterCard);
	REGIST_ENUMERATION( Script, ENpcType, ENpcType_Shadow);
	REGIST_ENUMERATION( Script, ENpcType, ENpcType_QuestBeckonNpc);
	REGIST_ENUMERATION( Script, ENpcType, ENpcType_AttackNearest);
	REGIST_ENUMERATION( Script, ENpcType, ENpcType_Truck);
	REGIST_ENUMERATION( Script, ENpcType, ENpcType_LittlePet);

	//====================== 注册trap的SeeType类型 =========================
	REGIST_ENUMTYPE(Script, ETrapSeeType);
	REGIST_ENUMERATION(Script, ETrapSeeType, ETrapSeeType_None);
	REGIST_ENUMERATION(Script, ETrapSeeType, ETrapSeeType_Player);
	REGIST_ENUMERATION(Script, ETrapSeeType, ETrapSeeType_Npc);
	REGIST_ENUMERATION(Script, ETrapSeeType, ETrapSeeType_All);

	//REGIST_B_CLASS( Script, CTalentHolder );
	//REGIST_STATICFUNCTION( Script, CTalentHolder, InsertName, "s" );
} 
