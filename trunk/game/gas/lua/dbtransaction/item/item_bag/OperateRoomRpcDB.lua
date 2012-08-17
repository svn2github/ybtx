gac_gas_require "item/item_info_mgr/ItemInfoMgr"
gac_gas_require "item/store_room_cfg/StoreRoomCfg"
local g_ItemInfoMgr = CItemInfoMgr:new()	
---------------------------------------------------------------------------
local OperateRoomRpcDB = CreateDbBox(...)
---------------------------------------------------------------------------

--【根据位置删除物品】
function OperateRoomRpcDB.DelItemByPos(parameters)
	local nCharID = parameters["nCharID"]
	local nRoomIndex =  parameters["nRoomIndex"]
	local nPos = parameters["nPos"]
	local nCount = parameters["nCount"]
	local sceneName = parameters["sceneName"]
	local delType = parameters["delType"]
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	if not g_RoomMgr.CheckByRIndex(nRoomIndex, nCharID)then    
			return 
	end
	local ItemBagLockDB = RequireDbBox("ItemBagLockDB")
  if ItemBagLockDB.HaveItemBagLock(nCharID) then
  	return
  end

	local ItemType,ItemName,ItemCount = g_RoomMgr.GetTypeCountByPosition(nCharID,nRoomIndex,nPos)
	if ItemCount == nil or ItemCount == 0 then
		return 
	end
	local res = g_RoomMgr.DelItemByPos(nCharID,nRoomIndex,nPos,nCount,delType)
	--任务物品
	local param = {}
	param["PlayerId"] = nCharID
	param["ItemType"] = tonumber(ItemType)
	param["ItemName"] = ItemName
	param["nCont"] = nCount
	param["sceneName"] = sceneName
	local RoleQuestDB = RequireDbBox("RoleQuestDB")
	local DecItemResult = RoleQuestDB.DecreQuestItemVar(param)
	if not DecItemResult[1] then
		return
	end
	--帮会物品
	if tonumber(ItemType) == g_ItemInfoMgr:GetTongItemBigID() then
		local TongItemDB = RequireDbBox("GasTongItemCreateDB")
		TongItemDB.DeleteTongBuildItem(param)
	end
	return res,ItemName,DecItemResult
end
----------------------------------------------------------------------------------------------
--【拆分物品】
function OperateRoomRpcDB.SplitItem(parameters)
	local nCharID = parameters["nCharID"]
	local nARoom = parameters["nARoom"] 
	local nAPos = parameters["nAPos"]
	local nBRoom = parameters["nBRoom"]
	local nBPos = parameters["nBPos"]
	local nCount = parameters["nCount"]
	
	local ItemBagLockDB = RequireDbBox("ItemBagLockDB")
  if ItemBagLockDB.HaveItemBagLock(nCharID) then
  	return
  end
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	if not ( g_RoomMgr.CheckByRIndex(nARoom,nCharID)and g_RoomMgr.CheckByRIndex(nBRoom,nCharID) )then
		return 
	end
	
	local res = g_RoomMgr.GetItemFromA2B(nCharID,nARoom,nAPos,nCharID,nBRoom,nBPos,nCount)
	
	return res
end

----------------------------------------------------------------------------------------------
--【移动物品】
function OperateRoomRpcDB.MoveItem(parameters)
	local nCharID = parameters["nCharID"]
	local nARoom = parameters["nARoom"] 
	local nAPos = parameters["nAPos"]
	local nBRoom = parameters["nBRoom"]
	local nBPos = parameters["nBPos"]
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	if not ( g_RoomMgr.CheckByRIndex(nARoom,nCharID)and g_RoomMgr.CheckByRIndex(nBRoom,nCharID) )then
		return {}
	end
	
	local res,type = g_RoomMgr.MoveItem(nCharID,nARoom,nAPos,nBRoom,nBPos)
	
	return {res,type} 
end
------------------------------------------------------------------------------------------------
--【右键点击移动物品】
function OperateRoomRpcDB.QuickMoveItem(parameters)
	local nCharID = parameters["nCharID"]
	local nARoom = parameters["nARoom"] 
	local nAPos = parameters["nAPos"]
	local nStoreMain = parameters["nStoreMain"]
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")

	if not ( g_RoomMgr.CheckByRIndex(nARoom,nCharID)and g_RoomMgr.CheckByRIndex(nStoreMain,nCharID) )then
		return 
	end
	
	local res = g_RoomMgr.QuickMoveItem(nCharID,nARoom,nAPos,nStoreMain)
	
	return res
end
------------------------------------------------------------------------------------------------
--【使用背包】
function OperateRoomRpcDB.UseBag(parameters)
	local nCharID = parameters["nCharID"]
	local nRoom = parameters["nRoom"] 
	local nPos = parameters["nPos"]
	local nSlot = parameters["nSlot"]
	local nCharLevel = parameters["nCharLevel"]
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	
	local ItemBagLockDB = RequireDbBox("ItemBagLockDB")
  if ItemBagLockDB.HaveItemBagLock(nCharID) then
  	return
  end
  
	if not ( g_RoomMgr.CheckBySlot(nSlot)and g_RoomMgr.CheckByRIndex(nRoom,nCharID) )then
		return 
	end
	local AppendBagMgr = RequireDbBox("CAppendBagMgrDB")
	local res = AppendBagMgr.UseBag(nCharID,nCharLevel,nRoom,nPos,nSlot)
	
	return res
end
--------------------------------------------------------------------------------------------------
--【取下背包】
function OperateRoomRpcDB.FetchBag(parameters)
	local nCharID = parameters["nCharID"]
	local nRoom = parameters["nRoom"] 
	local nPos = parameters["nPos"]
	local nSlot = parameters["nSlot"]
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")

	local ItemBagLockDB = RequireDbBox("ItemBagLockDB")
  if ItemBagLockDB.HaveItemBagLock(nCharID) then
  	return
  end
  
	if not ( g_RoomMgr.CheckBySlot(nSlot)and g_RoomMgr.CheckByRIndex(nRoom,nCharID) )then
		return 3
	end
	local AppendBagMgr = RequireDbBox("CAppendBagMgrDB")
	local res = AppendBagMgr.FetchBag(nCharID,nSlot,nRoom,nPos)
	
	return res
end
---------------------------------------------------------------------------------------------------
--【删除背包】
function OperateRoomRpcDB.DelBag(parameters)
	local nCharID = parameters["nCharID"]
	local nSlot = parameters["nSlot"]
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	
	local ItemBagLockDB = RequireDbBox("ItemBagLockDB")
  if ItemBagLockDB.HaveItemBagLock(nCharID) then
  	return
  end
  
	if not ( g_RoomMgr.CheckBySlot(nSlot) )then
		return
	end
	local AppendBagMgr = RequireDbBox("CAppendBagMgrDB")
	local res = AppendBagMgr.DelBagInUse(nCharID,nSlot)
	
	return res
end
----------------------------------------------------------------------------------------------------
--【交换背包】
function OperateRoomRpcDB.Change2Bag(parameters)
	local nCharID = parameters["nCharID"]
	local nASlot = parameters["nASlot"]
	local nBSlot = parameters["nBSlot"]
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")

	if not ( g_RoomMgr.CheckBySlot(nASlot)and g_RoomMgr.CheckBySlot(nBSlot) )then
		return
	end
	local AppendBagMgr = RequireDbBox("CAppendBagMgrDB")

	local res = AppendBagMgr.Change2BagInUse(nCharID,nASlot,nBSlot)
	
	return res
end
-----------------------------------------------------------------------------------------------------
--【整理背包】
function OperateRoomRpcDB.CleanBag(parameters)
	local nCharID = parameters["nCharID"]
	local nStoreMain = parameters["nStoreMain"]
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	local RetRes = g_RoomMgr.ResetAllItemPos(nCharID,nStoreMain)
	return RetRes
end

function OperateRoomRpcDB.DelWithTimeOutItem(parameters)
	local nCharID = parameters.nCharID
	local nItemID = parameters.nItemID
	local nItemType = parameters.nItemType
	local sItemName = parameters.sItemName
	local tbl = {}
	local ItemLifeMgrDB = RequireDbBox("ItemLifeMgrDB")
	local item_info = ItemLifeMgrDB.GetItemLifeInfoByID(nItemID)
	if item_info:GetRowNum() == 0 then
		--说明不是计时物品
		tbl.m_LeftTime = -1
		return tbl
	end
	local nType,sName = tonumber(item_info(1,1)),item_info(1,2)
	if not (nItemType == nType and sName == sItemName) then
		return false
	end
	local EquipMgrDB = RequireDbBox("EquipMgrDB")
	local equip_part = EquipMgrDB.GetEquipPartById(nCharID,nItemID)
	if equip_part ~= 0 then
		tbl.equip_part = equip_part
		tbl.equip_info = EquipMgrDB.GetEquipAllInfo(nItemID)
	else
		tbl.m_nLeftTime = ItemLifeMgrDB.GetOneItemLife(nItemID,nItemType,sItemName)
	end
	return tbl
end

function OperateRoomRpcDB.HaveItemTypeOrderInfo()
	local _,type_res = g_DbChannelMgr:TextExecute("select count(*) from tbl_item_type_order")
	local _,smalltype_res = g_DbChannelMgr:TextExecute("select count(*) from tbl_item_smalltype_order")
	local type_count = type_res:GetNumber(0,0)
	local smalltype_count = type_res:GetNumber(0,0)
	return (type_count == 0) or (smalltype_count == 0)
end

function OperateRoomRpcDB.InitItemTypeOrderDB(parameters)
	g_DbChannelMgr:TextExecute("delete from tbl_item_type_order")
	g_DbChannelMgr:TextExecute("delete from tbl_item_smalltype_order")
	local tbl_order = g_ItemInfoMgr:GetItemTypeOrder()
	local query_string = "insert into tbl_item_type_order(is_uType,is_uOrder) values"
	for m =1,#tbl_order -1 do
			query_string = query_string .. "(" .. tbl_order[m] .. "," .. m .. "),"
	end
	local order_len = #tbl_order
	query_string = query_string .. "(" .. tbl_order[order_len] .. "," .. order_len .. ")" 
	g_DbChannelMgr:TextExecute(query_string)
	for i =1,#tbl_order do
		--一个表执行一次
		local nBigType = tbl_order[i]
		local BigTbl = g_ItemInfoMgr.m_tblId2Table[nBigType]
		local tbl = {}
		local tblNames = BigTbl:GetKeys()
		for j =1,#tblNames do
			local v = BigTbl(tblNames[j])
			local SmallType = v("SmallType")
			local BaseLevel = v("BaseLevel")
			if (SmallType and SmallType ~= "") or (BaseLevel and BaseLevel ~= "") then
				local SmallType = (SmallType == "" or nil == SmallType) and 1000 or SmallType
				local BaseLevel = (BaseLevel == "" or nil == BaseLevel) and 0 or BaseLevel
				local strItemInfo = string.format("%s%d%s%s%s%d%s%d%s", "(", nBigType, ",'", tblNames[j], "',", SmallType, ",", BaseLevel,")")
				table.insert(tbl,strItemInfo)
			end
		end
		if #tbl > 0 then
			local query_string2 = "insert into tbl_item_smalltype_order(is_uType,is_sName,iso_uSmallType,iso_uBaseLevel) values"
			query_string2 = query_string2 .. table.concat(tbl,",")
			g_DbChannelMgr:TextExecute(query_string2)
		end
	end
	--local ToolsMallDB = RequireDbBox("ToolsMallDB")
	--ToolsMallDB.InitToolsmallTabTypeInfo()
end

function OperateRoomRpcDB.ResetSoulPearlTime(data)
	local nLimitType = data.nLimitType
	g_DbChannelMgr:TextExecute("delete from tbl_item_pearl_limit where ipl_uLimitType = " .. nLimitType)
end

function OperateRoomRpcDB.ClearItemAmountInfo(data)
	local _,res = g_DbChannelMgr:TextExecute("select count(*) from tbl_item_amount_limit")
	if res(1,1) > 0 then
		g_DbChannelMgr:TextExecute("delete from tbl_item_amount_limit")
	end
end
------------------------------------------------------------------------------------------------------
--返回闭包
SetDbLocalFuncType(OperateRoomRpcDB.DelItemByPos)
SetDbLocalFuncType(OperateRoomRpcDB.SplitItem)
SetDbLocalFuncType(OperateRoomRpcDB.MoveItem)
SetDbLocalFuncType(OperateRoomRpcDB.UseBag)
SetDbLocalFuncType(OperateRoomRpcDB.FetchBag)
SetDbLocalFuncType(OperateRoomRpcDB.DelBag)
SetDbLocalFuncType(OperateRoomRpcDB.Change2Bag)
SetDbLocalFuncType(OperateRoomRpcDB.DelWithTimeOutItem)
SetDbLocalFuncType(OperateRoomRpcDB.QuickMoveItem)
SetDbLocalFuncType(OperateRoomRpcDB.CleanBag)
return OperateRoomRpcDB