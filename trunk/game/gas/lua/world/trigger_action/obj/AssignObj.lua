
--数据库操作后  平均分配
local function AverageAssignOneObj(WhoClick, IntObj, TeamID, DropNpcID, data, result)
	if result[1] then
		local GainerId = result[2]
		local Gainer = g_GetPlayerInfo(GainerId)
		if result[3] then
			if IsCppBound(Gainer) then
				CRoleQuest.add_item_DB_Ret(Gainer, data["nType"], data["sName"], 1, result[3][2])
				if result[3][2][1][1] then
					Gas2Gac:AddItemMsgToConn(Gainer.m_Conn,result[3][2][1][1])
				end
				local Quality = g_ItemInfoMgr:GetItemInfo( data["nType"], data["sName"],"Quality" )
				local DisplayName = g_ItemInfoMgr:GetItemLanInfo(data["nType"],data["sName"],"DisplayName")
				if Quality == EAuctionStandard.eAS_CyanStandard then
					--SysMovementNotifyToClient(g_AllPlayerSender, 1002, Gainer.m_Properties:GetCharName(),DisplayName)
					--ShowContentToAllPlayer(1002,Gainer.m_Properties:GetCharName(),data["sName"])
				end
				if result[3][3] then
					SendPlayerFinishDirect(Gainer, result[3][3])
				end
			end
			if IsServerObjValid(IntObj) then
				g_IntObjServerMgr:Destroy(IntObj,false)
			end
		else
			if IntObj then
				IntObj.m_IsClick = nil
			end
			if IsCppBound(Gainer) then
				MsgToConn(Gainer.m_Conn, GetQuestAddItemErrorMsgID(3))
				if IsCppBound(WhoClick) and WhoClick.m_uID ~= GainerId then
					local sName = Gainer.m_Properties:GetCharName()
					MsgToConn(WhoClick.m_Conn, 3035, sName)
				end
			end
			if IsServerObjValid(IntObj) then
				IntObj.m_Properties:SetDropOwnerId(GainerId)
				IntObj.m_Properties:SetBelongToTeam(0)
				IntObj.m_Properties:SetDropTime(os.time())
			end
		end
		
		--更改平均分配的轮流顺序
		if TeamID and g_AverageAssignOrder[TeamID] then
			for i = 1, table.getn(g_AverageAssignOrder[TeamID]) do
				if g_AverageAssignOrder[TeamID][i] == GainerId then
					table.remove(g_AverageAssignOrder[TeamID],i)
					table.insert(g_AverageAssignOrder[TeamID],GainerId)
					break
				end
			end
		end
		
		if NpcDropObjTbl[DropNpcID] then
			local PlayerTbl = NpcDropObjTbl[DropNpcID].m_CanSharePlayer
			local PlayerNum = table.getn(PlayerTbl)
			for i = 1, PlayerNum do
				if PlayerTbl[i] == GainerId then
					table.remove(PlayerTbl,i)
					table.insert(PlayerTbl,GainerId)
					break
				end
			end
		end
	else
		if IntObj then
			IntObj.m_IsClick = nil
		end
		if IsCppBound(WhoClick) then
			MsgToConn(WhoClick.m_Conn,3033)
		end
	end
	return 
end

--进入物品分配
function AllotObj( Conn, ItemObj, ObjName, ObjID, data)
	local WhoClick = Conn.m_Player
	local IntObj = CIntObjServer_GetIntObjServerByID(ObjID)
--	if not IsCppBound(WhoClick) then
--		return
--	end
	local PlayerId = data["char_id"]
	local TeamID = ItemObj.m_Properties:GetBelongToTeam()
	local DropNpcID = ItemObj.m_BelongToNpc
	local function CallBack(result, IsAuctionAssign)
		--local WhoClick = g_GetPlayerInfo(PlayerId)
		--local ItemObj = CIntObjServer_GetIntObjServerByID(ObjID)
--		if ItemObj then
--			ItemObj.m_IsClick = nil
--		end
		
--		if data["AssignMode"] then
--			if not IsAuctionAssign then   --不是拍卖 平均分配
		if data["AssignMode"] == EAssignMode.eAM_AverageAssign then
			AverageAssignOneObj(WhoClick,IntObj, TeamID, DropNpcID, data,result) --平均分配
		elseif data["AssignMode"] == EAssignMode.eAM_AuctionAssign then
			--拍卖分配
		elseif data["AssignMode"] == EAssignMode.eAM_NeedAssign then
			 --需求分配
		else
			if result[1] then
				if IsServerObjValid(ItemObj) then
					if ItemObj.m_Properties:GetDropOwnerId() ~= 0 then
						g_IntObjServerMgr:Destroy(ItemObj,false)
					else
						g_IntObjServerMgr:Destroy(ItemObj,true)
					end
				end
				if IsCppBound(WhoClick) then 
					CRoleQuest.add_item_DB_Ret(WhoClick, data["nType"], data["sName"], 1, result[2])
					if result[2][1][1] then
						Gas2Gac:AddItemMsgToConn(WhoClick.m_Conn,result[2][1][1])
					end
					local Quality = g_ItemInfoMgr:GetItemInfo( data["nType"], data["sName"],"Quality" )
					local displayName = g_ItemInfoMgr:GetItemLanInfo( data["nType"], data["sName"],"DisplayName" )
					if Quality == EAuctionStandard.eAS_CyanStandard then
						--SysMovementNotifyToClient(g_AllPlayerSender, 1002, WhoClick.m_Properties:GetCharName(),displayName)
						--ShowContentToAllPlayer(GetStaticTextServer(13002,WhoClick.m_Properties:GetCharName(),data["sName"]))
					end
					if result[3] then
						SendPlayerFinishDirect(WhoClick, result[3])
					end
				end
			else
				if ItemObj then
					ItemObj.m_IsClick = nil
				end
				if IsCppBound(WhoClick) and GetQuestAddItemErrorMsgID(result[2]) then
					MsgToConn(Conn, GetQuestAddItemErrorMsgID(result[2]))
				end
			end
		end
	end
	
	ItemObj.m_IsClick = true
	local res = CallAccountManualTrans(Conn.m_Account, "ItemObjDB","PickUpOneItemObj", CallBack, data)
	if not res or res ~= 0 then
		ItemObj.m_IsClick = nil
	end
end

function NeedAssignGiveItem(ItemObj, data)

	local AccountID = data["char_id"]
	local TempPlayer = g_GetPlayerInfo(data["char_id"])
	if IsCppBound(TempPlayer) then 
		AccountID = TempPlayer.m_AccountID
	end
	data["sceneName"] = ItemObj.m_Scene.m_SceneName
	local function CallBack(result)
		local Player = g_GetPlayerInfo(data["char_id"])
		if result[1] then
			if IsServerObjValid(ItemObj) then
				if ItemObj.m_Properties:GetDropOwnerId() ~= 0 then
					g_IntObjServerMgr:Destroy(ItemObj,false)
				else
					g_IntObjServerMgr:Destroy(ItemObj,true)
				end
			end
			CRoleQuest.add_item_DB_Ret_By_Id(data["char_id"], data["nType"], data["sName"], 1, result[2])
			if result[2][1][1] then
				Gas2GacById:AddItemMsgToConn(data["char_id"],result[2][1][1])
			end				
			local Quality = g_ItemInfoMgr:GetItemInfo( data["nType"], data["sName"],"Quality" )
			local displayName = g_ItemInfoMgr:GetItemLanInfo( data["nType"], data["sName"],"DisplayName" )
			if Quality == EAuctionStandard.eAS_CyanStandard then
				--SysMovementNotifyToClient(g_AllPlayerSender, 1002, data["char_name"],displayName)
				--ShowContentToAllPlayer(GetStaticTextServer(13002,data["char_name"],data["sName"]))
			end
			
			if IsCppBound(Player) then 
				if result[3] then
					SendPlayerFinishDirect(Player, result[3])
				end
			end
		else
			if ItemObj then
				ItemObj.m_IsClick = nil
			end
			if IsCppBound(Player) then
--			if result[2] == 3 then
				MsgToConn(Player.m_Conn, 3038)
				if IsServerObjValid(ItemObj) then
					if Player.m_Scene == ItemObj.m_Scene then
						local Pos = CFPos:new()
						Player:GetPixelPos(Pos)
						Pos.x = Pos.x + 0.5
						Pos.y = Pos.y + 0.5
						ItemObj:SetPixelPos(Pos)
						ItemObj.m_Properties:SetDropOwnerId(Player.m_uID)
						ItemObj.m_Properties:SetBelongToTeam(0)
						ItemObj.m_Properties:SetDropTime(os.time())
					end
				end
			end
		end
	end
	ItemObj.m_IsClick = true
	CallDbTrans("ItemObjDB", "NeedAssignGiveItem", CallBack, data, AccountID)
end

--拍卖模式物品分配
function AcutionAssignGiveItem(IntObj, data)

	local AssignNum = 0 
	local ChannelIDList = {}
	for _, char_id in pairs(IntObj.m_AssignPlayer) do 
		local Player = g_GetPlayerInfo(char_id)
		if IsCppBound(Player) then
			table.insert(ChannelIDList, Player.m_AccountID)
		else
			table.insert(ChannelIDList, char_id)
		end
		AssignNum = AssignNum + 1
	end
	
	local TeamID = IntObj.m_Properties:GetBelongToTeam()
	if not TeamID then
		return
	end
	data["AssignPlayerTbl"] = IntObj.m_AssignPlayer
	data["TeamID"] = TeamID
	
	local MaxPrice = g_MoneyMgr:GetAftertaxMoney( data["GainMoney"] )
	local PerAssignMoney = math.floor(MaxPrice / AssignNum)
	local sysFee = math.floor((data["GainMoney"] - MaxPrice) / AssignNum)
	local RestMoney = MaxPrice % AssignNum
	data["addmoney"] = PerAssignMoney
	data["sysFee"] = sysFee
	data["SurplusMoney"] = RestMoney
	data["sceneName"] = IntObj.m_Scene.m_SceneName
	data["EventId"] = IntObj.m_AcutionEventId
	
	local function CallBack(result)
		if result[1] then
			if IsTable(result[2]) then
				if IsServerObjValid(IntObj) then
					if IntObj.m_Properties:GetDropOwnerId() ~= 0 then
						g_IntObjServerMgr:Destroy(IntObj,false)
					else
						g_IntObjServerMgr:Destroy(IntObj,true)
					end
				end
				CRoleQuest.add_item_DB_Ret_By_Id(data["char_id"], data["nType"], data["sName"], 1, result[2])
				if result[2][1][1] then
					Gas2GacById:AddItemMsgToConn(data["char_id"],result[2][1][1])
				end				
				local Quality = g_ItemInfoMgr:GetItemInfo( data["nType"], data["sName"],"Quality" )
				local displayName = g_ItemInfoMgr:GetItemLanInfo( data["nType"], data["sName"],"DisplayName" )
				if Quality == EAuctionStandard.eAS_CyanStandard then
					--SysMovementNotifyToClient(g_AllPlayerSender, 1002, data["char_name"],displayName)
				end
			elseif IsNumber(result[2]) then
				if IntObj then
					IntObj.m_IsClick = nil
				end
				MsgToConnById(data["char_id"], 3038)
				local Player = g_GetPlayerInfo(data["char_id"])
				if IsCppBound(Player) then
					if IsServerObjValid(IntObj) then
						if Player.m_Scene == IntObj.m_Scene then
							local Pos = CFPos:new()
							Player:GetPixelPos(Pos)
							Pos.x = Pos.x + 0.5
							Pos.y = Pos.y + 0.5
							IntObj:SetPixelPos(Pos)
							IntObj.m_Properties:SetDropOwnerId(Player.m_uID)
							IntObj.m_Properties:SetBelongToTeam(0)
							IntObj.m_Properties:SetDropTime(os.time())
						end
					end
				end
			end
				
			local SuccTbl = result[3]
			for char_id, moneyTbl in pairs(SuccTbl) do 
				if moneyTbl[1] then
					MsgToConnById(char_id, 3993, data["sName"], moneyTbl[2])
				end
			end
		else
			if IsNumber(result[3]) then
				MsgToConnById(data["char_id"], result[3])	
			end
		end
	end
	CallDbTrans("AuctionAssignDB","AcutionAssignGiveItem", CallBack, data, unpack(ChannelIDList))
end
