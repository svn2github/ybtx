CGasTongMarket = class()
local market_box = "GasTongMarketDB"


------------------------------------------------RPC函数------------------------------------------------------
----【收购粮草】
--function Gac2Gas:AddTongBuyOrder(Conn, nNum,nPrice)
--  local Player = Conn.m_Player
--  if not Player then
--  	return
--  end
--	local uPlayerID = Player.m_uID
--	local uTongID = Player.m_Properties:GetTongID()
--	if uTongID == 0 then return end
--	--回调函数【高速逻辑】
--	local function CallBack(uMsgID)
--			if IsNumber(uMsgID) then
--				MsgToConn(Conn, uMsgID)
--			else
--				MsgToConn(Conn, 9015)
--			end 
--	end
--	
--  local parameters = {}
--	parameters["nPrice"]	= nPrice
--	parameters["nNum"]	= nNum
--	parameters["uPlayerID"]	= uPlayerID
--	CallAccountManualTrans(Conn.m_Account, market_box, "AddTongBuyOrderDB", CallBack, parameters, uTongID)
--end

--【出售粮草】
function CGasTongMarket.SellTongResource(Conn,TongID)  
	local Player = Conn.m_Player
	if not Player then
		return
	end
	local Truck = Player:GetServantByType(ENpcType.ENpcType_Truck)
	if not IsServerObjValid(Truck) then
		MsgToConn(Conn, 9013)
		return
	end
	
	local Pos = CFPos:new()
	Truck:GetPixelPos(Pos)
	local uSellNum = Truck.m_TruckInfo:GetResource()
	local uPlayerID = Player.m_uID
	local uTongID = Player.m_Properties:GetTongID()
	local uCamp = Player:CppGetCamp()

	if uSellNum <= 0 then
		MsgToConn(Conn, 9036)
		return
	end
	
	local parameters = {}
	parameters["TongID"]	= TongID
	parameters["uPlayerID"]	= uPlayerID
	parameters["uSellNum"] = uSellNum
	parameters["uCamp"] = uCamp
	parameters["sceneName"] = Player.m_Scene.m_SceneName
	parameters["state"] = g_WarZoneMgr.m_CurrentTransportState
	local function CallBack(nFlag, uDropResource, profferFlag, profferValue)
		if nFlag then
			if uDropResource > 0 then
				DropResource(Truck.m_Scene, Pos.x, Pos.y, uDropResource)
			end
			UnloadResource(Truck)

			if profferFlag then
				MsgToConn(Conn, 9448, profferValue)
			else
				MsgToConn(Conn, 9044)
			end
			MsgToConn(Conn, 9014)
		else
			--此时uDropResource表示失败提示Id
			MsgToConn(Conn, uDropResource)
			return
		end
	end
	OnSavePlayerExpFunc({Player})
	CallAccountManualTrans(Conn.m_Account, market_box, "SellTongResourceDB", CallBack, parameters, TongID, uTongID, uPlayerID)
end

----【查询收购订单】
--function Gac2Gas:GetTongMarketOrderInfoList(Conn)
--	local Player = Conn.m_Player
--	if not Player then
--		return
--	end
--	local uTongID = Player.m_Properties:GetTongID()
--	if uTongID == 0 then
--		MsgToConn(Conn, 9401)
--		return
--	end
--	local uCamp = Player:CppGetCamp()
--	--回调函数【高速逻辑】
--	local function CallBack(tblRet)
--		Gas2Gac:ReturnGetTongMarketOrderInfoListBegin(Conn)
--		for i = 1, #tblRet do
--			local YBTongName = tblRet[i][1]
--			local YBTongWz = tblRet[i][2]
--			local YBTongWzs = tblRet[i][3]
--			local YBTongPrice = tblRet[i][4]
--			local YBTongLeftNum = tblRet[i][5]
--			Gas2Gac:ReturnGetTongMarketOrderInfoList(Conn, YBTongName, YBTongWz, YBTongWzs, YBTongPrice, YBTongLeftNum)
--		end
--		Gas2Gac:ReturnGetTongMarketOrderInfoListEnd(Conn)
--	end
--	
--  local parameters = {}
--  parameters["uCamp"] = uCamp
--	CallAccountManualTrans(Conn.m_Account, market_box, "GetMarketOrderInfoDB", CallBack, parameters)
--end

----【申请添加帮会收购资源】
--function Gac2Gas:RequestAddTongBuyOrder(Conn)
--	local Player = Conn.m_Player
--	if not Player then
--		return
--	end
--	local uPlayerID = Player.m_uID
--	local uTongID = Player.m_Properties:GetTongID()
--	if uTongID == 0 then
--		MsgToConn(Conn, 9401)
--		return
--	end
--	
--	local parameters = {}
--	parameters["uPlayerId"] = uPlayerID
--	parameters["PurviewName"] = "ResOrderManage"
--	
--	local function CallBack(Flag)
--		if not Flag then
--			MsgToConn(Conn, 9006)
--			return
--		else
--			Gas2Gac:ReturnRequestAddTongBuyOrder(Conn)
--		end
--	end
--	
--	CallAccountManualTrans(Conn.m_Account, "GasTongBasicDB", "IsPurviewEnough", CallBack, parameters, uPlayerID)
--end

--function Gac2Gas:CancelTongBuyOrder(Conn)
--	local Player = Conn.m_Player
--	if not Player then
--		return
--	end
--	local uPlayerID = Player.m_uID
--	local uTongID = Player.m_Properties:GetTongID()
--	if uTongID == 0 then
--		MsgToConn(Conn, 9401)
--		return
--	end
--	
--	
--	local parameters = {}
--	parameters["uPlayerID"] = uPlayerID
--	parameters["uTongID"] = uTongID
--	
--	local function CallBack(Flag)
--		if Flag then
--			MsgToConn(Conn, 9024)
--		else
--			MsgToConn(Conn, 9025)
--		end
--	end
--	
--	CallAccountManualTrans(Conn.m_Account, "GasTongMarketDB", "DeleteResourceOrder", CallBack, parameters, uTongID)
--end

--function Gac2Gas:GetTongSellResInfo(Conn, uTongID, flag)
--	
--	if not IsCppBound(Conn) then
--		return 
--	end
--	if not IsCppBound(Conn.m_Player) then
--		return
--	end
--	local Player = Conn.m_Player
--	local Truck = Player:GetServantByType(ENpcType.ENpcType_Truck)
--	if not IsServerObjValid(Truck) then
--		MsgToConn(Conn, 9013)
--		return
--	end
--	local SellResNum = Truck.m_TruckInfo:GetResource()
--	local parameters = {}
--	parameters["uTongID"] = uTongID
--	parameters["Flag"] = flag
--	parameters["SellResNum"] = SellResNum
--	parameters["Level"] = Player:CppGetLevel()
--	local function CallBack(result)
--		if not result then
--			return
--		end
--		local sellNum = result[1]
--		local proffer = result[2]
--		if not result[1] then
--			MsgToConn(Conn, proffer)
--			return 
--		end
--		Gas2Gac:ReturnGetTongSellResInfo(Conn, sellNum, proffer)
--	end
--	CallAccountManualTrans(Conn.m_Account, market_box, "SellTongForageInfoDB", CallBack, parameters, uTongID)
--end

--function Gac2Gas:GetTongMyResOrderInfo(Conn)
--	if not Conn.m_Player then
--		return
--	end
--	local uTongID = Conn.m_Player.m_Properties:GetTongID()
--	if uTongID == 0 then
--		MsgToConn(Conn, 9401)
--		return
--	end
--	
--	local function CallBack(nFlag, tblRet)
--		if nFlag then
--			Gas2Gac:ReturnGetTongMyResOrderInfo(Conn, tblRet[1], tblRet[2], tblRet[3], tblRet[4], tblRet[5])
--		else
--			Gas2Gac:ReturnGetTongMyResOrderInfo(Conn, "", 0, 0, 0, 0)
--		end
--	end
--	
--  local parameters = {}
--  parameters["uTongID"] = uTongID
--	CallAccountManualTrans(Conn.m_Account, market_box, "GetOrderInfoAndStationInfo", CallBack, parameters, uTongID)
--end

--function Gac2Gas:ShowTongSellResWnd(Conn, NpcID)
--	local Player = Conn.m_Player
--	if not Player then
--		return
--	end
--	local Truck = Player:GetServantByType(ENpcType.ENpcType_Truck)
--	--没有运输车
--	if not IsServerObjValid(Truck) then
--		MsgToConn(Conn, 9013)
--		return
--	end
--	local Npc = CCharacterDictator_GetCharacterByID(NpcID)
--	if not Npc then
--		return
--	end
--	local uTongID = Npc.m_TongID
--	if g_WarZoneMgr.m_CurrentTransportState ~= "Transporting" then
--		local function CallBack(result)
--			if not result then
--				return 
--			end
--			for _, tongId in pairs(result) do
--				if uTongID == tongId then
--					Gas2Gac:ShowTongSellResWnd(Conn, uTongID, true)
--					return 
--				end
--			end
--			MsgToConn(Conn, 9042)
--			return
--		end
--		CallDbTrans("GasTongMarketDB", "GetStatTongInfo",CallBack, {}, "RetFetchInfo")
--	else
--		Gas2Gac:ShowTongSellResWnd(Conn, uTongID, true)
--	end
--end

function CGasTongMarket.RandomShowTongSellResWnd(Conn, NpcID)
	local Player = Conn.m_Player
	if not IsCppBound(Player) or not IsCppBound(Conn) then
		return
	end
	local Truck = Player:GetServantByType(ENpcType.ENpcType_Truck)
	--没有运输车
	if not IsServerObjValid(Truck) then
		MsgToConn(Conn, 9013)
		return
	end
	local SellResNum = Truck.m_TruckInfo:GetResource()
	local Npc = CCharacterDictator_GetCharacterByID(NpcID)
	local uTongID = Npc.m_TongID
	local parameters = {}
	parameters["uTongID"] = uTongID
	parameters["SellResNum"] = SellResNum
	parameters["Level"] = Player:CppGetLevel()
	parameters["state"] = g_WarZoneMgr.m_CurrentTransportState
	local function CallBack(result)
		if not result then
			return
		end
		local flag = result[1]
		local sellNum = result[2]
		local proffer = result[3]
		if not flag then
			MsgToConn(Conn, sellNum)
			return 
		end
		if not uTongID then
			uTongID = 0
		end
		Gas2Gac:ReturnGetTongSellResInfo(Conn, sellNum, proffer, uTongID)
	end
	CallAccountManualTrans(Conn.m_Account, market_box, "SellTongForageInfoDB", CallBack, parameters, uTongID)
end



--function Gac2Gas:ShowMyResOrderWnd(Conn)
--	local Player = Conn.m_Player
--	if not Player then
--		return
--	end
--	local uTongID = Player.m_Properties:GetTongID()
--	if uTongID == 0 then
--		MsgToConn(Conn, 9401)
--		return
--	end
--	
--	Gas2Gac:ShowMyResOrderWnd(Conn)
--end

function CGasTongMarket.ShowTongResTransWnd(Conn, npcId)
	local Player = Conn.m_Player
	if not Player then
		return
	end
	
	if Player.m_Properties:GetTongID() == 0 then
		MsgToConn(Conn, 9401)
		return
	end
	
--	if g_WarZoneMgr.m_CurrentTransportState == "TransportEnd" then
--		MsgToConn(Conn, 9019)
--		return
--	end
	
	if not CheckAllNpcFunc(Conn.m_Player, npcId, "帮会资源运输处") then
		return
	end
	
	Gas2Gac:ShowTongResTransWnd(Conn)
end

function CGasTongMarket.TransToTongArea(Conn, TongName)
	local Player = Conn.m_Player
	if not Player then
		return
	end
	local Camp = Player:CppGetCamp()
	local nTongID = Player.m_Properties:GetTongID()
	
	if nTongID == 0 then
		MsgToConn(Conn, 9401)
		return
	end
	
	local data = {}
	data["sTongName"] = TongName
	data["uCamp"] = Camp
	
	local function CallBack(flag, sceneType, sceneId, serverId, warZoneId, stationId, tongId)
		if IsCppBound(Conn) and IsCppBound(Conn.m_Player) then
			if not flag and IsNumber(sceneType) then
				MsgToConn(Conn, sceneType, TongName)
				return
			end
			
			if sceneId == 0 then
				return
			end
			
			if sceneType == 7 then
				MsgToConn(Conn, 9038)
				return
			end
			
			if sceneType ~= 6 then
				return
			end	
			
			pos = g_TongFbMgr:GetEnterPos()
			MultiServerChangeScene(Conn, sceneId, serverId, pos[1], pos[2])
		end
	end
	
	CallAccountManualTrans(Conn.m_Account, "WarZoneDB", "GetTongStationInfoByName", CallBack, data, TongName)
end

function CGasTongMarket.IssueFetchQueryWnd(Conn)
	if not IsCppBound(Conn) then
		return
	end
	local Player = Conn.m_Player
	if not IsCppBound(Player) then
		return
	end
	local camp = Player:CppGetCamp()

	local function CallBack(result)
		if not IsCppBound(Conn) then
				return
			end
		if not result then
			Gas2Gac:RetNotForageWnd(Conn)
			return 
		end
		Gas2Gac:RetNotForageWnd(Conn)
		for _, tbl in pairs(result) do
			
			local tongName = tbl[1]
			local warzone = tbl[2]
			local stationId = tbl[3]
			local lineId = tbl[4]
			local time = tbl[5]
			if stationId then
				Gas2Gac:RetIssueFetchQueryWnd(Conn, tongName, warzone, stationId, lineId, time)
			end
		end
	end
	CallDbTrans("GasTongMarketDB", "GetQueryInfo", CallBack, {camp}, Player.m_uID)
end


