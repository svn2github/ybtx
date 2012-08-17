gas_require "world/scene_mgr/SceneType/MercenaryMonsterSceneInc"
gac_gas_require "team/TeamMgr"
--佣兵团刷怪本 类



--判断是否满足进入该副本的条件 参数：人物,进入的副本名
function CMercenaryMonsterScene:JoinScene(Player, SceneName)
	--是否满足基本条件
	if not CSceneBase.IfJoinScene(self, Player) then
		return nil
	end
	return true  --由于之前结构参数条件不够,将会在AreaFbChangeScene 中继续判断该进哪个副本
end


function CMercenaryMonsterScene:OnPlayerChangeOut(Player)
	LeaveMercenaryMonsterFbScene(Player, self.m_SceneId)
end

function CMercenaryMonsterScene:OnPlayerChangeIn(Player)
	IntoMercenaryMonsterFbScene(Player)
	CScenePkMgr.ChangePkState(Player)
	--AddMercenaryLevelCount(Player.m_Conn, "佣兵比赛", Player.m_Scene.m_SceneName)
end
