gas_require "world/scene_mgr/SceneType/DaTaoShaSceneInc"

local _ActionName = "½Ç¶·³¡"

function CDaTaoShaScene:JoinScene(Player, SceneName)
end

function CDaTaoShaScene:IsCanDestroy()
	if self:IsHavePlayer() or self.m_DtsInfo.m_State ~= g_FbRoomState.BeginGame then
		return false
	end
	return true
end

function CDaTaoShaScene:GetDestroyChannel()
	return self.m_SceneId, _ActionName
end

function CDaTaoShaScene:OnDestroy()
	OnDestroyDaTaoShaScene(self)
end

function CDaTaoShaScene:OnPlayerDeadInScene(Attacker, BeAttacker)
	PlayerDeadInDaTaoSha(Attacker, BeAttacker)
end

function CDaTaoShaScene:OnPlayerChangeOut(Player)
	OnPlayerChangeOutDaTaoShaScene(Player, self.m_SceneId)
end

function CDaTaoShaScene:OnPlayerChangeIn(Player)
	OnPlayerIntoDaTaoShaScene(Player)
	CScenePkMgr.ChangePkState(Player)
	--AddMercenaryLevelCount(Player.m_Conn, "Ó¶±ø±ÈÈü", Player.m_Scene.m_SceneName)
end

function CDaTaoShaScene:OnPlayerLogOut(Player)
	PlayerOffLineDaTaoShaFb(Player)
end
