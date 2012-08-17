cfg_load "fb_game/MatchGame_Common"
cfg_load "fb_game/FbActionDirect_Common"
cfg_load "fb_game/GM_ActionNpcAndItem"

gas_require "fb_game/ActionBasic"

gas_require "world/scene_mgr/TeamSceneMgr"
gas_require "fb_game/FbGame_EventInc"
gas_require "fb_game/ActionJoinMgr"
gac_gas_require "activity/fb/FbCfgCheck"
gac_gas_require "activity/fb/MercenaryEducateActCfgCheck"
gas_require "fb_game/FbActionExtraReward"
gas_require "fb_game/JiFenSaiFb"
gas_require "fb_game/DaTaoSha"
gas_require "fb_game/match_game/MatchGameMgr"
gas_require "fb_game/Warner"
gas_require "world/tong_area/Challenge"
gas_require "world/tong_area/TongRobResourceMgr"
gas_require "world/tong_area/WarZoneMgr"
cfg_load "scene/SceneProperty_Server"

local buffTbl = {
	["安装三合一豪华版状态"] = "撤销三合一豪华版状态",
	["三倍经验的buff"] = "撤销三倍经验的buff",
	["安装五合一卡状态"] = "撤销五合一卡状态",
}

function DeadUpdatePlayerInfo(Attacker, BeAttacker)
	local realAttacker
	if IsServerObjValid(Attacker) then
		if Attacker.m_Properties:GetType() == ECharacterType.Player then
			realAttacker = Attacker
		else
			local Master = Attacker:GetMaster()
			if IsServerObjValid(Master) and Master.m_Properties:GetType() == ECharacterType.Player then
				realAttacker = Master
			end
		end
	end
	BeAttacker.m_Scene:OnPlayerDeadInScene(realAttacker, BeAttacker)
end

function ReLivePlayerInfo(Player)
	ReLiveUpdatePlayerInfo(Player)--积分赛副本
end


function g_PlayerLeaveSceneFb(Player)
--在场景中切换时,先调用所有的跳出场景
	OnPlayerChangeOutArea(Player, Player.m_AreaName)
	CChanllengeFlagMgr.JudgeDistance(Player)
	--PlayerSaveTruckInfo(Player)												--跳转场景之前保存运输车数据
	Player.m_Scene:OnPlayerChangeOut(Player)
end

function ClearChangeInTbl(Player)
	Player.m_Scene.m_PlayerChangeInTbl[Player.m_uID] = nil
	if not next(Player.m_Scene.m_PlayerChangeInTbl) then
		if Player.m_Scene.m_DestroySceneTick then
			UnRegisterTick(Player.m_Scene.m_DestroySceneTick)
			Player.m_Scene.m_DestroySceneTick = nil
		end
	end
end

function ClearBuff(Player)
	if not IsCppBound(Player) then
		return
	end
	for server_id in pairs(g_ServerList) do
		Gas2GasAutoCall:ClearPlayerBuffTbl(GetServerAutoConn(server_id), Player.m_uID)
	end
	for buffName, delBuffName in pairs(buffTbl) do
		if Player:ExistState(buffName) then
			Player:PlayerDoFightSkillWithoutLevel(delBuffName)
		end
	end
end

function g_PlayerInSceneFb(Player)
	ClearBuff(Player)
	ClearChangeInTbl(Player)
	if Player.m_Scene.m_isWaitPlayerIn then
		Player.m_Scene.m_isWaitPlayerIn = nil
	end
	--PlayerLoadTruckInfo(Player)
	local pos = CPos:new()
	Player:GetGridPos(pos)
	local areaName = g_AreaMgr:GetSceneAreaName(Player.m_Scene.m_SceneName, pos)
	OnPlayerChangeInArea(Player, areaName)
	
	local sceneType = Player.m_Scene:GetSceneType()

	Player.m_Conn.m_LogoutSetting = SceneProperty_Server(sceneType,"PlayerLogoutSetting")

	Player.m_Scene:OnPlayerChangeIn(Player)
	
	if Player.m_Scene.m_IsDarkness ~= nil then
		Gas2Gac:SetSceneState(Player.m_Conn,Player.m_Scene.m_IsDarkness)
	end
	---------------------------------------
end

--玩家掉线,看是不是在副本中
function g_PlayerOffLineSceneFb(Player)
	OnPlayerChangeOutArea(Player, Player.m_AreaName)
	CChanllengeFlagMgr.JudgeDistance(Player)
	Player.m_Scene:OnPlayerLogOut(Player)
	--清理报名的玩家数据库信息
	MultiServerCancelAllAction(Player.m_uID)
end


--玩家登录时,初始化场景的信息
function g_PlayerLoginSceneFb(Player)
	local pos = CPos:new()
	Player:GetGridPos(pos)
	local areaName = g_AreaMgr:GetSceneAreaName(Player.m_Scene.m_SceneName, pos)
	OnPlayerChangeInArea(Player, areaName)
	ClearChangeInTbl(Player)
	if Player.m_Scene.m_isWaitPlayerIn then
		Player.m_Scene.m_isWaitPlayerIn = nil
	end
	local sceneType = Player.m_Scene:GetSceneType()
	if sceneType == 1 then
		ClearBuff(Player)
	end
	Player.m_Conn.m_LogoutSetting = SceneProperty_Server(sceneType,"PlayerLogoutSetting")
	Player.m_Scene:OnPlayerLogIn(Player)
	if Player.m_Scene.m_IsDarkness ~= nil then
		Gas2Gac:SetSceneState(Player.m_Conn,Player.m_Scene.m_IsDarkness)
	end
end


--报名了副本玩法,或在副本中切场景时
function g_PlayerChangeSceneInFb(Player)
	g_PlayerInSceneFb(Player)
	---------------------------------------
end

--检测NPC的类型和距离
function CheckAllNpcFunc(Player,NpcID,FuncName)
	local Npc = CCharacterDictator_GetCharacterByID(NpcID)
	if not IsServerObjValid(Npc) then
		return false
	end
	if Player and Npc then
		if g_GetDistance(Player, Npc) > 6 then
			return false
		end
		if Npc.m_Properties:GetType() == ECharacterType.Npc then
			local NpcName = Npc.m_Properties:GetCharName()
			local funcnametbl = Npc_Common(NpcName,"FuncName")
			if funcnametbl ~= "" then
				funcnametbl = GetCfgTransformValue(true, "Npc_Common", NpcName, "FuncName")
				for i = 1, #(funcnametbl) do
					if funcnametbl[i] == FuncName then
						return true
					end
				end
			end
		end
	end
	return false
end


function DeadInQuestFB(pCharacter)
	--除(大逃杀副本,训练塔副本)死亡不进外
	--其它的死亡在没有提示的时候,死后都会直接退出副本
	local SceneType = pCharacter.m_Scene.m_SceneAttr.SceneType
	
	if (SceneType ~= 14 and SceneType ~= 20) then
		
		local function Leave(PlayerID)
			local Player = g_GetPlayerInfo(PlayerID)
			if Player then
				-------------------------------------
				local function CallBack(result)
					if result then
						if IsCppBound(Player) then
							local SceneName,PosX,PosY = result[1],result[2],result[3]
							ChangeSceneByName(Player.m_Conn, SceneName, PosX, PosY)
						end
					end
				end
				-------------------------------------
			
				local param = {}
				param["char_id"] = PlayerID
				CallAccountAutoTrans(Player.m_Conn.m_Account, "CharacterMediatorDB","GetCharSceneNamePos",CallBack,param)
			end
		end
		
		local PlayerId = pCharacter.m_uID
		RegisterOnceTick(pCharacter,"LeaveShiLianFBTick",Leave, 3000, PlayerId)
	
	end
end


--===================================================================
local m_FbName3 = "佣兵训练活动"--佣兵训练活动
local m_FbName4	= "杀吟风"--GetStaticTextServer(9403)--杀吟风

--弹出消息后,同意进入
function Gac2Gas:AgreeJoinFbAction(Conn,FbName)
	if not (IsCppBound(Conn) and Conn.m_Player) then
		return
	end
--	local uPlayerID = Conn.m_Player.m_uID
--	local tongId = Conn.m_Player.m_Properties:GetTongID()
--	if FbName == "抢夺资源点" then
--		if tongId == 0 then
--			MsgToConn(Conn, 9450)
--			Gas2GacById:RetDelQueueFbAction(uPlayerID, FbName)
--			return
--		end
--	end
	MultiServerEnterActionScene(Conn, FbName)
end

--回应自动进入活动
function Gac2Gas:AutoJoinActio(Conn, serverId, roomId, actionName)
	if Conn.m_Player then
		Gas2GasAutoCall:PlayerEnterActionScene(GetServerAutoConn(serverId), Conn.m_Player.m_uID, roomId, actionName)
	end
end

local m_NotJoinFbActionFun = {
			[m_FbName3] = CMercenaryEducateAct.ExitYbEducateAction,
			[m_FbName4] = CKillYinFengServer.ExitKillYinFengFb,
			}

--弹出窗口后,取消加入
function Gac2Gas:NotJoinFbAction(Conn,FbName)
	if not (IsCppBound(Conn) and Conn.m_Player) then
		return
	end
	if m_NotJoinFbActionFun[FbName] then
		m_NotJoinFbActionFun[FbName](Conn)
	else
--		if FbName == "抢夺资源点" then
--			CancelRobResEnter(Conn, FbName)
--		else
		MultiServerCancelAction(Conn, FbName)
		--end
	end
end


--GM控制经验加倍活动
function Gas2GasDef:RetSetMulExpActivity(serverConn, modulus)
	g_ServerExpModulus = modulus
end

local function SetMulExpActivity(modulus)
	Gas2GasAutoCall:RetSetMulExpActivity(GetServerAutoConn(g_CurServerId), modulus)
	for _, otherServerConn in pairs(g_OtherServerConnList) do
		Gas2GasAutoCall:RetSetMulExpActivity(otherServerConn, modulus)
	end
end

function MulExpActivity(startTime, endTime, modulus, startWday, endWday)
	local date = {}
	date["time"] = {startTime}
	date["wday"] = {startWday}
	g_AlarmClock:AddTask("StartMulExpActivity", date, SetMulExpActivity, nil, modulus)
	date["time"] = {endTime}
	date["wday"] = {endWday}
	g_AlarmClock:AddTask("EndMulExpActivity", date, SetMulExpActivity, nil, 1)
end

--GM控制玩法活动的关闭
function CloseAction(actionName)
	--遍历解析配置表
	local isNotFindInCfg = true
	local action = {}
	isNotFindInCfg, action.strNpc, action.itemTbl = ActionNpcAndItemCfgCheck(actionName)
	if isNotFindInCfg then
		return
	end
	if not action.strNpc then
		action.strNpc = ""
	end
	
	--数据库操作
	local data = {}
	data["actionName"] = actionName
	local function CallBack(isFindInSql)
		if isFindInSql == false then
			ForbidActionItem(action.itemTbl, "FORBID")
			Gas2GasAutoCall:UpdataClosedAction(GetServerAutoConn(g_CurServerId), actionName, true, action.strNpc, "HIDE")
			for _, otherServerConn in pairs(g_OtherServerConnList) do
				Gas2GasAutoCall:UpdataClosedAction(otherServerConn, actionName, true, action.strNpc, "HIDE")
			end
		end
	end
	CallDbTrans("GMDB", "InsertClosedAction", CallBack, data)
end

--GM控制玩法活动的开启
function RestartAction(actionName)
	--遍历解析配置表
	local isNotFindInCfg = true
	local action = {}
	isNotFindInCfg, action.strNpc, action.itemTbl = ActionNpcAndItemCfgCheck(actionName)
	if isNotFindInCfg then
		return
	end
	if not action.strNpc then
		action.strNpc = ""
	end
	
	--数据库操作
	local data = {}
	data["actionName"] = actionName
	local function CallBack(isFindInSql)
		if isFindInSql then 
			ForbidActionItem(action.itemTbl, "UNFORBID")
			Gas2GasAutoCall:UpdataClosedAction(GetServerAutoConn(g_CurServerId), actionName, false, action.strNpc, "UNHIDE")
			for _, otherServerConn in pairs(g_OtherServerConnList) do
				Gas2GasAutoCall:UpdataClosedAction(otherServerConn, actionName, false, action.strNpc, "UNHIDE")
			end
		end
	end
	CallDbTrans("GMDB", "DeleteClosedAction", CallBack, data)
end

--服务器收到消息的处理函数
function Gas2GasDef:UpdataClosedAction(serverConn, actionName, isCloseOrRestart, strNpc, strType)
	if isCloseOrRestart then
		table.insert(g_ClosedActionTbl, actionName)
	else
		for i, name in pairs(g_ClosedActionTbl) do
			if name == actionName then
				table.remove(g_ClosedActionTbl,i)
			end
		end
	end
	--local npcTbl = loadstring("return {" .. strNpc .. "}")()
	--HideActionNpc(npcTbl, strType)
end

--遍历解析配置表
function ActionNpcAndItemCfgCheck(actionName)
	local isNotFindInCfg = true
	local action = {}
	for _, name in pairs(GM_ActionNpcAndItem:GetKeys()) do
		if name == actionName then		
			if GM_ActionNpcAndItem(name, "Npc") ~= "" then
				action.strNpc = GM_ActionNpcAndItem(name, "Npc")
			end
			if GM_ActionNpcAndItem(name, "Item") ~= "" then
				action.itemTbl = GetCfgTransformValue(true,"GM_ActionNpcAndItem", name, "Item")
			end
			isNotFindInCfg = false
		end
	end
	return isNotFindInCfg, action.strNpc, action.itemTbl
end

--隐藏相关npc
function HideActionNpc(npcTbl, strType)
	if npcTbl == nil then
		return
	end
	for _, scene in pairs(g_SceneMgr.m_tbl_Scene) do
		local uSceneId = scene.m_SceneId
		if g_NpcBornMgr._m_AllNpc[uSceneId] then
			for _, npc in pairs(g_NpcBornMgr._m_AllNpc[uSceneId]) do
				for _, npcName in pairs(npcTbl) do
					if npc.m_Properties:GetCharName() == npcName then
						if strType == "HIDE" then
							npc:SetSize(0)
						end
						if strType == "UNHIDE" then
							npc:SetSize(1)
						end
					end
				end
			end
		end
	end
	return true
end

--禁用相关物品
function ForbidActionItem(itemTbl, strType)
	if itemTbl == nil then
		return
	end
	local data = {}
	for _, itemName in pairs(itemTbl) do
		data["item_name"] = itemName
		if strType == "FORBID" then
			GMWebToolsDB.AddItemUseLimit(data)
		else
			GMWebToolsDB.DelItemUseLimit(data)
		end
	end
end

--判断是否是已关闭活动的相关npc，是则隐藏，第一个参数是Npc类型，第二个是str
function IsNpcOfClosedAction(npc, name)
	if g_ClosedActionTbl ~= nil then
		for _, actionName in pairs(g_ClosedActionTbl) do
			local npcTbl = {}
			_, npcTbl, _ = ActionNpcAndItemCfgCheck(actionName)			
			if npcTbl ~= nil then
				for _, npcName in pairs(npcTbl) do
					if npcName == name then
						npc:SetSize(0)
					end
				end
			end
		end
	end
end

--判定要参加的活动是否已关闭
function SearchClosedActionTbl(actionName)
	if g_ClosedActionTbl ~= nil then
		for _, closedActionName in pairs(g_ClosedActionTbl) do
			if closedActionName == actionName then
				return true
			end
		end
	end
	return false
end

--销毁GM指定的Npc
function Gas2GasDef:DestroyNpcAppointByGM(serverConn, npcName)
	if npcName == nil then
		return
	end
	for _, scene in pairs(g_SceneMgr.m_tbl_Scene) do
		g_SceneMgr:DestroyNpcByName(scene.m_SceneId, npcName)
	end
end

function Gas2GasDef:SendActionRollMsg(SerConn, sName, ActionName)
	Gas2Gac:RetShowRollMsg(g_AllPlayerSender, sName, ActionName)
end

function Gas2GasDef:SysPlayerBuffTbl(SerConn, charId, buffName)
	if not g_PlayerBuffTbl then
		g_PlayerBuffTbl = {}
	end
	if not g_PlayerBuffTbl[charId] then
		g_PlayerBuffTbl[charId] = {}
	end
	table.insert(g_PlayerBuffTbl[charId], buffName)
end

function Gas2GasDef:ClearPlayerBuffTbl(SerConn, charId)
	if g_PlayerBuffTbl and g_PlayerBuffTbl[charId] then
		g_PlayerBuffTbl[charId] = nil
	end
end
