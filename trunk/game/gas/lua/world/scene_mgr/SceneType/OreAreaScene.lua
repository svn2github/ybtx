gas_require "world/scene_mgr/SceneType/OreAreaSceneInc"
--矿区副本 类

--判断是否满足进入该副本的条件
function COreAreaScene:JoinScene(Player)
	--是否满足基本条件
	
	return nil
end

function COreAreaScene:OnCreate()
	CreateOreAreaNpc(self)
end

function COreAreaScene:OnPlayerChangeIn(Player)
	CScenePkMgr.ChangePkState(Player)
end

function COreAreaScene:OnPlayerLogIn(Player)
	CScenePkMgr.ChangePkState(Player)
end
