cfg_load "scene/NotShowMateIcon_Server"

--print("载入 tong !!!")
CGasTongmatePosMgr = class()
local g_TongmatePosMgr = {}
local GetPosTime = 5000 --毫秒

local function DestroyGetTongmatePosTick(TongID)
	--print("-- DestroyGetTongmatePosTick")
	if g_TongmatePosMgr[TongID] and 
		g_TongmatePosMgr[TongID].m_GetTongmatePosTick then
		UnRegisterTick(g_TongmatePosMgr[TongID].m_GetTongmatePosTick)
		g_TongmatePosMgr[TongID].m_GetTongmatePosTick = nil
	end
--	if g_TongmatePosMgr[TongID] and g_TongmatePosMgr[TongID].m_NeedSendPlayerTbl then
--		g_TongmatePosMgr[TongID].m_NeedSendPlayerTbl = nil
--	end
--	if g_TongmatePosMgr[TongID] and g_TongmatePosMgr[TongID].m_LastSendTime then
--		g_TongmatePosMgr[TongID].m_LastSendTime = nil
--	end
	--g_TongmatePosMgr[TongID] = nil
end


local function GetTongmatePosTick(Tick, TongID)
	--print("---tick")
	local tblMembers = g_GasTongMgr:GetMembers(TongID)
	-- 如果帮会成员只剩1个，注销Tick
	if #(tblMembers) <= 1 or next(g_TongmatePosMgr[TongID].m_NeedSendPlayerTbl) == nil then
		DestroyGetTongmatePosTick(TongID)
		return
	end
				
	local IsAnyOneOn = false
	local IsAnyOneNeed = false
	for i = 1, #(tblMembers) do
		local MemberId = tblMembers[i]
		local pMember = g_GetPlayerInfo(MemberId)
		if IsCppBound(pMember) then
			IsAnyOneOn = true
			local SceneName = pMember.m_Scene.m_LogicSceneName
			--只有在特定的地图才发送位置
			--for _, scene_name in pairs(g_GasTongMgr.MiddleMapShowTongMateScene) do
				--if SceneName == scene_name then	
					local GridPos = CPos:new()
					pMember:GetGridPos(GridPos)
					
					for id, Player in pairs(g_TongmatePosMgr[TongID].m_NeedSendPlayerTbl) do
						IsAnyOneNeed = true
						if IsCppBound(Player) then
							--for _, scene_name in pairs(g_GasTongMgr.MiddleMapShowTongMateScene) do
								--if Player.m_Scene.m_SceneName == scene_name then
									if id ~= MemberId then
										-- 如果MemberId是队友，不发消息			
										local IsTeamMate = false
										local TeamID = pMember.m_Properties:GetTeamID()
										local tblTeamMembers = g_TeamMgr:GetMembers(TeamID)
										for i = 1, #(tblTeamMembers) do
											local TeamMemberId = tblTeamMembers[i]
											if id == TeamMemberId then
												--print("这个是队友，不发消息")
												IsTeamMate = true
											end
										end
										if not IsTeamMate then
											local MemberName = pMember.m_Properties:GetCharName()
											if NotShowMateIcon_Server(SceneName) then
												Gas2GacById:HideLeavedTongmatePos(id, MemberId)	-- 通知所有在线的人隐藏 MemberId 的标志
											else
												Gas2GacById:SendTongmatePos(id,MemberId,SceneName,GridPos.x,GridPos.y, MemberName)
											end
										end
									end
								--end	
							--end
						end
					end
					
--					if not IsAnyOneNeed then
--						print("没人了")
--						DestroyGetTongmatePosTick(TongID)
--					end
				--end
			--end
		end
	end
	
	-- 如果没人需要发送消息，则注销	
	if not IsAnyOneOn then
		--print("没人在线")
		DestroyGetTongmatePosTick(TongID)
		--g_TongmatePosMgr[TongID] = nil
	end
	
	if not IsAnyOneNeed then
		--print("没人需要发送")
		DestroyGetTongmatePosTick(TongID)
		--g_TongmatePosMgr[TongID] = nil
	end
	
end

local function GetTongmatePos(Player)
	local TongID = Player.m_Properties:GetTongID()
	if TongID == 0 then
		return
	end
	local PlayerId = Player.m_uID
	
	if not g_TongmatePosMgr[TongID] then
		g_TongmatePosMgr[TongID] = {}
		--print("--------注册Tick")
		local tick = RegisterTick("GetTongmatePosTick",GetTongmatePosTick,GetPosTime,TongID,PlayerId)
		g_TongmatePosMgr[TongID].m_GetTongmatePosTick = tick
		g_TongmatePosMgr[TongID].m_NeedSendPlayerTbl = {}
		g_TongmatePosMgr[TongID].m_NeedSendPlayerTbl[PlayerId] = Player
	else
		if not g_TongmatePosMgr[TongID].m_GetTongmatePosTick then
			--print("--------注册Tick2222")
			local tick = RegisterTick("GetTongmatePosTick",GetTongmatePosTick,GetPosTime,TongID,PlayerId)
			g_TongmatePosMgr[TongID].m_GetTongmatePosTick = tick
		end
		if not g_TongmatePosMgr[TongID].m_NeedSendPlayerTbl then
			g_TongmatePosMgr[TongID].m_NeedSendPlayerTbl = {}
		end
		g_TongmatePosMgr[TongID].m_NeedSendPlayerTbl[PlayerId] = Player
	end
	
	---注册Tick前预先获取一次帮会成员的坐标
	local tblMembers = g_GasTongMgr:GetMembers(TongID)
	for i = 1, #(tblMembers) do
		local MemberId = tblMembers[i]
		if MemberId ~= PlayerId then
			local pMember = g_GetPlayerInfo(MemberId)
			
			-- 成员在特定地图才发送位置
			if IsCppBound(pMember) then
				--for _, scene_name in pairs(g_GasTongMgr.MiddleMapShowTongMateScene) do
					--if pMember.m_Scene.m_SceneName == scene_name then
				local SceneName = pMember.m_Scene.m_SceneName
				-- 只有同一张地图上的帮会成员的位置信息会被发送
				if SceneName == Player.m_Scene.m_SceneName then
					-- 如果MemberId是队友，不发消息			
					local IsTeamMate = false
					local TeamID = Player.m_Properties:GetTeamID()
					local tblTeamMembers = g_TeamMgr:GetMembers(TeamID)
					for i = 1, #(tblTeamMembers) do
						local TeamMemberId = tblTeamMembers[i]
						if MemberId == TeamMemberId then
							--print("这个是队友，不发消息")
							IsTeamMate = true
						end
					end
					if not IsTeamMate then
						local GridPos = CPos:new()
						pMember:GetGridPos(GridPos)
						
						local MemberName = pMember.m_Properties:GetCharName()
						-- 大于5秒才发送消息
						if not g_TongmatePosMgr[TongID].m_LastSendTime then
							g_TongmatePosMgr[TongID].m_LastSendTime = {}
						end
						
						if not g_TongmatePosMgr[TongID].m_LastSendTime[PlayerId] then -- 第一次打开
							--print("--------send第一次")
							if NotShowMateIcon_Server(SceneName) then
								Gas2GacById:HideLeavedTongmatePos(MemberId, PlayerId)	-- 通知所有在线的人隐藏PlayerID的标志
							else
								Gas2GacById:SendTongmatePos(PlayerId, MemberId, SceneName, GridPos.x, GridPos.y, MemberName)
								g_TongmatePosMgr[TongID].m_LastSendTime[PlayerId] = os.time()
							end
						else	
							local NowTime = os.time()
							if NowTime - g_TongmatePosMgr[TongID].m_LastSendTime[PlayerId] > 5 then
								if NotShowMateIcon_Server(SceneName) then
									Gas2GacById:HideLeavedTongmatePos(MemberId, PlayerId)	-- 通知所有在线的人隐藏PlayerID的标志
								else
									g_TongmatePosMgr[TongID].m_LastSendTime[PlayerId] = NowTime
									--print(NowTime.."--------------send!"..MemberName)
									Gas2GacById:SendTongmatePos(PlayerId, MemberId, SceneName, GridPos.x, GridPos.y, MemberName)
								end
							end 		
						end
					end
				end
					--end
				--end
			end
		end
	end
	

end

-- function CGasTongmatePosMgr:PlayerStopGetTongmatePos(Player)
function CGasTongmatePosMgr.PlayerStopGetTongmatePos(Player)
	local function Init()
		local TongID = Player.m_Properties:GetTongID()
		local PlayerId = Player.m_uID
		
		if g_TongmatePosMgr[TongID] then
			if g_TongmatePosMgr[TongID].m_NeedSendPlayerTbl 
				and g_TongmatePosMgr[TongID].m_NeedSendPlayerTbl[PlayerId] then
				g_TongmatePosMgr[TongID].m_NeedSendPlayerTbl[PlayerId] = nil
				-- 如果帮会成员只剩1个，注销Tick
				local tblMembers = g_GasTongMgr:GetMembers(TongID)
				if #(tblMembers) <= 1 or next(g_TongmatePosMgr[TongID].m_NeedSendPlayerTbl) == nil then
					DestroyGetTongmatePosTick(TongID)
				end
				
--				if g_TongmatePosMgr[TongID].m_LastSendTime 
--					and g_TongmatePosMgr[TongID].m_LastSendTime[PlayerId] then
--					g_TongmatePosMgr[TongID].m_LastSendTime[PlayerId] = nil
--				end
			end
		end
	
	end
	apcall(Init)
end

-- 新成员进入帮会时调用
--function CGasTongmatePosMgr:CreateGetTongmatePosTick(TongID,uTargetID,uPlayerID)
function CGasTongmatePosMgr.CreateGetTongmatePosTick(TongID,uTargetID,uPlayerID)
	Gas2GacById:CheckIsNeedSendTongmatePos(uTargetID, TongID)
end

-- 隐藏离开帮会的玩家、下线的玩家、离开指定地图的玩家的图标
-- function CGasTongmatePosMgr:StopSendLeavedTongmatePos(TongID, PlayerID)
function CGasTongmatePosMgr.StopSendLeavedTongmatePos(TongID, PlayerID)
	if not g_TongmatePosMgr[TongID] then
		return	
	end
	--print("隐藏离开帮会的玩家坐标  "..PlayerID.." 坐标")
	local tblMembers = g_GasTongMgr:GetMembers(TongID)
	for i = 1, #(tblMembers) do
		local MemberId = tblMembers[i]
		local pMember = g_GetPlayerInfo(MemberId)
		if IsCppBound(pMember) then		
			if MemberId ~= PlayerID then
				Gas2GacById:HideLeavedTongmatePos(MemberId, PlayerID)	-- 通知所有在线的人隐藏PlayerID的标志
			else
				-- 隐藏其他人的图标
				Gas2GacById:StopSendTongmatePos(PlayerID)
				
				if g_TongmatePosMgr[TongID] and g_TongmatePosMgr[TongID].m_NeedSendPlayerTbl then
					g_TongmatePosMgr[TongID].m_NeedSendPlayerTbl[PlayerID] = nil
				end
				if g_TongmatePosMgr[TongID].m_LastSendTime 
					and g_TongmatePosMgr[TongID].m_LastSendTime[PlayerId] then
					g_TongmatePosMgr[TongID].m_LastSendTime[PlayerId] = nil
				end
			end
		end
	end
	-- 下线时判断是否清空g_TongmatePosMgr
	if #(tblMembers) <= 1 
		or next(g_TongmatePosMgr[TongID]) == nil 
		or next(g_TongmatePosMgr[TongID].m_NeedSendPlayerTbl) == nil then
		--print("清空")
		DestroyGetTongmatePosTick(TongID)
		g_TongmatePosMgr[TongID] = nil
	end
end

-- function CGasTongmatePosMgr:GetTongmatePos(Conn)
function CGasTongmatePosMgr.GetTongmatePos(Conn)
	if not IsCppBound(Conn.m_Player) then
		return
	end
	
	local TongID = Conn.m_Player.m_Properties:GetTongID()
	local tblMembers = g_GasTongMgr:GetMembers(TongID)
	-- 如果帮会成员只剩1个，则返回
	if #(tblMembers) <= 1 then
		return
	end
	
	GetTongmatePos(Conn.m_Player)
end

-- function CGasTongmatePosMgr:StopGetTongmatePos(Conn)
function CGasTongmatePosMgr.StopGetTongmatePos(Conn)
	if not IsCppBound(Conn.m_Player) then
		return
	end
	CGasTongmatePosMgr.PlayerStopGetTongmatePos(Conn.m_Player)
end
