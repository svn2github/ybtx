cfg_load "fb_game/BossBattle_Server"
cfg_load "fb_game/FbActionDirect_Common"

local m_FbName = "Boss争夺战"
g_BossBattleMgr = {}

local function CheckBossBattleCfg()
	local SceneTbl = BossBattle_Server:GetKeys()
	for i=1, #SceneTbl do
		local SceneName = SceneTbl[i]
		local Npc = GetCfgTransformValue(true, "BossBattle_Server", SceneName, "Npc")
		g_BossBattleMgr[SceneName] = {}
		g_BossBattleMgr[SceneName].PlayerTbl = {}
		g_BossBattleMgr[SceneName].PlayerNum = {0,0,0}
		g_BossBattleMgr[SceneName].NpcTbl = {}
		g_BossBattleMgr[SceneName].BornPosTbl = GetCfgTransformValue(true, "BossBattle_Server", SceneName, "BornPos")
		g_BossBattleMgr[SceneName].MaxPlayer = BossBattle_Server(SceneName, "MaxPlayer")
		g_BossBattleMgr[SceneName].BossName = BossBattle_Server(SceneName, "Boss")
		for j=1, #Npc do
			g_BossBattleMgr[SceneName].NpcTbl[Npc[j]] = true
		end
	end
end
CheckBossBattleCfg()

function Gac2Gas:ShowBossBattleWnd(Conn, NpcID)
	if not (IsCppBound(Conn) and Conn.m_Player) then
		return
	end
	if not CheckAllNpcFunc(Conn.m_Player, NpcID, "Boss争夺战报名") then
		return
	end
	Gas2Gac:RetShowBossBattleWnd(Conn)
end

function Gac2Gas:CancelJoinBossBattle(Conn)
	for _, SceneName in pairs(BossBattle_Server:GetKeys()) do
		if BossBattle_Server(SceneName, "NpcSceneName") == Conn.m_Player.m_Scene:GetLogicSceneName() then
			QuitBossBattle(Conn.m_Player, SceneName)
		end
	end
end

function Gac2Gas:CanJoinBossBattle(Conn)
	if not (IsCppBound(Conn) and Conn.m_Player) then
		return
	end
	
	local Player = Conn.m_Player
	local PlayerId = Player.m_uID
	local TongId = Player.m_Properties:GetTongID()
	local Camp = Player:CppGetBirthCamp()
	local SceneName
	local tbl
	
	for _, sname in pairs(BossBattle_Server:GetKeys()) do
		if BossBattle_Server(sname, "NpcSceneName") == Player.m_Scene:GetLogicSceneName() then
			SceneName = sname
			tbl = g_BossBattleMgr[SceneName]
			break
		end
	end
	if not (tbl and SceneName) then	--玩家在报名时传送至其他场景，则取消报名
		return
	end
	
	if not tbl.StartTime then
		if not CheckActionIsBeginFunc(m_FbName) then
			MsgToConn(Conn, 192004)--活动还没有开放,不可以报名
		else
			local isAllOver = true
			for _, v in pairs(g_BossBattleMgr) do
				if v.StartTime then
					isAllOver = false
				end
			end
			if isAllOver then
				Gas2Gac:BossBattleJoinFailMsg(Conn, "")--本次活动已经结束,不可以报名
			else
				Gas2Gac:BossBattleJoinFailMsg(Conn, BossBattle_Server(SceneName, "NpcSceneName"))--本次活动已经结束,不可以报名
			end
		end
		return
	end
	
	if TongId == 0 then
		MsgToConn(Conn,192005)
		return
	end
	for name, _ in pairs(g_BossBattleMgr) do
		if g_BossBattleMgr[name].PlayerTbl[PlayerId] then
			Gas2Gac:BossBattleKillBossMsg(Conn, name, "", "", "")
			return
		end
	end
	
	local function CallBack(TongLevel, IsBattleTong)
		if not (IsCppBound(Conn) and Conn.m_Player) then
			return
		end
		if not TongLevel then
			MsgToConn(Conn,192007)
			return
		end
		if not IsBattleTong then
			MsgToConn(Conn,192014)
			return
		end
		if TongLevel < BossBattle_Server(SceneName, "TongLevel") then
			MsgToConn(Conn,192008)
			return
		end
		if tbl.PlayerNum[Camp] < tbl.MaxPlayer then
			--for ServerId, _ in pairs(g_ServerList) do
			--	Gas2GasAutoCall:UpdataBossBattlePlayerTbl(GetServerAutoConn(ServerId), SceneName, PlayerId, Camp, 0)
			--end
			Gas2Gac:RetEnterBossBattle(Conn, SceneName)
		else
			Gas2Gac:RetWaitBossBattle(Conn)
		end
	end
	CallAccountManualTrans(Conn.m_Account, "BossBattleDB", "CanJoinBossBattle", CallBack, {["TongId"] = TongId})
end

function Gac2Gas:EnterBossBattle(Conn, BtnIndex, SceneName)
	if not (IsCppBound(Conn) and IsCppBound(Conn.m_Player)) then
		return
	end
	
	local tbl = g_BossBattleMgr[SceneName]
	if not tbl.StartTime then
		local isAllOver = true
		for _, v in pairs(g_BossBattleMgr) do
			if v.StartTime then
				isAllOver = false
			end
		end
		if isAllOver then
			Gas2Gac:BossBattleJoinFailMsg(Conn, "")--本次活动已经结束,不可以报名
		else
			Gas2Gac:BossBattleJoinFailMsg(Conn, BossBattle_Server(SceneName, "NpcSceneName"))--本次活动已经结束,不可以报名
		end
		return
	end
	
	local Camp = Conn.m_Player:CppGetBirthCamp()
	if BtnIndex == 1 then
		local SceneId = tbl.SceneId
		local ServerId = tbl.ServerId
		local PosX = tbl.BornPosTbl[Camp][1]
		local PosY = tbl.BornPosTbl[Camp][2]
		local ContinualMin = FbActionDirect_Common(m_FbName, "ContinualMin")
		local Time = ContinualMin*60 - (os.time() - tbl.StartTime)
		assert(Time > 0,"Boss争夺战出现异常时间显示出错,tbl.StartTime:"..tbl.StartTime.."当前时间："..os.time())
		MultiServerChangeScene(Conn, SceneId, ServerId, PosX, PosY)
		BossBattleSetWnd(Conn, SceneName, Time, true)
		for ServerId, _ in pairs(g_ServerList) do
			assert(Conn.m_Player.m_uID > 0, "Boss争夺战玩家入场检测PlayerId出错")
			Gas2GasAutoCall:UpdataBossBattlePlayerTbl(GetServerAutoConn(ServerId), SceneName, Conn.m_Player.m_uID, Camp, 1)
		end
	--else
	--	for ServerId, _ in pairs(g_ServerList) do
	--		Gas2GasAutoCall:UpdataBossBattlePlayerTbl(GetServerAutoConn(ServerId), SceneName, Conn.m_Player.m_uID, Camp, -2)
	--	end
	end
end

function QuitBossBattle(Player, SceneName)
	if IsCppBound(Player) then
		local Camp = Player:CppGetBirthCamp()
		for ServerId, _ in pairs(g_ServerList) do
			Gas2GasAutoCall:UpdataBossBattlePlayerTbl(GetServerAutoConn(ServerId), SceneName, Player.m_uID, Camp, -1)
		end
		if IsCppBound(Player.m_Conn) then
			BossBattleSetWnd(Player.m_Conn, SceneName, 0, false)
		end
	end
end

local function EndBossBattle(SceneId, KillerId)
	local Scene = g_SceneMgr:GetScene(SceneId)
	local SceneName = Scene:GetLogicSceneName()
	local Proffer = BossBattle_Server(SceneName, "Proffer")
	local tbl = g_BossBattleMgr[SceneName]
	
	--print(SceneName, SceneId, "EndBossBattle")
	local function DestroyScene()
		--print(SceneName, SceneId, "DestroyScene")
		--CSceneMgr:DestroyScene( Scene )
		--CallDbTrans("SceneMgrDB", "_DeleteScene", nil, {SceneId}, SceneId)
		Scene:Destroy()
	end
	
	local function CallBack(AwardPlayerTbl)
		--print(SceneName, SceneId, "EndBossBattle CallBack")
		for PlayerId, _ in pairs(tbl.PlayerTbl) do
			local Player = g_GetPlayerInfo(PlayerId)
			if IsCppBound(Player) then
				if AwardPlayerTbl[PlayerId] then
					MsgToConn(Player.m_Conn,300021,Proffer)
				end
				BossBattleSetWnd(Player.m_Conn, SceneName, 0, false)
				Gas2Gac:RetLeaveBossBattle(Player.m_Conn, KillerId==0)
			end
		end
		for ServerId, _ in pairs(g_ServerList) do
			Gas2GasAutoCall:UpdataBossBattlePlayerTbl(GetServerAutoConn(ServerId), SceneName, 0, 0, 0)
		end
		RegisterOnceTick(Scene, "BossBattleDestroySceneTick", DestroyScene, 120000)	--为玩家离开场景保留时间
	end
	
	local data = {}
	data["KillerId"] = KillerId
	data["SceneName"] = SceneName
	data["SceneId"] = tbl.SceneId
	data["Proffer"] = Proffer
	CallDbTrans("BossBattleDB", "BossBattleAward", CallBack, data, tbl.SceneId)
end

function UpdateBossBattle(SceneName, NpcName, EntityId)
	local Player = CEntityServerManager_GetEntityByID(EntityId)
	local tbl = g_BossBattleMgr[SceneName]
	if not Player or EntityId == 0 then
		LogErr("boss争夺战boss死亡回调出错，Killer为nil")
		return
	end
	assert(tbl.StartTime,"Boss争夺战出现异常，tbl.StartTime在活动未结束时变为空,场景名："..SceneName)
	
	if NpcName == tbl.BossName then
		local PlayerName = Player.m_Properties:GetCharName()
		local TongName = Player.m_Properties:GetTongName()
		for ServerId, _ in pairs(g_ServerList) do
			Gas2GasAutoCall:BossBattleSendKillBossMsg(GetServerAutoConn(ServerId), SceneName, NpcName, TongName, PlayerName)
		end
		EndBossBattle(tbl.SceneId, Player.m_uID)
	end
end

function BossBattleTimeOver()
	--print("BossBattleTimeOver")
	for SceneName,_ in pairs(g_BossBattleMgr) do
		local tbl = g_BossBattleMgr[SceneName]
		local Scene = g_SceneMgr:GetScene(tbl.SceneId)
		if Scene and tbl.StartTime then
			EndBossBattle(tbl.SceneId, 0)
			tbl.StartTime = nil
		end
	end
	MsgToConn(g_AllPlayerSender,192011)
end

local function InitBossBattle(SceneName, SceneId, ServerId)
	
	if ServerId == g_CurServerId then
		local Scene = g_SceneMgr:GetScene(SceneId)
		if not Scene then
			Scene = g_SceneMgr:CreateScene(SceneName, SceneId)
		end
		local ContinualMin = FbActionDirect_Common(m_FbName, "ContinualMin")
		RegisterOnceTick(Scene, "EndBossBattleTick"..SceneName, BossBattleTimeOver, ContinualMin*60*1000)
	end
	
	g_BossBattleMgr[SceneName].PlayerTbl = {}
	g_BossBattleMgr[SceneName].PlayerNum = {0,0,0}
	g_BossBattleMgr[SceneName].StartTime = os.time()
	g_BossBattleMgr[SceneName].SceneId = SceneId
	g_BossBattleMgr[SceneName].ServerId = ServerId
	MsgToConn(g_AllPlayerSender,192012)
end

local function BeginBossBattle()
	--print("BeginBossBattle")
	local SceneTbl = BossBattle_Server:GetKeys()
	local function CallBack(SceneInfoTbl)
		for i=1, #SceneTbl do
			InitBossBattle(SceneInfoTbl[i][1], SceneInfoTbl[i][2], SceneInfoTbl[i][3])
		end
	end
	CallDbTrans("BossBattleDB", "CreateBossBattleScene", CallBack, SceneTbl, "CreateBossBattleScene")
end

function SetBossBattleTick()
	if not FbActionDirect_Common(m_FbName) then
		return
	end
	local SceneTbl = BossBattle_Server:GetKeys()
	local StartTime = GetCfgTransformValue(true, "FbActionDirect_Common", m_FbName, "StartTime")
	if table.getn(SceneTbl) == 0 or not StartTime.time or not StartTime.time[1] then
		return
	end
	
	local date = {}
	date.wday = StartTime.wday
	date.time = StartTime.time
	g_AlarmClock:AddTask("EndMulExpActivity", date, BeginBossBattle, nil)
end

function BossBattleSetWnd(Conn, SceneName, Time, Type)
	if not (IsCppBound(Conn) and IsCppBound(Conn.m_Player)) then
		return
	end
	--Gas2Gac:RetShowBossBattleInfoWnd(Conn, SceneName, Type)
	Gas2Gac:RetShowDownTimeWnd(Conn, Time)
end

function Gas2GasDef:UpdataBossBattlePlayerTbl(ServerConn, SceneName, PlayerId, Camp, Count)
	local tbl = g_BossBattleMgr[SceneName]
	if PlayerId > 0 then
		if Count >= 0 then
			tbl.PlayerTbl[PlayerId] = 0
			tbl.PlayerNum[Camp] = tbl.PlayerNum[Camp] + Count
		elseif Count == -1 then
			tbl.PlayerTbl[PlayerId] = nil
			tbl.PlayerNum[Camp] = ( (tbl.PlayerNum[Camp] == 0) and 0 ) or (tbl.PlayerNum[Camp] - 1)
		elseif Count == -2 then
			tbl.PlayerTbl[PlayerId] = nil
		end
	else
		tbl.StartTime = nil
		tbl.SceneId = nil
		tbl.ServerId = nil
	end
	--print(SceneName, PlayerId, Camp, Count, g_BossBattleMgr[SceneName].PlayerTbl[PlayerId], g_BossBattleMgr[SceneName].PlayerNum[Camp], "UpdataBossBattlePlayerTbl")
end

function Gas2GasDef:BossBattleSendKillBossMsg(ServerConn, SceneName, NpcName, TongName, PlayerName)
	Gas2Gac:BossBattleKillBossMsg(g_AllPlayerSender, SceneName, NpcName, TongName, PlayerName)
end

function Gac2Gas:TimeOverLeaveBossBattle(Conn)
	if not (IsCppBound(Conn) and IsCppBound(Conn.m_Player)) then
		return
	end
	local SceneName = Conn.m_Player.m_MasterSceneName
	local Pos = Conn.m_Player.m_MasterScenePos
	ChangeSceneByName(Conn, SceneName, Pos.x, Pos.y)
end

--function InitBossBattleWnd()
--	for SceneName, v in pairs(g_BossBattleMgr) do
--		if g_BossBattleMgr[SceneName] then
--			for id, _ in pairs(g_BossBattleMgr[SceneName].TongTbl) do
--				if id == TongID then
--					local ContinualMin = FbActionDirect_Common("Boss争夺战", "ContinualMin")
--					local time = ContinualMin*60 - (os.time() - g_BossBattleMgr[SceneName].StartTime)
--					Gas2Gac:RetShowDownTimeWnd(Connection, time)
--				end
--			end
--		end
--	end
--end

GMBossBattleTick = nil
local function EndGMTick()
	if GMBossBattleTick then
		UnRegisterTick(GMBossBattleTick)
		GMBossBattleTick = nil
	end
end

function GMBeginBossBattle(Conn)
	if GMBossBattleTick then
		GMCommand:print(Conn, "Destorying BossBattle instance scene. Please try again a few secends later.")
		return
	end

	--local StartTime = GetCfgTransformValue(true, "FbActionDirect_Common", m_FbName, "StartTime")
	--if StartTime.time and StartTime.time[1] then
	--	GMCommand:print(Conn, "This command can be only used within test edition. If you are, please delete the value about BossBattle at cfg \"FbActionDirect_Common\", row \"Boss争夺战\", column \"StartTime\".")
	--	return
	--end
	for _, v in pairs(g_BossBattleMgr) do
		if v.StartTime then
			GMCommand:print(Conn, "There are even several BossBattle instances running. If you are sure to restart this activity. Please end them by command \"EndBoss\" before this operate.")
			return
		end
	end
	for ServerId, _ in pairs(g_ServerList) do
		Gas2GasAutoCall:GMBeginBossBattle(GetServerAutoConn(ServerId))
	end
end

function GMEndBossBattle(Conn)
	for _, v in pairs(g_BossBattleMgr) do
		if v.StartTime then
			for ServerId, _ in pairs(g_ServerList) do
				Gas2GasAutoCall:GMEndBossBattle(GetServerAutoConn(ServerId))
			end
			return
		end
	end
	GMCommand:print(Conn, "There is not any BossBattle instance running.")
end

function GMSetBossBattleMaxPlayer(Conn, Num)
	for ServerId, _ in pairs(g_ServerList) do
		Gas2GasAutoCall:GMSetBossBattleMaxPlayer(GetServerAutoConn(ServerId), Num)
	end
end

function Gas2GasDef:GMBeginBossBattle(ServerConn)
	BeginBossBattle()
end

function Gas2GasDef:GMEndBossBattle(ServerConn)
	BossBattleTimeOver()
	GMBossBattleTick = RegisterTick("GMBossBattleTick", EndGMTick, 150000)		--为删场景的Tick保留时间
end

function Gas2GasDef:GMSetBossBattleMaxPlayer(ServerConn, Num)
	for SceneName, _ in pairs(g_BossBattleMgr) do
		g_BossBattleMgr[SceneName].MaxPlayer = Num
		--print("GMSetBossBattleMaxPlayer", g_BossBattleMgr[SceneName].MaxPlayer)
	end
end
