gas_require "world/scene_mgr/SceneType/AreaSceneInc"
gac_gas_require "team/TeamMgr"
--gas_require "fb_game/AreaFbServer"
--区域副本 类



--判断是否满足进入该副本的条件 参数：人物,进入的副本名
function CAreaScene:JoinScene(Player, SceneName)
	--是否满足基本条件
	if not CSceneBase.IfJoinScene(self, Player) then
		return nil
	end
	return true  --由于之前结构参数条件不够,将会在AreaFbChangeScene 中继续判断该进哪个副本
end

function CAreaScene:OnPlayerChangeIn(Player)
	CAreaFbServer.IntoAreaFbScene(Player)
	CScenePkMgr.ChangePkState(Player)
	
	--添加战斗统计的人并且打开窗口
	--CFbDpsServer.OpenFbDpsInfoWnd(Player, "1队")
	--AddMercenaryLevelCount(Player.m_Conn, "佣兵比赛", Player.m_Scene.m_SceneName)
end

function CAreaScene:OnPlayerLogIn(Player)
	CAreaFbServer.LoginAreaFbScene(Player)
	CScenePkMgr.ChangePkState(Player)
	
	--添加战斗统计的人并且打开窗口
	--CFbDpsServer.OpenFbDpsInfoWnd(Player, "1队")
end

function CAreaScene:OnPlayerChangeOut(Player)
	--关闭战斗统计窗口
	--CFbDpsServer.CloseFbDpsInfoWnd(Player)
end
function CAreaScene:OnPlayerLogOut(Player)
	--关闭战斗统计窗口
	--CFbDpsServer.CloseFbDpsInfoWnd(Player)
end
