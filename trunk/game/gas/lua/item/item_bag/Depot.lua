CDepotMgr =  class()

function CDepotMgr.GetDepotInfo( Connection)
	local Player = Connection.m_Player
	if not Player then return end
	local nCharID = Player.m_uID
	if not DistanceJudgeBetweenNpcAndPlayer(Connection,"²Ö¿â")	then
		return
	end
	local function CallBack(result)
		Gas2Gac:ReturnGetDepotInfo(Connection, result)
	end
	
	local parameters = {}
	parameters["nCharID"]	= nCharID	
	CallAccountManualTrans(Connection.m_Account, "DepotDB", "GetDepotInfo", CallBack, parameters)
end

--È¡Ç®
function CDepotMgr.GetMoneyFromDepot( Connection,nMoney)
	local Player = Connection.m_Player
	if not Player then return end
	local nCharID = Player.m_uID
	if not DistanceJudgeBetweenNpcAndPlayer(Connection,"²Ö¿â")	then
		return
	end
	if Connection.m_Player.m_ItemBagLock then
		MsgToConn(Connection,160024)
		return 
	end
	
	if nMoney <= 0 then
		MsgToConn(Connection,160008)
		return
	end
	
	local function CallBack(result,nMoneyTotal)
		if IsNumber(result) then
			MsgToConn(Connection,result)
			return
		end
		if not result then
			MsgToConn(Connection,160004)
		else
			MsgToConn(Connection,160005)
			Gas2Gac:ReturnGetDepotInfo(Connection, nMoneyTotal)
		end
	end
	
	local parameters = {}
	parameters["nCharID"]	= nCharID	
	parameters["nMoney"]	= nMoney	
	CallAccountManualTrans(Connection.m_Account, "DepotDB", "GetMoneyFromDepot", CallBack, parameters)
end

--´æÇ®
function CDepotMgr.SaveMoneyToDepot( Connection,nMoney)
	local Player = Connection.m_Player
	if not Player then return end
	local nCharID = Player.m_uID
	if not DistanceJudgeBetweenNpcAndPlayer(Connection,"²Ö¿â")	then
		return
	end
	
	if Connection.m_Player.m_ItemBagLock then
		MsgToConn(Connection,160023)
		return 
	end
	
	if nMoney <= 0 then
		MsgToConn(Connection,160008)
		return
	end
	
	local function CallBack(result,nMoneyTotal)
		if IsNumber(result) then
			MsgToConn(Connection,result)
			return
		end
		if not result then
			MsgToConn(Connection,160006)
		else
			MsgToConn(Connection,160007)
			Gas2Gac:ReturnGetDepotInfo(Connection, nMoneyTotal)
		end
	end
	
	local parameters = {}
	parameters["nCharID"]	= nCharID	
	parameters["nMoney"]	= nMoney	
	CallAccountManualTrans(Connection.m_Account, "DepotDB", "SaveMoneyToDepot", CallBack, parameters)
end