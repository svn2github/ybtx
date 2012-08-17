gac_gas_require "item/item_info_mgr/ItemInfoMgr"
gac_gas_require "relation/tong/TongMgr"
local g_ItemInfoMgr = CItemInfoMgr:new()
local g_TongMgr =	CTongMgr:new()
local StmtOperater = {}	
local event_type_tbl = event_type_tbl
local CDepotRpcDB = CreateDbBox(...)

----------------------------
--【获得物品信息】
function CDepotRpcDB.GetCDepotItemByPage(parameters)
	local nCharID = parameters.nCharID
	local nType = parameters.nType
	local nPage = parameters.nPage
	local sceneName = parameters.sceneName
	
	local g_CDepotSQLMgr = RequireDbBox("GasCDepotSQLMgrDB")
	local build_box = RequireDbBox("GasTongbuildingDB")
	local TongItemBox = RequireDbBox("GasTongItemCreateDB")
	
	--刷新帮会物品制造信息
	build_box.GetCreatingBuildingItemInfoDB({["uPlayerID"] = nCharID, ["sceneName"] = sceneName})
	TongItemBox.GetCreatingItemInfo({["uPlayerID"] = nCharID, ["sceneName"] = sceneName})
	
	local nDepotID = g_CDepotSQLMgr.GetTongDepotIdByCharID(nCharID)
	if nDepotID == 0 then
		--没有仓库
		return {}
	end	
	local tblCdepotItemInfo = g_CDepotSQLMgr.GetCDepotItemByPage(nCharID,nDepotID,nPage)
	return tblCdepotItemInfo
end



--【移动物品】
function CDepotRpcDB.CollectiveBagMoveItemSelf(parameters)
	local nCharID = parameters.nCharID
	local nType = parameters.nType
	local nFromPage = parameters.nFromPage
	local nFromPos = parameters.nFromPos
	local nToPage = parameters.nToPage
	local nToPos = parameters.nToPos
	local g_CDepotSQLMgr = RequireDbBox("GasCDepotSQLMgrDB")
	local nDepotID = g_CDepotSQLMgr.GetTongDepotIdByCharID(nCharID)
	if nDepotID == 0 then return end
	
	--使用权限判断
	local tong_box = RequireDbBox("GasTongBasicDB")
  if not (tong_box.JudgePurview(nCharID,g_TongMgr:GetDepotStrByPage(nFromPage)) and tong_box.JudgePurview(nCharID,g_TongMgr:GetDepotStrByPage(nToPage))) then
  	return 9305
  end
	local depotMgr = RequireDbBox("GasCDepotMgrDB")
	local nFromType,nFromName,nFromCount = g_CDepotSQLMgr.GetDepotItemTypeNameAndCountByPos(nDepotID,nFromPage,nFromPos)
	if (nFromCount == nil or nFromCount <= 0) then return end
	
	local result = depotMgr.MoveItem(nDepotID,nFromPage,nFromPos,nDepotID,nToPage,nToPos)
	
	if IsTable(result) then
		if  nFromPage ~= nToPage then
			--添加日志
			local ex = RequireDbBox("Exchanger")
			local player_name = ex.getPlayerNameById(nCharID)
			local from_room = g_TongMgr.m_tblPosLevel2MsgID[nFromPage]
			local to_room = g_TongMgr.m_tblPosLevel2MsgID[nToPage]
			local sMsg = "2018_" .. player_name .. "," .. nFromCount .. ",item:" .. nFromType .. "|" .. nFromName .. ",msgid:" .. from_room .. ",msgid:" .. to_room
		 	if not tong_box.AddTongLogs(tong_box.GetTongID(nCharID),sMsg, g_TongMgr:GetLogType("仓库")) then
		 	 	return 9107
			end
		end
  else
  	return result
  end
	return true,nFromType,nFromName
end

--【捐献物品】
function CDepotRpcDB.MoveItemFromBagToCDepotRPC(data)
	local nCharID = data.nCharID
	local nFromRoom = data.nFromRoom
	local nFromPos = data.nFromPos
	local nToRoom = data.nToRoom
	local nToPos = data.nToPos
	local nToPage = data.nToPage
	local g_CDepotSQLMgr = RequireDbBox("GasCDepotSQLMgrDB")
	local nDepotID = g_CDepotSQLMgr.GetTongDepotIdByCharID(nCharID)
	if nDepotID == 0 then return end
	
	--判断使用权限
	local tong_box = RequireDbBox("GasTongBasicDB")
  if not tong_box.JudgePurview(nCharID,g_TongMgr:GetDepotStrByPage(nToPage)) then
  	return 9305
  end
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")	
	local _,_,_,nBindingType = g_RoomMgr.GetOneItemByPosition(nCharID,nFromRoom,nFromPos)
	if nBindingType == 2 then
		return 9306
	end
	local ItemType,ItemName,ItemCount = g_RoomMgr.GetTypeCountByPosition(nCharID,nFromRoom,nFromPos)
	if not ItemCount or 0 == ItemCount then
		return 
	end
	if g_ItemInfoMgr:IsQuestItem(ItemType) then
		return 9300
	end
	if g_ItemInfoMgr:IsSoulPearl(ItemType) then
		--叠加物品的魂值不一样，不能出售
		if g_RoomMgr.CountSoulNumByPos(nCharID,nFromRoom,nFromPos) > 1 then
			return 9299
		end
	end
	local tbl_del_res = g_RoomMgr.MoveItemFromPackageByPos(nCharID,nFromRoom,nFromPos,ItemCount)
	if IsTable(tbl_del_res) then
		local tong_id = tong_box.GetTongID(nCharID)
		local player_pos = tong_box.GetPositionById(nCharID)
		local param = {}
		param.PlayerId = nCharID
		param.ItemType = tonumber(ItemType)
		param.ItemName= ItemName
		param.nCont = #tbl_del_res
		param.sceneName = data["sceneName"]

		local depotMgr = RequireDbBox("GasCDepotMgrDB")
		local tbl_add_res = depotMgr.AddCDepotItemByPos(nDepotID, ItemType,ItemName,nToPage, nToPos,tbl_del_res)
		if not tbl_add_res or IsNumber(tbl_add_res) then 
			CancelTran()
			return tbl_add_res
		end
		--添加日志
		local ex = RequireDbBox("Exchanger")
		local player_name = ex.getPlayerNameById(nCharID)
 		local to_room = g_TongMgr.m_tblPosLevel2MsgID[nToPage]
  	local sMsg = "2020_" .. player_name .. "," .. ItemCount .. ",item:" .. ItemType .. "|" .. ItemName .. ",msgid:" .. to_room
 	 	if not tong_box.AddTongLogs(tong_id,sMsg, g_TongMgr:GetLogType("仓库")) then
   	 	return 9107
  	end	
		local g_LogMgr = RequireDbBox("LogMgrDB")	
		for i =1,#tbl_del_res do
			local ItemID = tbl_del_res[i]
			g_LogMgr.DepotEventLog(tong_id,2,ItemID,nDepotID,nCharID,player_pos)
		end
		local tbl_ret_res = {}
		tbl_ret_res.ItemType 	= ItemType
		tbl_ret_res.ItemName 	= ItemName
		return tbl_ret_res
	end
end

--【左键领取物品】
function CDepotRpcDB.MoveItemFromCDepotToBag(parameters)
	local nCharID = parameters.nCharID
	local nFromRoom = parameters.nFromRoom
	local nFromPage = parameters.nFromPage
	local nFromPos = parameters.nFromPos
	local nToRoom = parameters.nToRoom
	local nToPos = parameters.nToPos
	local g_CDepotSQLMgr = RequireDbBox("GasCDepotSQLMgrDB")		
	local nDepotID = g_CDepotSQLMgr.GetTongDepotIdByCharID(nCharID)
	if nDepotID == 0 then return end

	--判断使用权限
	local tong_box = RequireDbBox("GasTongBasicDB")
  if not tong_box.JudgePurview(nCharID,g_TongMgr:GetDepotStrByPage(nFromPage)) then
  	return 9305
  end
	local sType,sName,nCount = g_CDepotSQLMgr.GetDepotItemTypeNameAndCountByPos(nDepotID,nFromPage,nFromPos)
	if not(sType and sName) then return end

	local depotMgr = RequireDbBox("GasCDepotMgrDB")
	local ItemIDs = depotMgr.DeleteItemByPos(nDepotID, nFromPage, nFromPos)
	if not g_ItemInfoMgr:HaveItem(sType,sName) then return end
	if IsTable(ItemIDs) then
		local item_bag_box = RequireDbBox("CPutItemToBagDB")
		local res = item_bag_box.AddItemByPos(nCharID, sType, sName, nToRoom, nToPos, ItemIDs)
		if not res or IsNumber(res) then
			CancelTran()
			return res
		end
		
		if IsTable(res) then
			--生命期
			local ItemLifeMgrDB = RequireDbBox("ItemLifeMgrDB")	
			local player_pos = tong_box.GetPositionById(nCharID)
			local tong_id = tong_box.GetTongID(nCharID)
			local g_LogMgr = RequireDbBox("LogMgrDB")	
			for i =1,#res do
				local ItemID = res[i]
				if not ItemLifeMgrDB.AddItemLifeInfo(sType, sName,ItemID,1) then
					CancelTran()
					return
				end
				g_LogMgr.DepotEventLog(tong_id,1,ItemID,nDepotID,nCharID,player_pos)
			end	
			--添加日志
			local ex = RequireDbBox("Exchanger")
			local player_name = ex.getPlayerNameById(nCharID)
	 		local from_room = g_TongMgr.m_tblPosLevel2MsgID[nFromPage]
	  	local sMsg = "2019_" .. player_name .. ",msgid:" .. from_room .. "," .. nCount .. ",item:" .. sType .. "|" .. sName
	 	 	if not tong_box.AddTongLogs(tong_id,sMsg, g_TongMgr:GetLogType("仓库")) then
	   	 	return 9107
	  	end
		end
	end
	return true
end

--【右键领取】
function CDepotRpcDB.QuikMoveItemFromCDepotToBag(parameters)
	local nCharID = parameters.nCharID
	local nFromRoom = parameters.nFromRoom
	local nFromPage = parameters.nFromPage
	local nFromPos = parameters.nFromPos
	local nToStoreRoom = parameters.nToStoreRoom
	local g_CDepotSQLMgr = RequireDbBox("GasCDepotSQLMgrDB")		
	local nDepotID = g_CDepotSQLMgr.GetTongDepotIdByCharID(nCharID)
	if nDepotID == 0 then return end

	--判断使用权限
	local tong_box = RequireDbBox("GasTongBasicDB")
  if not tong_box.JudgePurview(nCharID,g_TongMgr:GetDepotStrByPage(nFromPage)) then
  	return 9305
  end
	local sType,sName,nCount = g_CDepotSQLMgr.GetDepotItemTypeNameAndCountByPos(nDepotID,nFromPage,nFromPos)
	if not(sType and sName) then return end
	if not g_ItemInfoMgr:HaveItem(sType,sName) then 
		return 
	end
	local depotMgr = RequireDbBox("GasCDepotMgrDB")
	local ItemIDs = depotMgr.DeleteItemByPos(nDepotID, nFromPage, nFromPos)
	if IsTable(ItemIDs) then
		local ItemLifeMgrDB = RequireDbBox("ItemLifeMgrDB")	
		local g_LogMgr = RequireDbBox("LogMgrDB")	
		local tong_id = tong_box.GetTongID(nCharID) 
		local player_pos = tong_box.GetPositionById(nCharID)
		local tbl_item = {}
		for i =1,#ItemIDs do
			local ItemID = ItemIDs[i]
			if not ItemLifeMgrDB.AddItemLifeInfo(sType, sName,ItemID,1)	then
				CancelTran()
				return
			end
			g_LogMgr.DepotEventLog(tong_id,1,ItemID,nDepotID,nCharID,player_pos)
			table.insert(tbl_item,{ItemID})
		end
		local params = {}
		params.m_nCharID = nCharID
		params.m_nItemType = sType
		params.m_sItemName = sName
		params.m_tblItemID = tbl_item
		local item_bag_box = RequireDbBox("CPutItemToBagDB")
		local bFlag, ret_res = item_bag_box.AddCountItem(params)
		if not bFlag then
			CancelTran()
			return ret_res
		end		
		--添加日志
		local ex = RequireDbBox("Exchanger")
		local player_name = ex.getPlayerNameById(nCharID)
 		local from_room = g_TongMgr.m_tblPosLevel2MsgID[nFromPage]
  	local sMsg = "2019_" .. player_name .. ",msgid:" .. from_room .. "," .. nCount .. ",item:" .. sType .. "|" .. sName
 	 	if not tong_box.AddTongLogs(tong_id,sMsg, g_TongMgr:GetLogType("仓库")) then
   	 	return 9107
  	end
 	end
	return true
end

SetDbLocalFuncType(CDepotRpcDB.GetCDepotItemByPage)
------------------------------------------
return CDepotRpcDB
