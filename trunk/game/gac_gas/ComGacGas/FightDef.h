#pragma once

/*
* =======================================================
*						角色相关
* =======================================================
*/

#if !defined _FightSkillCounter
	//#define _FightSkillCounter
#endif

enum EBroadcastArea{ eSyncSight = 0, eViewSight, eOnlyToDirector};

enum ETeamType
{
	eTT_Team=1,
	eTT_Raid
};

// 角色事件
enum ECharacterEvent			
{
	eCE_BeAttacked,	
	eCE_Attack,
	eCE_MoveBegan,				// 角色开始移动
	eCE_MoveEnd,
	eCE_MoveSuccess,
	eCE_MoveStop,
	eCE_Die,
	eCE_Offline,
	eCE_LevelChanged,			//角色等级变化
	eCE_ChangeMapBegin,				//切换地图开始
	eCE_ChangeMapEnd,				// 角色切换地图
	eCE_Reborn,
	eCE_CancelCoerceMove,		// 客户端取消服务端强迫角色移动			
	eCE_BeAttackedFromNotDot,
	eCE_EntryEnmity,			// 进入仇恨列表
	eCE_BeInterrupted, 
	eCE_MovePathChange,
	eCE_CoreEvent,
	eCE_BeHurtFirstTime,		// 第一次被伤害(扣血)
	eCE_Transport,				// 传送
	eCE_BeforeDie,				// 死亡前
	eCE_OnAniEnd,
	eCE_FinishDuel,
	eCE_BattleRelationChange,	//战斗关系改变
	eCE_DebaseEnmity,			//降低仇恨
	eCE_Max,                    // 枚举最大值
};

/*
* =======================================================
*						属性相关
* =======================================================
*/

// 属性元素
enum EPropertyFactorType
{
	ePFT_Value,
	ePFT_Adder,
	ePFT_Multiplier,
	ePFT_Agile,
	ePFT_Count
};

// 属性ID
enum EPropertyID
{
	// HealthPoint = 0, HealthPointAdder = 0 + 1, HealthPointMultiplier = 0 + 2, HealthPointAgile = 0 + 3
	ePID_HealthPoint,					// 最高血量
	ePID_ManaPoint,						// 最高蓝量
	ePID_ComboPoint,					// 最大连击
	ePID_RagePoint,						// 最大怒气
	ePID_EnergyPoint,					// 最大能量

	ePID_AgileValueCount,				// AgileValue和CalcValue以此为分界线

	ePID_HPUpdateRate,					// 回血率
	ePID_MPUpdateRate,					// 回魔率
	ePID_RPUpdateValue,					// 减怒值
	ePID_EPUpdateValue,					// 回能值
	ePID_RevertPer,						// 战斗状态回复速度 = RevertPer * 非战斗状态回复速度
	ePID_MPConsumeRate,					// 魔法消耗系数
	ePID_RPConsumeRate,					// 怒气消耗系数
	ePID_EPConsumeRate,					// 能量消耗系数

	ePID_NatureMPConsumeRate,			// 自然系魔法消耗系数
	ePID_DestructionMPConsumeRate,		// 破坏系魔法消耗系数
	ePID_EvilMPConsumeRate,				// 黑暗系魔法消耗系数

	ePID_RPProduceRate,					// 怒气产生系数

	ePID_RunSpeed,						// 移动速度
	ePID_WalkSpeed,						// 行走速度

	ePID_StrikeMultiValue,				//爆击上限（爆击伤害倍数值）
	ePID_StrikeValue,					//爆击值（用于计算爆击率）

	ePID_Defence,						//护甲（防御值）
	ePID_BlockRate,						//盾牌格挡率，无盾牌时为0
	ePID_BlockDamage,					//盾牌所能格挡掉的伤害
	ePID_PhysicalDodgeValue,			//物理攻击躲避值（用于计算物理攻击多比率）
	ePID_MagicDodgeValue,				//法术躲避值（用于计算法术躲避率）
	ePID_MagicHitValue,					//魔法命中值（用于计算魔法命中率）
	ePID_MissRate,						//未命中率（任何攻击都有一定概率无法击中）

	ePID_ImmuneRate,					//免疫率（用于无敌效果）
	ePID_PunctureDamageImmuneRate,		//穿刺伤害免疫率
	ePID_ChopDamageImmuneRate,			//砍斫伤害免疫率
	ePID_BluntDamageImmuneRate,			//钝击伤害免疫率
	ePID_NatureDamageImmuneRate,		//自然伤害免疫率
	ePID_DestructionDamageImmuneRate,	//破坏伤害免疫率
	ePID_EvilDamageImmuneRate,			//暗黑伤害免疫率
	ePID_ControlDecreaseImmuneRate,		//控制状态免疫率
	ePID_PauseDecreaseImmuneRate,		//定身状态免疫率
	ePID_CripplingDecreaseImmuneRate,	//减速状态免疫率
	ePID_DebuffDecreaseImmuneRate,		//负面状态免疫率
	ePID_DOTDecreaseImmuneRate,			//伤害状态免疫率
	ePID_SpecialDecreaseImmuneRate,		//特殊状态免疫率
	ePID_CureImmuneRate,				//治疗免疫率
	ePID_MoveMagicImmuneRate,			//位移魔法免疫率
	ePID_NonTypePhysicsDamageImmuneRate,	//无类型物理伤害免疫率
	ePID_NonTypeDamageImmuneRate,			//无类型伤害免疫率
	ePID_NonTypeCureImmuneRate,			//无类型治疗免疫率
	ePID_InterruptCastingProcessImmuneRate,			//打断流程免疫率

	ePID_ParryValue,					//招架值（用于计算招架率）
	ePID_ResilienceValue,				//韧性值
	ePID_StrikeResistanceValue,					//免爆值
	ePID_AccuratenessValue,				//精确值
	ePID_PhysicalDPS,					//攻击力（附加DPS）
	ePID_PunctureDamage,				//穿刺伤害（每次进行穿刺攻击时附加的伤害值）
	ePID_ChopDamage,					//砍凿伤害（每次进行砍凿攻击时附加的伤害值）
	ePID_BluntDamage,					//钝击伤害（每次进行钝器攻击时附加的伤害值）

	ePID_MagicDamageValue,				//法伤加成（用于计算法术附加伤害值）
	ePID_NatureDamageValue,				//自然法术法伤加成（用于计算自然法术附加伤害值）
	ePID_DestructionDamageValue,		//破坏法术法伤加成（用于计算破坏法术附加伤害值）
	ePID_EvilDamageValue,				//黑暗法术法伤加成（用于计算黑暗法术附加伤害值）

	ePID_NatureResistanceValue,			//自然法术抵抗值（用于计算自然法术抵抗率）
	ePID_DestructionResistanceValue,	//破坏法术抵抗值（用于计算破坏法术抵抗率）
	ePID_EvilResistanceValue,			//黑暗法术抵抗值（用于计算黑暗法术抵抗率）

	ePID_NatureDecreaseResistanceValue,			//自然减抗值（用于穿透最终比例计算）
	ePID_DestructionDecreaseResistanceValue,	//破坏减抗值（用于穿透最终比例计算）
	ePID_EvilDecreaseResistanceValue,			//黑暗减抗值（用于穿透最终比例计算）
	ePID_DefencePenetrationValue,				//护甲穿透值（用于穿透最终比例计算）
	ePID_ValidityCoefficient,					//有效性系数（用于穿透最终比例计算）
	ePID_PenetrationFinalRate,			//穿透最终比例

	ePID_PhysicalDamage,				//技能附加物理伤害（由武器或者极少数装备提供）
	ePID_AssistPhysicalDamage,			//技能附加物理伤害（由武器或者极少数装备提供）
	ePID_MagicDamage,					//技能附加法术伤害（由法术或者极少数装备提供）
	ePID_DOTDamage,						//技能附加DOT伤害

	ePID_MainHandMinWeaponDamage,		//主手武器最小伤害（由武器提供）
	ePID_MainHandMaxWeaponDamage,		//主手武器最大伤害（由武器提供）

	ePID_AssistantMinWeaponDamage,		//副手武器最小伤害（由武器提供）
	ePID_AssistantMaxWeaponDamage,		//副手武器最大伤害（由武器提供）

	ePID_MainHandWeaponInterval,		//主手武器攻击间隔（由武器提供）
	ePID_AssistantWeaponInterval,		//副手武器攻击间隔（由武器提供）
	ePID_MHWeaponIntervalExtraDamageRate,		//主手武器攻击间隔伤害增加系数（由武器提供）
	ePID_AHWeaponIntervalExtraDamageRate,		//副手武器攻击间隔伤害增加系数（由武器提供）

	ePID_ResistCastingInterruptionRate,	//施法流程抗中断率
	
	ePID_PenetrationValue,				//穿透值,军械&建筑专用
	ePID_ProtectionValue,				//防护值,军械&建筑专用

	ePID_PunctureDamageResistanceRate,	//穿刺伤害减免比例
	ePID_ChopDamageResistanceRate,		//砍斫伤害减免比例
	ePID_BluntDamageResistanceRate,		//钝击伤害减免比例

	ePID_ExtraDOTDamageRate,			//最终附加DOT伤害加成比例
	ePID_ExtraPunctureDamageRate,		//最终附加穿刺伤害加成比例
	ePID_ExtraChopDamageRate,			//最终附加砍斫伤害加成比例
	ePID_ExtraBluntDamageRate,			//最终附加钝击伤害加成比例
	ePID_ExtraNatureDamageRate,			//最终附加自然伤害加成比例
	ePID_ExtraEvilDamageRate,			//最终附加暗黑伤害加成比例
	ePID_ExtraDestructionDamageRate,	//最终附加破坏伤害加成比例
	ePID_ExtraBowDamageRate,			//最终附加弓武器伤害加成比例
	ePID_ExtraCrossBowDamageRate,		//最终附加弩武器伤害加成比例
	ePID_ExtraTwohandWeaponDamageRate,	//最终附加双手武器伤害加成比例
	ePID_ExtraPolearmDamageRate,		//最终附加长柄武器伤害加成比例
	ePID_ExtraPaladinWeaponDamageRate,	//最终附加骑士武器伤害加成比例
	ePID_ExtraAssistWeaponDamageRate,	//最终附加副手武器伤害加成比例
	ePID_ExtraLongDistDamageRate,		//最终附加远程武器伤害加成比例
	ePID_ExtraShortDistDamageRate,		//最终附加近程武器伤害加成比例
	ePID_DamageDecsRate,				//最终伤害减免比例
	ePID_ExtraCureValueRate,			//最终治疗值加成比例
	ePID_ExtraBeCuredValueRate,			//最终被治疗值加成比例
	ePID_ExtraPhysicDefenceRate,		//最终物理防护率加成比例
	ePID_ExtraPhysicDodgeRate,			//最终物理躲避率加成比例
	ePID_ExtraParryRate,				//最终招架率加成比例
	ePID_ExtraStrikeRate,				//最终暴击率加成比例
	ePID_ExtraMagicDodgeRate,			//最终法术躲避加成比例
	ePID_ExtraMagicResistanceRate,		//最终法术抵抗率加成比例
	ePID_ExtraNatureResistanceRate,		//最终自然法术抵抗率加成比例
	ePID_ExtraDestructionResistanceRate,//最终破坏法术抵抗率加成比例
	ePID_ExtraEvilResistanceRate,		//最终暗黑法术抵抗率加成比例
	ePID_ExtraCompleteResistanceRate,	//最终法术完全抵抗率加成比例
	ePID_LongDistWeaponDamageRate,		//远程武器近身伤害修正比例

	ePID_ControlDecreaseResistRate,		//控制抗性
	ePID_PauseDecreaseResistRate,		//定身抗性
	ePID_CripplingDecreaseResistRate,	//减速抗性
	ePID_DebuffDecreaseResistRate,		//Debuff抗性
	ePID_DOTDecreaseResistRate,			//DOT抗性
	ePID_SpecialDecreaseResistRate,		//特殊抗性

	ePID_ControlDecreaseTimeRate,		//控制时间系数
	ePID_PauseDecreaseTimeRate,			//定身时间系数
	ePID_CripplingDecreaseTimeRate,		//减速时间系数
	ePID_SpecialDecreaseTimeRate,		//特殊时间系数
	
	ePID_CastingProcessTimeRatio,		//施法流程时间系数
	ePID_NatureSmashValue,				//自然碾压值
	ePID_DestructionSmashValue,			//破坏碾压值
	ePID_EvilSmashValue,				//暗黑碾压值
	ePID_DefenceSmashValue,				//护甲碾压值
	ePID_ExtraSmashRate,				//最终附加碾压比率

	ePID_Count							//属性个数
};

// 扩展方法属性ID
enum EMethodAttrID
{
	eMAID_Begin = ePID_Count * ePFT_Count,
	eMAID_NADamage,						//普攻伤害
	eMAID_FighterLevel,					//人物等级
	eMAID_MainHandWeaponIsSingle,		//主手武器是否单手
	eMAID_End,
	eMAID_Undefined,					//未定义
};



// 易变属性类型
enum EAgileType
{
	eAT_HP=1,	//生命
	eAT_MP,		//魔法	
	eAT_RP,		//怒气
	eAT_EP,		//能量
	eAT_CP,		//连击点
};

// 攻击类型
enum EAttackType
{
	eATT_None,			// 无攻击类型
	eATT_FollowWeapon,	// 跟随武器类型
	eATT_Puncture,		// 穿刺
	eATT_Chop,			// 砍斫
	eATT_BluntTrauma,	// 钝击	
	eATT_Nature,		// 自然
	eATT_Destroy,		// 破坏
	eATT_Evil,			// 暗黑
	eATT_TaskNonBattle,	// 任务非战斗
	eATT_TaskBomb,		// 任务炸弹
	eATT_TaskSpecial,	// 任务特殊
	eATT_TaskNonNationBattleBuild,		//非国战建筑伤害
	eATT_End			// 结束
};

//技能冷却类型
enum ESkillCoolDownType
{
	eSCDT_FightSkill,		//战斗技能,具有1秒公共冷却时间	
	eSCDT_NoComCDFightSkill,//无公共CD战斗技能
	eSCDT_PublicSkill,		//公共技能,没有冷却时间,可以在普通坐骑使用
	eSCDT_DrugItemSkill,	//药品技能,回复类具有小类公共冷却时间,可以在普通坐骑使用
	eSCDT_SpecialItemSkill,	//特殊类物品技能, 具有自身单独的公共冷却时间,可以在普通坐骑使用
	eSCDT_OtherItemSkill,	//其他物品技能, 只具有1秒公共冷却时间,可以在普通坐骑使用
	eSCDT_TempSkill,		//临时技能,没有冷却时间
	eSCDT_UnrestrainedSkill,//无限制技能,无冷却,禁止使用技能无效
	eSCDT_OtherSkill,		//其它技能,没有公共冷却时间,具有自身单独的公共冷却时间
	eSCDT_NonFightSkill,	// 非战斗技能,例如骑术
	eSCDT_EquipSkill,		//装备技能
	eSCDT_UnrestrainedFightSkill,	//无限制战斗技能
	eSCDT_End
};
// 武器类型
//调整枚举值的位置时请同时修改下面的全局函数
enum EWeaponType
{
	eWT_None,

	eWT_Shield,			//盾牌

	eWT_SHSword,		//单手剑
	eWT_SHAxe,			//单手斧
	eWT_SHHammer,		//单手锤
	eWT_SHKnife,		//单手刀	

	eWT_THSword,		//双手剑
	eWT_THAxe,			//双手斧
	eWT_THHammer,		//双手锤

	eWT_PaladinGun,		//骑士枪
	eWT_Lance,			//长矛
	eWT_LongKnife,		//长刀
	eWT_LongStick,		//长棒

	// 必须放后面，会用eWeaponType > eWT_LongStick来判断是否是远程武器
	eWT_Bow,			//双手弓
	eWT_CrossBow,		//双手弩
	eWT_SHWand,			//单手杖
	eWT_THWand,			//双手杖

	eWT_End,
};

inline bool IsLongDistWeapon(EWeaponType eWeaponType)
{
	if(eWeaponType >= eWT_Bow && eWeaponType <= eWT_THWand)
		return true;
	return false;
}

inline bool IsPolearmWeapon(EWeaponType eWeaponType)
{
	if(eWeaponType >= eWT_PaladinGun && eWeaponType <= eWT_LongStick)
		return true;
	return false;
}

//是否为双手武器
inline bool IsShortDistTwoHandsWeapon(EWeaponType eWeaponType)
{
	if(eWeaponType == eWT_THSword || eWeaponType == eWT_THAxe || eWeaponType == eWT_THHammer)
		return true;
	return false;
}

//bool IsSingleHandWeapon(EWeaponType eWeaponType)
//{
//}

inline bool IsSingleHandsWeapon(EWeaponType eWeaponType)
{
	switch(eWeaponType)
	{
	case eWT_Shield:
	case eWT_SHSword:
	case eWT_SHAxe:
	case eWT_SHHammer:
	case eWT_SHKnife:
	case eWT_SHWand:
		return true;
	default:
		break;
	}
	return false;
}

inline bool IsTwoHandsWeapon(EWeaponType eWeaponType)
{
	if(eWeaponType == eWT_None)
		return false;
	return !IsSingleHandsWeapon(eWeaponType);
}

// 战斗控制状态
enum EFighterCtrlState
{
	eFCS_None					= 0x00000000,

	eFCS_InNormalHorse			= 0x00000001,		// 骑乘普通坐骑
	eFCS_InBattleHorse			= 0x00000002,		// 骑乘战斗坐骑	
	eFCS_FeignDeath				= 0x00000004,		// 假死
	eFCS_InBattle				= 0x00000008,		// 战斗状态
	eFCS_OnMission				= 0x00000010,		// 玩法状态中
	eFCS_InDuel					= 0x00000020,		// 决斗状态
	eFCS_InWalkState			= 0x00000040,		// 走路状态		,移走动作状态
	eFCS_InDrawWeaponMode		= 0x00000080,		// 收拔武器状态	,移走动作状态
	eFCS_AllowChangeDirection	= 0x00000100,		// 禁止活动和禁止移动中允许改变方向
	eFCS_InNormalAttacking		= 0x00000200,		// 开启普攻追踪
	eFCS_ForbitAutoTrack		= 0x00000400,		// 禁止客户端自动追踪

	eFCS_CountCtrlValueBegin	= 0x00010000,	
	eFCS_ForbitNormalAttack		= 0x00020000,		// 禁止普通攻击
	eFCS_ForbitMove				= 0x00040000,		// 禁止移动
	eFCS_ForbitTurnAround		= 0x00080000,		// 禁止转身
	eFCS_ForbitUseWeapon		= 0x00100000,		// 缴械
	eFCS_Max					= 0x40000000,
};

/*
* =======================================================
*						技能相关
* =======================================================
*/

//技能使用状态(InDoingSkill && 禁技能)
enum EDoSkillCtrlState
{
	eDSCS_None,
	eDSCS_InDoingSkill,					// 正在使用技能

	eDSCS_CountBegin,
	eDSCS_ForbitUseSkill,				// 禁止使用技能
	eDSCS_ForbitNatureMagic,			// 禁止使用自然系法术
	eDSCS_ForbitDestructionMagic,		// 禁止使用破坏系法术
	eDSCS_ForbitEvilMagic,				// 禁止使用暗黑系法术
	eDSCS_ForbitUseClassSkill,			// 禁止职业技能
	eDSCS_ForbitUseDrugItemSkill,		// 禁止药品物品技能
	eDSCS_ForbitUseNonFightSkill,		// 禁止非战斗技能
	eDSCS_ForbitUseMissionItemSkill,	// 禁止玩法物品技能
	eDSCS_Max,
};

// 技能类型
enum ESkillType
{
	eST_None,			// 无类型
	eST_Physical,		// 物理技能
	eST_Magic,			// 魔法技能
	eST_DOT,			// DOT技能
	eST_End,			// 结束
};

// Inst类别
enum EInstType
{
	eIT_None,
	eIT_SkillInst,
	eIT_StateInst,
	eIT_MagicStateInst,
	eIT_NormalAttackInst
};

enum ETalentType
{
	eTT_Class,
	eTT_Camp,
	eTT_Level,
	eTT_Skill,
	eTT_Equip,
};

enum ETalentRuleType
{
	eTRT_Stackup,	//叠加
	eTRT_Unique,	//唯一
	eTRT_Decentral, //分散
};

// 技能施放结果返回值
enum EDoSkillResult
{	
	eDSR_None		= 0,		// 失败无提示
	eDSR_Success		= 1800,		// 技能释放成功
	eDSR_Fail,						// 技能释放失败
	eDSR_CharacterHasDead,			// 角色死亡
	eDSR_NoSkill,					// 无此技能
	eDSR_InCDTime,					// CD时间未到
	eDSR_DoingOtherSkill,			// 正在施放其他技能
	eDSR_ForbitUseSkill,			// 禁止施放技能
	eDSR_TargetNotExist,			// 目标不存在
	eDSR_TargetIsDead,				// 目标已死亡
	eDSR_TargetIsAlive,				// 目标不是尸体
	eDSR_TargetIsFriend,			// 不能攻击友方
	eDSR_TargetIsSelf,				// 不能攻击自己
	eDSR_TargetIsEnemy,				// 不能协助敌人
	eDSR_TargetIsNeutral,			// 中立目标不参与战斗
	eDSR_SelfUnderPKLevel,			// 自己等级低于PK底线等级，不能送死
	eDSR_TargetUnderPKLevel,		// 目标等级低于PK底线等级，不能收割
	eDSR_SelfBeyondLv,				// 目标等级过低
	eDSR_TargetBeyondLv,			// 目标等级过高
	eDSR_SelfSafetyState,			// 自己处于安全状态，无法攻击
	eDSR_TargetSafetyState,			// 目标处于安全状态，无法攻击
	eDSR_CanNotAssist,				// 自己处于安全状态，无法协助对抗状态目标
	eDSR_TargetInDuel,				// 目标处于决斗状态，不能进行该操作
	eDSR_DuelTargetWrong,			// 你处于挑战状态中，目标选择错误
	eDSR_ToTargetHaveBarrier,		// 到达目标有障碍
	eDSR_OutOfDistance,				// 超出施法距离
	eDSR_NotEnoughDistance,			// 不足施法距离
	eDSR_HealthPointIsGreater,		// 生命值过高，不满足条件	
	eDSR_HealthPointIsLesser,		// 生命值过低，不满足条件		
	eDSR_ManaPointIsLesser,			// 魔法值过低，不满足条件
	eDSR_EnergyPointIsLesser,		// 能量值过低，不满足条件
	eDSR_RagePointIsLesser,			// 怒气值过低，不满足条件
	eDSR_ComboPointIsLesser,		// 连击点过低，不满足条件	
	eDSR_NotInTheMagicState,		// 不满足处于魔法状态条件
	eDSR_NotInTheTriggerState,		// 不满足处于触发器状态条件
	eDSR_NotInTheDamageChangeState,	// 不满足处于伤害变更状态条件
	eDSR_NotInTheSpecialState,		// 不满足处于特殊状态条件
	eDSR_NotInTheStanceSkill,		// 不满足处于姿态技能条件
	eDSR_HaveNotShieldEquip,		// 不满足装备盾牌条件
	eDSR_HaveNotMainHandEquip,		// 不满足装备主手武器条件
	eDSR_IsNotExistItem,			// 不满足物品需求条件
	eDSR_IsNotExistPet,				// 不满足宠物存在的条件
	eDSR_IsNotExistTotem,			// 不满足图腾存在的条件
	eDSR_IsNotExistHorse,			// 不满足坐骑存在的条件
	eDSR_IsExistHorse,				// 存在坐骑，请先取消
	eDSR_IsNotInBattleSkill,		// 该技能只能在非战斗状态使用
	eDSR_NotEnoughMagicStateCascade,// 没有足够的魔法状态叠加层数
	eDSR_NormalAttackNotAllowed,	// 正在施放技能，不能进行普攻
	eDSR_ForbitUseClassSkill,		// 禁止使用职业技能
	eDSR_ForbitUseNonFightSkill,	// 禁止使用非战斗技能
	eDSR_SkillEndWithOutSing,
	eDSR_InDecreaseState,			// 处于负面状态
	eDSR_CannotBeControll,			// 目标不能被控制
	eDSR_IsNotNPC,					// 目标不是NPC
	eDSR_TargetLevelIsbigger,		// 目标等级大于自身等级
	eDSR_NotInNormalHorseState,		// 战斗坐骑状态
	eDSR_TargetIsNotRaidmates,		// 目标不是团队成员
	eDSR_InSkillItemCDTime,			// 技能物品未冷却
	eDSR_UseSkillItemFail,			// 技能物品使用失败
	eDSR_InTheMagicState,			// 不满足不处于魔法状态条件
	eDSR_InTheDamageChangeState,	// 不满足不处于伤害变更状态条件
	eDSR_InTheSpecialState,			// 不满足不处于特殊状态条件
	eDSR_AlreadyInTheMagicState,	// 不可重复状态,即已拥有此状态不可重复使用
	eDSR_SkillBeInterrupted,		// 流程技能被打断
	eDSR_CannotDoSkillToRivalry,	// 不能对对抗状态友方使用此技能
	eDSR_CannotBeRavin,				// 目标不能被猎食
	eDSR_TargetIsFriendType,		// 目标是友方不能释放此技能
	eDSR_NowSceneForbidUse,			// 当前场景不允许释放
	eDSR_HaveNotAssistHandEquip,	// 不满足装备副手武器条件
	eDSR_TargetNotTeammatesOrEnemy,	// 目标不是队友或敌方目标
};

// 敌友关系
enum ESimpleRelation
{
	eSR_None			= 0x0,		//   0 
	eSR_PlayerFriend	= 0x2,		//	10
	eSR_PlayerEnemy		= 0x3,		//	11
	eSR_NPCFriend		= 0x4,		// 100
	eSR_NPCEnemy		= 0x5,		// 101
	eSR_MonsterFriend	= 0x8,		//1000,  宠物或被控制的怪物
	eSR_MonsterEnemy	= 0x9,		//1001,  怪物
};

// 目标类型
enum ETargetType
{
	eTT_None,
	eTT_Fighter,
	eTT_Position,
};

// 释放技能目标类型
enum EFSTargetType
{
	eFSTT_None,					// 无目标，用于初始化
	eFSTT_Position,				// 空地
	eFSTT_PositionOrFriend,		// 空地或友方目标
	eFSTT_PositionOrEnemy,		// 空地或敌方目标
	eFSTT_EnemyObject,			// 敌方目标
	eFSTT_FriendObject,			// 友方目标
	eFSTT_NeutralObject,		// 中立目标
	eFSTT_ObjectPosition,		// 目标位置
	eFSTT_Self,					// 自身
	eFSTT_SelfPosition,			// 自身位置
	eFSTT_SelfDirection,		// 自身朝向
	eFSTT_FriendandSelf,		// 友方及自身
	eFSTT_AllObject,			// 所有目标
	eFSTT_FriendAndEnemy,		// 敌友目标
	eFSTT_NotNeutral,			// 非中立所有
	eFSTT_SelectFriendObject,	// 选择友方目标
	eFSTT_PositionOrLockedTarget, // 空地或锁定目标
	eFSTT_NotFriendObject,		//非友方目标
	eFSTT_TeammatesOrEnemy,		//队友或敌方目标
	eFSTT_End,
};

// 选择地点类型
enum ESelectGroundType
{
	eSGT_GroundSelector,	//光圈选择类型
	eSGT_MouseSelector,		//鼠标选择类型
	eSGT_TargetSelector,	//目标选择类型
};

// 目标筛选类型
enum EObjFilterType
{
	eOF_None,
	// 单对象(起始)
	eOF_Self,			// 自身
	eOF_Target,			// 目标
	eOF_Position,		// 地点

	// 无参数筛选
	eOF_Pet,			// 宠物
	eOF_NotCtrlSummon,	// 非控制仆人
	eOF_Master,			// 主人
	eOF_DeadBody,		// 尸体
	eOF_ExceptSelf,		// 自己除外
	eOF_ExceptTarget,	// 目标除外
	eOF_IsVestInSelf,	// 归属自身
	eOF_ExpIsVestInSelf,// 经验归属自身
	eOF_LatestTarget,	// 最近目标
	eOF_LeastHP,		// 生命值最小
	eOF_FilterPlayer,	// 筛选玩家
	eOF_ExpOwner,		// 取目标的经验拥有者
	eOF_LockedEnemyTarget,	// 当前锁定敌方目标

	// 带范围参数筛选
	eOF_Teammates,		// 小队成员
	eOF_Raidmates,		// 团队成员
	eOF_Tongmates,		// 佣兵小队成员
	eOF_AroundFriend,	// 周围友方
	eOF_AroundEnemy,	// 周围敌方

	// 带数量参数筛选
	eOF_Amount,			// 数量
	eOF_FilterRandom,	// 随机筛选

	// 带字符串参数筛选
	eOF_InTriggerState,	// 处于触发器状态
	eOF_InMagicState,	// 处于魔法状态
	eOF_NotInMagicState,// 不处于魔法状态
	eOF_OutSpecialState,// 不处于特殊状态
	eOF_PosMagicTarget,	// 地点魔法目标
	eOF_Class,			// 职业

	// 可有参可无参
	eOF_FilterNPC,		// 筛选NPC
	eOF_ExceptDeadBoby,	// 尸体除外

	eOF_ShockWaveEff1,		// 冲击波1
	eOF_ShockWaveEff2,		// 冲击波2
	eOF_ShockWaveEff3,		// 冲击波3
	eOF_ShockWaveEff4,		// 冲击波4
	eOF_ShockWaveEff5,		// 冲击波5

	// 用于相同对象多效果
	eOF_FilterResult,	// 上次目标筛选结果
};

// 魔法作用对象类型
enum EOperateObjectType
{
	eOOT_None,
	eOOT_Releaser,			// 释放者
	eOOT_Self,				// 自身
	eOOT_Target,			// 目标
	eOOT_Friend,			// 友方
	eOOT_Enemy,				// 敌方
	eOOT_FriendAndEnemy,	// 敌友目标
	eOOT_Teammates,			// 小队成员
	eOOT_Raidmates,			// 团队成员
	eOOT_Tongmates,			// 佣兵小队成员
	eOOT_SelfMaster,		// 自身主人
	eOOT_TargetAroundEnemy,	// 目标周围敌方
	eOOT_SelfAndRaidmates,	// 释放者及团队成员
};

// 伤害类型（主要用于客户端角色的头顶显示扣血量的效果）
enum EHurtResult
{
	eHR_Fail=0,		// 失败
	eHR_Success,	// 成功
	eHR_Hit,		// 命中--(物理、魔法、治疗、效果)
	eHR_Miss,		// 未命中--(物理、效果)
	eHR_Strike,		// 暴击--(物理、魔法、治疗)
	eHR_PhyDodge,	// 物理闪避--(物理)
	eHR_MagDodge,	// 魔法闪避--(魔法)
	eHR_Immune,		// 免疫(物理、魔法、DOT)
	eHR_Resist,		// 抵抗--(魔法)
	eHR_ComResist,	// 完全抵抗--(魔法)
	eHR_Parry,		// 招架--(物理)
	eHR_Block,		// 格挡--(物理)
	eHR_Hurt,		// 伤害
	eHR_SuckBlood,	// 吸血
	eHR_EmptyAni,	// 普攻动作挥空
	eHR_HurtExptDOT,		//非DOT伤害
	eSET_Attack,	// 攻击
	eSET_BeforeChangeHurt,			// 吸收前伤害
	eHR_End,
};

// 消耗类型
enum EConsumeType	
{
	eCT_None,		// 无消耗	
	eCT_HP,			// 血量值
	eCT_MP,			// 魔法值
	eCT_RG,			// 怒气值
	eCT_EG,			// 能量值		
	eCT_CP			// 连击点
};

// 流程类型
enum ECastingProcessType
{
	eCPT_Wink,				// 瞬发
	eCPT_Sing,				// 吟唱	
	eCPT_Channel,			// 引导
	eCPT_GradeSing,			// 蓄力吟唱
	eCPT_ImmovableSing		// 硬直吟唱
};

enum ECastingInterruptType
{
	eCIT_None,
	eCIT_Move,
	eCIT_TurnAround
};

// 姿态类型
enum EStanceType
{
	eSS_Stance	=	1,	//姿态	
	eSS_MutexStance,	//互斥姿态   
	eSS_Aure,			//光环
	eSS_Form,			//变身
	eSS_Shield,			//盾
	eSS_CascadeAure,	//叠加光环
	eSS_InvisibleAure,	//隐形光环

};

// 魔法状态总类别
enum EStateGlobalType
{
	eSGT_Undefined,
	eSGT_MagicState,
	eSGT_TriggerState,
	eSGT_DamageChangeState,
	eSGT_CumuliTriggerState,
	eSGT_SpecialState,
	eSGT_End
};

//触发状态类型
enum ETriggerStateType
{
	eTST_All,
	eTST_Trigger,
	eTST_DamageChange,
	eTST_Special,
};

// 魔法状态编号起始值
enum EStateIdFloor
{
	eSIC_TriggerState		= 1,
	eSIC_DamageChangeState	= 10001,
	eSIC_CumuliTriggerState = 19001,
	eSIC_SpecialState		= 20001,
	eSIC_MagicState			= 30001,
	eSIC_DynamicMagicState	= 100001
};

// 状态的负面效果类型
enum EDecreaseStateType
{
	eDST_Null,					//无类型
	eDST_Riding,				//坐骑
	eDST_FeignDeath,			//假死
	eDST_Increase,				//正面
	eDST_TouchBattleStateBegin,	//进入战斗状态判断开始分界线
	//eDST_IncreaseTouchBattleState,	//正面进战斗
	eDST_IncreaseEnd,			//正面类型结束判断分界线
	eDST_Control,				//控制
	eDST_Pause,					//定身
	eDST_Crippling,				//减速
	eDST_Debuff,				//负面
	eDST_DOT,					//伤害
	eDST_Special,				//特殊
};

// 魔法状态叠加规则
enum EReplaceRuler
{
	eRR_KeepCurState	= 0,	// 叠加到上限不再叠加后续效果
	eRR_Refresh		= 1,		// 叠加过程累加效果，刷新时间，到上限只刷新时间
};

// 用于普通魔法状态的群体叠加类型
enum ECascadeType
{
	eCT_Central = 1,			// 集中
	eCT_Decentral = 2,			// 分散
	eCT_Unique = 3,				// 唯一
	eCT_Share = 4,				// 共享（类似于集中，但必须共同撤销）

};

// 特殊魔法状态的类型
enum ESpecialStateType
{
	eSST_Reflect,				//反射魔法
	eSST_DOTImmunity,			//DOT免疫
	eSST_DirMove,				//方向移动
	eSST_DeadBody
};

// 特殊触发事件类型
enum ESpecialEventType
{
	eSET_Start			= 0,		//枚举开始符号
	eSET_SubMP,						//减蓝
	eSET_SubEP,						//减能量
	eSET_SubRP,						//减怒气
	eSET_SubCP,						//减连击
	eSET_Heal,						//治疗
	eSET_SuckBlood,					//吸血
	eSET_HPChanged,					//生命值改变

	eSET_DoSkill,					// 施放技能
	eSET_DoFightSkill,			//施放战斗技能
	eSET_DoNatureMagic,				// 施放自然系法术
	eSET_DoDestructionMagic,		// 施放破坏系法术
	eSET_DoEvilMagic,				// 施放暗黑系法术
	//eSET_Attack,					// 攻击
	eSET_ShortNormalAttack,			// 普通攻击
	eSET_ShortNormalAttackDamage,
	eSET_NormalAttackStrike,
	eSET_ShortNormalAttackSingletonDamage,
	eSET_LongNormalAttackDamage,
	eSET_MainHandNADamage,
	eSET_InstallerDie,				//安装者死亡
	eSET_Kill,						//杀死前
	eSET_Killed,					//杀死后
	eSET_KillNPC,
	eSET_KillPlayer,
	eSET_KillByInstaller,			//被安装者杀死
	eSET_CancelSpecifiedNoSingTime,	//指定的吟唱无时间用完
	eSET_CancelAnyNoSingTime,		//任何吟唱无时间用完
	eSET_LeftBattle,				//离开战斗
	eSET_EnterBattle,				//进入战斗
	eSET_RagePointIsFull,			//满怒气
	eSET_DeadlyHurt,				//致死伤害（死前）
	eSET_Clear,						//消失前
	eSET_RemoveFromMaster,			//与主人解除关系
	eSET_AttackByEnemyObjectSkill,	//单攻技能攻击,无视免疫
	eSET_MoveBegin,					//开始移动
	eSET_MoveEnd,					//停止移动
	eSET_WeaponModChange,			//武器模型改变
	eSET_SetupWeapon,				//装备武器
	eSET_RemoveWeapon,				//撤销武器
	eSET_ChangeEquip,				//更换装备
	eSET_SetupControlState,			
	eSET_SetupCripplingState,		
	eSET_SetupPauseState,	
	eSET_SetupSpecialState,		
	eSET_InterruptMopHit,					//打断施法(流程)技命中
	//以下是给特殊状态用的
	eSET_DirectionOrMoveSpeedChange,
	eSET_BeforeChangeHeal,			// 吸收前治疗

	eSET_End			= 5000,		//枚举结束符，表示枚举的上限
};

//状态的（按图标）取消条件
enum EIconCancelCond
{
	eICC_None,				//所有人安装的状态都不可撤销
	eICC_Self,				//只有自己安装状态的可以撤销
	eICC_All,				//所有人安装的状态都能撤销
	eICC_End
};

// 魔法总类别
enum EMagicGlobalType
{
	eMGT_Bullet,
	eMGT_ShockWave,
	eMGT_Transferable,
	eMGT_Position,
	eMGT_Move,
	eMGT_Aure,
	eMGT_BattleArray,
	eMGT_Barrier
};

//阵法状态
enum EBattleArrayState
{
	eBAS_None=0,
	eBAS_InArray,
	eBAS_OutArray,
	eBAS_ReturnArray,
	eBAS_FinishArray
};


// 子弹轨迹类型
enum EBulletTrackType			
{
	eBTT_Line		=1,			// 平飞
	eBTT_Random,				// 随机移动
	eBTT_Parabola,				// 抛物线
	eBTT_MarblesSoul,			// 弹球爆魂
};


// 爆魂类型
enum EBurstSoulType
{
	eBST_Normal=1,
	eBST_Skill,
	eBST_Quest,
};
/*
* =======================================================
*					宠物召唤兽相关
* =======================================================
*/

// 宠物类型
enum EPetType
{
	EPT_SUMMON= 1,
	EPT_PET, 
	EPT_GUARDIAN,          
	EPT_MINI      
};

// 宠物状态
enum EPetState
{
	EPS_Attack= 1,
	EPS_Defense,   
	EPS_Passivity 
};

// 宠物移动状态
enum EPetMoveState
{
	EPMS_Attack= 1,
	EPMS_Follow,   
	EPMS_Stay 
};

// 战斗信息Index
enum EFightInfoIndex
{
	EFII_Hurt = 1,
	EFII_StrikeHurt,
	EFII_Miss,
	EFII_Resist,
	EFII_Parry,
	EFII_Dodge,
	EFII_SetUpState,
	EFII_ClearState,
	EFII_Block,
	EFII_ComResist,
	EFII_Immune,
	EFII_NonTypeAttackHit,
	EFII_StateEffectHit,
	EFII_MultiAbsorb,
	EFII_Heal,
	EFII_StrikeHeal,
	EFII_DoSkill,
	EFII_DoPosSkill,
	EFII_RenewMP,
	EFII_ConsumeMP,
	EFII_LostMP,
	EFII_ConsumeHP,
	EFII_SuccessDoCasting,
	EFII_InterruptCasting,
	EFII_RenewRP,
	EFII_Die,
	EFII_ConsumeRP,
	EFII_AddBurstSoulTimes,
	EFII_LostRP,
	EFII_StateEffectStrikeHit,
};

//状态强制存类型
enum EForceSaveOrForceNoSave
{
	eFSOFNS_ForceNoSave,
	eFSOFNS_ForceOfflineSave,
	eFSOFNS_ForceOnTickSave
};

/*
* =======================================================
*					非战斗行为相关
* =======================================================
*/
enum NonCombatBehavior
{
	nCB_ItemSkill= 1,
	nCB_ExpModulusChange,
	nCB_ExpModulusInFBChange,
	nCB_FetchModulusChange,
	nCB_Max
};

enum EAlterNormalAttackType
{ 
	eANAT_NoChange, 
	eANAT_StartAndAutoTrack, 
	eANAT_Start, 
	eANAT_AbsolutelyStartAndAutoTrack, 
	eANAT_AbsolutelyStart, 
	eANAT_Cancel,
	eANAT_End
};
