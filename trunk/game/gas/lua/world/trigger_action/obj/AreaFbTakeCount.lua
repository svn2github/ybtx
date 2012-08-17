--这里的传送是指 点击一个obj后传到另一个地方(要读条的)
gas_require "framework/main_frame/SandBoxDef"

local g_AreaFbSceneMgr = g_AreaFbSceneMgr
local AreaFb_Common = AreaFb_Common
local FbActionDirect_Common = FbActionDirect_Common
local CallDbTrans = CallDbTrans
local Entry = CreateSandBox(...)

function Entry.Exec(Conn, ServerIntObj, ObjName, GlobalID)
	local Player = Conn.m_Player
	local Scene = Player.m_Scene
	local SceneName = Scene.m_LogicSceneName

	--print("你开了个宝箱")
	--Scene.m_KilledBossNum = Scene.m_KilledBossNum + 1
	
	-- 如果已经计过数就不再计了
	if Scene.m_AddAreaFbCount then
		return
	end

	Scene.m_AddAreaFbCount = true	-- 表示已经被记过数
	local parameters = {}
	parameters["SceneID"] = Scene.m_SceneId
	
	local ResetMode = g_AreaFbSceneMgr.ResetMode[SceneName]
	if ResetMode[1] == 1 or ResetMode[1] == 2 then
		parameters["ExtraLimitType"] = SceneName
	end
	local ActionName = AreaFb_Common(SceneName, "ActionName")	
	local limitType = FbActionDirect_Common(ActionName, "MaxTimes")
	parameters["ActivityName"] = limitType
	parameters["SceneName"]	= SceneName
	
	for PlayerID, _ in pairs(Scene.m_tbl_Player) do
		parameters["PlayerId"] = PlayerID
		--print(PlayerID.."计数")

		local function CallBack()
			-- 标示PlayerID加过计数
			if not Scene.m_AddCountPlayerList then
				Scene.m_AddCountPlayerList = {}
			end
			Scene.m_AddCountPlayerList[PlayerID] = true
		end
		CallDbTrans("ActivityCountDB", "AddAreaFbCount", CallBack, parameters, PlayerID)
	end

end
 
return Entry
 
