gas_require "world/scene_mgr/SceneType/SceneInc"

--普通场景 类

function CScene:Init(uSceneId, sSceneName, CoreScene)
	CSceneBase.Init(self, uSceneId, sSceneName, CoreScene)
	self.m_AreaInfoTbl = {}
end



--判断是否满足进入该副本的条件(这里只是基本条件)
function CScene:JoinScene(Player, SceneName)
	--是否满足基本条件
	if not CSceneBase.IfJoinScene(self, Player) then
		return 
	end
	local sceneId, serverId = g_SceneMgr:GetSceneByName(SceneName)
	assert(sceneId, " 主场景: " .. SceneName .. " 未在 g_SceneMgr.m_tbl_MainScene_name 表中存在")

	return sceneId, serverId
end
