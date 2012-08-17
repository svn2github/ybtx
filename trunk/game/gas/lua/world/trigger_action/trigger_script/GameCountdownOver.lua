local function Script(Arg, Trigger, Player)

	local Object = Trigger or Player
	local Scene = Object.m_Scene
--	if not Scene.m_SceneCountDownTick then
--		CfgLogErr("触发脚本填写出现错误!", "副本("..Scene.m_SceneName..")没有进行倒计时,不能触发关闭倒计时")
--		return
--	end
	if Scene.m_SceneCountDownTick then
		UnRegisterTick(Scene.m_SceneCountDownTick)
		Scene.m_SceneCountDownTick = nil
		Scene.m_SceneCountDown = nil
		Gas2Gac:CloseGameCountdownWnd(Scene.m_CoreScene)
	end
end

return Script