cfg_load "fb_game/AreaFb_Common"


g_AreaFbSceneMgr = {}
g_AreaFbSceneMgr["general"] = {}--存普通的区域副本(按类型来分)
g_AreaFbSceneMgr["typeName"] = {"自然", "暗黑", "破坏", "护甲"} --类型对应名字
g_AreaFbSceneMgr["BossNames"] = {}
g_AreaFbSceneMgr["BossNum"] = {}
g_AreaFbSceneMgr["ResetMode"] = {}
g_AreaFbLev = {}  -- 存储各个副本的等级
AddCheckLeakFilterObj(g_AreaFbSceneMgr)
AddCheckLeakFilterObj(g_AreaFbLev)

local function AreaFbSceneMgr()
	--普通副本 AreaFb_Common 读取
	for _, SceneName in pairs(AreaFb_Common:GetKeys()) do
		local p = AreaFb_Common(SceneName)
--		if g_AreaFbSceneMgr["general"][p("Type")] == nil then
--			g_AreaFbSceneMgr["general"][p("Type")] = {}
--		end
--		table.insert(g_AreaFbSceneMgr["general"][p("Type")], SceneName)
		
		-- 记录副本等级
		g_AreaFbLev[SceneName] = AreaFb_Common(SceneName, "NeedLevel")
		
		-- 记录副本重置模式
		g_AreaFbSceneMgr.ResetMode[SceneName] = loadstring("return {" .. AreaFb_Common(SceneName, "ResetMode") .. "}")()
		
		-- 记录副本BOSS名称
	  local BossNames = loadstring("return {" .. AreaFb_Common(SceneName, "BossName") .. "}")()
	  local BossNum = 0
	  if not g_AreaFbSceneMgr.BossNames[SceneName] then
	  	g_AreaFbSceneMgr.BossNames[SceneName] = {}
	  end
	  for _, Name in pairs(BossNames[1]) do
	  	BossNum = BossNum + 1
	  	g_AreaFbSceneMgr.BossNames[SceneName][Name] = true
	  end
	  g_AreaFbSceneMgr.BossNum[SceneName] = BossNum
	  
	end	
end
AreaFbSceneMgr()


