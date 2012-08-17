gas_require "world/trap/trap_event/AllTrapEvent"

InTrapEvent = 
{
	["调用技能"] = CCommonTrap,
	["调用不清除技能"] = CNonCleanTrap,								--调用技能出陷阱不清除状态
	["加临时技能"] = CAddTemporarySkill,			--加临时技能到临时技能栏
	["调用地点技能"] = CDoSkillOnPos,			--对地点释放技能
	["改变箱子状态"] = CChangeBoxStateTrap,
	["传送"] = CTransportTrap,
	--活动专用传送点------------
	["大逃杀传送"] = CDaTaoShaTransportTrap,
	["大夺宝传送"] = CDaDuoBaoTransportTrap,
	["佣兵训练传送"] = CYbEducateActionTrap,
	["佣兵训练活动加计数"] = CYbEducateActionAddNum,
	-------------------------------
	["特殊领域传送"] = CSpecilAreaTrap,--领域传送
	["进Trap取消状态"] = CInTrapCancelBuf,
	["玩家踩Trap加任务计数"] = CPlayerInTrapAddVar,
	["Npc踩天然Trap加任务计数"] = CNpcInExistTrapAddVar,
	["Npc踩摆放Trap加任务计数"] = CNpcInPutTrapAddVar,
	["玩家踩Trap给予物品"] =  CPlayerInTrapAddRes,
	["玩家踩Trap给钱"] =  CPlayerAddMoney,
	["玩家踩Trap给经验"] =  CPlayerAddExper,
	["玩家踩Trap传送到指定位置"] =  CPlayerTransport,
	["Npc踩天然Trap给予物品"] =  CNpcInExistTrapAddRes,
	["Npc踩摆放Trap给予物品"] =  CNpcInPutTrapAddRes,
	----------------------------------------
	--这两个还没有用到，主要是合并以前功能用的
	["踩Trap加任务计数"] = CInPutTrapAddVar,
	["踩Trap给予物品"] = CInPutTrapAddItem,
	------------------------------------------
	["陷阱自删除"] = CDestroyMySelf,
	["触发剧场"] = CTriggerTheater,
	["Npc进入Trap删除Npc"] = CNpcInTrapDeleteNpc,
	["Player进入Trap刷NpcOrObj"] = CPlayerInTrapAddNpcOrObj,
	["小副本加计数"] = CSmallEctypeAddCount,
	["测试用地图发消息"] = CInTrapSendMsg,
	--------------------------------------------
	["指定刷新"] = CCreateNpcOnPos,
	["替换"] = CReplaceNpc,
	["加生命"] = CAddHp,
	["指定对象进入Trap自删除"] = CNamedObjInPutTrapDelete,
	["Npc进入Trap自删除"] = CNpcIntoTrapDeleted,
	["抢占野外复活点"] = CRapRebornPoint,
	["指定对象进入Trap自删除"] = CNamedObjInPutTrapDelete,
	["MatchGame加计数"] = CMatchGameAddCount,
	["Npc离开Trap自删除"] = CNpcOutTrapDeleteNpc,
	["虫洞虫子跑出计数"] = CBugRunOut,
	["佣兵训练不同Npc加不同计数"]  = CAddDifferentCount,
	["佣兵训练不同状态加不同计数"] = CAddDifferentStateCount,
	["相同Npc加计数"] = CAddSameNpcCount,
	["佣兵训练活动Npc加计数"] = CYbActionNpcAddNum,
	["地下城传送"] = CAreaFbTransportTrap,
	["进入帮会驻地"] = CChangeTongSceneByTrap,
	["带buff进Trap加活动计数"] = CTakeBuffAddCount,
}
