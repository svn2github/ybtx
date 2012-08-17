cfg_load "scene/NotShowMateIcon_Server"

CGasTeammatePosMgr = class()
local g_TeammatePosMgr = {}
local GetPosTime = 3000 --毫秒

local function GetTeammatePosTick(Tick, TeamID)
	--print("TeamTick!!!!!!!!!!~~~")
	local tblMembers = g_TeamMgr:GetMembers(TeamID)
	for i = 1, #(tblMembers) do
		local MemberId = tblMembers[i]
		local pMember = g_GetPlayerInfo(MemberId)
		if IsCppBound(pMember) then
			local GridPos = CPos:new()
			pMember:GetGridPos(GridPos)
			local SceneName = pMember.m_Scene.m_LogicSceneName
			for id, Player in pairs(g_TeammatePosMgr[TeamID].m_NeedSendPlayerTbl) do
				if id ~= MemberId then
					if NotShowMateIcon_Server(SceneName) then
						Gas2GacById:HideLeavedTeammatePos(id, MemberId)
					else
						Gas2GacById:SendTeammatePos(id,MemberId,SceneName,GridPos.x,GridPos.y)
					end
				end
			end
		end
	end
end

--function CGasTeammatePosMgr:GetTeammatePosition(Player)
function CGasTeammatePosMgr.GetTeammatePosition(Player)
	local TeamID = Player.m_Properties:GetTeamID()
	if TeamID == 0 then
		return
	end
	
	local PlayerId = Player.m_uID
	
	---注册Tick前预先获取一次队友的坐标
	local tblMembers = g_TeamMgr:GetMembers(TeamID)
	for i = 1, #(tblMembers) do
		local MemberId = tblMembers[i]
		if MemberId ~= PlayerId then
			local pMember = g_GetPlayerInfo(MemberId)
			if IsCppBound(pMember) then
				local GridPos = CPos:new()
				pMember:GetGridPos(GridPos)
				local SceneName = pMember.m_Scene.m_LogicSceneName
				
				if not NotShowMateIcon_Server(SceneName) then
					Gas2GacById:SendTeammatePos(PlayerId,MemberId,SceneName,GridPos.x,GridPos.y)
				end
				
				-- 如果Player和Member在一个工会，则清除Member在Player中地图上的工会图标
				Gas2GacById:HideLeavedTongmatePos(PlayerId, MemberId)
			end
		end
	end
	
	if not g_TeammatePosMgr[TeamID] then
		g_TeammatePosMgr[TeamID] = {}
		local tick = RegisterTick("GetTeammatePosTick",GetTeammatePosTick,GetPosTime,TeamID)
		g_TeammatePosMgr[TeamID].m_GetTeammatePosTick = tick
		g_TeammatePosMgr[TeamID].m_NeedSendPlayerTbl = {}
		g_TeammatePosMgr[TeamID].m_NeedSendPlayerTbl[PlayerId] = Player
	else
		if not g_TeammatePosMgr[TeamID].m_GetTeammatePosTick then
			local tick = RegisterTick("GetTeammatePosTick",GetTeammatePosTick,GetPosTime,TeamID)
			g_TeammatePosMgr[TeamID].m_GetTeammatePosTick = tick
		end
		g_TeammatePosMgr[TeamID].m_NeedSendPlayerTbl[PlayerId] = Player
	end
end

-- function CGasTeammatePosMgr:PlayerStopGetTeammatePos(Player)
function CGasTeammatePosMgr.PlayerStopGetTeammatePos(Player)
	local function Init()
		local TeamID = Player.m_Properties:GetTeamID()
		local PlayerId = Player.m_uID
		if g_TeammatePosMgr[TeamID] then
			g_TeammatePosMgr[TeamID].m_NeedSendPlayerTbl[PlayerId] = nil
			if next(g_TeammatePosMgr[TeamID].m_NeedSendPlayerTbl) == nil then
				CGasTeammatePosMgr.DestroyGetTeammatePosTick(TeamID)
			end
		end
	end
	apcall(Init)
end

-- function CGasTeammatePosMgr:CreateGetTeammatePosTick(TeamID,uTargetID,uPlayerID)
function CGasTeammatePosMgr.CreateGetTeammatePosTick(TeamID,uTargetID,uPlayerID)
	Gas2GacById:CheckIsNeedSendTeammatePos(uTargetID)
	Gas2GacById:CheckIsNeedSendTeammatePos(uPlayerID)
end

-- function CGasTeammatePosMgr:DestroyGetTeammatePosTick(TeamID)
function CGasTeammatePosMgr.DestroyGetTeammatePosTick(TeamID)
	--print("DestroyGetTeammatePosTick")
	if g_TeammatePosMgr[TeamID] and 
		g_TeammatePosMgr[TeamID].m_GetTeammatePosTick then
		UnRegisterTick(g_TeammatePosMgr[TeamID].m_GetTeammatePosTick)
	end
	g_TeammatePosMgr[TeamID] = nil
end

-- function CGasTeammatePosMgr:LeaveTeamStopGetTeammatePos(TeamID,PlayerId)
function CGasTeammatePosMgr.LeaveTeamStopGetTeammatePos(TeamID,PlayerId)
	if g_TeammatePosMgr[TeamID] then
		g_TeammatePosMgr[TeamID].m_NeedSendPlayerTbl[PlayerId] = nil
		local tblMembers = g_TeamMgr:GetMembers(TeamID)
		if #(tblMembers) <= 1 or next(g_TeammatePosMgr[TeamID].m_NeedSendPlayerTbl) == nil then
			CGasTeammatePosMgr.DestroyGetTeammatePosTick(TeamID)
		end
	end
end

function CGasTeammatePosMgr.GetTeammatePos(Conn)
	if not IsCppBound(Conn.m_Player) then
		return
	end
	CGasTeammatePosMgr.GetTeammatePosition(Conn.m_Player)
end

function CGasTeammatePosMgr.StopGetTeammatePos(Conn)
	if not IsCppBound(Conn) or not IsCppBound(Conn.m_Player) then
		return
	end
	CGasTeammatePosMgr.PlayerStopGetTeammatePos(Conn.m_Player)
end
