--获取ActionTbl里每个活动的次数
function Gac2Gas:GetActionCount(Conn)	
	if not (IsCppBound(Conn) and Conn.m_Player) then
		return
	end
	
	local player = Conn.m_Player
	local playerId = player.m_uID	
	
	local function CallBack(result)
		for ActionName, CountsTbl in pairs(result) do
			Gas2Gac:SetActionCount(Conn, ActionName, CountsTbl[1], CountsTbl[2])
		end
		-- 发送完所有活动次数后，打开窗口
		local nowtime = os.time()
		local nowdate = os.date("*t")
		local nowhour = nowdate.hour	
		local nowmin = nowdate.min	
		Gas2Gac:RetShowActionInfoWnd(Conn, nowhour, nowmin)
	end
	
	local data = {}
	data["char_id"] = playerId
	CallAccountManualTrans(Conn.m_Account, "ActivityCountDB", "GetAllActivityCount", CallBack, data)
end

function Gac2Gas:GetDungeonCount(Conn)	
	if not (IsCppBound(Conn) and Conn.m_Player) then
		return
	end
	
	local player = Conn.m_Player
	local playerId = player.m_uID	
	
	local function CallBack(result)
		for ActionName, Counts in pairs(result) do
			Gas2Gac:SetDungeonCount(Conn, ActionName, Counts)
		end
		-- 发送完所有活动次数后，打开窗口
		Gas2Gac:RetShowDungeonInfoWnd(Conn)
	end
	
	local data = {}
	data["char_id"] = playerId
	CallAccountManualTrans(Conn.m_Account, "ActivityCountDB", "GetAllDungeonCount", CallBack, data)
end

function DbCallBack:UpdataActionNum(uCharId, ActionName, AddNum)
	if ActionName and ActionName ~= "" then 
		Gas2GacById:AddActionNum(uCharId, ActionName, AddNum)
	end
end

function Gac2Gas:GetActionCountEx(Conn)
	if not (IsCppBound(Conn) and Conn.m_Player) then
		return
	end
	
	local player = Conn.m_Player
	local playerId = player.m_uID
	
	local function CallBack(result)
		Gas2Gac:ReturnGetActionCountExBegin(Conn)
		for ActionName, CountsTbl in pairs(result[1]) do
			Gas2Gac:ReturnGetActionCountEx(Conn, ActionName, CountsTbl[1], CountsTbl[2])
		end
		for i = 1, result[2]:GetRowNum() do
			Gas2Gac:ReturnGetActionAllTimes( Conn, result[2]:GetData(i-1,0), result[2]:GetData(i-1,1) )
		end
		Gas2Gac:ReturnGetActionCountExEnd(Conn)
	end
	
	local data = {}
	data["char_id"] = playerId
	CallAccountManualTrans(Conn.m_Account, "ActivityCountDB", "GetGuideInfo", CallBack, data)
end

function Gac2Gas:EnterFbSceneFromWnd(Conn, SceneName)
	local Player = Conn.Conn.m_Player
	if(not Player and not IsCppBound(Player) ) then return end
	if( Player:IsInBattleState() ) then
		MsgToConn(Conn, 410023)
		return
	end
	local SceneCommon = Scene_Common[SceneName]
	if SceneCommon then
		NewChangeScene(Conn, SceneName, SceneCommon.InitPosX , SceneCommon.InitPosY)
	end
end
