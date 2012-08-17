gas_require "world/scene_mgr/SceneType/KillYingFengSceneInc"
--PVP∏±±æ ¿‡

function CKillYingFengScene:OnPlayerChangeOut(Player)
	CKillYinFengServer.LeaveKillYinFengFb(Player, self.m_SceneId)
end

function CKillYingFengScene:OnPlayerChangeIn(Player)
	CKillYinFengServer.IntoKillYinFengFb(Player)
end

function CKillYingFengScene:OnPlayerLogIn(Player)
	CKillYinFengServer.PlayerLoginYF_Fb(Player)
end

function CKillYingFengScene:OnPlayerLogOut(Player)
	CKillYinFengServer.PlayerOffLineYF_Fb(Player)
end