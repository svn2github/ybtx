#pragma once
#include "FightDef.h"
#include "CConfigMallocObject.h"
#include "TConfigAllocator.h"

class CSkillCfg : public CConfigMallocObject
{
public:
	typedef map<string, ETalentType, less<string>, 
		TConfigAllocator<pair<string, ETalentType > > >			MapTalentType;
	typedef map<string, ETalentRuleType, less<string>, 
		TConfigAllocator<pair<string, ETalentRuleType > > >		MapTalentRuleType;
	typedef map<string, EFSTargetType, less<string>, 
		TConfigAllocator<pair<string, EFSTargetType > > >			MapTargetType;
	typedef map<string, ESkillCoolDownType, less<string>, 
		TConfigAllocator<pair<string, ESkillCoolDownType > > >	MapCoolDownType;
	typedef map<string, EAttackType, less<string>, 
		TConfigAllocator<pair<string, EAttackType > > >			MapAttackType;
	typedef map<string, ECastingProcessType, less<string>, 
		TConfigAllocator<pair<string, ECastingProcessType > > >	MapCastingType;
	typedef map<string, EOperateObjectType, less<string>, 
		TConfigAllocator<pair<string, EOperateObjectType > > >	MapOperateObjectType;
	typedef map<string, EObjFilterType, less<string>, 
		TConfigAllocator<pair<string, EObjFilterType > > >		MapObjFilterType;
	typedef map<string, ECastingInterruptType, less<string>, 
		TConfigAllocator<pair<string, ECastingInterruptType > > >	MapCastingInterruptType;
	typedef map<string, string, less<string>,
		TConfigAllocator<pair<string, string > > >	 MapTempVarType;
	typedef map<string, string, less<string>,
		TConfigAllocator<pair<string, string > > >	 MapPassivityTempVarType;
	
	static MapTalentType						ms_mapTalentType;
	static MapTalentRuleType					ms_mapTalentRuleType;
	static MapTargetType						ms_mapTargetType;
	static MapCoolDownType						ms_mapCoolDownType;
	static MapAttackType						ms_mapAttackType;
	static MapCastingType						ms_mapCastingType;
	static MapCastingInterruptType				ms_mapCastingInterruptType;
	static MapOperateObjectType					ms_mapOperateObjectType;
	static MapObjFilterType						ms_mapObjFilterType;
	static MapTempVarType						ms_mapTempVarType;
	static MapPassivityTempVarType				ms_mapPassivityTempVarType;

	inline static void			InitMapTalentType()
	{
		ms_mapTalentType["职业天赋"]		= eTT_Class;
		ms_mapTalentType["阵营天赋"]		= eTT_Camp;
		ms_mapTalentType["等级天赋"]		= eTT_Level;
		ms_mapTalentType["技能天赋"]		= eTT_Skill;
		ms_mapTalentType["装备天赋"]		= eTT_Equip;
	}

	inline static void			InitMapTalentRuleType()
	{
		ms_mapTalentRuleType["叠加"]		= eTRT_Stackup;
		ms_mapTalentRuleType["分散"]		= eTRT_Decentral;
		ms_mapTalentRuleType["唯一"]		= eTRT_Unique;
	}

	inline static bool			InitMapTargetType()				
	{
		ms_mapTargetType["空地"]				= eFSTT_Position;
		ms_mapTargetType["敌方目标"]			= eFSTT_EnemyObject;
		ms_mapTargetType["友方目标"]			= eFSTT_FriendObject;
		ms_mapTargetType["中立目标"]			= eFSTT_NeutralObject;
		ms_mapTargetType["目标位置"]			= eFSTT_ObjectPosition;
		ms_mapTargetType["自身"]				= eFSTT_Self;
		ms_mapTargetType["自身位置"]			= eFSTT_SelfPosition;
		ms_mapTargetType["自身朝向"]			= eFSTT_SelfDirection;
		ms_mapTargetType["空地或敌方目标"]		= eFSTT_PositionOrEnemy;
		ms_mapTargetType["空地或友方目标"]		= eFSTT_PositionOrFriend;
		ms_mapTargetType["友方及自身"]			= eFSTT_FriendandSelf;
		ms_mapTargetType["所有目标"]			= eFSTT_AllObject;
		ms_mapTargetType["敌友目标"]			= eFSTT_FriendAndEnemy;
		ms_mapTargetType["所有目标除中立"]		= eFSTT_NotNeutral;
		ms_mapTargetType["选择友方目标"]		= eFSTT_SelectFriendObject;
		ms_mapTargetType["空地或锁定目标"]		= eFSTT_PositionOrLockedTarget;
		ms_mapTargetType["非友方目标"]			= eFSTT_NotFriendObject;
		ms_mapTargetType["队友或敌方目标"]		= eFSTT_TeammatesOrEnemy;
		return true;
	}

	inline static bool			InitMapCoolDownType()
	{
		ms_mapCoolDownType["战斗技能"]			= eSCDT_FightSkill;
		ms_mapCoolDownType["无公共CD战斗技能"]	= eSCDT_NoComCDFightSkill;
		ms_mapCoolDownType["公共技能"]			= eSCDT_PublicSkill;
		ms_mapCoolDownType["恢复物品技能"]		= eSCDT_DrugItemSkill;
		ms_mapCoolDownType["特殊物品技能"]		= eSCDT_SpecialItemSkill;
		ms_mapCoolDownType["其他物品技能"]		= eSCDT_OtherItemSkill;
		ms_mapCoolDownType["临时技能"]			= eSCDT_TempSkill;
		ms_mapCoolDownType["无限制技能"]		= eSCDT_UnrestrainedSkill;
		ms_mapCoolDownType["其它技能"]			= eSCDT_OtherSkill;
		ms_mapCoolDownType["非战斗技能"]		= eSCDT_NonFightSkill;
		ms_mapCoolDownType["装备技能"]			= eSCDT_EquipSkill;
		ms_mapCoolDownType["无限制战斗技能"]	= eSCDT_UnrestrainedFightSkill;
		return true;
	}

	inline static void InitMapAttackType()					
	{
		ms_mapAttackType["无类型"]	= eATT_None;
		ms_mapAttackType["穿刺"]	= eATT_Puncture;
		ms_mapAttackType["砍斫"]	= eATT_Chop;
		ms_mapAttackType["钝击"]	= eATT_BluntTrauma;
		ms_mapAttackType["自然"]	= eATT_Nature;
		ms_mapAttackType["破坏"]	= eATT_Destroy;
		ms_mapAttackType["暗黑"]	= eATT_Evil;
		ms_mapAttackType["跟随武器类型"]	= eATT_FollowWeapon;
	}

	inline static bool					InitMapCastingType()		//以后需要详细分类			
	{
		ms_mapCastingType["吟唱"]		= eCPT_Sing;
		ms_mapCastingType["引导"]		= eCPT_Channel;
		ms_mapCastingType["蓄力吟唱"]	= eCPT_GradeSing;
		ms_mapCastingType["硬直吟唱"]	= eCPT_ImmovableSing;
		return true;
	}

	inline static bool					InitMapCastingInterruptType()
	{
		ms_mapCastingInterruptType["否"]	= eCIT_None;
		ms_mapCastingInterruptType["移动"]	= eCIT_Move;
		ms_mapCastingInterruptType["转向"]	= eCIT_TurnAround;

		return true;
	}

	inline static bool					InitMapOperateObjectType()	
	{	
		ms_mapOperateObjectType["释放者"]		= eOOT_Releaser;
		ms_mapOperateObjectType["自身"]			= eOOT_Self;
		ms_mapOperateObjectType["目标"]			= eOOT_Target;
		ms_mapOperateObjectType["友方"]			= eOOT_Friend;
		ms_mapOperateObjectType["敌方"]			= eOOT_Enemy;
		ms_mapOperateObjectType["敌友目标"]		= eOOT_FriendAndEnemy;
		ms_mapOperateObjectType["小队成员"]		= eOOT_Teammates;
		ms_mapOperateObjectType["团队成员"]		= eOOT_Raidmates;
		ms_mapOperateObjectType["佣兵小队成员"]	= eOOT_Tongmates;
		ms_mapOperateObjectType["自身主人"]		= eOOT_SelfMaster;
		ms_mapOperateObjectType["目标周围敌方"]	= eOOT_TargetAroundEnemy;
		ms_mapOperateObjectType["释放者及团队成员"]	= eOOT_SelfAndRaidmates;

		return true;
	}

	inline static bool					BuildObjFilterMap()
	{
		ms_mapObjFilterType["自身"]				= eOF_Self;
		ms_mapObjFilterType["目标"]				= eOF_Target;
		ms_mapObjFilterType["地点"]				= eOF_Position;
		ms_mapObjFilterType["宠物"]				= eOF_Pet;
		ms_mapObjFilterType["非控制仆人"]		= eOF_NotCtrlSummon;
		ms_mapObjFilterType["主人"]				= eOF_Master;
		ms_mapObjFilterType["小队成员"]			= eOF_Teammates;
		ms_mapObjFilterType["团队成员"]			= eOF_Raidmates;
		ms_mapObjFilterType["佣兵小队成员"]		= eOF_Tongmates;
		ms_mapObjFilterType["周围友方"]			= eOF_AroundFriend;
		ms_mapObjFilterType["周围敌方"]			= eOF_AroundEnemy;
		ms_mapObjFilterType["上次目标筛选结果"]	= eOF_FilterResult;

		ms_mapObjFilterType["数量"]				= eOF_Amount;
		ms_mapObjFilterType["尸体"]				= eOF_DeadBody;
		ms_mapObjFilterType["自己除外"]			= eOF_ExceptSelf;
		ms_mapObjFilterType["目标除外"]			= eOF_ExceptTarget;
		ms_mapObjFilterType["处于触发器状态"]	= eOF_InTriggerState;
		ms_mapObjFilterType["处于魔法状态"]		= eOF_InMagicState;
		ms_mapObjFilterType["不处于魔法状态"]	= eOF_NotInMagicState;
		ms_mapObjFilterType["不处于特殊状态"]	= eOF_OutSpecialState;
		ms_mapObjFilterType["归属自身"]			= eOF_IsVestInSelf;
		ms_mapObjFilterType["经验归属自身"]		= eOF_ExpIsVestInSelf;
		ms_mapObjFilterType["筛选NPC"]			= eOF_FilterNPC;
		ms_mapObjFilterType["筛选玩家"]			= eOF_FilterPlayer;
		ms_mapObjFilterType["随机筛选"]			= eOF_FilterRandom;
		ms_mapObjFilterType["地点魔法目标"]		= eOF_PosMagicTarget;
		ms_mapObjFilterType["最近目标"]			= eOF_LatestTarget;
		ms_mapObjFilterType["生命值最小"]		= eOF_LeastHP;			// 没用到
		ms_mapObjFilterType["职业"]				= eOF_Class;			// 没用到
		ms_mapObjFilterType["经验拥有者"]		= eOF_ExpOwner;
		ms_mapObjFilterType["当前锁定敌方目标"]	= eOF_LockedEnemyTarget;
		ms_mapObjFilterType["尸体除外"]			= eOF_ExceptDeadBoby;
		ms_mapObjFilterType["冲击波1"]			= eOF_ShockWaveEff1;
		ms_mapObjFilterType["冲击波2"]			= eOF_ShockWaveEff2;
		ms_mapObjFilterType["冲击波3"]			= eOF_ShockWaveEff3;
		ms_mapObjFilterType["冲击波4"]			= eOF_ShockWaveEff4;
		ms_mapObjFilterType["冲击波5"]			= eOF_ShockWaveEff5;

		return true;
	}

	inline static bool InitDCStateTempVarMap()
	{
		ms_mapTempVarType["近程普通攻击伤害"] = "伤害";
		ms_mapTempVarType["近程普通攻击单体伤害"] = "伤害";
		ms_mapTempVarType["远程普通攻击伤害"] = "伤害";
		ms_mapTempVarType["主手普通攻击伤害"] = "伤害";
		ms_mapTempVarType["伤害"]		= "伤害";
		ms_mapTempVarType["非DOT伤害"]	= "伤害";
		ms_mapTempVarType["致死伤害"]	= "伤害";
		ms_mapTempVarType["吸血"]		= "治疗";
		ms_mapTempVarType["治疗"]		= "治疗";
		return true;
	}
	
	inline static bool InitPassivityDCStateTempVarMap()
	{
		ms_mapPassivityTempVarType["近程普通攻击伤害"] = "被伤害";
		ms_mapPassivityTempVarType["近程普通攻击单体伤害"] = "被伤害";
		ms_mapPassivityTempVarType["远程普通攻击伤害"] = "被伤害";
		ms_mapPassivityTempVarType["主手普通攻击伤害"] = "被伤害";
		ms_mapPassivityTempVarType["伤害"]		= "被伤害";
		ms_mapPassivityTempVarType["非DOT伤害"]	= "被伤害";
		ms_mapPassivityTempVarType["致死伤害"]	= "被伤害";
		ms_mapPassivityTempVarType["吸血"]		= "被治疗";
		ms_mapPassivityTempVarType["治疗"]		= "被治疗";
		return true;
	}
};
