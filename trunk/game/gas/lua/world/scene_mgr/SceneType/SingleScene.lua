gas_require "world/scene_mgr/SceneType/SingleSceneInc"
--ÑµÁ·Ëþ¸±±¾

function CSingleScene:OnPlayerChangeIn(Player)
	CScenePkMgr.ChangePkState(Player)
	local parameters = {}
	parameters["charId"] = Player.m_uID
	parameters["SceneName"] = Player.m_Scene.m_SceneName
	CallDbTrans("ActivityCountDB", "RecordActionTimes", nil, parameters, Player.m_uID)
end
