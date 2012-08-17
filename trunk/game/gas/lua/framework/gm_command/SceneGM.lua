gas_require "fb_game/ActionBasic"


--【测试场景(副本)跳转】
function GMCommand.Executor.ChangeScene( Connection, SceneName, x , y )
	local RealName = SceneName
	local Player = Connection.m_Player
	if not Scene_Common[RealName] then
		local str = "[" .. SceneName .. "] dosn`t exist!"
		Gas2Gac:ChangeSceneErrMsg(Connection, str, "")
		return
	elseif Scene_Common[RealName].SceneType ~= 1 and Scene_Common[RealName].SceneType ~= 7 then
		local str = "[%s] is an instance!"
		Gas2Gac:ChangeSceneErrMsg(Connection, str, RealName)
		return
	end
	
	local SceneType = g_SceneMgr:_GetSceneType(RealName)
	local TargetMateScene = nil
	if SceneType ~= nil then
		TargetMateScene = SceneType.MetaScene
		local TargetSceneWidth = TargetMateScene:GetWidthInGrid()
		local TargetSceneHeight = TargetMateScene:GetHeightInGrid()
		if x > TargetSceneWidth-1 or x < 0 or y > TargetSceneHeight-1 or y < 0  then
			local str = "The size of [%s] is [" .. TargetSceneWidth-1 .. " , " .. TargetSceneHeight-1 .. "]. The coordinate you input is out of the map, please check and input again"
			Gas2Gac:ChangeSceneErrMsg(Connection, str, SceneName)
			return 
		end
	end

	if Player.m_Scene.m_SceneName ~= RealName then
		ChangeSceneByName( Connection, RealName, x , y )
		--ChangeScene( Connection, SceneName, x , y )
	else
		local Pos = CPos:new()
		Pos.x = x
		Pos.y = y
		Player:SetGridPosByTransport(Pos)
		StressRpcToClient(Gas2Gac,"ChangedInSameSceneSuc", Player.m_Conn)
	end
end

--【修改其他角色场景】
function GMCommand.Executor.TargetChangescene( Connection, sTargetName,  SceneName, x , y )
	local RealName = SceneName
	local Player = g_GetPlayerInfoByName(sTargetName)
	if Player ~= nil then
		if not Scene_Common[RealName] then
			local str = "[" .. SceneName .. "] dosn`t exist!"
			Gas2Gac:ChangeSceneErrMsg(Connection, str, "")
			return
		elseif Scene_Common[RealName].SceneType ~= 1 and Scene_Common[RealName].SceneType ~= 7 then
			local str = "[%s] is an instance!"
			Gas2Gac:ChangeSceneErrMsg(Connection, str, RealName)
			return
		end
	
		local SceneType = g_SceneMgr:_GetSceneType(RealName)
		local TargetMateScene = nil
		if SceneType ~= nil then
			TargetMateScene = SceneType.MetaScene
			local TargetSceneWidth = TargetMateScene:GetWidthInGrid()
			local TargetSceneHeight = TargetMateScene:GetHeightInGrid()
			if x > TargetSceneWidth-1 or x < 0 or y > TargetSceneHeight-1 or y < 0  then
				local str = "The size of [%s] is [" .. TargetSceneWidth-1 .. " , " .. TargetSceneHeight-1 .. "]. The coordinate you input is out of the map, please check and input again"
				Gas2Gac:ChangeSceneErrMsg(Connection, str, RealName)
				return 
			end
		end

		if Player.m_Scene.m_SceneName ~= RealName then
			ChangeSceneByName( Player.m_Conn, RealName, x , y )
		else
			local Pos = CPos:new()
			Pos.x = x
			Pos.y = y
			Player:SetGridPosByTransport(Pos)
			StressRpcToClient(Gas2Gac,"ChangedInSameSceneSuc", Player.m_Conn)
		end
	else
		MsgToConn(Connection,19)
	end
end

--【人物在当前场景传送】
function GMCommand.Executor.goto( Connection, x , y )
		local Player = Connection.m_Player
		local newpos = CPos:new()
		newpos.x = x
		newpos.y = y
		Player:SetGridPosByTransport( newpos )
end

--【设置连击时间】
function GMCommand.Executor.setComboTime( Connection, time)
		g_ComboTime = time * 1000000
end

--【获取当前场景的格子的x，y最大值】
function GMCommand.Executor.SceneMaxGrid(Connection, sName)
	local SceneName = nil 
	if sName then
		SceneName = sName 
		if not Scene_Common[SceneName] then
			local str = "[" .. SceneName .. "] dosn`t exist!"
			Gas2Gac:RetGMGetSceneGridPos(Connection, str, SceneName)
			return
		end
	else
		SceneName = Connection.m_Player.m_Scene.m_SceneName
	end
	local SceneType = g_SceneMgr:_GetSceneType(SceneName)
	local GridX = SceneType.MetaScene:GetWidthInGrid()-1
	local GridY = SceneType.MetaScene:GetHeightInGrid()-1
	local str = "The Max coordinates of [%s] are [" .. GridX .. " , " .. GridY .. "], and the numbers of grid are (" .. (GridX+1) .. "," .. (GridY+1) .. ")"
	Gas2Gac:RetGMGetSceneGridPos(Connection, str, SceneName)
end

--【修改其他角色在当前场景的位置】
function GMCommand.Executor.TargetGoto( Connection, sTargetName, x , y )
		local Player = g_GetPlayerInfoByName(sTargetName)
		local newpos = CPos:new()
		newpos.x = x
		newpos.y = y
		if Player ~= nil then
			Player:SetGridPosByTransport( newpos )
		else
			MsgToConn(Connection,19)
		end
end

function GMCommand.Executor.ListScene( Connection )

	local SceneList = g_SceneMgr:ListScene()
	
	for i,v in ipairs(SceneList) do
		GMCommand:print( Connection, tostring(v.CoreSceneId) .. "\t" .. v.MetaSceneName  )
	end
end


function GMCommand.Executor.ClearLimitCount(Conn)
	if not Conn.m_Player then
		return 
	end
	local function callback()
		StressRpcToClient(Gas2Gac,"ClearActionTimesSucMsg", Conn)
	end
	local data = {}
	data["charId"] = Conn.m_Player.m_uID
	CallAccountAutoTrans(Conn.m_Account, "ActivityCountDB", "ClearJoinCount", callback, data)
end

g_FreeActionTime = false


function Gas2GasDef:FreeActionTime(Conn, type)
	g_FreeActionTime = (type == 1)
end

function GMCommand.Executor.FreeActionTime(Conn, type)
	for server_id in pairs(g_ServerList) do
		Gas2GasAutoCall:FreeActionTime(GetServerAutoConn(server_id), type)
	end
end

function Gas2GasDef:SetAreaPlayerNum(Conn, mainNum, fbNum)
	g_PublicAreaSceneMgr.m_IsGMSet = true
	g_PublicAreaSceneMgr.m_GM_MainNum = mainNum
	g_PublicAreaSceneMgr.m_GM_FbNum = fbNum
end

function Gas2GasDef:RevertAreaPlayerNum(Conn, mainNum, fbNum)
	g_PublicAreaSceneMgr.m_IsGMSet = nil
end



function GMCommand.Executor.SetAreaPlayerNum(Conn, mainNum, fbNum)
	for server_id in pairs(g_ServerList) do
		Gas2GasAutoCall:SetAreaPlayerNum(GetServerAutoConn(server_id), mainNum, fbNum)
	end
end


function GMCommand.Executor.RevertAreaPlayerNum(Conn)
	for server_id in pairs(g_ServerList) do
		Gas2GasAutoCall:RevertAreaPlayerNum(GetServerAutoConn(server_id))
	end
end


function GMCommand.Executor.SetActionTeamNum(Conn, actionName, minNum, maxNum, memberNum)
	local actionInfo = ActionBasic:GetActionInfo(actionName)
	if not actionInfo  then
		GMCommand:print(Conn, "This action is not exist")
		return
	end
	if not (actionInfo["MinTeams"] and actionInfo["MaxTeams"]) then
		GMCommand:print(Conn, "The number of players allowed to join this action can't be changed")
		return
	end
	if minNum <= 0 or minNum > maxNum or memberNum <= 0 then
		GMCommand:print(Conn, "the parameter's range is wrong")
		return
	end
	local data = {}
	data["ActionName"] = actionName
	data["MinNum"] = minNum
	data["MaxNum"] = maxNum
	data["MemberNum"] = memberNum
	CallDbTrans("JoinActionDB", "GM_SetActionTeamNum", nil, data)
end


local doFunctionSwitch = false

function GMCommand.Executor.DoFunction(Conn, code)
	if doFunctionSwitch then
		local fun, msg = loadstring(code)
		if not fun then
			GMCommand:print(Conn, msg)
		end
		fun()
	elseif code == "open" then
		doFunctionSwitch = true
	else
		GMCommand:print(Conn, "This GM Command is been shut down")
	end
end

function GMCommand.Executor.BeginBoss(Conn)
	GMBeginBossBattle(Conn)
end

function GMCommand.Executor.EndBoss(Conn)
	GMEndBossBattle(Conn)
end

function GMCommand.Executor.ResetDragon(Conn)
	CallDbTrans("DragonCaveDB", "DeleteDragonCave", nil, {})
end

function GMCommand.Executor.SetBossMaxPlayer(Conn, Num)
	GMSetBossBattleMaxPlayer(Conn, Num)
end

function GMCommand.Executor.SetWarner(Conn, GMPerValue, OnTickTime, MaxHoldTime, AllOpen)
	for server_id in pairs(g_ServerList) do
		Gas2GasAutoCall:GMSetWarner(GetServerAutoConn(server_id), GMPerValue, OnTickTime, MaxHoldTime, AllOpen)
	end
end
