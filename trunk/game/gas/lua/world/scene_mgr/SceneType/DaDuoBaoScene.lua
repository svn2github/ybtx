gas_require "world/scene_mgr/SceneType/DaDuoBaoSceneInc"


function CDaDuoBaoScene:OnPlayerDeadInScene(Attacker, BeAttacker)
	CaptureTreasure.PlayerDeadInDaDuoBao(Attacker, BeAttacker)
end


function CDaDuoBaoScene:OnPlayerChangeOut(Player)
	CaptureTreasure.LeaveDaDuoBaoScene(Player, self.m_SceneId)
end

function CDaDuoBaoScene:OnPlayerChangeIn(Player)
	CaptureTreasure.IntoDaDuoBaoScene(Player)
	CScenePkMgr.ChangePkState(Player)
	--AddMercenaryLevelCount(Player.m_Conn, "Ó¶±ø±ÈÈü", Player.m_Scene.m_SceneName)
end

function CDaDuoBaoScene:OnPlayerLogOut(Player)
	CaptureTreasure.PlayerOffLineDaDuoBaoFb(Player)
end
