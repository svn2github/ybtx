local function Script(Arg, Trigger, Player)
	
	local Scene = Trigger.m_Scene
	if Scene.m_SceneAttr.SceneType == 13 then
		Gas2Gac:SetMatchGameWndTitleText(Scene.m_CoreScene, Arg[1])
	end
end

return Script