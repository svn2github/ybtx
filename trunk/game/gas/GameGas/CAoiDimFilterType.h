#pragma once

//维度类型，这个必须固定死
enum EAoiDimType
{
	eADT_Player,						//玩家层								需要隐身
	eADT_Magic,							//魔法（子弹、光环、冲击波）层
	eADT_RefMagic,						//反弹魔法（反弹魔法的魔法）层
	eADT_Servant,						//召唤兽层								需要隐身
	eADT_Trap,							//陷阱层
	eADT_TrapSeePlayer,					//只能看见玩家的陷阱层
	eADT_TrapSeeNpc,					//只能看见Npc的陷阱层
	eADT_InteractObj,					//交互NPC/IntObj的战斗层				需要隐身
	eADT_VisiPlayerObj,					//可看见玩家的NPC/IntObj的战斗层		需要隐身
	eADT_VisiPlayerObjInterested,		//可看见玩家的NPC/IntObj的兴趣层		需要隱身
	eADT_InvisiPlayerObj,				//不可看见玩家的NPC/IntObj的战斗层
	eADT_InvisiPlayerObjInterested,		//不可看见玩家的NPC/IntObj的兴趣层
	eADT_Count,							//类型总数
	eADT_Undefined						//未定义
};

//看见类型，这个目前也要固定死，因为实现依赖于维度类型
enum EAoiItemEyeSightType
{
	eAIET_Player,						//玩家
	eAIET_Magic,						//普通魔法
	eAIET_RefMagic,						//反弹魔法
	eAIET_Servant,						//召唤兽
	eAIET_Trap,							//陷阱看见所有玩家和Npc
	eAIET_TrapSeePlayer,				//陷阱只能看见玩家
	eAIET_TrapSeeNpc,					//陷阱只能看见Npc
	eAIET_InteractObj,					//交互NPC/IntObj
	eAIET_VisiPlayerObj,				//可看见玩家的NPC/IntObj（只有第一个EyeSight）（主动怪、对玩家的陷阱IntObj、需要看玩家的剧场NPC）
	eAIET_DoubleEyeSightVisiPlayerObj,	//可看见玩家的NPC/IntObj（有两个不同的EyeSight）（有兴趣设定的主动怪）
	eAIET_InvisiPlayerObj,				//不可看见玩家的NPC/IntObj（只有第一个EyeSight）（被动怪、对NPC的陷阱IntObj、不需要看玩家的剧场NPC）
	eAIET_DoubleEyeSightInvisiPlayerObj,//不可看见玩家的NPC/IntObj（有两个不同的EyeSight）（有兴趣设定的被动怪）
	eAIET_InterestedObj,				//纯兴趣NPC/IntObj（只有第二个EyeSight）（有兴趣设定的任务NPC）
	eAIET_BlindObj,						//看不见任何东西的NPC/IntObj（纯任务NPC、普通IntObj）
	eAIET_Count,						//类型总数
	eAIET_Undefined						//未定义
};

//被看见类型，这个可以做成配置表调配
enum EAoiItemSizeType
{
	eAIST_PlayerOrServant,				//玩家
	eAIST_Magic,						//普通魔法
	eAIST_TaskNPC,						//任务NPC（只能被玩家和陷阱看见）
	eAIST_IntObj,						//普通物品（只能被玩家看见）
	eAIST_Monster,						//普通怪（只能被玩家、召唤兽、魔法看见）
	eAIST_InteractObj,					//交互NPC/IntObj
	eAIST_InterestingMonster,			//被个性关注的普通怪
	eAIST_InterestingNPC,				//被个性关注的NPC（比如走近会被打招呼的任务NPC）
	eAIST_InterestingIntObj,			//被个性关注的IntObj（比如嗜肉个性里的肉）
	eAIST_DeadPlayer,					//死亡后的玩家
	eAIST_SleepNPC,						//休眠NPC（不能被任何东西看见）
	eAIST_Count,						//类型总数
	eAIST_Undefined						//未定义
};

