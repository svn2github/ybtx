#pragma once

// 角色移动状态
enum EMoveState
{ 
	eMS_LockStop,		// 0	Die
	eMS_Stop,			// 1	Idle
	eMS_Move,			// 2	Walk、Run
	eMS_LockMove,		// 3	Channel、Jump、HitFly、HitMove
	eMS_Max				// Max
};

// 角色行为状态
enum EActionState
{ 
	eAS_Error,
	// 玩家基础
	eAS_Die,				// 死亡倒地
	eAS_Dead,				// 死亡躺地
	eAS_Idle_BackWpn,		// 无武器站
	eAS_Idle_HoldWpn,		// 持武器站
	eAS_Idle_Battle,		// 战斗站立
	eAS_Walk_BackWpn,		// 无武器走
	eAS_Walk_HoldWpn,		// 持武器走
	eAS_Walk_Battle,		// 战斗倒走
	eAS_Run_BackWpn,		// 无武器跑
	eAS_Run_HoldWpn,		// 持武器跑
	eAS_Run_Battle,			// 战斗跑动

	// 玩家战斗
	eAS_Attack_Right,		// 向右攻击
	eAS_Attack_Left,		// 向左攻击
	eAS_Attack_All,			// 全身攻击
	eAS_Strike,				// 全身暴击
	eAS_Attack_Assist,		// 副手攻击
	eAS_Attack_Ride,		// 坐骑攻击
	eAS_Attack_Fist,		// 拳头攻击
	eAS_Suffer_Back,		// 向后被击
	eAS_Suffer_Left,		// 向左被击
	eAS_Suffer_Right,		// 向右被击
	eAS_Dodge,				// 躲闪
	eAS_Block,				// 格挡
	eAS_Parry,				// 招架
	eAS_Stun,				// 击晕
	eAS_HitDown,			// 击倒
	eAS_Sleep,				// 睡眠
	eAS_Combo,				// 连击
	eAS_Whirl,				// 回旋击
	eAS_HoldStill,			// 静止
	eAS_ResumeAni,			// 恢复动作
	eAS_HoldWeapon,			// 拔武器
	eAS_BackWeapon,			// 收武器
	eAS_ReachUp,			// 起手
	eAS_Channel,			// 引导
	eAS_Sing,				// 吟唱
	eAS_Cast,				// 出手

	eAS_MoveStart,			// 移动开始	
	eAS_MoveProcess,		// 移动过程	
	eAS_MoveEnd,			// 移动结束	

	// 玩家非战斗
	eAS_Take,				// 捡起
	eAS_Fire,				// 扔出
	eAS_Kick,				// 脚踢
	eAS_Gather,				// 采集
	eAS_PickUp,				// 拾取
	eAS_SitDown,			// 坐下
	eAS_Sitting,			// 坐着
	eAS_TakeFood,			// 进食
	eAS_Item_Use,			// 物品瞬发
	eAS_Item_Sing,			// 物品吟唱
	eAS_Item_Cast,			// 物品出手
	eAS_Item_Channel,		// 物品引导
	eAS_Fun_Start,			// 表情起始
	eAS_Fun_Keep,			// 表情保持

	eAS_Throw,				// 甩竿坐下
	eAS_WaitBite,			// 等待咬钩
	eAS_Disturbed01,		// 不老实1
	eAS_Disturbed02,		// 不老实2
	eAS_Disturbed03,		// 不老实3
	eAS_Bite,				// 鱼咬钩
	eAS_WaitPull,			// 等待拽竿
	eAS_Pull01,				// 拽竿1
	eAS_Pull02,				// 拽竿2
	eAS_Pull03,				// 拽竿3
	eAS_Succeed01,			// 钓鱼成功1
	eAS_Succeed02,			// 钓鱼成功2

	eAS_PickOre,			// 采矿

	// NPC 独有
	eAS_Birth,				// NPC重生
	eAS_Still_1,			// NPC初始动作一
	eAS_Still_2,			// NPC初始动作二
	eAS_Still_3,			// NPC初始动作三
	eAS_Respond_Enter,		// NPC与玩家交互开始
	eAS_Respond_Keep,		// NPC与玩家交互中
	eAS_Respond_Leave,		// NPC与玩家交互结束
	eAS_Idle_Special,		// NPC特殊待机
	eAS_Idle_Special_Loop,	// NPC特殊待机循环
	eAS_Die_Special,		// NPC特殊倒地
	eAS_Dead_Special,		// NPC特殊躺地
	eAS_Pace_Right,			// NPC向右踱步
	eAS_Pace_Left,			// NPC向左踱步
	eAS_Talk,

	eAS_Max
};

// 动作播放类型
enum EActionPlayMode
{
	eAPM_AllBodyLoop,		// 全身循环
	eAPM_AllBodyOnce,		// 全身一次
	eAPM_UpBodyOnce			// 上身一次
};

// 动作播放优先级
enum EActionPlayPriority
{
	eAPP_NULL	= 0,
	eAPP_DeathAction,		// 死亡动作
	eAPP_CoerceAction,		// 强制动作
	eAPP_SkillAction,		// 技能动作
	eAPP_AttackAction,		// 攻击动作
	eAPP_SufferAction,		// 被击动作
	eAPP_MoveStopAction,	// 移动待机
	eAPP_SwitchAction,		// 切换动作
	eAPP_SpecialAction,		// 特殊动作
	eAPP_Max,
};

enum EFxType
{
	eFT_None,
	eFT_Leading,			//引导特效
	eFT_Suffer,				//受击特效
	eFT_Local,				//本地特效
	eFT_LineDirection,	//连线方向特效
};

enum EWeaponPlayType
{
	eWPT_None,				// 无变形
	eWPT_Always,			// 一个动作循环播
	eWPT_BattleChange,		// 根据战斗转变动作
};

// 移动方式
enum EMoveMode
{
	eMM_Run,
	eMM_WalkBack,
};
