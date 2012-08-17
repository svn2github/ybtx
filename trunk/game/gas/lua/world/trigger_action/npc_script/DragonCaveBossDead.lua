local function Script(Arg, Trigger, Player)
	local Scene = Trigger.m_Scene
	local NpcName = Trigger.m_Properties:GetCharName()
	local SceneId = Scene.m_SceneId
	local SceneName = Scene:GetLogicSceneName()
	local DragonCaveMgr = g_DragonCaveMgr[SceneName]
	if DragonCaveMgr and NpcName == DragonCaveMgr.BossName then
		UpdateDragonCaveState(SceneId)
	else
		LogErr("龙穴本boss", NpcName.."死亡地点不在对应龙穴")
	end
end

return Script
