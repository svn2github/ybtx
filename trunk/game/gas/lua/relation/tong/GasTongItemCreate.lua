CTongItemCreate = class()
local TongItemBox = "GasTongItemCreateDB"
function CTongItemCreate.CreateTongProdItem(Conn, sItemName,nItemType,nDepot)
  local Player = Conn.m_Player
  if not Player then return end
  if Player.m_ItemBagLock then
		MsgToConn(Conn,160040)
		return 
	end
	local uPlayerID = Player.m_uID
	local uTongID = Player.m_Properties:GetTongID()
	if not DistanceJudgeBetweenNpcAndPlayer(Conn,"进行制造")	then
		return
	end
	local function CallBack(res,sMsg)
		if IsNumber(res) then
			if sMsg then
				MsgToConn(Conn,res,sMsg)
			else
				MsgToConn(Conn,res)
			end
			return
		end
		Gas2Gac:ReturnGetCreatingProdItemInfo(Conn,unpack(res))
	end
	
  local parameters = {}
	parameters.nDepot	= nDepot
	parameters.sItemName	= sItemName
	parameters.nItemType	= nItemType
	parameters.uPlayerID	= uPlayerID
	parameters.sceneName = Player.m_Scene.m_SceneName
	CallAccountManualTrans(Conn.m_Account, TongItemBox, "CreateTongItem", CallBack, parameters, uTongID)
end

function CTongItemCreate.GetCreatingProdItemInfo(Conn)
  local Player = Conn.m_Player
  if not Player then return end
	local uPlayerID = Player.m_uID
	local uTongID = Player.m_Properties:GetTongID()
	if not DistanceJudgeBetweenNpcAndPlayer(Conn,"进行制造")	then
		return
	end
	local function CallBack(res, tblRetRes)
		if tblRetRes then
			local nRes = tblRetRes.m_nRetRes
			local nMoney = tblRetRes.m_nRetMoney
			local nDropRes = tblRetRes.m_nDropRes
			if IsNumber(nRes) and nRes > 0 then
				MsgToConn(Conn,9309)
			end

			if IsNumber(nMoney) and nMoney > 0 then
				Gas2Gac:TongMoneyMsgToConn(Conn,9332,nMoney)
			end
		end
		if # res > 0 then
			Gas2Gac:ReturnGetCreatingProdItemInfo(Conn,unpack(res))
		end
	end
	
	local parameters = {}
	parameters.uPlayerID	= uPlayerID
	parameters.sceneName = Player.m_Scene.m_SceneName
	CallAccountManualTrans(Conn.m_Account, TongItemBox, "GetCreatingItemInfo", CallBack, parameters, uTongID)
end

function CTongItemCreate.GetTongRelativeLine(Conn)
	local Player = Conn.m_Player
	if not Player then return end
		
	local function CallBack(stationLine)
		Gas2Gac:ReturnGetTongRelativeLine(Conn, stationLine)
	end
	
	local parameters		= {}
	parameters.uPlayerID	= Player.m_uID
	CallAccountManualTrans(Conn.m_Account, TongItemBox, "GetTongRelativeLine", CallBack, parameters)
end

function CTongItemCreate.CancelCreateProdItem(Conn,nOrderID)
  local Player = Conn.m_Player
  if not Player then return end
	local uPlayerID = Player.m_uID
	local uTongID = Player.m_Properties:GetTongID()
	if not DistanceJudgeBetweenNpcAndPlayer(Conn,"进行制造")	then
		return
	end
	if Player.m_ItemBagLock then
		MsgToConn(Conn,160040)
		return 
	end
	local function CallBack(res,tblRetRes)
		if IsNumber(res) then
			MsgToConn(Conn,res)
			return
		end
		if tblRetRes then
			local nRes = tblRetRes.m_nRetRes
			local nMoney = tblRetRes.m_nRetMoney
			local nDropRes = tblRetRes.m_nDropRes
			if IsNumber(nMoney) and nMoney > 0 then
				Gas2Gac:TongMoneyMsgToConn(Conn,9332,nMoney)
			end
		end
		if IsTable(res) and #res > 0 then
			Gas2Gac:ReturnGetCreatingProdItemInfo(Conn,unpack(res))
		end
	end
	
  local data = {}
	data.uPlayerID	= uPlayerID
	data.nOrderID	= nOrderID
	data.sceneName = Player.m_Scene.m_SceneName
	CallAccountManualTrans(Conn.m_Account, TongItemBox, "CancelMakeItem", CallBack, data, uTongID)
end
