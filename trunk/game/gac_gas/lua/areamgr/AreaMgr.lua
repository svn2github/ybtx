gac_gas_require "areamgr/AreaMgrInc"
cfg_load "scene/Scene_Basic_Common"
cfg_load "area/SceneArea_Common"

g_SceneAreaList = {}

local SceneNameMapping = 
{
	["艾米帝国树屋酒吧"] =  "鲜血艾米树屋酒吧",
	["神圣教廷树屋酒吧"] =  "鲜血神圣树屋酒吧",
	["修斯帝国树屋酒吧"] =  "鲜血修斯树屋酒吧",
}

local function InitSceneCfgShell()
	CreateCfgShell("Scene_Basic_Common_Shell", Scene_Basic_Common)
	CreateCfgShell("SceneArea_Common_Shell", SceneArea_Common)
	
	local arg = g_ParseCommandline and g_ParseCommandline()[3]

	if (GasConfig and GasConfig.GasType == 1) or (arg and arg.gastype == 1) then
		for oldSceneName, newSceneName in pairs(SceneNameMapping) do
			local newAttribute = Scene_Basic_Common(newSceneName)
			Scene_Basic_Common_Shell:UpdateAttribute(oldSceneName, "NpcSceneFile", newAttribute("NpcSceneFile") )
			Scene_Basic_Common_Shell:UpdateAttribute(oldSceneName, "SetAreaFile", newAttribute("SetAreaFile") )
										
			local newArea = SceneArea_Common(newSceneName)
			SceneArea_Common_Shell:UpdateAttribute(oldSceneName, "PictureWndIndex", newArea("PictureWndIndex") )
			SceneArea_Common_Shell:UpdateAttribute(oldSceneName, "AreaList", newArea("AreaList") )
			SceneArea_Common_Shell:UpdateAttribute(oldSceneName, "SceneMaxPos", newArea("SceneMaxPos") )
			SceneArea_Common_Shell:UpdateAttribute(oldSceneName, "PictureMinPos", newArea("PictureMinPos") )
			SceneArea_Common_Shell:UpdateAttribute(oldSceneName, "PictureMaxPos", newArea("PictureMaxPos") )
			SceneArea_Common_Shell:UpdateAttribute(oldSceneName, "PictureWidthBeforeCut", newArea("PictureWidthBeforeCut") )
			SceneArea_Common_Shell:UpdateAttribute(oldSceneName, "CutTop", newArea("CutTop") )
			SceneArea_Common_Shell:UpdateAttribute(oldSceneName, "CutBottom", newArea("CutBottom") )
			SceneArea_Common_Shell:UpdateAttribute(oldSceneName, "CutLeft", newArea("CutLeft") )
			SceneArea_Common_Shell:UpdateAttribute(oldSceneName, "CutRight", newArea("CutRight") )
		end
	end
end

InitSceneCfgShell()


local function LoadSceneArea_Common()
	local errnum = 0
	local mt = { 
		__index = function(t, k)
			 return rawget(t, k)
		end,
		__newindex = function() 
			error("can't not write a read-only table")
		end,
	}
	
	local Keys = SceneArea_Common_Shell:GetKeys()
	for i=1, table.getn(Keys) do
		local SceneName = Keys[i]
		g_SceneAreaList[SceneName] = {}
		local AreaList = assert(loadstring("return {" .. SceneArea_Common_Shell(SceneName, "AreaList") .. "}"))()
		for j = 1,table.getn(AreaList) do
			local AreaName = AreaList[j][1]
			if AreaName == "主路" then
				AreaName = SceneName.."主路"
			end
			g_SceneAreaList[SceneName][AreaName] = {AreaList[j][2], j}
		end
	end
	setmetatable(g_SceneAreaList, mt)
end

LoadSceneArea_Common()


local SceneNameTamp = ""
g_AreaMgr = CAreaMgr:new()

AddCheckLeakFilterObj(g_AreaMgr)

g_AreaMgr.m_SceneAreaTbl = {}
g_AreaMgr.m_SceneAreaKeyTbl = {}
g_AreaMgr.m_ForceAreaTbl = {}
g_AreaMgr.m_SceneAreaMusicTbl = {}
g_AreaMgr.m_SceneAreaFbTbl = {}
g_AreaMgr.m_RatingPlayerNumTbl = {}
g_AreaMgr.m_SceneAreaFbTblCache = {}


function CAreaMgr:InitSceneAreaSetting(key, areaNames, region)
	assert(region and areaNames, "区域文件版本不对!! 请更新摆怪编辑器,然后重新导出区域文件.")
	
	g_AreaMgr.m_SceneAreaKeyTbl[key] = {}
	g_AreaMgr.m_SceneAreaKeyTbl[key].AreaNames = areaNames
	g_AreaMgr.m_SceneAreaKeyTbl[key].Region = region
	
	local newNames = {}
	for i, name in pairs(areaNames) do
		if name == "主路" then
			newNames[i] = SceneNameTamp .. "主路"
		else
			newNames[i] = name
		end
	end
	self.m_SceneAreaTbl[SceneNameTamp] = {}
	self.m_SceneAreaTbl[SceneNameTamp].AreaNames = newNames
	self.m_SceneAreaTbl[SceneNameTamp].Region = region
end

function CAreaMgr:ReLoadAreaFile(SetAreaFile)
	local SrcAreaData = g_AreaMgr.m_SceneAreaKeyTbl[SetAreaFile]
	local areaNames = SrcAreaData.AreaNames
	local region = SrcAreaData.Region
	local newNames = {}
	for i, name in pairs(areaNames) do
		if name == "主路" then
			newNames[i] = SceneNameTamp .. "主路"
		else
			newNames[i] = name
		end
	end
	self.m_SceneAreaTbl[SceneNameTamp] = {}
	self.m_SceneAreaTbl[SceneNameTamp].AreaNames = newNames
	self.m_SceneAreaTbl[SceneNameTamp].Region = region
	
	self.m_SceneAreaFbTbl[SceneNameTamp] = self.m_SceneAreaFbTblCache[SetAreaFile]
end

function CAreaMgr:InitSceneMusicSetting(key, tbl)
	assert(nil == self.m_SceneAreaMusicTbl[key])
	self.m_SceneAreaMusicTbl[key] = tbl
	for name, music in pairs(tbl) do
		if name == "主路" then
			tbl[SceneNameTamp .. "主路"] = music
			tbl[name] = nil
		end
	end
end

function CAreaMgr:InitAreaState(key, areaFbTbl)
	assert(SceneNameTamp and Scene_Basic_Common_Shell(SceneNameTamp,"SceneType") == 1, "只有主场景才能设置 区域为副本")
	self.m_SceneAreaFbTbl[SceneNameTamp] = areaFbTbl
	self.m_SceneAreaFbTblCache[key] = areaFbTbl
	assert(areaFbTbl["主路"] == nil, "主路不能为 副本形式")
end

function CAreaMgr:InitRatingPlayerTbl(filePath, ratingPlayerTbl)
	assert(SceneNameTamp and Scene_Basic_Common_Shell(SceneNameTamp,"SceneType") == 1)
	self.m_RatingPlayerNumTbl[SceneNameTamp] = ratingPlayerTbl
end

function CAreaMgr:GetAreaMaxPlayerNum(SceneName, areaName)
	if self.m_SceneAreaFbTbl[SceneName] then
		return self.m_SceneAreaFbTbl[SceneName][areaName]
	end
end

function CAreaMgr:GetRatingPlayerNum(SceneName, areaName)
	if self.m_RatingPlayerNumTbl[SceneName] then
		return self.m_RatingPlayerNumTbl[SceneName][areaName]
	end
end

function CAreaMgr:GetSceneAreaMusic(SceneName, areaName)
	if Scene_Common[SceneName] then
		local SetAreaFile = Scene_Common[SceneName].SetAreaFile
		if SetAreaFile ~= nil and SetAreaFile ~= "" and self.m_SceneAreaMusicTbl[SetAreaFile] then
			return self.m_SceneAreaMusicTbl[SetAreaFile][areaName]
		end
	end
	return nil
end

function CAreaMgr:GetSceneAreaName(SceneName, Pos)
	local areaData = self.m_SceneAreaTbl[SceneName]
	if not areaData then
		return ""
	end
	local areaLine = areaData.Region[math.floor(Pos.x)]
	if areaLine then
		local y = math.floor(Pos.y)
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


function CAreaMgr:CheckIsInSameArea(SceneName,Player1,Player2)
	local GridPos1 = CPos:new()
	Player1:GetGridPos(GridPos1)
	
	local Area1 = self:GetSceneAreaName(SceneName,GridPos1)
	if not Area1 or Area1 == "" then
		return false
	end
	
	local GridPos2 = CPos:new()
	Player2:GetGridPos(GridPos2)
	
	local Area2 = self:GetSceneAreaName(SceneName,GridPos2)
	if Area2 ~= Area1 then
		return false
	end
	return true
end	



local function LoadSceneAreaSetting()
	local errnum = 0
	for _, SceneName in pairs(Scene_Basic_Common_Shell:GetKeys()) do
		if Scene_Basic_Common_Shell(SceneName, "SetAreaFile") ~= "" then
			SceneNameTamp = SceneName
			local SetAreaFile = Scene_Basic_Common_Shell(SceneName, "SetAreaFile")
			if not g_AreaMgr.m_SceneAreaKeyTbl[SetAreaFile] then
				cfg_require(SetAreaFile)
			else
				g_AreaMgr:ReLoadAreaFile(SetAreaFile)
			end
			local areaData = g_AreaMgr.m_SceneAreaTbl[SceneName]
			if not areaData then
				print("区域划分文件【"..SetAreaFile.."】中的【InitSceneAreaSetting】的路径参数有错！")
				errnum = errnum+1
			else
				for _, AreaName in pairs( areaData.AreaNames ) do
					if AreaName ~= "" 
						and SceneArea_Common_Shell(SceneName)
						and not g_SceneAreaList[SceneName][AreaName] then
						print("区域划分文件【"..SetAreaFile.."】中的【"..AreaName.."】在SceneArea_Common【"..SceneName.."】的AreaList中不存在")
						errnum = errnum+1
					end
				end
			end
		else
			--print("地图"..SceneName.."没有划分区域坐标")
		end
	end
	if errnum > 0 then
		assert(false,"区域划分文件存在以上错误！")
	end
end

LoadSceneAreaSetting()
