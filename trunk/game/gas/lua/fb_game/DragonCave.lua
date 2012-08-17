cfg_load "fb_game/DragonCave_Server"

CDragonCave = class()

g_DragonCaveMgr = {}

local function CheckDragonCaveCfg()
	local SceneTbl = DragonCave_Server:GetKeys()
	for i=1, #SceneTbl do
		local SceneName = SceneTbl[i]
		g_DragonCaveMgr[SceneName] = {}
		g_DragonCaveMgr[SceneName].StartDay = DragonCave_Server(SceneName, "StartDay")
		g_DragonCaveMgr[SceneName].StartTime = DragonCave_Server(SceneName, "StartTime")
		g_DragonCaveMgr[SceneName].BornPosX = DragonCave_Server(SceneName, "BornPosX")
		g_DragonCaveMgr[SceneName].BornPosY = DragonCave_Server(SceneName, "BornPosY")
		g_DragonCaveMgr[SceneName].BossName = DragonCave_Server(SceneName, "BossName")
		g_DragonCaveMgr[SceneName].LimitLevel = DragonCave_Server(SceneName, "LimitLevel")
		g_DragonCaveMgr[SceneName].TongAward = DragonCave_Server(SceneName, "TongAwardName")
		g_DragonCaveMgr[SceneName].Award = DragonCave_Server(SceneName, "AwardName")
		g_DragonCaveMgr[SceneName].Npc = GetCfgTransformValue(false, "DragonCave_Server", SceneName, "Npc")
		g_DragonCaveMgr[SceneName].SkillTbl = GetCfgTransformValue(true, "DragonCave_Server", SceneName, "SkillName")
		if g_DragonCaveMgr[SceneName].StartDay and g_DragonCaveMgr[SceneName].StartDay == 7 then
			g_DragonCaveMgr[SceneName].StartDay = 0
		end
		if g_DragonCaveMgr[SceneName].Award and g_DragonCaveMgr[SceneName].TongAward then
			g_DragonCaveMgr[SceneName].AwardTbl = loadstring("return "..g_DragonCaveMgr[SceneName].Award)()
			g_DragonCaveMgr[SceneName].TongAwardTbl = loadstring("return "..g_DragonCaveMgr[SceneName].TongAward)()
		end
	end
end
CheckDragonCaveCfg()

local function TimeIsOver(SceneName)
	local CurTime = os.date("*t")
	local nowTime = os.time()
	local day = tonumber(os.date("%w", nowTime))
	local hour = CurTime.hour
	local tbl = g_DragonCaveMgr[SceneName]
	if day == tbl.StartDay then
		if hour < tbl.StartTime then
			return true
		end
	else
		return false 
	end
end

function CheckDragonCaveSceneState(Conn, SceneName)
	if not (IsCppBound(Conn) and Conn.m_Player) then
		return
	end
	local tbl = g_DragonCaveMgr[SceneName]
	local Player = Conn.m_Player
	local PlayerLevel = Player:CppGetLevel()
	local TongId = Player.m_Properties:GetTongID()
	if TongId == 0 then
		MsgToConn(Conn, 440001)
		return false
	elseif TimeIsOver(SceneName) then
		MsgToConn(Conn, 440002)
		return false
	elseif PlayerLevel < tbl.LimitLevel then
		MsgToConn(Conn, 440003)
		return false
	else
		return true
	end
end

function CDragonCave:ShowDragonCaveWnd(Conn, NpcID)
	if not (IsCppBound(Conn) and Conn.m_Player) then
		return
	end
	if not CheckAllNpcFunc(Conn.m_Player, NpcID, "红龙之穴" ) then
		return
	end
	Gas2Gac:RetShowDragonCaveWnd(Conn, NpcID)
end

function CDragonCave:EnterDragonCave(Conn, NpcName)
	if not (IsCppBound(Conn) and Conn.m_Player) then
		return
	end
	local Player = Conn.m_Player
	local TongId = Player.m_Properties:GetTongID()
	local SceneName
	local IsNpc = false
	for _, sname in pairs(DragonCave_Server:GetKeys()) do
		local Npc = GetCfgTransformValue(false, "DragonCave_Server", sname, "Npc")
		for k=1,#Npc do
			if Npc[k] == NpcName then
				IsNpc = true
				break
			end
		end
		if IsNpc then
			SceneName = sname
			break
		end
	end
	if not CheckDragonCaveSceneState(Conn, SceneName) then
		return
	end
	local data = {}
	data["charId"] = Player.m_uID
	data["TongId"] = TongId
	local function CallBack(SceneState) 
		if not (IsCppBound(Conn) and IsCppBound(Player)) then
			return false
		end
		if SceneState then
			MsgToConn(Conn, 440002) --活动已结束
			return false
		end 
--		if PlayerPos < 2 then
--			MsgToConn(Conn, 440004) -- 普通队员无法参加该活动
--			return false
--		end
		Gas2Gac:RetEnterDragonCave(Conn, SceneName)
	end
	CallAccountManualTrans(Conn.m_Account, "DragonCaveDB", "GetDragonCaveSceneState", CallBack, data, TongId)
end
	
function CDragonCave:IntoDragonCave(Conn, SceneName)
	local Player = Conn.m_Player
	local PlayerId = Player.m_uID
	local TongId = Player.m_Properties:GetTongID()
	if not IsCppBound(Player) then
		return
	end
	local tbl = g_DragonCaveMgr[SceneName]
	local PosX = tbl.BornPosX
	local PosY = tbl.BornPosY
	if TongId == 0 then
		MsgToConn(Conn, 440001) --未加入佣兵小队
		return
	end
	if TimeIsOver(SceneName) then
		MsgToConn(Conn, 440002) --活动已结束
		return
	end
	local function CallBack(SceneState, ServerId, SceneId) 
		if not (IsCppBound(Conn) and IsCppBound(Player)) then
			return false
		end
		if SceneState then
			MsgToConn(Conn, 440002) --活动已结束
			return false
		end 
--		if PlayerPos < 2 then
--			MsgToConn(Conn, 440004) -- 普通队员无法参加该活动
--			return false
--		end
		MultiServerChangeScene(Conn, SceneId, ServerId, PosX, PosY)
	end
	local data = {}
	data["charId"] = Player.m_uID
	data["TongId"] = TongId
	data["SceneName"] = SceneName
	CallAccountManualTrans(Conn.m_Account, "DragonCaveDB", "GetDragonCaveScene", CallBack, data, TongId)
end

function UpdateDragonCaveState(SceneId)
	local Scene = g_SceneMgr:GetScene(SceneId)
	local TongId = Scene.m_TongId
	local Camp = Scene.m_Camp
	local SceneName = Scene:GetLogicSceneName()
	local tbl = g_DragonCaveMgr[SceneName]
	local AwardId = tbl.AwardTbl[1]
	local Award = tbl.AwardTbl[2]
	local AwardCount = tbl.AwardTbl[3]
	local TongAwardName
	if Camp == 1 then
		TongAwardName = tbl.TongAwardTbl[2][1]
	elseif Camp == 2 then
		TongAwardName = tbl.TongAwardTbl[2][2]
	else
		TongAwardName = tbl.TongAwardTbl[2][3]
	end
	local function CallBack(SceneResult)
		local tbl = g_DragonCaveMgr[SceneName]
		if SceneResult then
			for i=0, SceneResult:GetRowNum()-1 do
				local ResultServerId, ResultSceneId = SceneResult:GetData(i,0),SceneResult:GetData(i,1)
				if ResultSceneId == SceneId then	
					local scene = g_SceneMgr:GetScene(ResultSceneId)
--					for _,player in pairs(scene.m_tbl_Player) do
--						local playerId = player.m_uID
--					  NotifyPlayerLeaveFbScene(playerId, 10, 380004)
--					end
					Gas2Gac:SendMsgToLocalSence(scene.m_CoreScene)
				else
					Gas2GasAutoCall:LeaveOutOfCave(GetServerAutoConn(ResultServerId), ResultSceneId)
				end
			end
		end
	end
	local data = {}
	data["TongId"] = TongId 
	data["SceneName"] = SceneName
	data["SceneId"] = SceneId
	data["TongAwardId"] = tbl.TongAwardTbl[1]
	data["TongAward"] = TongAwardName
	data["TongAwardCount"] = tbl.TongAwardTbl[3]
	data["AwardId"] = AwardId
	data["Award"] = Award
	data["AwardCount"] = AwardCount
	CallDbTrans("DragonCaveDB", "SendAwardToTong", CallBack, data, TongId)
end

function CDragonCave:LeaveDragonCave(Conn)
	if not (IsCppBound(Conn) and IsCppBound(Conn.m_Player)) then
		return
	end
	local player = Conn.m_Player
	local Scene = player.m_Scene
	local SceneName = Scene:GetLogicSceneName()
	local tbl = g_DragonCaveMgr[SceneName]
	if tbl then
		if tbl.SkillTbl then
			for _,skill in pairs (tbl.SkillTbl) do
				player:RemoveTempSkill(skill, 1)
			end
		end
		ChangeTongScene(Conn)
	end
end	

function CDragonCave:DragonCaveTick()
	local function DeleteDragonCave()
		CallDbTrans("DragonCaveDB", "DeleteDragonCave", nil, {})
	end	
	g_AlarmClock:AddTask("DeleteDragonCave",{wday = {1}, time = {"06:00"}}, DeleteDragonCave, nil)
end

function CDragonCave:CheckDragonCaveList()
	CallDbTrans("DragonCaveDB", "CheckDragonCaveList", nil, {})
end 
function CDragonCave:DeleteDragonBuildingTask()
	if g_CurServerId == 1 then
		local function DeleteDragonBuilding()		
			local function CallBack(buildingTbl)
				if buildingTbl then
					for i=1,#buildingTbl do
						local TongId = buildingTbl[i][1]
						local buildingId = buildingTbl[i][2]
						for server_id in pairs(g_ServerList) do
							Gas2GasAutoCall:DestroyDragon(GetServerAutoConn(server_id),TongId, buildingId)
						end					
	--					local buildingNpc = g_BuildingMgr:GetBuildingNpc(TongId, buildingId)
	--					if buildingNpc then
	--						g_NpcServerMgr:DestroyServerNpcNow(buildingNpc, false)
	--					end
					end
				end
			end
			CallDbTrans("DragonCaveDB", "DeleteDragonBuilding", CallBack, {})
		end	
		g_AlarmClock:AddTask("DeleteDragonBuilding",{wday = {}, time = {"00:00"}}, DeleteDragonBuilding, nil)
	end
end

function CDragonCave:KickOutOfCave(PlayerId)
	local KickPlayer = g_GetPlayerInfo(PlayerId)
	if KickPlayer and IsCppBound(KickPlayer) then
		local Scene = KickPlayer.m_Scene
		local SceneType = Scene.m_SceneAttr.SceneType
		if SceneType == 28 or SceneType == 26 then     
			local SceneName = KickPlayer.m_MasterSceneName
			local pos = KickPlayer.m_MasterScenePos 
			ChangeSceneByName(KickPlayer.m_Conn,SceneName,pos.x,pos.y)
		end
	end
end

function CDragonCave:LeaveOutOfCave(SceneId)
	local Scene = g_SceneMgr:GetScene(SceneId)
	if Scene then
		Gas2Gac:SendMsgToOtherSence(Scene.m_CoreScene)
--		for _,player in pairs(Scene.m_tbl_Player) do
--			local playerId = player.m_uID
--		  NotifyPlayerLeaveFbScene(playerId, 10, 380005)
--		end
	end
end

function CDragonCave:DestroyDragon(TongId, buildingId)
	local buildingNpc = g_BuildingMgr:GetBuildingNpc(TongId, buildingId)
	if buildingNpc then
		g_NpcServerMgr:DestroyServerNpcNow(buildingNpc, false)
	end
end