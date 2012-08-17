CGasToolsMall = class()

--【购买】
function CGasToolsMall.BuyItemInToolsMall(Conn,nItemType,sItemName,nCount,nPayType)
		local Player = Conn.m_Player
		if not (Player and IsCppBound(Player)) then return end
		local nCharID = Player.m_uID
		local function CallBack(MsgID,result)
				if not MsgID then return end
				if IsNumber(MsgID) then
					if result then
						MsgToConn(Conn,MsgID,result)
					else
						MsgToConn(Conn,MsgID)
					end
					return
				end
				MsgToConn(Conn,403)
				if result.m_AddType == "ToMailBox" then
					MsgToConn(Conn,407)
				elseif result.m_AddType == "ToBag" then
		 		end
		 		if nPayType == 1 then
		 			Gas2GacById:ReturnAddTicket(nCharID,result.nPay)
		 		end
		end
		local data = {}
		data.m_nBuyerID = nCharID
		data.m_sItemName = sItemName
		data.m_nItemType = nItemType
		data.m_nCount = nCount
		data.m_nPayType = tonumber(nPayType)
		CallAccountManualTrans(Conn.m_Account, "ToolsMallDB", "BuyItemFromToolsMall", CallBack, data)	
end

--【赠送】
function CGasToolsMall.GiveItemInToolsMall(Conn,nItemType,sItemName,nCount,nPayType,sGeterName,leftMsg)
	local Player = Conn.m_Player
	if not (Player and IsCppBound(Player)) then return end
	local nCharID = Player.m_uID
	
	local function CallBack(MsgID,result)
				if not MsgID then return end
				if IsNumber(MsgID) then
					if result then
						MsgToConn(Conn,MsgID,result)
					else
						MsgToConn(Conn,MsgID)
					end
					return
				end
				MsgToConn(Conn,406)
				if nPayType == 1 then
		 			Gas2GacById:ReturnAddTicket(nCharID,result.nPay)
		 		end
	end
	local data = {}
	data.m_nBuyerID = nCharID
	data.m_sGeterName = sGeterName
	data.m_sItemName = sItemName
	data.m_nItemType = nItemType
	data.m_nCount = nCount
	data.m_nPayType = tonumber(nPayType)
	data.m_sMsg = leftMsg
	CallAccountManualTrans(Conn.m_Account, "ToolsMallDB", "GiveItemInToolsMallDB", CallBack, data)	
end

--【查询热卖商品】
function CGasToolsMall.GetHotSaleItemInfo(Conn, type)
    local function CallBack(result)
    	Gas2Gac:RetHotSaleItemBegin(Conn)
    	for i =1,result:GetRowNum() do
   			Gas2Gac:RetHotSaleItem(Conn, type, result(i,2), result(i,1), result(i,3))
   		end
   		Gas2Gac:RetHotSaleItemEnd(Conn, type)
		end
		local data = {}
		data.m_uType = type
    CallDbTrans("ToolsMallDB", "GetHotSaleItemInfo", CallBack, data)	
end

function CGasToolsMall.ClearToolsmallBuyedInfo()
	local function CallBack()
		if not g_App:CallDbHalted() then
			CallDbTrans("ToolsMallDB", "ClearBuyedInfoByTime", nil, {})	
		end
	end
	g_AlarmClock:AddTask("ClearToolsmallBuyedInfo", {wday = {1,2,3,4,5,6,7},time = {"00:00"}}, CallBack)
end
