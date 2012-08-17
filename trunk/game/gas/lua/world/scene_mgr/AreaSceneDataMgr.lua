CAreaSceneDataMgr = class()

g_AreaSceneDataMgr = CAreaSceneDataMgr:new()

g_AreaSceneDataMgr.m_AreaNpcIndexTbl = {}
g_AreaSceneDataMgr.m_AreaObjIndexTbl = {}
g_AreaSceneDataMgr.m_AreaTrapIndexTbl = {}
AddCheckLeakFilterObj(g_AreaSceneDataMgr.m_AreaNpcIndexTbl)
AddCheckLeakFilterObj(g_AreaSceneDataMgr.m_AreaObjIndexTbl)
AddCheckLeakFilterObj(g_AreaSceneDataMgr.m_AreaTrapIndexTbl)

function CAreaSceneDataMgr:GetNpcTbl(Scene, key, isDynamicCreate)
	local SrcTbl = g_NpcBornMgr._m_SinglePointBornTbl[key]
	local areaName = Scene.m_SceneAttr.AreaName
	if areaName and not isDynamicCreate then
		local sceneName = Scene.m_SceneName
		local Indextbl = self.m_AreaNpcIndexTbl[sceneName]
		if not Indextbl then
			Indextbl = {}
			for i, v in pairs(SrcTbl) do
				local name = g_AreaMgr:GetSceneAreaName(sceneName, {x = v.GridX, y = v.GridY})
				if name == areaName then
					table.insert(Indextbl, i)
				end
			end
			self.m_AreaNpcIndexTbl[sceneName] = Indextbl
		end
		
		return SrcTbl, Indextbl
	else
		return SrcTbl
	end
end


function CAreaSceneDataMgr:GetObjTbl(Scene, key, isDynamicCreate)
	local SrcTbl = g_IntObjServerMgr._m_IntObjCreateTbl[key]
	local areaName = Scene.m_SceneAttr.AreaName
	if areaName and not isDynamicCreate then
		local sceneName = Scene.m_SceneName
		local Indextbl = self.m_AreaObjIndexTbl[sceneName]
		
		if not Indextbl then
			Indextbl = {}
			for i, v in pairs(SrcTbl) do
				local name = g_AreaMgr:GetSceneAreaName(sceneName, {x = v.GridX, y = v.GridY})
				if name == areaName then
					table.insert(Indextbl, i)
				end
			end
			self.m_AreaObjIndexTbl[sceneName] = Indextbl
		end
		return SrcTbl, IndexTbl
	else
		return SrcTbl
	end
end

function CAreaSceneDataMgr:GetTrapTbl(Scene, key, isDynamicCreate)
	local SrcTbl = g_TrapServerMgr._m_TrapCreateTbl[key] or {}
	local areaName = Scene.m_SceneAttr.AreaName
	if areaName and not isDynamicCreate then
		local sceneName = Scene.m_SceneName
		local Indextbl = self.m_AreaTrapIndexTbl[sceneName]
		
		if not Indextbl then
			Indextbl = {}
			for i, v in pairs(SrcTbl) do
				local name = g_AreaMgr:GetSceneAreaName(sceneName, {x = v.GridX, y = v.GridY})
				if name == areaName then
					table.insert(Indextbl, i)
				end
			end
			self.m_AreaTrapIndexTbl[sceneName] = Indextbl
		end
		return SrcTbl, IndexTbl
	else
		return SrcTbl
	end
end
