gas_require "world/scene_mgr/SceneType/GameSceneInc"



function CGameScene:GetGameRoom()
	local gameName = self.m_MatchGameData["GameName"]
	local roomId = self.m_MatchGameData["RoomId"]
	local room =  g_MatchGameMgr.m_GameTbl[gameName]:GetRoom(roomId)
	return room
end

function CGameScene:GetDestroyChannel()
	return self.m_SceneId, self.m_MatchGameData["GameName"]
end

function CGameScene:LeaveScene()
	local SceneId = self.m_SceneId
	if self:IsHavePlayer() then
		return
	end
	
	-- 当玩家退出时候,有可能在游戏的等待玩家进入阶段, 此时里面为空了, 但不能删场景
	-- 需与游戏房间的状态比较. 当游戏房间关闭后 m_SceneId 会清理, 此时才能删场景
	local room = self:GetGameRoom()
	
	local function CloseScene()
		if self.m_CloseTick then
			UnRegisterTick(self.m_CloseTick)
			self.m_CloseTick = nil
		end
		
		if next(self.m_tbl_Player) then
			return
		end
		
		if room then
			room:Close()
		end
		self:Destroy()
	end
		
		
	if room and not room:IsClose() then
		if room:IsCanClose() then
			CloseScene(nil, self)
		else
			if self.m_CloseTick then
				UnRegisterTick(self.m_CloseTick)
				self.m_CloseTick = nil
			end
			self.m_CloseTick = RegisterTick("CloseSceneTick", CloseScene, 5*60*1000)
		end
	else
		CloseScene()
	end
end

function CGameScene:OnPlayerChangeOut(Player)
	LeaveMatchGameScene(Player, self.m_SceneId)
	CFbPlayerPosMgr.StopSendLeavedFbPlayerPos(self, Player)
end

function CGameScene:OnPlayerChangeIn(Player)
	IntoMatchGameScene(Player)
	CScenePkMgr.ChangePkState(Player)
	--AddMercenaryLevelCount(Player.m_Conn, "佣兵比赛", Player.m_Scene.m_SceneName)
end

function CGameScene:OnPlayerLogIn(Player)
	PlayerLoginMatchGame(Player)
	CScenePkMgr.ChangePkState(Player)
end

function CGameScene:OnPlayerLogOut(Player)
	PlayerOffLineMatchGameFb(Player)
end

function CGameScene:OnDestroy()
	local SceneID = self.m_SceneId	
	CFbPlayerPosMgr.OnSceneDestroy(SceneID)
end
