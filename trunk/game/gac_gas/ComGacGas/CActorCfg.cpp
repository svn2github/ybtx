#include "stdafx.h"
#include "CActorCfg.h"

CActorCfg::MapActionState CActorCfg::ms_mapActionState;

void CActorCfg::BuildMapString2Enum()
{
	ms_mapActionState["死亡倒地"]	=	eAS_Die;
	ms_mapActionState["死亡躺地"]	=	eAS_Dead;
	ms_mapActionState["无武器站"]	=	eAS_Idle_BackWpn;
	ms_mapActionState["持武器站"]	=	eAS_Idle_HoldWpn;
	ms_mapActionState["战斗站立"]	=	eAS_Idle_Battle;
	ms_mapActionState["无武器走"]	=	eAS_Walk_BackWpn;
	ms_mapActionState["持武器走"]	=	eAS_Walk_HoldWpn;
	ms_mapActionState["战斗倒走"]	=	eAS_Walk_Battle;
	ms_mapActionState["无武器跑"]	=	eAS_Run_BackWpn;
	ms_mapActionState["持武器跑"]	=	eAS_Run_HoldWpn;
	ms_mapActionState["战斗跑动"]	=	eAS_Run_Battle;

	ms_mapActionState["向右攻击"]	=	eAS_Attack_Right;
	ms_mapActionState["向左攻击"]	=	eAS_Attack_Left;
	ms_mapActionState["全身攻击"]	=	eAS_Attack_All;
	ms_mapActionState["全身暴击"]	=	eAS_Strike;
	ms_mapActionState["坐骑攻击"]	=	eAS_Attack_Ride;
	ms_mapActionState["副手攻击"]	=	eAS_Attack_Assist;
	ms_mapActionState["拳头攻击"]	=	eAS_Attack_Fist;
	ms_mapActionState["向后被击"]	=	eAS_Suffer_Back;
	ms_mapActionState["向左被击"]	=	eAS_Suffer_Left;
	ms_mapActionState["向右被击"]	=	eAS_Suffer_Right;
	ms_mapActionState["躲闪"]		=	eAS_Dodge;
	ms_mapActionState["格挡"]		=	eAS_Block;
	ms_mapActionState["招架"]		=	eAS_Parry;
	ms_mapActionState["击晕"]		=	eAS_Stun;
	ms_mapActionState["击倒"]		=	eAS_HitDown;
	ms_mapActionState["睡眠"]		=	eAS_Sleep;
	ms_mapActionState["连击"]		=	eAS_Combo;
	ms_mapActionState["回旋击"]		=	eAS_Whirl;
	ms_mapActionState["拔武器"]		=	eAS_HoldWeapon;
	ms_mapActionState["收武器"]		=	eAS_BackWeapon;
	ms_mapActionState["起手"]		=	eAS_ReachUp;
	ms_mapActionState["引导"]		=	eAS_Channel;
	ms_mapActionState["吟唱"]		=	eAS_Sing;
	ms_mapActionState["出手"]		=	eAS_Cast;
	ms_mapActionState["移动开始"]	=	eAS_MoveStart;
	ms_mapActionState["移动过程"]	=	eAS_MoveProcess;
	ms_mapActionState["移动结束"]	=	eAS_MoveEnd;

	ms_mapActionState["捡起"]		=	eAS_Take;
	ms_mapActionState["扔出"]		=	eAS_Fire;
	ms_mapActionState["脚踢"]		=	eAS_Kick;
	ms_mapActionState["采集"]		=	eAS_Gather;
	ms_mapActionState["拾取"]		=	eAS_PickUp;
	ms_mapActionState["坐下"]		=	eAS_SitDown;
	ms_mapActionState["坐着"]		=	eAS_Sitting;
	ms_mapActionState["进食"]		=	eAS_TakeFood;
	ms_mapActionState["物品瞬发"]	=	eAS_Item_Use;
	ms_mapActionState["物品吟唱"]	=	eAS_Item_Sing;
	ms_mapActionState["物品出手"]	=	eAS_Item_Cast;
	ms_mapActionState["物品引导"]	=	eAS_Item_Channel;
	ms_mapActionState["表情起始"]	=	eAS_Fun_Start;
	ms_mapActionState["表情保持"]	=	eAS_Fun_Keep;

	ms_mapActionState["甩竿坐下"]	=	eAS_Throw;
	ms_mapActionState["等待咬钩"]	=	eAS_WaitBite;
	ms_mapActionState["不老实1"]	=	eAS_Disturbed01;
	ms_mapActionState["不老实2"]	=	eAS_Disturbed02;
	ms_mapActionState["不老实3"]	=	eAS_Disturbed03;
	ms_mapActionState["鱼咬钩"]		=	eAS_Bite;
	ms_mapActionState["等待拽竿"]	=	eAS_WaitPull;
	ms_mapActionState["拽竿1"]		=	eAS_Pull01;
	ms_mapActionState["拽竿2"]		=	eAS_Pull02;
	ms_mapActionState["拽竿3"]		=	eAS_Pull03;
	ms_mapActionState["钓鱼成功1"]	=	eAS_Succeed01;
	ms_mapActionState["钓鱼成功2"]	=	eAS_Succeed02;

	ms_mapActionState["采矿"]		=	eAS_PickOre;

	ms_mapActionState["出生动作"]	=	eAS_Birth;
	ms_mapActionState["初始动作一"]	=	eAS_Still_1;
	ms_mapActionState["初始动作二"]	=	eAS_Still_2;
	ms_mapActionState["初始动作三"]	=	eAS_Still_3;
	ms_mapActionState["交互开始"]	=	eAS_Respond_Enter;
	ms_mapActionState["交互保持"]	=	eAS_Respond_Keep;
	ms_mapActionState["交互结束"]	=	eAS_Respond_Leave;
	ms_mapActionState["特殊待机"]	=	eAS_Idle_Special;
	ms_mapActionState["特殊待机循环"]	=	eAS_Idle_Special_Loop;
	ms_mapActionState["特殊倒地"]	=	eAS_Die_Special;
	ms_mapActionState["特殊躺地"]	=	eAS_Dead_Special;
	ms_mapActionState["向右踱步"]	=	eAS_Pace_Right;
	ms_mapActionState["向左踱步"]	=	eAS_Pace_Left;
	ms_mapActionState["谈话"]		=	eAS_Talk;

	ms_mapActionState["假死"]		= eAS_Die;
	ms_mapActionState["静止"]		= eAS_HoldStill;
}

void CActorCfg::CleanMap()
{
	ms_mapActionState.clear();
}

EActionState CActorCfg::GetEnumByString(const TCHAR* strAct)
{
	MapActionState::iterator it = ms_mapActionState.find(strAct);
	if (it != ms_mapActionState.end())
	{
		return it->second;
	}
	return eAS_Error;
}
