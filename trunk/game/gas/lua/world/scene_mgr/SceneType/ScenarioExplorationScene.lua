CSenarioExplorationScene = class(CSceneBase)

--判断是否满足进入该副本的条件 参数：人物,进入的副本名
function CSenarioExplorationScene:JoinScene(Player, SceneName)
	--是否满足基本条件
	if not CSceneBase.IfJoinScene(self, Player) then
		return nil
	end
	return true  --由于之前结构参数条件不够,将会在AreaFbChangeScene 中继续判断该进哪个副本
end

function CSenarioExplorationScene:IsCanDestroy()
	if self:IsHavePlayer() then
		return false
	end
	if self.m_State ~= g_FbRoomState.GameEnd then
		return false
	end
	return true
end

function CSenarioExplorationScene:OnDestroy()
	if self.m_matchTeam and self.m_matchTeam.m_TempTeamId then
		g_TemporaryTeamMgr:BreakTeam(self.m_matchTeam.m_TempTeamId)
	end
	SavePlayerWarnValue(self.m_SceneId)
	CScenarioExploration.Destroy(self)
end

function CSenarioExplorationScene:OnPlayerChangeIn(Player)
	Player:BeginStatistic("")
	CScenarioExploration.IntoScene(Player)
	CScenePkMgr.ChangePkState(Player)
	
	--添加战斗统计的人并且打开窗口
	--CFbDpsServer.OpenFbDpsInfoWnd(Player, "1队")
end

function CSenarioExplorationScene:OnPlayerChangeOut(Player)
	SavePlayerWarnValue(self.m_SceneId, Player.m_uID)
	CScenarioExploration.LeaveScene(Player, true)
	CScenePkMgr.ChangePkState(Player)
	
	--关闭战斗统计窗口
	--CFbDpsServer.CloseFbDpsInfoWnd(Player)
end


function CSenarioExplorationScene:OnPlayerLogIn(Player)
	Player:BeginStatistic("")
	CScenarioExploration.LogIn(Player)
	
	--添加战斗统计的人并且打开窗口
	--CFbDpsServer.OpenFbDpsInfoWnd(Player, "1队")
end

function CSenarioExplorationScene:OnPlayerLogOut(Player)
	CScenarioExploration.LeaveScene(Player)
	
	--关闭战斗统计窗口
	--CFbDpsServer.CloseFbDpsInfoWnd(Player)
end
