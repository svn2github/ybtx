gac_gas_require "areamgr/AreaMgr"


local function CopySceneAttrTbl(srcTbl, SceneName)
		
	local tbl = {}

	--for _, info in pairs(srcTbl) do
	tbl.SceneName = SceneName
	tbl.InitPosX = srcTbl("InitPosX")
	tbl.InitPosY = srcTbl("InitPosY")
	
	tbl.MapFile = srcTbl("MapFile")
	tbl.AoiPic = srcTbl("AoiPic")
	tbl.MaxSceneNum = srcTbl("MaxSceneNum")
	
	tbl.NpcSceneFile = srcTbl("NpcSceneFile")
	tbl.NpcSceneFile = srcTbl("NpcSceneFile")
	tbl.SetAreaFile = srcTbl("SetAreaFile")
	
	tbl.RebornType = srcTbl("RebornType")
	tbl.SceneType = srcTbl("SceneType")
	tbl.LifeCycle = srcTbl("LifeCycle")
	tbl.Camp = srcTbl("Camp")
	tbl.PkType = srcTbl("PkType")
	
	tbl.MinPlayerNum = srcTbl("MinPlayerNum")
	tbl.MaxPlayerNum = srcTbl("MaxPlayerNum")
	tbl.LowestLev = srcTbl("LowestLev")
	tbl.HighestLev = srcTbl("HighestLev")
	tbl.IsCanTransmit = srcTbl("IsCanTransmit")
	tbl.IsCanSearch = srcTbl("IsCanSearch")
	tbl.NpcLimit = srcTbl("NpcLimit")
	tbl.ObjLimit = srcTbl("ObjLimit")
	tbl.TrapLimit = srcTbl("TrapLimit")
	return tbl
end   
      
      
function GetAttributeSceneName(sceneName)
	if GasConfig and GasConfig.GasType == 1 then
		return SceneNameMapping[sceneName] or sceneName
	end
	return sceneName
end


function ResetSceneFile(sceneName, fileName)
	
end

      
local function Load_All_Scene_Common()
	if Scene_Common then
		return
	end 
	    
	Scene_Common = {}
	AddCheckLeakFilterObj(Scene_Common)

	for _, name in pairs(Scene_Basic_Common_Shell:GetKeys()) do
		Scene_Common[name] = CopySceneAttrTbl(Scene_Basic_Common_Shell(name),name)
		--Scene_Common(name) = Scene_Basic_Common_Shell(name)
	end 
	
	for sceneName, v in pairs(g_AreaMgr.m_SceneAreaFbTbl) do
		for areaName, playerNum in pairs(v) do
			local uniteName = sceneName .. areaName
			assert(Scene_Common[uniteName] == nil)
			local attribute = CopySceneAttrTbl(Scene_Basic_Common_Shell(sceneName),sceneName)
			attribute.SceneName = uniteName
			attribute.SceneType = 25
			attribute.MaxPlayerNum = playerNum
			attribute.AreaName = areaName
			attribute.BasicName = sceneName
			Scene_Common[uniteName] = attribute
			
			g_AreaMgr.m_SceneAreaTbl[uniteName] = g_AreaMgr.m_SceneAreaTbl[sceneName]
			g_AreaMgr.m_RatingPlayerNumTbl[uniteName] = g_AreaMgr.m_RatingPlayerNumTbl[sceneName]
		end
	end
	
	--- 重新设置pvp活动服务器的场景属性

	
end



Load_All_Scene_Common()
