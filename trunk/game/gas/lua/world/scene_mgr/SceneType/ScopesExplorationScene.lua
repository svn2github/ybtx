gas_require "world/scene_mgr/SceneType/ScopesExplorationSceneInc"


--判断是否满足进入该副本的条件 参数：人物,进入的副本名
function CScopesExplorationScene:JoinScene(Player, SceneName)
	--是否满足基本条件
	if not CSceneBase.IfJoinScene(self, Player) then
		return nil
	end
	return true  --由于之前结构参数条件不够,将会在AreaFbChangeScene 中继续判断该进哪个副本
end

function CScopesExplorationScene:IsCanDestroy()
	if self:IsHavePlayer() then
		return false
	end
--	if self.m_State ~= g_FbRoomState.GameEnd then
--		return false
--	end
	return true
end

function CScopesExplorationScene:OnPlayerChangeIn(Player)
	CScopesExploration.IntoScene(Player)
	CScenePkMgr.ChangePkState(Player)
end
