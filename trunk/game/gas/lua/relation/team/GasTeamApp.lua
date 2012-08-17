CTeamAppl = class()
local appteamlist_box = "GasAppTeamListDB"
local teamlist = "appteamlist"
--查询列表信息
function CTeamAppl.getAppTeamList(Conn,nType)
	local Player = Conn.m_Player
	if not (Player and IsCppBound(Player)) then return end
		
		local function CallBack(AppTeamList)
			if not (Player and IsCppBound(Player)) then return end
			Gas2Gac:GetTeamListBegin(Conn,nType)
				 for k,v in pairs(AppTeamList) do
				 		local nID    = AppTeamList[k][1]
				 		local sName  = AppTeamList[k][2]
				 		local nCamp  = AppTeamList[k][3]
				 		local nClass = AppTeamList[k][4]
				 		local nLevel = AppTeamList[k][5] 
				 		local nType  = AppTeamList[k][6] 
				 		local sMsg 	 = AppTeamList[k][7] 
				 		local nTime  = AppTeamList[k][8] 
				 		if nCamp == Player:CppGetBirthCamp()then
				 		Gas2Gac:GetTeamList(Conn, nID, sName, nCamp, nClass, nLevel, nType, sMsg, nTime)
						end	
				 end
			Gas2Gac:GetTeamListEnd(Conn,nType)	
		end	
		local parameters = {}
		parameters.uCharID = Player.m_uID
		parameters.nType = nType
		--parameters.saveTime = tonumber(GetStaticTextClient(990003)) --配置表配置时间
		CallAccountManualTrans(Conn.m_Account, appteamlist_box, "inquireAppTeamList", CallBack, parameters, teamlist)
end


 --取消报名。离开排队列表
function CTeamAppl.leaveList(Conn,nType)
	local Player = Conn.m_Player
	if not (Player and IsCppBound(Player)) then return end

	local function CallBack(bFlag)
		if not bFlag then
			return
		else
			Gas2Gac:RenewTeamList(Conn,nType)  --返回成功取消，用于刷新列表
		end
	end
	
	local parameters = {}
		parameters.uCharID = Conn.m_Player.m_uID
		parameters.nType = nType
		CallAccountManualTrans(Conn.m_Account, appteamlist_box, "removeAppTeamList", CallBack, parameters, teamlist)
end

 --申请加入列表
function CTeamAppl.joinTeamList(Conn,nType,sMsg)
	local Player = Conn.m_Player
	if not (Player and IsCppBound(Player)) then return end
		
		local function CallBack(bFlag)
			if not bFlag then
				return
			else
				Gas2Gac:RenewTeamList(Conn,nType)  --用于刷新列表
				Gas2Gac:ChangeBtnState(Conn,nType)  --用于改变按键状态
			end
		end
		local parameters = {}
			parameters.uCharID = Player.m_uID
			parameters.nType   = nType
			parameters.sMsg = sMsg
			CallAccountManualTrans(Conn.m_Account, appteamlist_box, "addAppTeamList", CallBack, parameters, teamlist)
end

--玩家点击刷新列表
function CTeamAppl.upFreshTeamList(Conn,nType)
	
	local Player = Conn.m_Player
		if(Player.m_nUpdateOnlineTeamListTimeMark and os.time() - Player.m_nUpdateOnlineTeamListTimeMark < 10 ) then
				return --刷新时间间隔小于12秒(客户端有限制,正常情况不会进入这里)
			else
				Player.m_nUpdateOnlineTeamListTimeMark = os.time()
			end
	local function CallBack(AppTeamList)
				Gas2Gac:GetTeamListBegin(Conn,nType)
					 for k,v in pairs(AppTeamList) do
					 		local nID    = AppTeamList[k][1]
					 		local sName  = AppTeamList[k][2]
					 		local nCamp  = AppTeamList[k][3]
					 		local nClass = AppTeamList[k][4]
					 		local nLevel = AppTeamList[k][5] 
					 		local nType  = AppTeamList[k][6] 
					 		local sMsg 	 = AppTeamList[k][7] 
					 		local nTime  = AppTeamList[k][8] 
					 		if nCamp == Conn.m_Player:CppGetBirthCamp()then
					 		Gas2Gac:GetTeamList(Conn, nID, sName, nCamp, nClass, nLevel, nType, sMsg, nTime)
							end	
					 end
				Gas2Gac:GetTeamListEnd(Conn,nType)	
			end	
			local parameters = {}
			parameters.uCharID = Conn.m_Player.m_uID
			parameters.nType = nType
			--parameters.saveTime = tonumber(GetStaticTextClient(990003)) --配置表配置时间
			CallAccountManualTrans(Conn.m_Account, appteamlist_box, "inquireAppTeamList", CallBack, parameters, teamlist)
end
