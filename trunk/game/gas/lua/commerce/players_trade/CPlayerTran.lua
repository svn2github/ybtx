gas_require "commerce/players_trade/CPlayerTranInc"

local PlayersTranDB = "PlayersTranDB"

function CPlayerTran:Ctor(PlayerId)
	self.m_Id=PlayerId
end

function  CPlayerTran:ClearTradeState(Connection)
	local function Init()
		local traderConn = Connection.TradingPlayerConn	
		Gas2Gac:ReturnCanceledTrade(traderConn)
		Gas2Gac:ReturnCanceledTrade(Connection)
		self:ClearTranTbl(Connection)
		self:ClearTranTbl(traderConn)
		MsgToConn(Connection, 6027)
		MsgToConn(traderConn, 6027)
		Connection.m_Player:DelListeningWnd()
		traderConn.m_Player:DelListeningWnd()
	end
	apcall(Init)
end

--邀请交易
function CPlayerTran:InviteTransaction(inviteeid, playerStateRet)
	local invitee=g_GetPlayerInfo(inviteeid)
	local Conn=g_GetPlayerInfo(self.m_Id).m_Conn
	if( invitee == nil ) then
		MsgToConn(Conn, 6017)
		Conn.busystate = nil
		return false
	end
	
	if(tradeState==0) then
		MsgToConn(Conn, 6016)
		Conn.busystate = nil
		return false
	end
	local InvitorName = g_GetPlayerInfo(self.m_Id).m_Properties:GetCharName()
	
	--记录发送交易请求的玩家信息：玩家id，玩家姓名
	if invitee.m_Conn.invitor_info_tbl == nil then	
		invitee.m_Conn.invitor_info_tbl = {}
	end
	local haveSendInvitation = false
	local len = # invitee.m_Conn.invitor_info_tbl 
	for i=1, len do
		if invitee.m_Conn.invitor_info_tbl[i][1] == self.m_Id then
			haveSendInvitation = true
			break
		end
	end
	if haveSendInvitation == false then
		table.insert(invitee.m_Conn.invitor_info_tbl, {self.m_Id, InvitorName})
	end
	Gas2Gac:ConcealOrNotPlayersTradeRequest(invitee.m_Conn, true)
	return
end

local function RemovePlayersTradeRequestInfo(Conn, invitorid)
    if Conn.invitor_info_tbl == nil then
        return  
    end
	local tblLen = # Conn.invitor_info_tbl
	if tblLen > 1 then
		for i=1, tblLen do
			if Conn.invitor_info_tbl[i][1] == invitorid then
				table.remove(Conn.invitor_info_tbl, i)
				break
			end
		end
	else
	    Gas2Gac:ConcealOrNotPlayersTradeRequest(Conn, false)
		Conn.invitor_info_tbl= {}
	end
end
--发出邀请交易的玩家下线时，通知对方客户端关掉交易窗口
function CPlayerTran:AgreeToTrade(Conn, invitorid)
	local invitor	=	g_GetPlayerInfo(invitorid)
	local player = CheckPlayerIsOnline(invitorid) --多服上检查玩家在线否

	if invitor == nil then
	    if player == nil then
	        MsgToConn(Conn,6017)
	    else
	        MsgToConn(Conn,6016)
	    end
		self:ClearTranTbl( Conn )  --将自己关于交易的一系列状态置为未交易状态
		return
	end		
	if Conn.m_Player.m_UseItemLoadingTick then      --如果正在读条，则打断读条
		StopItemProgressTick(Conn.m_Player)
	end				
    if invitor.m_Conn.m_Player.m_UseItemLoadingTick then      --如果正在读条，则打断读条
		StopItemProgressTick(invitor.m_Conn.m_Player)
	end		
	Gas2Gac:ReturnAgreeToTrade(invitor.m_Conn, Conn.m_Player.m_uID)
	Gas2Gac:ReturnAgreeToTrade(Conn, invitorid)
	return
end

function CPlayerTran:ClearTranTbl(Conn)
	Conn.busystate			=	nil
	Conn.Submitstate		=	nil
	Conn.tbl_trade_items	=	nil
	Conn.Lockstate			=	nil
	Conn.TradingPlayerConn	=	nil
end

--------------------------------------rpc----------------------------------------------------------------
--邀请交易
local function SendTradeInvitation(data)
	local Conn, inviteeid, tradeState = unpack(data)
    if not IsCppBound(Conn.m_Player) then
        return 
    end
	local invitor= CPlayerTran:new(Conn.m_Player.m_uID)
	local sender = Conn.m_Player
	local invitee=g_GetPlayerInfo(inviteeid)
	local player = CheckPlayerIsOnline(inviteeid) --多服上检查玩家在线否
	if invitee == nil  then
		if player == nil then
			MsgToConn(Conn, 6017)   --对方不在线
		else    
			MsgToConn(Conn, 6016)   --距离过远
		end
		return 
	end
    if(g_GetDistance(sender,invitee)>20) then
		MsgToConn(Conn, 6016)
		return
	end
		
	if (Conn.busystate ~= nil
		and Conn.busystate == 1 )then
		MsgToConn(Conn, 6018)
		return
	end
	
	if(invitee.m_Conn.busystate ~= nil
		and invitee.m_Conn.busystate==1) then
		MsgToConn(Conn, 6019)
		return 
	end
	if(g_GetDistance(sender,invitee)>20) then
		MsgToConn(Conn, 6016)
		return
	end
	
	invitor:InviteTransaction(inviteeid, tradeState)
	return
end

local function SendTrade(Conn, invitee, InviteeId)
	if Conn.m_Player.m_Scene.m_SceneName ~= invitee.m_Scene.m_SceneName then
	   MsgToConn(Conn, 6014)
	   return  
	end
	if Conn.m_Player.m_PickOre then
		MsgToConn(Conn, 6034)
		return 
	end
	if invitee.m_PickOre then
		MsgToConn(Conn, 6033)
		return 
	end
	local data = {["InviteeID"] =InviteeId, ["InvitorID"]=Conn.m_Player.m_uID }
	local function CallBack(suc, result, errorMsg)
		if suc then
			local sendData = {Conn, InviteeId, result}
			SendTradeInvitation(sendData)
		else
			local errorMsgId = result
			if errorMsgId == 160016 then
				MsgToConn(Conn, errorMsgId,invitee.m_Properties:GetCharName())
			end
		end
	end
	--local entry = (g_DBTransDef["PlayersTranDB"])
	CallAccountManualTrans(Conn.m_Account, PlayersTranDB, "SendTradeInvitation", CallBack, data)	
end

--发送交易请求
function Gac2Gas:SendTradeInvitation( Conn, InviteeId )
	if not IsCppBound(Conn.m_Player) then
		return 
	end
	
	if Conn.m_Player.m_EnterObserverMode then
	    MsgToConn(Conn,6036)
        return  
	end
  if Conn.m_Player.m_ItemBagLock then
		MsgToConn(Conn,160015)
		return
	end
  local invitee=g_GetPlayerInfo(InviteeId)
  local player = CheckPlayerIsOnline(InviteeId) --多服上检查玩家在线否
  if invitee and invitee.m_ItemBagLock then
		MsgToConn(Conn,160016,invitee.m_Properties:GetCharName())
		return
	end
	if invitee and invitee.m_EnterObserverMode then
	    MsgToConn(Conn,6037)
        return  
	end
    if invitee == nil then
        if player == nil then
        	MsgToConn(Conn, 6017)   --对方不在线
        else
        	MsgToConn(Conn, 6016)   --距离过远
      	end
        return
    end
	SendTrade(Conn, invitee, InviteeId)
end


function Gac2Gas:SenTradeInvitationByPlayerName(Conn, inviteeName)
	if Conn.m_Player.m_ItemBagLock then
		MsgToConn(Conn,160015)
		return
	end
    local invitee = g_GetPlayerInfoByName(inviteeName)
    if invitee and invitee.m_ItemBagLock then
			MsgToConn(Conn,160016,invitee.m_Properties:GetCharName())
			return
		end
    local player = CheckPlayerIsOnlineByName(inviteeName)
    if invitee == nil then
        if player == nil then
        	MsgToConn(Conn, 6017)   --对方不在线
        else
        	MsgToConn(Conn, 6016)   --距离过远
      	end
        return
    end
    local InviteeId = invitee.m_uID
    SendTrade(Conn, invitee, InviteeId)
end

--响应邀请
--拒绝交易
local function DenyedTrade(data)
	local Conn,invitorid=unpack(data)
	local invitor=g_GetPlayerInfo(invitorid)
	if not IsCppBound(Conn.m_Player) then
        return 
	end

	local sender=CPlayerTran:new(Conn.m_Player.m_uID)
	if Conn.invitor_info_tbl == nil then
	    sender:ClearTranTbl(Conn)
	    return
	end
	
	if # Conn.invitor_info_tbl == 1 then
		if invitor ~= nil  then
		    local charName = Conn.m_Player.m_Properties:GetCharName()
			MsgToConn(invitor.m_Conn, 6020, charName)
		end
		Conn.invitor_info_tbl = {} 
		Gas2Gac:ConcealOrNotPlayersTradeRequest(Conn, false)
	else
		for i=1,  # Conn.invitor_info_tbl do
			if Conn.invitor_info_tbl[i][1] == invitorid then
			    local charName = Conn.m_Player.m_Properties:GetCharName()
			    if invitor ~= nil then
				    MsgToConn(invitor.m_Conn, 6020, charName)
				end
				table.remove(Conn.invitor_info_tbl, i)
				break
			end
		end
	 	local invitorInfoTblLen = # Conn.invitor_info_tbl
		if invitorInfoTblLen == 1 then
			Gas2Gac:GotTradeInvitation( Conn, Conn.invitor_info_tbl[1][1], Conn.invitor_info_tbl[1][2] )
		else
			Gas2Gac:GetPlayersTradeRequestBegin(Conn)
			for i=1, invitorInfoTblLen do
				Gas2Gac:GetPlayersTradeRequest(Conn, Conn.invitor_info_tbl[i][1], Conn.invitor_info_tbl[i][2])
			end
			Gas2Gac:GetPlayersTradeRequestEnd(Conn)
		end
	end
	return
end
function Gac2Gas:DenyedTrade( Conn ,InvitorId)
	local data = { Conn ,InvitorId}
	DenyedTrade(data)
end
--接受邀请
local function AgreeToTrade(data)
	local Conn,invitorid=unpack(data)
    if not IsCppBound(Conn.m_Player) then
        return 
    end
	local invitor=g_GetPlayerInfo(invitorid)
	local player = CheckPlayerIsOnline(invitorid) --多服上检查玩家在线否
	RemovePlayersTradeRequestInfo(Conn, invitorid)
	if( invitor == nil) then
	    if player == nil then
		    MsgToConn(Conn, 6017)
		else
		    MsgToConn(Conn, 6016)
		end
        Gas2Gac:ReturnDenyedTrade( Conn )
	    return
	end
	if (Conn.m_Player.m_Scene.m_SceneName ~= invitor.m_Scene.m_SceneName) then
	    MsgToConn(Conn, 6016)
	    Gas2Gac:ReturnDenyedTrade( Conn )
	    return
	end
	
	local invitee=Conn.m_Player
	if(g_GetDistance(invitee,invitor)>20) then
		MsgToConn(Conn, 6016)
		MsgToConn(invitor.m_Conn, 6016)
		return
	end
	
	if Conn.busystate ~= nil and Conn.busystate == 1 then
		MsgToConn(Conn, 6021)
		return
	end
	
	if invitor.m_Conn.busystate ~= nil and invitor.m_Conn.busystate == 1 then
		MsgToConn(Conn, 6022)
		return
	end
	
	Conn.busystate					=	1   --交易忙碌状态置为1（正在交易），nil为没在交易
	Conn.Submitstate 				= 	0		--未提交交易，提交后的状态为1
	Conn.tbl_trade_items			=	{}	--保存交易的物品列表
	Conn.Lockstate					=	0		--未锁定交易，锁定时的状态为1
	Conn.TradingPlayerConn			=	invitor.m_Conn
	
	invitor.m_Conn.busystate		=	1
	invitor.m_Conn.tbl_trade_items	=	{}
	invitor.m_Conn.Submistate		=	0
	invitor.m_Conn.Lockstate		=	0
	invitor.m_Conn.TradingPlayerConn=	Conn
	invitor:AddListeningWnd()				--监听是否移动
	invitee:AddListeningWnd()				--监听是否移动
	local inviteeObj	=	CPlayerTran:new(Conn.m_Player.m_uID)
	
	inviteeObj:AgreeToTrade(Conn, invitorid)
	return
end
function Gac2Gas:AgreeToTrade(Conn,InvitorId)
	local data = {Conn,InvitorId}
	AgreeToTrade(data)
end

--选择交易物品，同时在客户端显示
local function ChoosedTradeItem(data)
	local Conn, recieverid, roomIndex, pos, itemCount, itemID=unpack(data)
	local sender=CPlayerTran:new(Conn.m_Player.m_uID)
	local reciever=g_GetPlayerInfo(recieverid)
	if reciever == nil then
		MsgToConn(Conn, 6017)
		sender:ClearTranTbl(Conn)
		Gas2Gac:ReturnCanceledTrade( Conn )  --关闭客户端交易面板
		return false
	end 
	if( Conn.tbl_trade_items == nil or Conn.Lockstate == 1 ) then --玩家的交易状态已经被取消了，是否已经锁定交易
		return false
	end
	local tbl_trade_item_len = table.getn(Conn.tbl_trade_items)
	for i=1, tbl_trade_item_len do
		if(Conn.tbl_trade_items[i][1]==roomIndex and Conn.tbl_trade_items[i][2]==pos) then
			return false
		end
	end	
	
	if( tbl_trade_item_len >= 16 ) then
		return false
	end

	return true
end

local function RetChoosedTradeItem(Conn, recieverid, result)
	local itemType = result["ItemType"]
	local itemName = result["ItemName"]
	local itemInfoTbl = result["ItemInfoTbl"]
	local roomIndex = result["RoomIndex"]
	local pos = result["Pos"]
	local itemCount = result["ItemCount"]
	local itemID = result["ItemID"]
	if Conn.tbl_trade_items == nil then
	   return 
	end
	local slot=table.getn(Conn.tbl_trade_items)+1
	local tbl_items={roomIndex, pos, itemCount, itemType, itemName, slot, itemID, itemInfoTbl}
	table.insert(Conn.tbl_trade_items, tbl_items)

	local sender=CPlayerTran:new(Conn.m_Player.m_uID)
	local reciever=g_GetPlayerInfo(recieverid)
	if( reciever == nil ) then
		MsgToConn(Conn, 6017)
		sender:ClearTranTbl(Conn)
		
		Gas2Gac:ReturnCanceledTrade( Conn )  --关闭客户端交易面板
		return
	end
	Gas2GacById:ReturnChoosedTradeItem( Conn.m_Player.m_uID, slot, itemType, itemName, itemCount, itemID )
end

--添加交易物品
function Gac2Gas:ChoosedTradeItem(Conn,recieverid, roomIndex, pos, itemCount, itemID)
	
	local data = {["CharID"]= Conn.m_Player.m_uID,["recieverid"]= recieverid, ["RoomIndex"] =roomIndex, 
					["Pos"] = pos, ["ItemCount"] =itemCount, ["ItemID"] = itemID}
	
	local choosedTradeItemData = {Conn, recieverid, roomIndex, pos, itemCount, itemID}
	local ret = ChoosedTradeItem(choosedTradeItemData)
	if ret == false then
		return
	end
	
	local function CallBack(suc, result, roomIdx, npos)
		if suc then
			RetChoosedTradeItem(Conn, recieverid, result)
		else
			local errorMsgId = result
			MsgToConn(Conn, errorMsgId)
		    Gas2Gac:ReturnChoosedTradeItemError(Conn, roomIdx, npos )
		end
	end
	
	--local entry = (g_DBTransDef["PlayersTranDB"])
	local ret = CallAccountManualTrans(Conn.m_Account, PlayersTranDB, "ChoosedTradeItem", CallBack, data)
    if ret == 1 then --锁被占用着，此rpc消息无效
        Gas2Gac:ReturnChoosedTradeItemError(Conn, roomIndex, pos )
    end
end


local function CancelChoosedItem(data)
	local Conn, recieverid,slot=unpack(data)
	local sender=CPlayerTran:new(Conn.m_Player.m_uID)
	local reciever=g_GetPlayerInfo(recieverid)
	if( reciever == nil ) then
		MsgToConn(Conn, 6017)
		sender:ClearTranTbl(Conn)
		Gas2Gac:ReturnCanceledTrade( Conn )  --关闭客户端交易面板
		return
	end
	if( Conn.tbl_trade_items == nil or Conn.Lockstate == 1 ) then
		return
	end
	if( table.getn(Conn.tbl_trade_items) < slot ) then
		return 
	end

	Gas2Gac:RetPlayerItemSound(Conn,Conn.tbl_trade_items[slot][4],Conn.tbl_trade_items[slot][5])
	
	table.remove(Conn.tbl_trade_items,slot)
	
	Gas2Gac:ReturnCancelChoosedItemSuc(Conn)
	for i=1,table.getn(Conn.tbl_trade_items) do
		local count, stype, name, slot, itemId=Conn.tbl_trade_items[i][3],Conn.tbl_trade_items[i][4],Conn.tbl_trade_items[i][5],i, Conn.tbl_trade_items[i][7]
		local itemtype=tonumber(stype)
		Conn.tbl_trade_items[i][6] = i
		Gas2Gac:ReturnResetItemList(Conn, slot, itemtype, name, count, itemId)
	end
	Gas2Gac:ReturnResetItemListEnd(Conn, slot)
	return
end
function Gac2Gas:CancelChoosedItem(Conn, recieverid,slot)
	local data = {Conn, recieverid,slot}
	CancelChoosedItem(data)
end

--替换某个已经添加的要交易的物品
local function ReplaceItem(data)
	local Conn, recieverid, slot, roomIndex, pos=unpack(data)
	local sender = CPlayerTran:new(Conn.m_Player.m_uID)
	local reciever = g_GetPlayerInfo(recieverid)
	if( reciever == nil ) then
		MsgToConn(Conn, 6017)
		sender:ClearTranTbl(Conn)
		Gas2Gac:ReturnCanceledTrade( Conn )  --关闭客户端交易面板
		return false
	end
	
	if( Conn.tbl_trade_items == nil or Conn.Lockstate == 1 ) then
		return false
	end
	local tbl_trade_item_len = table.getn(Conn.tbl_trade_items)
	if(tbl_trade_item_len<slot) then
		MsgToConn(Conn, 6023)
		return false
	end

	for i=1,tbl_trade_item_len do
		if(Conn.tbl_trade_items[i][1]==roomIndex and Conn.tbl_trade_items[i][2]==pos) then
			return false
		end
	end
end

local function RetReplaceItem(Conn, result)
	if Conn.tbl_trade_items == nil then
	   return 
	end
	local nType = result["ItemType"]
	local name	= result["ItemName"]
	local itemInfoTbl = result["ItemInfoTbl"]
	local roomIndex = result["RoomIndex"]
	local pos = result["Pos"]
	local itemCount = result["ItemCount"]
	local itemID = result["ItemID"]
	local slot = result["Slot"]
	local replacedRoomIndex = result["ReplacedRoomIndex"]
	local replacedPos = result["ReplacedPos"]
	Conn.tbl_trade_items[slot][1] = roomIndex
	Conn.tbl_trade_items[slot][2] = pos
	Conn.tbl_trade_items[slot][3] = itemCount
	Conn.tbl_trade_items[slot][4] = nType
	Conn.tbl_trade_items[slot][5] = name
	Conn.tbl_trade_items[slot][6] = slot
	Conn.tbl_trade_items[slot][7] = itemID
	Conn.tbl_trade_items[slot][8] = itemInfoTbl
		
	Gas2GacById:ReturnReplaceItem( Conn.m_Player.m_uID, slot, nType, name, itemCount, itemID, roomIndex, pos, replacedRoomIndex, replacedPos)
end

--替换某个已经添加的要交易的物品
function Gac2Gas:ReplaceItem(Conn, recieverid, slot, roomIndex, pos, itemCount, itemID)
 	local data = {["CharID"]=Conn.m_Player.m_uID, ["recieverid"]=recieverid,["RoomIndex"]=roomIndex, 
 					["Pos"]= pos, ["ItemCount"] =itemCount, ["ItemID"] = itemID,["Slot"] = slot}
 	local checkNeedData = {Conn, recieverid, slot, roomIndex, pos}
 	local ret = ReplaceItem(checkNeedData)
 	if ret == false then
 		return
 	end
 	data["ReplacedRoomIndex"] = Conn.tbl_trade_items[slot][1]
 	data["ReplacedPos"] = Conn.tbl_trade_items[slot][2]
 	
 	local function CallBack(suc, result)
 		if suc then
 			RetReplaceItem(Conn, result)
 		else
 			local errorMsgId = result["MsgID"]
			MsgToConn(Conn, errorMsgId)
 			Gas2Gac:ReturnReplaceItemError(Conn, result["RoomIndex"], result["Pos"])	
 		end
	end
 	
 	--local entry = (g_DBTransDef["PlayersTranDB"])
	local ret = CallAccountManualTrans(Conn.m_Account, PlayersTranDB, "ReplaceItem", CallBack, data)
	if ret == 1 then --锁被占用着，此rpc消息无效
        Gas2Gac:ReturnReplaceItemError(Conn, roomIndex, pos)
    end
end


--选择交易的金钱数目，同时在对方客户端显示
local function ChoosedTradeMoney(data)
	local Conn, recieverid, moneyCount, haveMoneyCount = unpack(data)
	assert( tonumber(moneyCount) >= 0 )
	local reciever=g_GetPlayerInfo(recieverid)
	if not IsCppBound(Conn.m_Player) then
	    return
	end
	local sender=CPlayerTran:new(Conn.m_Player.m_uID)
	if( reciever == nil ) then
		MsgToConn(Conn, 6017)
		sender:ClearTranTbl(Conn)
		
		
		Gas2Gac:ReturnCanceledTrade( Conn )  --关闭客户端交易面板
		return
	end
	if(moneyCount > haveMoneyCount) then
		MsgToConn(Conn, 6009)
		return
	end
	if Conn.tbl_trade_items == nil then
	   return 
	end
	Conn.tbl_trade_items.money=tonumber(moneyCount)
	Gas2Gac:ReturnChoosedTradeMoney( Conn, Conn.tbl_trade_items.money )
	return
end

--交易金钱时
function Gac2Gas:ChoosedTradeMoney( Conn, recieverid, moneyCount )
	assert(tonumber(moneyCount)>= 0)
	local data = { ["CharID"]= Conn.m_Player.m_uID}
	local function CallBack(result)
		local choosedMoneyData  = {Conn, recieverid, moneyCount, result}
		ChoosedTradeMoney(choosedMoneyData)
	end
	
	--local entry = (g_DBTransDef["PlayersTranDB"])
	CallAccountManualTrans(Conn.m_Account, PlayersTranDB, "ChoosedTradeMoney", CallBack, data)
end

--锁定交易
local function LockTrade(data)
	local Conn,recieverid=unpack(data)
	local reciever=g_GetPlayerInfo(recieverid)
	local sender=CPlayerTran:new(Conn.m_Player.m_uID)
	if( reciever == nil ) then
		MsgToConn(Conn, 6017)
		sender:ClearTranTbl(Conn)
		Gas2Gac:ReturnCanceledTrade( Conn )  --关闭客户端交易面板
		return
	end
	local myself=g_GetPlayerInfo(Conn.m_Player.m_uID)
	if(g_GetDistance(myself,reciever)>20) then
		MsgToConn(Conn, 6016)
		MsgToConn(reciever.m_Conn, 6016)
		sender:ClearTranTbl(Conn)
		sender:ClearTranTbl(reciever.m_Conn)
		Gas2Gac:RetSubmitedTrade(Conn)
		Gas2Gac:RetSubmitedTrade(reciever.m_Conn)
		return
	end
	Conn.Lockstate=1
	if Conn.tbl_trade_items == nil then
	   return 
	end
	Gas2Gac:ReturnChoosedInviteeTradeMoney( reciever.m_Conn, Conn.tbl_trade_items.money )
	for i=1,table.getn(Conn.tbl_trade_items) do
		local slot=Conn.tbl_trade_items[i][6]
		local itemtype=Conn.tbl_trade_items[i][4]
		local name=Conn.tbl_trade_items[i][5]
		local itemCount=Conn.tbl_trade_items[i][3]
		local itemID = Conn.tbl_trade_items[i][7]
		Gas2GacById:ReturnInviteeChoosedItem(reciever.m_uID,slot,itemtype, name, itemCount, itemID)
	end
	Gas2GacById:ReturnLockedTrade(reciever.m_uID )
	return
end
function Gac2Gas:HaveLockedTrade( Conn,recieverid )
	local data = {Conn,recieverid}
	LockTrade(data)
end
--取消交易
local function CanceledTrade(data)
	local Conn,recieverid=unpack(data)
	local reciever=g_GetPlayerInfo(recieverid)
	if not IsCppBound(Conn.m_Player) then
	    return
	end
	local invitor = g_GetPlayerInfo(Conn.m_Player.m_uID)
	local sender=CPlayerTran:new(Conn.m_Player.m_uID)
	if( reciever == nil ) then
		MsgToConn(Conn, 6017)
		sender:ClearTranTbl(Conn)
		return
	end
	if Conn.busystate ~= nil then
	    sender:ClearTranTbl(reciever.m_Conn)
	    MsgToConn(reciever.m_Conn, 6024)
	end
	sender:ClearTranTbl(Conn)
	reciever:DelListeningWnd()		--取消移动监听
	invitor:DelListeningWnd()			--取消移动监听
	
	Gas2Gac:ReturnCanceledTrade(reciever.m_Conn)
	return
end
function Gac2Gas:CanceledTrade( Conn,recieverid )
	local data = {Conn,recieverid }
	CanceledTrade(data)
end
--提交交易
local function SubmitedTrade(data)
	local Conn, recieverid=unpack(data)
	local reciever=g_GetPlayerInfo(recieverid)
	if not IsCppBound(Conn.m_Player) then
	   return  
	end
	local sender=CPlayerTran:new(Conn.m_Player.m_uID)
	if( reciever == nil ) then
		MsgToConn(Conn, 6017)
		sender:ClearTranTbl(Conn)
		Gas2Gac:ReturnCanceledTrade( Conn )  --关闭客户端交易面板
		return false
	end
	if(Conn.Lockstate==0) then
		MsgToConn(Conn,6026)
		return false
	end
	if (Conn.Lockstate==nil or reciever.m_Conn.Lockstate == nil) then
	    MsgToConn(Conn,6025)
		return false
	end
	local myself=g_GetPlayerInfo(Conn.m_Player.m_uID)
	if(g_GetDistance(myself,reciever)>20) then
		MsgToConn(Conn, 6016)
		MsgToConn(reciever.m_Conn, 6016)
		sender:ClearTranTbl(Conn)
		sender:ClearTranTbl(reciever.m_Conn)
		Gas2Gac:RetSubmitedTrade(Conn)
		Gas2Gac:RetSubmitedTrade(reciever.m_Conn)
		return false
	end
	Conn.Submitstate=1
	if(reciever.m_Conn.Submitstate~=1) then
		if(reciever.m_Conn.Lockstate==0) then
			MsgToConn(Conn, 6025)
			return false
		else
			MsgToConn(Conn, 6025)
			return false
		end	
	end
	return true
end

local function RetExcuteTrade(Conn, invitee_conn, result)
	local sender_itemids		=	result["Sender_ItemIDs"]
	local reciever_itemids		=	result["Reciever_ItemIDs"]
	local sender_infos			=	result["Sender_Infos"]
	local reciever_infos		=	result["Reciever_Infos"]
	local senderTradeMoney      =   result["Sender_Money"] 
	local  invitor = CPlayerTran:new(Conn.m_Player.m_uID)
	invitor:ClearTranTbl(Conn)
	invitor:ClearTranTbl(invitee_conn)
	Gas2Gac:RetSubmitedTrade(Conn)
	Gas2Gac:RetSubmitedTrade(invitee_conn)
	MsgToConn(Conn,6030)
	MsgToConn(invitee_conn,6030)
end

function Gac2Gas:GetTradeCess(Conn,recieverid)
 	local invitee = g_GetPlayerInfo(recieverid)
 	if not (invitee and IsCppBound(invitee))  then return end
 	if not invitee.m_Conn.tbl_trade_items then
 		Gac2Gas:SubmitedTrade( Conn, recieverid )
 		return
 	end
 	local money = invitee.m_Conn.tbl_trade_items.money
 	if money and money > 0 then
 		local function CallBack(cess)
 			if money*cess <= 0 then
 				Gac2Gas:SubmitedTrade( Conn, recieverid)
 				return
 			end
 			Gas2Gac:RetGetTradeCess(Conn,recieverid,money*cess,cess*100)
 		end
 		CallDbTrans("MoneyMgrDB", "GetMoneyExchangeCess", CallBack, {["char_id"] = recieverid,["exchange_money"] = money})
	else
		Gac2Gas:SubmitedTrade( Conn, recieverid )
	end
end

--提交交易
function Gac2Gas:SubmitedTrade( Conn, recieverid )
	local checkData = {Conn,recieverid}
	local ret  = SubmitedTrade(checkData)
	if ret == false then
		return
	end
    if not IsCppBound(Conn.m_Player)  then
        return 
	end
	local invitee = g_GetPlayerInfo(recieverid)
	local data = {["InvitorID"] =Conn.m_Player.m_uID , ["InviteeID"]= recieverid, 
					["InvitorTradeInfoTbl"] =Conn.tbl_trade_items, ["InviteeTradeInfoTbl"]=invitee.m_Conn.tbl_trade_items}

	local invitee_conn = invitee.m_Conn	

	if not IsCppBound(invitee) then
        return 
	end
	local succ1 = Conn.m_Player:BlockDBTrans() --先锁物品
	local succ2 = invitee:BlockDBTrans() --先锁物品
	if (not succ1) then
		MsgToConn(Conn, 802)
		return
	end
	if (not succ2) then
		MsgToConn(invitee_conn, 802)
		return
	end
	
	local function CallBack(suc, result, errorMsg)
		Conn.m_Player:UnBlockDBTrans()				--释放物品锁
		invitee:UnBlockDBTrans()						--释放物品锁
		if suc then
			RetExcuteTrade(Conn, invitee_conn, result)
		else
			local  invitor = CPlayerTran:new(Conn.m_Player.m_uID)
			local errorMsgId = result
			if IsNumber(errorMsgId) then
				if errorMsg ~= nil then
			    MsgToConn(Conn, errorMsgId, errorMsg)
			    MsgToConn(invitee_conn, errorMsgId, errorMsg)
				else
			    MsgToConn(Conn, errorMsgId)
			    MsgToConn(invitee_conn, errorMsgId)
				end
			end
			invitor:ClearTranTbl(Conn)
			invitor:ClearTranTbl(invitee_conn)
			Gas2Gac:ReturnCanceledTrade(Conn)
			Gas2Gac:ReturnCanceledTrade(invitee_conn)
		end
	end
	
	--local entry = (g_DBTransDef["PlayersTranDB"])
	CallDbTrans(PlayersTranDB, "ExcuteTrade", CallBack, data, Conn.m_Account:GetID(), invitee.m_Conn.m_Account:GetID())
end

local function GetPlayersTradeRequestList(Conn)
	if Conn.invitor_info_tbl == nil or # Conn.invitor_info_tbl == 0 then
		--, "没有交易请求信息！"
		MsgToConn(Conn, 6010)
		return
	else
		if Conn.busystate ~= nil and Conn.busystate ==1 then
			--, "正在交易，不能查看其他交易请求！"
			MsgToConn(Conn, 6011)
			return 
		end
	 	local invitorInfoTblLen = # Conn.invitor_info_tbl
		if invitorInfoTblLen == 1 then
			Gas2Gac:GotTradeInvitation( Conn, Conn.invitor_info_tbl[1][1], Conn.invitor_info_tbl[1][2] )
		else
			Gas2Gac:GetPlayersTradeRequestBegin(Conn)
			for i=1, invitorInfoTblLen do
				Gas2Gac:GetPlayersTradeRequest(Conn, Conn.invitor_info_tbl[i][1], Conn.invitor_info_tbl[i][2])
			end
			Gas2Gac:GetPlayersTradeRequestEnd(Conn)
		end
	end
end

--请求打开交易请求信息面板
function Gac2Gas:GetPlayersTradeRequestList(Conn)
	GetPlayersTradeRequestList(Conn)
end

local function DenyAllPlayersTradeRequest(Conn)
    if Conn.invitor_info_tbl == nil then
       return 
    end
	local invitorInfoTblLen = # Conn.invitor_info_tbl
	for i=1, invitorInfoTblLen do
		local player = g_GetPlayerInfo(Conn.invitor_info_tbl[i][1])
		if player ~= nil then			--玩家在线
			local Conn= player.m_Conn
			local charName = player.m_Properties:GetCharName()
			MsgToConn(Conn, 6020, charName)
		end
	end
	Gas2Gac:ConcealOrNotPlayersTradeRequest(Conn, false)
	Conn.invitor_info_tbl = {}
end

function Gac2Gas:DenyAllPlayersTradeRequest(Conn)
	DenyAllPlayersTradeRequest(Conn)
end
