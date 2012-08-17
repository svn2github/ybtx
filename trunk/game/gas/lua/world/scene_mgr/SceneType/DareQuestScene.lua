gas_require "world/scene_mgr/SceneType/DareQuestSceneInc"


function CDareQuestScene:OnPlayerChangeOut(Player)
	CDareQuestFb.LeaveDareQuestFbScene(Player, self.m_SceneId)
end