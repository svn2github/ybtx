CBossBattleScene = class(CSceneBase)

local function BossSceneState(SceneName, SceneId)
	local tbl = g_BossBattleMgr[SceneName]
	local NpcTbl = g_NpcBornMgr._m_OnlyNpc[SceneId]
	local NpcName = tbl.BossName
	local Npc = GetOnlyNpc(SceneId, NpcName)
	local CurTime = os.date("*t")
	if Npc and Npc:CppIsLive() then
		if CurTime.hour < 18 then
			return true
		end
	end
end

function CBossBattleScene:OnPlayerChangeOut(Player)
	QuitBossBattle(Player, self.m_SceneName)
end

function CBossBattleScene:OnPlayerLogOut(Player)
	QuitBossBattle(Player, self.m_SceneName)
end

function CBossBattleScene:OnPlayerChangeIn(Player)
	local SceneName = self.m_SceneName
	local SceneId = self.m_SceneId
	if not BossSceneState(SceneName, SceneId) then
		BossBattleSetWnd(Player.m_Conn, SceneName, 0, false)
		NotifyPlayerLeaveFbScene(Player.m_uID, 5, 380004)
	end
	Player:CppSetPKState(true)
	Gas2Gac:UpdatePkSwitchState(Player.m_Conn)
	Gas2Gac:UpdateHeadInfoByEntityID(Player:GetIS(0), Player:GetEntityID())
	MsgToConn(Player.m_Conn, 193034)
	AddVarNumForTeamQuest(Player, "完成Boss争夺战", 1)
end

function CBossBattleScene:OnPlayerLogIn(Player)
	Player:CppSetPKState(true)
	Gas2Gac:UpdatePkSwitchState(Player.m_Conn)
	Gas2Gac:UpdateHeadInfoByEntityID(Player:GetIS(0), Player:GetEntityID())
	MsgToConn(Player.m_Conn, 193034)
	AddVarNumForTeamQuest(Player, "完成Boss争夺战", 1)
end

function CBossBattleScene:IsCanDestroy()
	if not BossSceneState(self.m_SceneName, self.m_SceneId) and not self:IsHavePlayer() then
		return true
	end
end
