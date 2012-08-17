gac_gas_require "activity/Trigger/TrapTrigger"
gac_gas_require "activity/Trigger/ObjTrigger"
gac_gas_require "activity/item/ItemUseInfoMgr"
gas_require "fb_game/random_game/RandomGameMgr"
gas_require "world/trigger_action/item/ItemLoadProgress"
gas_require "activity/NpcTalk"
cfg_load "item/SpecialItem_Server"
cfg_load "npc/Npc_Common"

CTriggerMgr = class()

function CTriggerMgr:Ctor()
	self.TriggerType = {}
	self.TriggerType[ECharacterType.Npc]		= self.NpcTrigger
	self.TriggerType[ECharacterType.IntObj]	= self.ObjTrigger
	self.TriggerType[ECharacterType.Trap]		= self.TrapTrigger
	self.TriggerType[ECharacterType.Player]	= self.PlayerTrigger
	
	self.TargetFun = {}
	self.TargetFun["自身"] = function(Trigger, Player) return Trigger end
	self.TargetFun["触发者"] = function(Trigger, Player) return Player end
	self.TargetFun["创建者"] = function(Trigger, Player) 
		local ID = Trigger.m_CreatorEntityID or Trigger.m_OwnerId
		if ID == nil then
			return
		end
		return CEntityServerManager_GetEntityByID(ID)
	end
	self.TargetFun["OnlyNpc"] = function(Trigger, Player, Name) 
		local SceneId = Trigger.m_Scene.m_SceneId
		return g_NpcBornMgr._m_OnlyNpc[SceneId][Name] 
	end
	
	self.ExpressionsFun = {}
	
	self:ItemUseInit()
end

function CTriggerMgr:ItemUseInit()
	self.m_ItemUse = {}
	self.m_ItemUse.m_ItemOnClicked = CDelegate:new()
	local entry = RequireSandBox("world/trigger_action/item/item_trigger/UseItemServer")
	if entry ~= nil then
		self.m_ItemUse.m_ItemOnClicked:Add(entry.Exec)
	end
	
	--经验球功能临时保留
	self.m_ItemUse.m_ExpItemOnClicked = CDelegate:new()
	entry = RequireSandBox("world/trigger_action/item/use_item_script/UseExpItem")
	if entry ~= nil then
		self.m_ItemUse.m_ExpItemOnClicked:Add(entry.Exec)
	end
	
	self.m_ItemUse.m_BigIdItemScript = {}
	local BigIdItemScript =
	{
		[30] = "world/trigger_action/item/use_item_script/UseOreAreaMap",
		[32] = "world/trigger_action/item/use_item_script/UseFlowerSeed",
		[37] = "world/trigger_action/item/use_item_script/UseMercCardItem",
		[45] = "world/trigger_action/item/use_item_script/UsePickOreItem",
		[46] = "world/trigger_action/item/use_item_script/UseMatchGameItem",
	}
	
	for ScriptName,fileName in pairs(BigIdItemScript) do
		self.m_ItemUse.m_BigIdItemScript[ScriptName] = gas_require (fileName)
		assert(IsFunction(self.m_ItemUse.m_BigIdItemScript[ScriptName]), fileName.." 文件中函数不正确")
	end
end

--触发总入口
function CTriggerMgr:Trigger(TriggerType, Trigger, Player)
	if not IsCppBound(Trigger) then
		return
	end
	local Type = Trigger.m_Properties:GetType()
	--print("CTriggerMgr:Trigger", Type, TriggerType)
	if Trigger.m_StopTrigger then
		return
	end
	
	if self.TriggerType[Type] then
		self.TriggerType[Type](self, TriggerType, Trigger, Player)
	end
end

-- 点击Npc触发行为
function CTriggerMgr.ClickNpc(Conn, NpcId)
	if not Conn.m_Player or (not IsCppBound(Conn.m_Player)) then
		return
	end
	local function IsFunOrQuestNpc(Npc, Conn)
		if not IsCppBound(Conn) or not IsCppBound(Conn.m_Player) then
			return false
		end
		local NpcName = Npc.m_Properties:GetCharName()
		local havefunc = false
		if g_WhereGiveQuestMgr["Npc"][NpcName] or g_WhereFinishQuestMgr["Npc"][NpcName] then
			havefunc = true
		end
		
		local ShowContent = g_NpcShowContentCfg[NpcName] 
		if ShowContent and ShowContent["对话框"] and ShowContent["对话框"].ShowContext ~= "" then
			havefunc = true
		end
		
		if not havefunc then
			local funcnametbl = Npc_Common(NpcName,"FuncName")
			if funcnametbl == "" then
				return false
			end
			funcnametbl = loadstring("return {"..funcnametbl.."}")()
			if funcnametbl[1] == "无" then
				return false
			end
		end
		local pos = CPos:new()
		Npc:GetGridPos( pos )
		if IfAcceptQuestArea(Conn.m_Player,pos) then
			local uEntityID	= Npc:GetEntityID()
			local uTongID	= Npc.m_TongID or 0
			Npc:TalkWithPlayerStart(Conn.m_Player:GetEntityID())
			ShowFuncNpcOrObjTalkWnd(Conn, uEntityID, uTongID)
		end
		return true
	end
	local Npc = CCharacterDictator_GetCharacterByID(NpcId)
	if not IsServerObjValid(Npc) then
		return
	end
	--如果该Npc是可以被接收的，则先进入接收流程
	if Npc.m_CanBeTakeOver ~= nil and type(Npc.m_CanBeTakeOver) == "table" then
		Gas2Gac:NotifyWhetherTakeOverNpc(Conn, Npc:GetEntityID())
		return
	end
	
	local PlayerCamp = Conn.m_Player:CppGetCamp()
	local PlayerGameCamp = Conn.m_Player:CppGetGameCamp()
	local NpcCamp = Npc:CppGetCamp()
	local NpcName = Npc.m_Properties:GetCharName()
	local NpcGameCamp = Npc:CppGetGameCamp()
	local PlayerId = Conn.m_Player.m_uID
	if not IsPasserbyCamp(NpcCamp, PlayerCamp, NpcGameCamp, PlayerGameCamp) then
		return
	end

	if GetNpcTriggerArg(NpcName, "点击") then
		local ConditionTbl = GetTriggerCondition("Npc", NpcName, "点击", "任务需求")
		local QuestNameTbl = ConditionTbl and ConditionTbl.Arg
		if QuestNameTbl ~= nil then
			if QuestNameTbl[1] and QuestNameTbl[1] ~= "" then
				
				local function OnClickNpc()
					local Player = g_GetPlayerInfo(PlayerId)
					if not Player then
						return
					end
					local Npc = CCharacterDictator_GetCharacterByID(NpcId)
					if not IsServerObjValid(Npc) then
						return
					end
					IsFunOrQuestNpc(Npc, Conn)
					g_TriggerMgr:Trigger("点击", Npc, Player)
				end
				if QuestNameTbl[2] == nil or QuestNameTbl[2] == "" then
					if CRoleQuest.DoingQuest_Check(Conn.m_Player, QuestNameTbl[1]) then
						OnClickNpc()
					end
				else
					local function CallBack(NeedNum)
						if NeedNum then
							OnClickNpc()
						end
					end
					local RoleQuestDB = "RoleQuestDB"
					local data = 
					{
						["sQuestName"] = QuestNameTbl[1],
						["sVarName"] = QuestNameTbl[2],
						["uCharId"] = PlayerId
					}
					CallAccountManualTrans(Conn.m_Account, RoleQuestDB,"CheckQuestVarNeedNum", CallBack, data)
				end
			end
		else
			IsFunOrQuestNpc(Npc, Conn)
			g_TriggerMgr:Trigger("点击", Npc, Conn.m_Player)
		end
	else
		IsFunOrQuestNpc(Npc, Conn)
	end
end

--Npc触发分支
--@param TriggerType	触发类型:出生,死亡,点击,生存期结束,销毁,脱离战斗,手,选择
--@param Npc					触发本体
--@param Player				触发者:{出生,生存期结束,销毁}时为nil值,{死亡,点击}时可能为触发事件的玩家
function CTriggerMgr:NpcTrigger(TriggerType, Npc, Player)
	--print("CTriggerMgr:NpcTrigger", TriggerType)
	
	RandomGameTrigger(TriggerType, Npc, Player)
	local NpcName = Npc.m_Properties:GetCharName()
--	local ShowCfg = g_NpcShowContentCfg[NpcName]
--	if ShowCfg and ShowCfg[TriggerType] then
		NpcShowContent(TriggerType, Npc) --出生说话
--	end
	
	--统一触发流程
	local ScriptTbl = GetNpcTriggerArg(NpcName, TriggerType)
	if not ScriptTbl then
		return
	end
	
	for i, v in pairs(ScriptTbl) do
	--需要加条件检测 不符合不能触发
--		print("CTriggerMgr:NpcTrigger", TriggerType, NpcName, i, Player and Player.m_uID)
		g_TriggerScript:RunScript(i, v.Arg, Npc, Player)
	end
end

--Obj触发分支
function CTriggerMgr:ObjTrigger(TriggerType, Obj, Player)
	--print("CTriggerMgr:ObjTrigger", TriggerType)
	
	local ObjName = Obj.m_Properties:GetCharName()
	
	local ScriptTbl = GetObjTriggerArg(ObjName, TriggerType)
	if not ScriptTbl then
		return
	end
	
	for i, v in pairs(ScriptTbl) do
		--需要加条件检测 不符合不能触发
--		print("CTriggerMgr:ObjTrigger", TriggerType, ObjName, v.Script, Player and Player.m_uID)
		g_TriggerScript:RunScript(v.Script, v.Arg, Obj, Player)
	end	
end

--Trap触发分支
--@param TriggerType	触发类型:出生,销毁,进,出
--@param Trap					触发本体
--@param Trigger			触发者:{出生,销毁}时为nil值,{进,出}时可能为触发事件的玩家或Npc
function CTriggerMgr:TrapTrigger(TriggerType, Trap, Trigger)
	--print("CTriggerMgr:TrapTrigger", TriggerType)
	
	local TrapName = Trap.m_Properties:GetCharName()
	
	local ScriptTbl = GetTrapTriggerArg(TrapName, TriggerType)
	if not ScriptTbl then
		return
	end
	
	for i, v in pairs(ScriptTbl) do
		--需要加条件检测 不符合不能触发
--		print("CTriggerMgr:TrapTrigger", TriggerType, TrapName, v.Script, Trigger and Trigger.m_uID)
		g_TriggerScript:RunScript(v.Script, v.Arg, Trap, Trigger)
	end
end

--Player触发分支
function CTriggerMgr:PlayerTrigger(TriggerType, Trigger, Player)
	
end

--Item触发分支
function CTriggerMgr:ItemTrigger(Conn, BigID, ItemName, RoomIndex, Pos)
	if Conn.m_Player == nil or not IsCppBound(Conn.m_Player) then
		return
	end
	local ItemInfo = SpecialItem_Server(ItemName)
	if ItemInfo ~= nil then
		self.m_ItemUse.m_ExpItemOnClicked(Conn, ItemName, ItemInfo, RoomIndex, Pos)
		return
	end
	
	if ItemInfo == nil then
		local ItemInfo = g_ItemUseInfoMgr:GetItemUseInfo(ItemName)

		if ItemInfo ~= nil and ItemInfo["BigID"] == BigID then
			self.m_ItemUse.m_ItemOnClicked(Conn,ItemName,ItemInfo, RoomIndex, Pos)
			return
		end
	end
	
	local fun = self.m_ItemUse.m_BigIdItemScript[BigID]
	if IsFunction(fun) then
		fun(Conn, ItemName, RoomIndex, Pos)
		return
	end
	
	--接取任务物品
	if (BigID == 16 or BigID == 1) then
		Gas2Gac:OnRClickQuestItem(Conn,BigID,ItemName)
	end
end

function CTriggerMgr:GetTarget(GetType, Trigger, Player, Name)
	return self.TargetFun[GetType](Trigger, Player, Name)
end

function CTriggerMgr:GetExpressions(str)
	if self.ExpressionsFun[str] then
		return self.ExpressionsFun[str]
	end
	local funStr = "local function GetExpressions(PL, NL)  return (" .. str .. ") end return GetExpressions"
	self.ExpressionsFun[str] = loadstring(funStr)()
	return self.ExpressionsFun[str]
end