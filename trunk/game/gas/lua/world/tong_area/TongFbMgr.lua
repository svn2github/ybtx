gas_require "world/tong_area/TongFbMgrInc"
g_TongFbMgr = CTongFbMgr:new()
g_TongFbMgr.m_TongSceneName = {}
g_TongFbMgr.m_TongSceneName[1] = "帮会驻地副本01"
g_TongFbMgr.m_TongSceneName[2] = "帮会驻地副本02"
g_TongFbMgr.m_TongSceneName[3] = "帮会驻地副本03"

g_TongFbMgr.m_TongSceneTbl = {}

local EnterPos = {64, 64}
local CenterPos = {64, 64}

function CTongFbMgr:GetEnterPos()
	return EnterPos
end

function CTongFbMgr:GetCenterPos()
	return CenterPos
end

function CTongFbMgr:AddTongScene(tongId, scene, dbResult)
	if self.m_TongSceneTbl[tongId] then --快速跨服搬迁可能导致场景还没删掉
		self:CloseTongScene(tongId, self.m_TongSceneTbl[tongId].m_SceneId)
	end

	scene.m_TongId = tongId
	self.m_TongSceneTbl[tongId] = scene
	
	local dbBuildingTbl = dbResult["BuildingTbl"]
	local tongCamp = dbResult["TongCamp"]
	local centerPos = self:GetCenterPos()
	for i, buildingInfo in pairs(dbBuildingTbl) do
		g_BuildingMgr:CreateBuildingNpc(scene,buildingInfo, tongCamp, centerPos)
	end
end

function CTongFbMgr:RemoveTongScene(tongId)
	if tongId then
		self.m_TongSceneTbl[tongId] = nil
	end
end

--关闭帮会副本,(帮会解散或帮会驻地搬迁)
function CTongFbMgr:CloseTongScene(tongId, sceneId)
	local scene = self.m_TongSceneTbl[tongId]
	if not scene  or scene.m_IsClose or sceneId ~= scene.m_SceneId then
		return
	end

	
	g_BuildingMgr:DestroyAllTongBuildingNpc(tongId)
	self.m_TongSceneTbl[tongId] = nil

	scene.m_IsClose = true
	for playerId in pairs(scene.m_tbl_Player) do
		NotifyPlayerLeaveFbScene(playerId, 30, 8803)
	end
end