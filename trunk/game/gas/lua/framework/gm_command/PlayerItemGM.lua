gac_gas_require "item/store_room_cfg/StoreRoomCfg"


local function CheckAppendMagic(nAddCount,nChaseCount,nMagicPurple,nStrength,nAgile,nConstitution,nBrains,nSprint)
	
	if nAddCount < 0 or nChaseCount < 0 or nMagicPurple <= 0 or
		nStrength < 0 or nAgile < 0 or nConstitution < 0 or nBrains < 0 or nSprint < 0 then
		 return false
	end
	return true
end

function  GMCommand.Executor.delitem( Connection, nBigID,nIndex,nCount )
	local function CallBack(result)
		if not result[1] then
			Gas2Gac:RetDelItemGMError(Connection,nBigID,nIndex,result[2])
		end	
	end
	
	local parameter = 
	{
		['player_id'] = Connection.m_Player.m_uID,
		['item_type'] = nBigID,
		['item_name'] = nIndex,
		['count'] = nCount,
	}
	
	--local lGMDBExecutor = (g_DBTransDef["GMDB"])
	CallAccountAutoTrans(Connection.m_Account, "GMDB", "DelItem", CallBack, parameter)
	return ""
end

function  GMCommand.Executor.targetdelitem( Connection,sTargetName, nBigID,nIndex,nCount )
	local Player = g_GetPlayerInfoByName(sTargetName)
	if Player == nil then
		MsgToConn(Connection,19)
		return
	end
	
	local function CallBack(result)
		if not result[1] then
			Gas2Gac:RetDelItemGMError(Connection,nBigID,nIndex,result[2])
		else
			local Player = g_GetPlayerInfoByName(sTargetName)
			if Player == nil then
				return
			end
		end	
	end
	
	local parameter = 
	{
		['player_id'] = Player.m_uID,
		['item_type'] = nBigID,
		['item_name'] = nIndex,
		['count'] = nCount,
	}
	
	--local lGMDBExecutor = (g_DBTransDef["GMDB"])
	CallAccountAutoTrans(Connection.m_Account, "GMDB", "DelItem", CallBack, parameter)
	return ""
end
--参数：魂根类型、每个魂珠所含魂值
function GMCommand.Executor.AddSoulPearl(Connection, sSoulRoot, uSoulCount )
 	if uSoulCount > 2000000000 then
		--, "魂珠所含魂值数目过大"
		MsgToConn(Connection, 8)
		return 
	end
	local item_name = g_ItemInfoMgr:GetSoulpearlInfo(sSoulRoot)
	if item_name == nil then return end

	local itemInfo = g_ItemInfoMgr:HaveItem(g_ItemInfoMgr:GetSoulPearlBigID(), item_name)
	if not itemInfo then
		return 
	end
	
	local nBindingType = 0
	local function CallBack(result)
		if result == false then
			return
		end
	end

	local parameter = {}
	parameter["player_id"] = Connection.m_Player.m_uID
	parameter["item_name"] = item_name
	parameter["index"] = g_StoreRoomIndex.PlayerBag
	parameter["soultype"] = sSoulRoot
	parameter["soulcount"] = uSoulCount
	parameter["SceneName"] = Connection.m_Player.m_Scene.m_SceneName
	--local lGMDBExecutor = (g_DBTransDef["GMDB"])
	CallAccountAutoTrans(Connection.m_Account, "GMDB", "AddSoulPearl", CallBack, parameter)
	return ""
end

 function GMCommand.Executor.AddJiFen(Connection, jiFenType, jiFenCount )
    local data = {["JiFenType"] = jiFenType, ["JiFenCount"]= jiFenCount, ["CharID"] = Connection.m_Player.m_uID,
    							["SceneName"] = Connection.m_Player.m_Scene.m_SceneName }    
    if jiFenType  > 65535 then
        MsgToConn(Connection, 15)
        return
    end
    if jiFenType < 1 or jiFenType > 10 then
        MsgToConn(Connection, 17)
        return
    end
    local function CallBack(result, modifyJiFenCount)
        if result then
            Gas2Gac:UpdatePlayerPoint(Connection,jiFenType,modifyJiFenCount )
            MsgToConn(Connection,18)
        else
            local errorMsgID = modifyJiFenCount
            if IsNumber(errorMsgID) then
            	MsgToConn(Connection, errorMsgID)
            end
        end
    end
    
    CallAccountAutoTrans(Connection.m_Account, "GMDB", "AddJiFen", CallBack, data)
end

function GMCommand.Executor.TargetAddJiFen(Connection,sTargetName, jiFenType, jiFenCount )
    local Player = g_GetPlayerInfoByName(sTargetName)
		if Player == nil then
			MsgToConn(Connection,19)
			return
		end
    
    local data = {["JiFenType"] = jiFenType, ["JiFenCount"]= jiFenCount, ["CharID"] = Player.m_uID,
    							["SceneName"] = Player.m_Scene.m_SceneName}    
    if jiFenType  > 65535 then
        MsgToConn(Connection, 15)
        return
    end
    if jiFenType < 1 or jiFenType > 5 then
        MsgToConn(Connection, 17)
        return
    end
    local function CallBack(result, modifyJiFenCount)
        if result then
            Gas2Gac:UpdatePlayerPoint(Player.m_Conn,jiFenType,modifyJiFenCount )
            MsgToConn(Connection,18)
        else
            local errorMsgID = modifyJiFenCount
            if IsNumber(errorMsgID) then
	            MsgToConn(Connection, errorMsgID)
	          end
        end
    end
    
    CallAccountAutoTrans(Connection.m_Account, "GMDB", "AddJiFen", CallBack, data)
end

local function AddItemSuc(Conn, nType, sName, result, count)

	Gas2Gac:RetAddItemAllGMEnd(Conn,nType,sName,count)
end

function GMCommand.Executor.additem( Connection, nBigID,nIndex,nCount )
	if not g_ItemInfoMgr:CanAddByAddItemGM(nBigID) then
		sError = "This type of item can`t be created by 'additem()' "
		Gas2Gac:RetAddItemGMError(Connection,nBigID,nIndex,sError)
		return 	
	end
	
	local ItemInfo = g_ItemInfoMgr:HaveItem( nBigID, nIndex )	
	if not ItemInfo then
		return   --说明用gm指令添加的物品不存在
	end
	
	local function CallBack(result)
		if not result[1] then
			Gas2Gac:RetAddItemGMError(Connection,nBigID,nIndex,result[2])
			return 	
		end
		AddItemSuc(Connection, nBigID, nIndex, result[2], nCount)
	end

	local parameter = {}
	parameter["itemtype"] = nBigID
	parameter["itemname"] = nIndex 
	parameter["count"] = nCount
	parameter["index"] = g_StoreRoomIndex.PlayerBag
	parameter["player_id"] = Connection.m_Player.m_uID
	parameter["bindingStyle"] = g_ItemInfoMgr:GetItemInfo( nBigID, nIndex,"BindingStyle" )
		
	--local lGMDBExecutor = (g_DBTransDef["GMDB"])
	CallAccountAutoTrans(Connection.m_Account, "GMDB", "AddItem", CallBack, parameter)
	return ""
end

function  GMCommand.Executor.targetadditem( Connection,sTargetName, nBigID,nIndex,nCount )
	local Player = g_GetPlayerInfoByName(sTargetName)
	if Player == nil then
		MsgToConn(Connection,19)
		return
	end
	
	if not g_ItemInfoMgr:CanAddByAddItemGM(nBigID) then
		sError = "This type of item can`t be created by 'targetadditem()' "
		Gas2Gac:RetAddItemGMError(Connection,nBigID,nIndex,sError)
		return 	
	end
	
	local ItemInfo = g_ItemInfoMgr:HaveItem( nBigID, nIndex )	
	if not ItemInfo then
		return   --说明用gm指令添加的物品不存在
	end
	local function CallBack(result)
		if not result[1] then
			Gas2Gac:RetAddItemGMError(Connection,nBigID,nIndex,result[3])
			return 	
		end
		AddItemSuc(Player.m_Conn, nBigID, nIndex, result[2], nCount)
	end

	local parameter = {}
	parameter["itemtype"] = nBigID
	parameter["itemname"] = nIndex 
	parameter["count"] = nCount
	parameter["index"] = g_StoreRoomIndex.PlayerBag
	parameter["player_id"] = Player.m_uID
	parameter["bindingStyle"] = g_ItemInfoMgr:GetItemInfo( nBigID, nIndex,"BindingStyle" )
	
	--local lGMDBExecutor = (g_DBTransDef["GMDB"])
	CallAccountAutoTrans(Connection.m_Account, "GMDB", "AddItem", CallBack, parameter)
	return ""
end

function GMCommand.Executor.AddBreakitemExp( Connection,nExp )
	local function CallBack(result)
		if result then
			Gas2Gac:AddBreakItemExp(Connection,nExp)
		end
	end

	local data = {}
	data.m_nExp = nExp
	data.m_nCharID = Connection.m_Player.m_uID
	CallAccountAutoTrans(Connection.m_Account, "BreakItemDB", "GMAddBreakItemExp", CallBack, data)
	return ""
end

function GMCommand.Executor.setaddcount(Connection,nItemID,nNum)
	return
end

function GMCommand.Executor.AddIntenEquip(Conn, equipType, equipName, suitName)
    local data = {["CharID"] =Conn.m_Player.m_uID , ["EquipType"] =equipType, ["EquipName"] =equipName, ["SuitName"] = suitName, 
                   ["SceneName"] = Conn.m_Player.m_Scene.m_SceneName,
                   ["Index"] = g_StoreRoomIndex.PlayerBag}
    local function CallBack(suc, result)
        if suc then
            local equipID = result["EquipID"]
            local equipType = result["EquipType"]
            local equipName = result["EquipName"]
            local equipInfo = result["EquipInfo"]
            local equip_mgr = g_GetEquipMgr()
            local equip = equip_mgr:GetEquipFromRet(equipID, Conn.m_Player.m_uID, tonumber(equipType), equipName, equipInfo)
            equip:SendEquipInfo2Conn(Conn.m_Player.m_uID)
            MsgToConn(Conn, 180022)
        else
           local errMsgID = result["MsgID"]
           MsgToConn(Conn, errMsgID) 
        end
    end

    CallAccountAutoTrans(Conn.m_Account, "EquipIntensifyDB", "AddIntenEquip", CallBack, data)		
end
