
local function CheckIsPickUpAllItem(Player, GlobalID)
	if NpcDropItemTbl[GlobalID] == nil then
		return false
	end
	
--	if not NpcDropItemTbl[GlobalID].m_PickUpIndex or not IsTable(NpcDropItemTbl[GlobalID].m_PickUpIndex) then
--		return false
--	end
	
	if table.getn(NpcDropItemTbl[GlobalID].m_PickUpIndex) > 0 then
		return false
	end
	Player:DelListeningWnd()
	Gas2Gac:NotifyClosePickUpWnd(Player.m_Conn)
	local cServerNpc = CCharacterDictator_GetCharacterByID(GlobalID)
	cServerNpc.m_Properties:SetDropOwnerId(0)
	cServerNpc.m_Properties:SetDropTime(0)
	cServerNpc.m_Properties:SetBelongToTeam(0)
	NpcDropItemTbl[GlobalID].m_Openner = nil
	NpcDropItemTbl[GlobalID].m_PickUpIndex = nil
	Player.m_PickUpNpcID = nil
	return true
end

function OpenPickUpDropWnd(Openner, GlobalID, PickUpIndexTbl)
	if not PickUpIndexTbl or table.maxn(PickUpIndexTbl) == 0 then
		return false
	end
		
	NpcDropItemTbl[GlobalID].m_PickUpIndex = PickUpIndexTbl
--	print("---------PickUpDropWnd-----------------")
	for i = 1,table.maxn(PickUpIndexTbl) do
		local ItemIndex = PickUpIndexTbl[i]
		local ItemType = NpcDropItemTbl[GlobalID].m_Items_List[ItemIndex][1]
		local ItemName = NpcDropItemTbl[GlobalID].m_Items_List[ItemIndex][2]
		local ItemNum = NpcDropItemTbl[GlobalID].m_Items_List[ItemIndex][3]
		
--		print("ItemType,ItemName,ItemNum",ItemType,ItemName,ItemNum)
		Gas2Gac:RetNpcDropItem(Openner.m_Conn,ItemType,ItemName,ItemNum,true)
	end
	Openner:AddListeningWnd()	
	Gas2Gac:RetNpcDropItemEnd(Openner.m_Conn,GlobalID)
	return true
end

local function CheckIsCanSelectItem(Player,ItemHolder,GlobalID)
	if NpcDropItemTbl[GlobalID] == nil then
		return false
	end
	
	if ItemHolder == nil then
		return false
	end
	
	if g_GetDistance(Player,ItemHolder)>6 then
		return false
	end
	
	local Items_List = NpcDropItemTbl[GlobalID].m_Items_List
	if Items_List == nil then
		return false
	end
	
	local PlayerId = Player.m_uID
	if NpcDropItemTbl[GlobalID].m_Openner ~= PlayerId then
		return false
	end
	
	if not NpcDropItemTbl[GlobalID].m_PickUpIndex then
		return false
	end
	return true
end

function Gac2Gas:SelectSingleGridDropItem(Conn, PannelIndex, GlobalID)
	local ItemHolder = CCharacterDictator_GetCharacterByID(GlobalID)
	if not CheckIsCanSelectItem(Conn.m_Player,ItemHolder,GlobalID) then
		return
	end
	
	local GetIndex = NpcDropItemTbl[GlobalID].m_PickUpIndex[PannelIndex]
	if not GetIndex then
		return
	end
	
	local Items_List = NpcDropItemTbl[GlobalID].m_Items_List
	local itemtbl = Items_List[GetIndex]
	if itemtbl == nil then --物品等待拍卖，不能拾取
		return
	end
	
	if itemtbl["QuestDropOwner"] and itemtbl["QuestDropOwner"] ~= Conn.m_Player.m_uID then   --该物品已经分配出去
		return
	end
	
	--[[
	if itemtbl["isclick"] == true or NpcDropItemTbl[GlobalID]["IsAllClick"] == true then
--		print("已经单击过此物品,等待数据库的处理中...")
		return
	else
		itemtbl["isclick"] = true
	end
	--]]
	
	local itemtype = itemtbl[1]
	local itemname = itemtbl[2]
	local itemnum = itemtbl[3]
	local Opener = Conn.m_Player
	local PlayerId = Opener.m_uID
	
	local function CallBack(result)
--		local Opener = g_GetPlayerInfo(PlayerId)
		if result[1] then
			if GetIndex and NpcDropItemTbl[GlobalID] then
				NpcDropItemTbl[GlobalID].m_Items_List[GetIndex] = nil
				--数据库处理过程中玩家掉线或者走动m_PickUpIndex会被清掉
				if NpcDropItemTbl[GlobalID].m_PickUpIndex then
					table.remove(NpcDropItemTbl[GlobalID].m_PickUpIndex, PannelIndex)
				end
				--local cServerNpc = CCharacterDictator_GetCharacterByID(GlobalID)
				if IsCppBound(ItemHolder) and table.maxn(NpcDropItemTbl[GlobalID].m_Items_List) == 0 then
					Gas2Gac:NotifyNpcDropItem(ItemHolder:GetIS(0),GlobalID,false)
				end
			end
			
			if IsCppBound(Opener) then
				CRoleQuest.add_item_DB_Ret(Opener,itemtype,itemname,itemnum,result[2])
				g_MatchGameMgr:AddMatchGameCount(Opener, 4, itemname, itemnum)
				Gas2Gac:RetGetSingleGridDropItem(Opener.m_Conn,PannelIndex)
				--数据库处理过程中玩家掉线或者走动m_PickUpIndex会被清掉
				if NpcDropItemTbl[GlobalID].m_PickUpIndex then
					CheckIsPickUpAllItem(Opener, GlobalID) --判断是否所有物品都已经提取完
				end
			end
		elseif GetQuestAddItemErrorMsgID(result[2]) and IsCppBound(Opener) then
			MsgToConn(Opener.m_Conn, GetQuestAddItemErrorMsgID(result[2]))
		end
		
		--if NpcDropItemTbl[GlobalID] and NpcDropItemTbl[GlobalID].m_Items_List and NpcDropItemTbl[GlobalID].m_Items_List[GetIndex] then
		--	NpcDropItemTbl[GlobalID].m_Items_List[GetIndex]["isclick"] = nil--将不可再点击的标识去掉
		--end
	end
	
	local parameter = {}
	parameter["char_id"] = PlayerId
	parameter["nType"] = itemtype
	parameter["sName"] = itemname
	parameter["nCount"] = itemnum
	parameter["sceneName"] = Conn.m_Player.m_Scene.m_SceneName
	parameter["CheckQuestNeed"] = nil
	if Items_List[GetIndex]["QuestDropOwner"] then  
		parameter["CheckQuestNeed"] = true
	end
	
	CallAccountManualTrans(Conn.m_Account, "NpcDropItemDB", "SelOneGridDropItem", CallBack, parameter)
end

function Gac2Gas:CancelGetDropItems(Conn, GlobalID)
	if NpcDropItemTbl[GlobalID] == nil then
		return
	end
	
	local PlayerId=Conn.m_Player.m_uID
	if NpcDropItemTbl[GlobalID].m_Openner ~= PlayerId then
		return
	end
	
	Conn.m_Player:DelListeningWnd()
	NpcDropItemTbl[GlobalID].m_Openner = nil
	NpcDropItemTbl[GlobalID].m_PickUpIndex = nil
	Conn.m_Player.m_PickUpNpcID = nil
end

function Gac2Gas:SelectAllDropItems(Conn, GlobalID)
	local ItemHolder = CCharacterDictator_GetCharacterByID(GlobalID)
	if not CheckIsCanSelectItem(Conn.m_Player,ItemHolder,GlobalID) then
		return
	end
	
	local Items_List = NpcDropItemTbl[GlobalID].m_Items_List
	------------------------------------------	
	--[[
	if NpcDropItemTbl[GlobalID]["IsAllClick"] == true then
--		print("已经单击了全部拾取,等待数据库的处理中...")
		return
	end
	
	for i=1, table.getn(NpcDropItemTbl[GlobalID].m_PickUpIndex) do
		if Items_List[i] and Items_List[i]["isclick"] == true then
--			print("已经单击过此物品,等待数据库的处理中...")
			return
		end
	end
	
	NpcDropItemTbl[GlobalID]["IsAllClick"] = true
	--]]
	------------------------------------------
	local Player = Conn.m_Player
	local PlayerId = Player.m_uID
	local TeamID = NpcDropItemTbl[GlobalID].m_BelongToTeam
	local function CallBack(result)
--		local Player = g_GetPlayerInfo(PlayerId)
		local AddResTbl = result[1]
		local RemoveIndexTbl = result[2]
		
		if IsCppBound(Player) then
			for i = 1,table.getn(AddResTbl) do
				if AddResTbl[i][1] then
					CRoleQuest.add_item_DB_Ret(Player,AddResTbl[i]["nType"],AddResTbl[i]["sName"],AddResTbl[i]["nCount"],AddResTbl[i][2])
					g_MatchGameMgr:AddMatchGameCount(Player, 4, AddResTbl[i]["sName"], AddResTbl[i]["nCount"])
				elseif GetQuestAddItemErrorMsgID(AddResTbl[i][2]) then
					MsgToConn(Player.m_Conn, GetQuestAddItemErrorMsgID(AddResTbl[i][2]))
				end
			end
		end
		
		if NpcDropItemTbl[GlobalID] then
			--NpcDropItemTbl[GlobalID]["IsAllClick"] = nil
			for i = 1, table.getn(RemoveIndexTbl) do
				local ItemIndex = RemoveIndexTbl[i]
				NpcDropItemTbl[GlobalID].m_Items_List[ItemIndex] = nil
				--数据库处理过程中玩家掉线或者走动m_PickUpIndex会被清掉
				if NpcDropItemTbl[GlobalID].m_PickUpIndex then
					for GridIndex = 1, table.getn(NpcDropItemTbl[GlobalID].m_PickUpIndex) do
						if NpcDropItemTbl[GlobalID].m_PickUpIndex[GridIndex] == ItemIndex then
							Gas2Gac:RetGetDropItemGrid(Conn,GridIndex)
						end
					end
				end
			end
			
			if table.maxn(NpcDropItemTbl[GlobalID].m_Items_List) == 0 then
				local cServerNpc = CCharacterDictator_GetCharacterByID(GlobalID)
				Gas2Gac:NotifyNpcDropItem(cServerNpc:GetIS(0),GlobalID,false)
			end
			
			for i = 1, table.getn(RemoveIndexTbl) do
				local ItemIndex = RemoveIndexTbl[i]
				--数据库处理过程中玩家掉线或者走动m_PickUpIndex会被清掉
				if NpcDropItemTbl[GlobalID].m_PickUpIndex then
					for GridIndex = 1, table.getn(NpcDropItemTbl[GlobalID].m_PickUpIndex) do
						if NpcDropItemTbl[GlobalID].m_PickUpIndex[GridIndex] == ItemIndex then
							table.remove(NpcDropItemTbl[GlobalID].m_PickUpIndex, GridIndex)
						end
					end
				end
			end
			
			if IsCppBound(Player) then
				--数据库处理过程中玩家掉线或者走动m_PickUpIndex会被清掉
				if NpcDropItemTbl[GlobalID].m_PickUpIndex then
			 		if not CheckIsPickUpAllItem(Player, GlobalID) then
						Gas2Gac:RetSelectAllDropItems(Player.m_Conn, false)
					end
				end
			end
		end
	end
	
	local parameter = {}
	parameter["PlayerId"] = PlayerId
	parameter["sceneName"] = Player.m_Scene.m_SceneName
	parameter["DropItemTbl"] = NpcDropItemTbl[GlobalID]
	
	CallAccountManualTrans(Conn.m_Account, "NpcDropItemDB", "SelAllDropItems", CallBack, parameter)
end
