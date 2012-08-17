gas_require "world/tong_area/BuildingAreaMgrInc"

g_BuildingAreaMgr = CBuildingAreaMgr:new()
g_BuildingAreaMgr.m_SceneAreaTbl = {}
g_BuildingAreaMgr.m_SceneAreaKeyTbl = {}

AddCheckLeakFilterObj(g_BuildingAreaMgr.m_SceneAreaTbl)

local WarZoneSceneName = {}
WarZoneSceneName[1] = "花语平原1"
WarZoneSceneName[2] = "花语平原2"
WarZoneSceneName[3] = "花语平原3"
WarZoneSceneName[4] = "花语平原4"
WarZoneSceneName[5] = "花语平原5"
WarZoneSceneName[6] = "花语平原6"

local WarZoneDefenderSceneName = {}
WarZoneDefenderSceneName[1] = "防御塔花语平原1"
WarZoneDefenderSceneName[2] = "防御塔花语平原2"
WarZoneDefenderSceneName[3] = "防御塔花语平原3"
WarZoneDefenderSceneName[4] = "防御塔花语平原4"
WarZoneDefenderSceneName[5] = "防御塔花语平原5"
WarZoneDefenderSceneName[6] = "防御塔花语平原6"

local WarZoneLifeTreeSceneName = {}
WarZoneLifeTreeSceneName[1] = "生命树花语平原1"
WarZoneLifeTreeSceneName[2] = "生命树花语平原2"
WarZoneLifeTreeSceneName[3] = "生命树花语平原3"
WarZoneLifeTreeSceneName[4] = "生命树花语平原4"
WarZoneLifeTreeSceneName[5] = "生命树花语平原5"
WarZoneLifeTreeSceneName[6] = "生命树花语平原6"

local TongFbSceneName = {}
TongFbSceneName[1] = "帮会驻地副本01"
TongFbSceneName[2] = "帮会驻地副本02"
TongFbSceneName[3] = "帮会驻地副本03"

local TongFbDefenderSceneName = {}
TongFbDefenderSceneName[1] = "防御塔帮会驻地副本01"
TongFbDefenderSceneName[2] = "防御塔帮会驻地副本02"
TongFbDefenderSceneName[3] = "防御塔帮会驻地副本03"

local TongFbLifeTreeSceneName = {}
TongFbLifeTreeSceneName[1] = "生命树帮会驻地副本01"
TongFbLifeTreeSceneName[2] = "生命树帮会驻地副本02"
TongFbLifeTreeSceneName[3] = "生命树帮会驻地副本03"

local TongChallengeSceneName = "帮会挑战本"

g_BuildingAreaMgr.m_SceneAreaKeyTbl["scene/Fb_HYPY_Area"] = WarZoneSceneName[1]
g_BuildingAreaMgr.m_SceneAreaKeyTbl["scene/Fb_HYPYDefender_Area"] = WarZoneDefenderSceneName[1]
g_BuildingAreaMgr.m_SceneAreaKeyTbl["scene/Fb_HYPYLifeTree_Area"] = WarZoneLifeTreeSceneName[1]
g_BuildingAreaMgr.m_SceneAreaKeyTbl["scene/Fb_TongArea01_Area"] = TongFbSceneName[1]
g_BuildingAreaMgr.m_SceneAreaKeyTbl["scene/Fb_TongArea02_Area"] = TongFbSceneName[2]
g_BuildingAreaMgr.m_SceneAreaKeyTbl["scene/Fb_TongArea03_Area"] = TongFbSceneName[3]
g_BuildingAreaMgr.m_SceneAreaKeyTbl["scene/Fb_TongDefenderArea01_Area"] = TongFbDefenderSceneName[1]
g_BuildingAreaMgr.m_SceneAreaKeyTbl["scene/Fb_TongDefenderArea02_Area"] = TongFbDefenderSceneName[2]
g_BuildingAreaMgr.m_SceneAreaKeyTbl["scene/Fb_TongDefenderArea03_Area"] = TongFbDefenderSceneName[3]
g_BuildingAreaMgr.m_SceneAreaKeyTbl["scene/Fb_TongLifeTreeArea01_Area"] = TongFbLifeTreeSceneName[1]
g_BuildingAreaMgr.m_SceneAreaKeyTbl["scene/Fb_TongLifeTreeArea02_Area"] = TongFbLifeTreeSceneName[2]
g_BuildingAreaMgr.m_SceneAreaKeyTbl["scene/Fb_TongLifeTreeArea03_Area"] = TongFbLifeTreeSceneName[3]
g_BuildingAreaMgr.m_SceneAreaKeyTbl["scene/Fb_TongChallenge_Area"] = TongChallengeSceneName

function CBuildingAreaMgr:InitWarZoneAreaSetting(key, stationToArea, areaNames, region)
	assert(stationToArea and region and areaNames, "区域文件版本不对!! 请更新摆怪编辑器,然后重新导出区域文件.")
	
	local SceneName = self.m_SceneAreaKeyTbl[key]
	self.m_SceneAreaKeyTbl[key] = SceneName
	self.m_SceneAreaTbl[SceneName] = {}
	self.m_SceneAreaTbl[SceneName].StationToArea = stationToArea
	self.m_SceneAreaTbl[SceneName].AreaNames = areaNames
	self.m_SceneAreaTbl[SceneName].Region = region
	
	for index = 2, 6 do
		self.m_SceneAreaTbl[WarZoneSceneName[index]] = self.m_SceneAreaTbl[SceneName]
	end
end

function CBuildingAreaMgr:InitWarZoneDefenderAreaSetting(key, stationToArea, areaNames, region)
	assert(stationToArea and region and areaNames, "区域文件版本不对!! 请更新摆怪编辑器,然后重新导出区域文件.")
	
	local SceneName = self.m_SceneAreaKeyTbl[key]
	self.m_SceneAreaKeyTbl[key] = SceneName
	self.m_SceneAreaTbl[SceneName] = {}
	self.m_SceneAreaTbl[SceneName].StationToArea = stationToArea
	self.m_SceneAreaTbl[SceneName].AreaNames = areaNames
	self.m_SceneAreaTbl[SceneName].Region = region
	
	for index = 2, 6 do
		self.m_SceneAreaTbl[WarZoneDefenderSceneName[index]] = self.m_SceneAreaTbl[SceneName]
	end
end

function CBuildingAreaMgr:InitWarZoneLifeTreeAreaSetting(key, stationToArea, areaNames, region)
	assert(stationToArea and region and areaNames, "区域文件版本不对!! 请更新摆怪编辑器,然后重新导出区域文件.")
	
	local SceneName = self.m_SceneAreaKeyTbl[key]
	self.m_SceneAreaKeyTbl[key] = SceneName
	self.m_SceneAreaTbl[SceneName] = {}
	self.m_SceneAreaTbl[SceneName].StationToArea = stationToArea
	self.m_SceneAreaTbl[SceneName].AreaNames = areaNames
	self.m_SceneAreaTbl[SceneName].Region = region
	
	for index = 2, 6 do
		self.m_SceneAreaTbl[WarZoneLifeTreeSceneName[index]] = self.m_SceneAreaTbl[SceneName]
	end
end

function CBuildingAreaMgr:InitTongAreaSetting(key, areaNames, region)
	assert(region and areaNames, "区域文件版本不对!! 请更新摆怪编辑器,然后重新导出区域文件.")
	local SceneName = self.m_SceneAreaKeyTbl[key]
	self.m_SceneAreaKeyTbl[key] = SceneName
	self.m_SceneAreaTbl[SceneName] = {}
	self.m_SceneAreaTbl[SceneName].AreaNames = areaNames
	self.m_SceneAreaTbl[SceneName].Region = region
end

--function CBuildingAreaMgr:InitTongDefenderAreaSetting(key, areaNames, region)
--	assert(region and areaNames, "区域文件版本不对!! 请更新摆怪编辑器,然后重新导出区域文件.")
--	local SceneName = self.m_SceneAreaKeyTbl[key]
--	self.m_SceneAreaKeyTbl[key] = SceneName
--	self.m_SceneAreaTbl[SceneName] = {}
--	self.m_SceneAreaTbl[SceneName].AreaNames = areaNames
--	self.m_SceneAreaTbl[SceneName].Region = region
--end

cfg_require "scene/Fb_HYPY_Area"
cfg_require "scene/Fb_TongArea01_Area"
cfg_require "scene/Fb_TongArea02_Area"
cfg_require "scene/Fb_TongArea03_Area"
cfg_require "scene/Fb_TongChallenge_Area"
cfg_require "scene/Fb_HYPYDefender_Area"
cfg_require "scene/Fb_HYPYLifeTree_Area"
cfg_require "scene/Fb_TongDefenderArea01_Area"
cfg_require "scene/Fb_TongDefenderArea02_Area"
cfg_require "scene/Fb_TongDefenderArea03_Area"
cfg_require "scene/Fb_TongLifeTreeArea01_Area"
cfg_require "scene/Fb_TongLifeTreeArea02_Area"
cfg_require "scene/Fb_TongLifeTreeArea03_Area"

function CBuildingAreaMgr:GetSceneAreaName(SceneName, Pos)
	local areaData = self.m_SceneAreaTbl[SceneName]
	if not areaData then
		return ""
	end
	local areaLine = areaData.Region[Pos.x]
	if areaLine then
		local y = Pos.y
		for i = 1, #areaLine, 3 do
			if areaLine[i] <= y then
				if areaLine[i + 1] >= y then
					return areaData.AreaNames[areaLine[i + 2]]
				end
			else
				return ""
			end
		end
	end
	return ""
end

function CBuildingAreaMgr:GetSceneAreaNameByStationId(SceneName, stationId)
	local SceneArea = self.m_SceneAreaTbl[SceneName]
	assert(SceneArea, "区域文件摆放有问题")
	local AreaIndex = SceneArea.StationToArea[stationId]
	local AreaName = SceneArea.AreaNames[AreaIndex]
	return AreaName
end
