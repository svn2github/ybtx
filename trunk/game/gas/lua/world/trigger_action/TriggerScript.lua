gas_require "world/tong_area/DynamicCreateMgr"
cfg_load "trigger/TriggerEvent_Server"

CTriggerScript = class()

function CTriggerScript:RunScriptByIndex(Index, Trigger, Player, Arg)
	local info = TriggerEvent_Server(tostring(Index))
	if not info then
--		print(Index.."在TriggerEvent_Server不存在")
		return {Index.."在TriggerEvent_Server不存在"}
	end
	local arg = GetCfgTransformValue(true, "TriggerEvent_Server", tostring(Index), "Arg")
	if Arg then	--扩展args
		for i=1, #arg do
			for index, value in pairs(Arg) do
				arg[i][index] = value
			end
		end
	end
	self:RunScript(info("Script"), arg, Trigger, Player)
end

function CTriggerScript:RunScript(TriggerAction, Arg, Trigger, Player)
	if not self.ScriptFun[TriggerAction] then
		CfgLogErr("触发脚本填写出现错误!", TriggerAction.." 触发脚本不存在!")
		return
	end
	for i, v in pairs(Arg) do
		self.ScriptFun[TriggerAction](v, Trigger, Player)
	end	
end

function CTriggerScript:Ctor()

local AllScript =
{
	--通用脚本?
	["创建"] = "world/trigger_action/trigger_script/CreateOnPos",
	["自毁"] = "world/trigger_action/trigger_script/DeleteMySelf",
	
	["触发剧场"] = "world/trigger_action/trigger_script/TriggerTheater",
	["剧场解锁"] = "world/trigger_action/trigger_script/UnLockTheater",
	
	--["增加任务计数"] = "world/trigger_action/trigger_script/AddQuestNum",
	["触发隐藏任务"] = "world/trigger_action/trigger_script/ActivationHideQuest",
	["副本用清怪"] = "world/trigger_action/trigger_script/DelClearObject",
	["副本用尸体复活"] = "world/trigger_action/trigger_script/ReplaceAllObject",
	["副本用倒计时"] = "world/trigger_action/trigger_script/GameCountdownStart",
	["关闭倒计时"] = "world/trigger_action/trigger_script/GameCountdownOver",
	["场景状态"] = "world/trigger_action/trigger_script/SceneChangeState",
	["计数触发"] = "world/trigger_action/trigger_script/TriggerCountNpc",
	["定时触发"] = "world/trigger_action/trigger_script/TimeTriggerStart",
	["随机选择"] = "world/trigger_action/trigger_script/RandomSelect",
	["创建动态摆怪文件"] = "world/trigger_action/trigger_script/CreateDynamicFile",
	
	["领域玩家死亡"] = "world/trigger_action/trigger_script/TriggerPlayerDead",
	["触发事件"] = "world/trigger_action/trigger_script/TriggerEvent",
	["给玩家发消息"] = "world/trigger_action/trigger_script/TriggerSendMsg",
	--["重置军资石"] = "world/trigger_action/trigger_script/ResetResourceStone",
	["设置比赛本面板文字"] = "world/trigger_action/trigger_script/SetMatchGameText",
	["释放技能"] = "world/trigger_action/trigger_script/DoSkill",
	
	--Npc专属脚本
	
	["添加动态路径"] = "world/trigger_action/npc_script/AddDynamicPath",
	["添加修行塔动态路径"] = "world/trigger_action/npc_script/AddMercenaryDynamicPath",
	["随机添加动态路径"] = "world/trigger_action/npc_script/RandomAddDynamicPath",
	["修行塔随机添加动态路径"] = "world/trigger_action/npc_script/YbEduRandomAddDynamicPath",
	["选取最近路径"] = "world/trigger_action/npc_script/ChooseShortCut",
	["修行塔选取最近路径"] = "world/trigger_action/npc_script/YbEduChooseShortCut",
	
	["锁定目标"] = "world/trigger_action/npc_script/AimLock",
	["延时锁定目标"] = "world/trigger_action/npc_script/DelayAimLock",
	["设置朝向"] = "world/trigger_action/npc_script/SetDirection",
	["占领"] = "world/trigger_action/npc_script/Occupy",
	["小游戏动态刷新"] = "world/trigger_action/npc_script/CreateNpcByGame",
	["烧树专用脚本"] = "world/trigger_action/npc_script/CreateBossAndTrap",
	["随机移动"] = "world/trigger_action/npc_script/BornRandomMove",
	["头顶飘字"] = "world/trigger_action/npc_script/PlayHeadEffect",
	["挑战结束"] = "world/trigger_action/npc_script/ChallengeOver",
	["建筑打爆"] = "world/trigger_action/npc_script/BuildingDropResource",
	["Npc驻地建筑打爆"] = "world/trigger_action/npc_script/StationNpcDropResource",
	
	["切换背景音乐"] = "world/trigger_action/npc_script/ChangeBackgroundMusic",
	["还原背景音乐"] = "world/trigger_action/npc_script/RevertBackgroundMusic",
	
	--有Player的情况下可触发
	--["答题"] = "废弃脚本,客户端已经截住答题了",
	["小游戏"] = "smallgames/NpcSmallGameManager",
	["抱"] = "world/trigger_action/trigger_script/Cuddle",
	["获得物品"] = "world/trigger_action/trigger_script/GetItem",
	["替换"] = "world/trigger_action/trigger_script/ReplaceSelf",
	["阵营替换"] = "world/trigger_action/trigger_script/ReplaceByCamp",
	
	["增加团贡"] = "world/trigger_action/trigger_script/AddTongProffer",
	["传送"] = "world/trigger_action/trigger_script/Transport",
	--["小副本加计数"] = "world/trigger_action/trigger_script/SmallFbAddNum",
	["踩Trap给奖励"] = "world/trigger_action/trigger_script/TriggerPlayerAward",
	--["带buff进Trap加活动计数"] = "world/trigger_action/trigger_script/AddNumByBuff",
	["进Trap改变箱子状态"] = "world/trigger_action/trigger_script/InChangeBoxState",
	["出Trap改变箱子状态"] = "world/trigger_action/trigger_script/OutChangeBoxState",
	["加生命值"] = "world/trigger_action/trigger_script/TriggerAddHp",
	["加计数"] = "world/trigger_action/trigger_script/AddNumByType",
	["抢占野外复活点"] = "world/trigger_action/trigger_script/RobRebornPlace",
	["删除"] = "world/trigger_action/trigger_script/TriggerDelete",
	["传送到指定位置"] = "world/trigger_action/trigger_script/TransSamePos",
	["指南针"] = "world/trigger_action/trigger_script/TriggerCompass",
	["龙穴活动"] = "world/trigger_action/npc_script/DragonCaveBossDead",

}
	self.ScriptFun = {}
	AddCheckLeakFilterObj(self.ScriptFun)
	self.ScriptFun["停止触发"] = function(Arg, Trigger) Trigger.m_StopTrigger= true end
	for ScriptName,fileName in pairs(AllScript) do
		self.ScriptFun[ScriptName] = gas_require (fileName)
		assert(IsFunction(self.ScriptFun[ScriptName]), fileName.." 文件中函数不正确")
	end
end
