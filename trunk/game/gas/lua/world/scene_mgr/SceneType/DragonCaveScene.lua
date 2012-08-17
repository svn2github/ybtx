CDragonCaveScene = class(CSceneBase)


function CDragonCaveScene:OnCreate()
	local SceneId = self.m_SceneId
	local Scene = g_SceneMgr:GetScene(SceneId)
	local function SetDragonCaveTick()
		Gas2Gac:SendMsgToEverySence(Scene.m_CoreScene)
--		for _,player in pairs(Scene.m_tbl_Player) do
--			local playerId = player.m_uID
--		  NotifyPlayerLeaveFbScene(playerId, 10, 380006)
--		end
	end
	local data = os.date("*t")
	local Day =data.wday
	local Hour =data.hour
	local Min = data.min
	local Second = data.sec
	local LeaveDay 
	if Day == 1 then
		LeaveDay = 1
	else
		LeaveDay = 2 + 7 - Day
	end
	local LeaveHour = 0 - Hour
	local LeaveTime = ((LeaveDay * 24 + LeaveHour)* 60 - Min)*60 - Second 
	RegisterOnceTick(Scene, "TimeOverTick", SetDragonCaveTick, LeaveTime * 1000, SceneId)
end

local function IsChangePlayerOutDragonCave(Player, SceneName)
	if not (IsCppBound(Player.m_Conn)) then
		return
	end
	CScenePkMgr.ChangePkState(Player)
	local Conn = Player.m_Conn
	local sceneName = Player.m_MasterSceneName
	local Pos = Player.m_MasterScenePos
	if not CheckDragonCaveSceneState(Conn, SceneName) then
		ChangeSceneByName(Player.m_Conn, sceneName, Pos.x, Pos.y)
	else
		local data = {}
		data["charId"] = Player.m_uID
		data["TongId"] = TongId
		local function CallBack(SceneState) 
			if not (IsCppBound(Conn) and IsCppBound(Player)) then
				return 
			end
			if SceneState then
				ChangeSceneByName(Player.m_Conn, sceneName, Pos.x, Pos.y)
			end
		end
		CallAccountManualTrans(Player.m_Conn.m_Account, "DragonCaveDB", "GetDragonCaveSceneState", CallBack, data, TongId)
	end	
end
function CDragonCaveScene:OnPlayerChangeIn(Player)
	local SceneName = self.m_SceneName
	IsChangePlayerOutDragonCave(Player, SceneName)
end

function CDragonCaveScene:OnPlayerLogIn(Player, SceneName)
	local SceneName = self.m_SceneName
	IsChangePlayerOutDragonCave(Player, SceneName)
end

