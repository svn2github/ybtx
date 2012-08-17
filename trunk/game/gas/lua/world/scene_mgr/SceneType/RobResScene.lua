gas_require "world/scene_mgr/SceneType/RobResSceneInc"
local _ActionName = "抢夺资源点"

function CRobResScene:JoinScene(Player, SceneName)
end

function CRobResScene:IsCanDestroy()
	if not g_RobResMgr.m_CurrentType or g_RobResMgr.m_CurrentType == "Ending" or g_RobResMgr.m_CurrentType == "Winning" then
		return true
	end
end

function CRobResScene:GetDestroyChannel()
	return self.m_SceneId, _ActionName
end

function CRobResScene:OnPlayerChangeOut(Player)
	--ChangeOutRobResScene(Player, self.m_SceneId)
end

function CRobResScene:OnPlayerChangeIn(Player)
	CTongRobResMgr.IntoRobResScene(Player)
	Player:CppSetPKState(true)
	Gas2Gac:UpdatePkSwitchState(Player.m_Conn)
	Gas2Gac:UpdateHeadInfoByEntityID(Player:GetIS(0), Player:GetEntityID())
	MsgToConn(Player.m_Conn, 193034)
	AddVarNumForTeamQuest(Player, "完成抢夺资源点", 1)
end

function CRobResScene:OnPlayerLogIn(Player)
	CTongRobResMgr.PlayerLoginRobRes(Player)
	Player:CppSetPKState(true)
	Gas2Gac:UpdatePkSwitchState(Player.m_Conn)
	Gas2Gac:UpdateHeadInfoByEntityID(Player:GetIS(0), Player:GetEntityID())
	MsgToConn(Player.m_Conn, 193034)
end

function CRobResScene:OnPlayerLogOut(Player)
	CTongRobResMgr.PlayerOffLineRobResFb(Player)
end
