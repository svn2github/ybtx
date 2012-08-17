#pragma once

enum ENpcMoveType
{
	ENpcMove_None,				//固定
	ENpcMove_Cycle,				//循环巡逻
	ENpcMove_Trip,				//往返巡逻
	ENpcMove_Random,			//随即移动
	ENpcMove_SingleLine,		//单线巡逻
	ENpcMove_Single2Random,		//先单线后随机
	ENpcMove_MoveInPath,		//组合路径巡逻
};

enum ENpcEnmityType
{
	ENpcEnmityType_None,			
	ENpcEnmityType_Nomal,			//普通Npc
	ENpcEnmityType_Dummy,			//群组控制器
	ENpcEnmityType_Member,			//群组成员
};

enum ENpcAIDestructEsg
{
	ENpcAIDestructEsg_False,
	ENpcAIDestructEsg_True,		//析构NpcAI的AOI回调消息
};

enum ENpcTargetType
{
	ENpcTargetType_AllTarget,			//所有目标
	ENpcTargetType_AllPlayer,			//所有玩家
	ENpcTargetType_AllNpc,				//所有Npc
	ENpcTargetType_AppointNpc,			//指定Npc
};
