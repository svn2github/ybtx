g_AllObjScript = 
{
	["开邮箱"]     =	"world/trigger_action/obj/MailBox/MailBoxServer",
	["召唤队友"] =	"world/trigger_action/obj/CallPlayerObj/IntObjCallPlayerServer",
	["传送"]	 		= "world/trigger_action/obj/Transport",
	["领域本传送"] = "world/trigger_action/obj/Scopes_Transport",
	["佣兵任务池"] = "world/trigger_action/obj/mercenary_questpool/MercenaryQuestPool",
	["有序点击"] = "world/trigger_action/obj/OrderClick",
	["所在副本列表"] = "world/trigger_action/obj/fb_list/FbList",
	--自动拾取的OBJ
	["可拾取的Obj"]     = "world/trigger_action/obj/ItemObj",   --苹果等点击后可以直接进入背包的Obj
	["拾取矿石"]     = "world/trigger_action/obj/ore_obj/CollectOreObj",
	["采集矿脉"]     = "world/trigger_action/obj/CollObj/IntObjCollObjServer",
	["可拾取的Obj临时技"]	= "world/trigger_action/obj/TemporaryBuffObj",
	["神符"] = "world/trigger_action/obj/BuffObj",
	["临时技神符"] = "world/trigger_action/obj/TemporaryBuffObj",
	---------------
	["活动报名Obj"]		= "world/trigger_action/obj/SignUpObj",
	["发起挑战"]		= "world/trigger_action/obj/ChallengeFlagObj",
	["任务Obj"]		= "world/trigger_action/obj/QuestObjServer",
	["采集物品"] 	= "world/trigger_action/obj/CollObj/IntObjCollObjServer",
	["副本采集物品"] 	= "world/trigger_action/obj/CollObj/FbCollObj",
	["读条采集物品Obj保留"] 	= "world/trigger_action/obj/CollObj/IntObjCollObjServer",
	["读条采集物品Obj消失"] 	= "world/trigger_action/obj/CollObj/IntObjCollObjServer",
	["读条完美采集物品Obj消失"] 	= "world/trigger_action/obj/CollObj/IntObjCollObjServer",
	
	["不读条采集物品Obj消失"] 	= "world/trigger_action/obj/CollObj/IntObjCollObjServer",
	["读条完美连击计数"] 	= "world/trigger_action/obj/CollObj/IntObjPerfectCollServer",
	["公告牌"] = "world/trigger_action/obj/BulletinBoard",
	["小游戏"] = "smallgames/ObjSmallGameManager",
	["魔法泉"] = "world/trigger_action/obj/MoFaQuanObj/MoFaQuanObj",
	["加任务计数"] = "world/trigger_action/obj/AddQuestCount",
	["加任务计数清除Obj"] = "world/trigger_action/obj/AddQuestCount",
	["收集资源"] = "world/trigger_action/obj/CollectRes",
	["非战斗技能学习"] = "world/trigger_action/obj/LearnNoneFightSkill",
	---------非战斗行为-------------
	--手部动作调用
	["抱"] = "world/trigger_action/obj/action/HandBehavior",
	["举"] = "world/trigger_action/obj/action/HandBehavior",
	["捡"] = "world/trigger_action/obj/action/HandBehavior",
	--脚部动作调用
	["蓄力踢Obj保留"] = "world/trigger_action/obj/action/FeetBehavior",
	["蓄力踢Obj消失"] = "world/trigger_action/obj/action/FeetBehavior",
	["小踢Obj保留"] = "world/trigger_action/obj/action/FeetBehavior",
	["小踢Obj消失"] = "world/trigger_action/obj/action/FeetBehavior",
	-----------------------------------
	["钓鱼"] = "world/trigger_action/obj/HeadFishingObj",
	["挑战占领"] = "world/trigger_action/obj/ChallengeOccupy",
	["临时包拾取OBJ"] = "world/trigger_action/obj/CollectTempBag",
	["刷出奖励NPC"] = "world/trigger_action/obj/CreateAwardNpc",
	["读条抢夺资源点"] 	= "world/trigger_action/obj/RobResource",
	["地下城计数"] = "world/trigger_action/obj/AreaFbTakeCount",
	["读条不被攻击打断"] 	= "world/trigger_action/obj/CollObj/IntObjCollObjServer",
}

--需求分配全局表
--{[PlayerID] = {[ObjID] = Time, [ObjID1] = Time1}, [PlayerID1] = {[ObjID] = Time, [ObjID1] = Time1}}
g_NeedAssignTbls = {}
g_AcutionAssignTbls = {}
g_AcutionAssignTeamTbls = {}
RegMemCheckTbl("NeedAssignTbls", g_NeedAssignTbls)
RegMemCheckTbl("AcutionAssignTbls", g_AcutionAssignTbls)
RegMemCheckTbl("AcutionAssignTeamTbls", g_AcutionAssignTeamTbls)