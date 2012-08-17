CMainSceneAreaMgr = class()

g_MainSceneAreaMgr = CMainSceneAreaMgr:new()

function CMainSceneAreaMgr:GetAreaPlayerNum(scene, areaName)
	if scene.m_AreaInfoTbl and scene.m_AreaInfoTbl[areaName] then
		return scene.m_AreaInfoTbl[areaName].m_PlayerNum
	end
	return 0
end


function CMainSceneAreaMgr:IsAreaFull(scene, areaName)
	local uniteName = scene.m_SceneName .. areaName
	local curNum = self:GetAreaPlayerNum(scene, areaName)
	if Scene_Common[uniteName] then
		local maxnum = Scene_Common[uniteName].MaxPlayerNum
		if g_PublicAreaSceneMgr.m_IsGMSet then
			maxnum = g_PublicAreaSceneMgr.m_GM_MainNum
		end
		return curNum >= maxnum
	end
	return false
end

local minRebornRate =	0.2				--调节到的最小刷行率

function CMainSceneAreaMgr:GetRebornRate(scene, pos)
	local sceneType = scene.m_SceneAttr.SceneType
	if sceneType == 1 then
		local areaName = g_AreaMgr:GetSceneAreaName(scene.m_SceneName, pos)
		local ratingNum = g_AreaMgr:GetRatingPlayerNum(scene.m_SceneName, areaName)
		if not ratingNum then --不设置区域人数不调节刷新率
			return 1
		end
			
		local playerNum = self:GetAreaPlayerNum(scene, areaName)
		if playerNum <= ratingNum then  --小于额定人数,不调节刷新速率
			return 1
		end
		
		return ratingNum / playerNum
	elseif sceneType == 25  then
		local ratingNum = g_AreaMgr:GetRatingPlayerNum(scene.m_SceneName, scene.m_SceneAttr.AreaName)
		if not ratingNum then --不设置区域人数不调节刷新率
			return 1
		end
		local playerNum = g_SceneMgr:GetScenePlayerNumber(scene)
		if playerNum <= ratingNum then  --小于额定人数,不调节刷新速率
			return 1
		end
		return ratingNum / playerNum
	else
		return 1
	end
end





























