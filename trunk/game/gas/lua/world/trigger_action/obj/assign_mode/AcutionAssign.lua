
local RefreshTime = 30
function AuctionAssignOneObj(Conn, data, IntObj, ObjId)
	if IntObj.m_IsAcution then
		MsgToConn(Conn, 3059)
		return
	end
	local TeamID = IntObj.m_Properties:GetBelongToTeam()
	if g_AcutionAssignTeamTbls[TeamID] then
		MsgToConn(Conn, 3059)
		return
	end
	IntObj.m_IsAcution = true
	g_AcutionAssignTeamTbls[TeamID] = true
	local ItemType = data["nType"]
	local ItemNum = 1
	local ItemName = data["sName"]
	data["ObjID"] = ObjId
	IntObj.m_AcutionParam = {0,0}
	
	IntObj.m_AssignPlayers = 0
	IntObj.m_CountTime = RefreshTime
	IntObj.m_AddItemData = data
	for _, CharId in pairs(IntObj.m_AssignPlayer) do
		local player = g_GetPlayerInfo(CharId)
		AddPlayerAcutionAssignTbl(CharId, ObjId)
		IntObj.m_AssignPlayers = IntObj.m_AssignPlayers + 1
		if IsCppBound(player) then
			Gas2Gac:ShowAcutionAssignWnd(player.m_Conn, ItemType, ItemName, ItemNum, g_CurServerId, ObjId, RefreshTime)
		end
	end
	local function AssignFun(Tick, IntObj)
		if not IsServerObjValid(IntObj) then
			UnRegisterTick(Tick)
			IntObj.m_AssignTick = nil
			return
		end
		IntObj.m_CountTime = IntObj.m_CountTime - 1
		if IntObj.m_CountTime <= 0 then
			UnRegisterTick(Tick)
			IntObj.m_AssignTick = nil
			AcutionOver(IntObj)
		end
	end
	IntObj.m_AssignTick = RegisterTick("AssignTick", AssignFun, 1000, IntObj)
end

local function SendAssignMessageToMember(PlayersTbl, MsgID,...)
	local arg = {...}
	for _, CharId in pairs(PlayersTbl) do
		MsgToConnById(CharId, MsgID, ...)
	end
end

local function SynchronizationPrice(PlayersTbl, Price, ObjId)
	for _, CharId in pairs(PlayersTbl) do
		Gas2GacById:NotifyChangeAuctionPrice(CharId, Price, ObjId)
	end
end

function AcutionOver(IntObj)
	if not IsServerObjValid(IntObj) then
		UnRegisterTick(IntObj.m_AssignTick)
		IntObj.m_AssignTick = nil
	end
	local data = IntObj.m_AddItemData
	local AcutionParam = IntObj.m_AcutionParam      --PlayerId, OfferMoney, PlayerName
	if not AcutionParam then
		return
	end
	IntObj.m_AcutionParam = nil
	if AcutionParam[2] > 0 then
		data["char_id"] = AcutionParam[1]
		data["GainMoney"] = AcutionParam[2]
		data["char_name"] = AcutionParam[3]
		SendAssignMessageToMember(IntObj.m_AssignPlayer, 3994, AcutionParam[3], AcutionParam[2], data["sName"])
		AcutionAssignGiveItem(IntObj, data)
	else
		SendAssignMessageToMember(IntObj.m_AssignPlayer, 3040, data["sName"])
		if IntObj.m_Properties:GetDropOwnerId() ~= 0 then
			g_IntObjServerMgr:Destroy(IntObj,false)
		else
			g_IntObjServerMgr:Destroy(IntObj,true)
		end
	end
	RemoveObjAllAssignTbl(IntObj)
end

function Gac2Gas:SelectAcutionAssign(Conn, ServerId, ObjID, Price, IsAcution)

	if not IsCppBound(Conn) or not IsCppBound(Conn.m_Player) then
		return
	end
	local Player = Conn.m_Player
	
	if ServerId == g_CurServerId then
		SelectAcutionAssign(Conn, ObjID, Price, IsAcution, Player.m_uID, Player.m_Properties:GetCharName())
	else
		Gas2GasCall:SelectAcutionAssign(GetServerAutoConn(ServerId), ObjID, Price, IsAcution, Player.m_uID, Player.m_Properties:GetCharName())
	end
end

function Gas2GasDef:SelectAcutionAssign(Conn, ObjID, Price, IsAcution, PlayerId, CharName)
	SelectAcutionAssign(nil, ObjID, Price, IsAcution, PlayerId, CharName)
end

function SelectAcutionAssign(Conn, ObjID, Price, IsAcution, PlayerId, PlayerName)

	local IntObj = CIntObjServer_GetIntObjServerByID(ObjID)
	if not IsServerObjValid(IntObj) then
		return
	end
	if not IntObj.m_AssignTick then
		return
	end
	local IntObjName = IntObj.m_Properties:GetCharName()
	
	if not IntObj.m_AssignPlayer then
		LogErr("拍卖分配物品时出现异常!", "玩家("..PlayerName..") ID("..PlayerId..") OBJ("..IntObjName..")没有分配列表", Conn)
		return
	end
	
	if IsAcution == true then
		local money = IntObj.m_AcutionParam[2]
		if Price <= money then 
			MsgToConnById(PlayerId, 3998, money)
			return
		end
		if IntObj.m_AddItemData["AuctionBasePrice"] > Price then
			MsgToConnById(PlayerId, 3998, IntObj.m_AddItemData["AuctionBasePrice"])
			return
		end
	else
		if IntObj.m_AcutionParam[1] == PlayerId then
			MsgToConnById(PlayerId, 3999)
			return
		else
			if not RemovePlayerAcutionAssignTbl(PlayerId, ObjID) then
				return
			end
			IntObj.m_AssignPlayers = IntObj.m_AssignPlayers - 1
			SendAssignMessageToMember(IntObj.m_AssignPlayer, 3041, PlayerName, IntObjName)
			if IntObj.m_AssignPlayers == 0 then
				AcutionOver(IntObj)
			elseif IntObj.m_AssignPlayers == 1 and IntObj.m_AcutionParam[2] > 0 then
				AcutionOver(IntObj)
			end
			return
		end
	end
	
	local function CallBack(result, result2, result3, EventId)
		if not IsServerObjValid(IntObj) then
			return
		end
		if not IntObj.m_AssignTick then
			return
		end
		if result then
			SendAssignMessageToMember(IntObj.m_AssignPlayer, 3996, PlayerName, IntObjName, Price)
			IntObj.m_CountTime = RefreshTime
			IntObj.m_AcutionParam = {PlayerId, Price, PlayerName}
			if EventId then
				IntObj.m_AcutionEventId = EventId
			end
			MsgToConnById(PlayerId, 3991, result2)
			SynchronizationPrice(IntObj.m_AssignPlayer, Price, ObjID)
			if result3 then
				if result3[1] then
				else
					MsgToConnById(result3[2],result3[3])
				end
			end
		else
			if result2 then
				if result2[2] then
					MsgToConnById(PlayerId, result2[1], result2[2])
				else
					MsgToConnById(PlayerId, result2[1])
				end
			end
		end
	end
	local data = {}
	data["player_id"] = PlayerId
	data["Price"] = Price
	data["ObjID"] = ObjID
	if IntObj.m_AcutionEventId then
		data["EventId"] = IntObj.m_AcutionEventId
	end
	local ChannelID = nil
	if IntObj.m_AcutionParam[2] > 0 then
		data["PreCharID"] = IntObj.m_AcutionParam[1]
		if PlayerId ~= IntObj.m_AcutionParam[1] then
			ChannelID = IntObj.m_AcutionParam[1]
		end
	end
	if Conn then
		CallAccountManualTrans(Conn.m_Account, "AuctionAssignDB", "ValidateAuctionMoney", CallBack, data, ChannelID)
	else
		CallDbTrans("AuctionAssignDB", "ValidateAuctionMoney", CallBack, data, PlayerId, ChannelID)
	end
end

function AddPlayerAcutionAssignTbl(PlayerID, ObjID)
	if not g_AcutionAssignTbls[PlayerID] then
		g_AcutionAssignTbls[PlayerID] = {}
	end
	g_AcutionAssignTbls[PlayerID][ObjID] = true
end

function RemovePlayerAcutionAssignTbl(PlayerID, ObjID)
	if g_AcutionAssignTbls[PlayerID] and g_AcutionAssignTbls[PlayerID][ObjID] then
		g_AcutionAssignTbls[PlayerID][ObjID] = nil
		if table.maxn(g_AcutionAssignTbls[PlayerID]) == 0 then
			g_AcutionAssignTbls[PlayerID] = nil
		end
		Gas2GacById:FinishAuction(PlayerID, ObjID)
		return true
	end
end

function ReturnAllAuctionMoney(auctionRes)
	for i, v in pairs(auctionRes) do
		if v[1] then
			MsgToConnById(v[1], 3992, v[2])
		end
	end
end
