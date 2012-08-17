local function Script(Arg, Trigger, Player)

	local Index	= Arg[1]
	local Time	= Arg[2]
	local Object = Trigger or Player
	local Scene = Object.m_Scene
	if Scene.m_SceneCountDownTick then
		CfgLogErr("触发脚本填写出现错误!", "副本("..Scene.m_SceneName..")倒计时正在进行,不能再触发副本用倒计时")
		return
	end
	Scene.m_SceneCountDown = Time
	local function TickFun(Tick, Scene)
		Scene.m_SceneCountDown = Scene.m_SceneCountDown - 1
		if Scene.m_SceneCountDown <= 0 then
			UnRegisterTick(Tick)
			Tick = nil
			Scene.m_SceneCountDown = nil
			Scene.m_SceneCountDownTick = nil
		end
	end
	Scene.m_SceneCountDownTick = RegisterTick("GameCountdownTick", TickFun, 1000, Scene)
	Gas2Gac:OpenGameCountdownWnd(Scene.m_CoreScene, Scene.m_SceneCountDown, Index)
end

return Script