gac_gas_require "item/item_info_mgr/ItemInfoMgr"
gas_require "item/item_bag/BreakItemMgr"
local g_BreakItemMgr = CBreakItemMgr:new()
local g_ItemInfoMgr = CItemInfoMgr:new()		

local StmtOperater = {}	
local event_type_tbl = event_type_tbl

local BreakItemDB = CreateDbBox(...)

local StmtDef = {
	"_AddBreakExp",
	[[
		insert into tbl_item_break_exp
		set cs_uId = ?,ibp_uExp = ?
	]]
}
DefineSql( StmtDef, StmtOperater )

local StmtDef = {
	"_GetBreakExp",
	[[
		select ibp_uExp from  tbl_item_break_exp
		where cs_uId = ?
	]]
}
DefineSql( StmtDef, StmtOperater )

local StmtDef = {
	"_UpBreakExp",
	[[
		update tbl_item_break_exp 
		set ibp_uExp = ibp_uExp + ?
		where cs_uId = ?
	]]
}
DefineSql( StmtDef, StmtOperater )

function BreakItemDB.GetBreakExpByChar(nCharID)
	local tbl = StmtOperater._GetBreakExp:ExecStat(nCharID)
	if tbl:GetRowNum() > 0 then
		return tbl(1,1)
	end
	return
end

function BreakItemDB.CanBreakItem(parameters)
	local nCharID = parameters.nCharID
	local nRoomIndex = parameters.nRoomIndex
	local nPos = parameters.nPos
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	if not g_RoomMgr.CheckByRIndex(nRoomIndex, nCharID)then   
			return
	end
	local ItemType,ItemName,ItemCount = g_RoomMgr.GetTypeCountByPosition(nCharID,nRoomIndex,nPos)
	if ItemType == nil or ItemName == nil then
		return
	end
	local bCanBreak,BreakInfo = g_BreakItemMgr:CanBreak(ItemName,ItemType)
	if not bCanBreak then
		return 24
	end
	local nTotalExp = BreakItemDB.GetBreakExpByChar(nCharID) or 0
	if nTotalExp < BreakInfo("NeedExp") then
		return 34,BreakInfo("NeedExp")
	end
	return true
end

function BreakItemDB.BreakItem(parameters)
	local nCharID = parameters.nCharID
	local nRoomIndex = parameters.nRoomIndex
	local nPos = parameters.nPos
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	if not g_RoomMgr.CheckByRIndex(nRoomIndex, nCharID)then   
			return
	end
	
	local ItemBagLockDB = RequireDbBox("ItemBagLockDB")
  if ItemBagLockDB.HaveItemBagLock(nCharID) then
  	return false,160010
  end
  
	local ItemType,ItemName,ItemCount = g_RoomMgr.GetTypeCountByPosition(nCharID,nRoomIndex,nPos)
	if ItemType == nil or ItemName == nil then
		return false,26
	end
	if not g_BreakItemMgr:CanBreak(ItemName,ItemType) then
		return false,24
	end
	local tbl_del = g_RoomMgr.DelItemByPos(nCharID,nRoomIndex,nPos,1,event_type_tbl["分解删除"])
	local tblProducts = g_BreakItemMgr:GetBreakProducts(ItemName,ItemType)
	local item_bag_box = RequireDbBox("CPutItemToBagDB")
	local tbl_add = {}
	for i =1,#tblProducts do
		local prod_name,prod_type,prod_num = unpack(tblProducts[i])
		if not g_ItemInfoMgr:HaveItem(prod_type,prod_name)  then
			assert(false,prod_type .. "大类物品表中不存在物品：" .. prod_name)
			CancelTran()
			return false
	 	end
		local tbl_item_id = {}
		local params= {}
		params.m_nType = prod_type
		params.m_sName = prod_name
		params.m_nBindingType = g_ItemInfoMgr:GetItemInfo(prod_type,prod_name,"BindingStyle")
		params.m_nCharID = nCharID
		params.m_sCreateType = event_type_tbl["分解产出"]
		for j=1,prod_num do
			local item_id = g_RoomMgr.CreateItem(params)
			if not item_id then
				CancelTran()
				return
			end
			table.insert(tbl_item_id, {item_id})
		end
		
		local params2 = {}
		params2.m_nCharID = nCharID
		params2.m_nItemType = prod_type
		params2.m_sItemName = prod_name
		params2.m_tblItemID = tbl_item_id
		local succ, info = item_bag_box.AddCountItem(params2)
		if(not succ) then
			CancelTran()
			return false,info
		end
		table.insert(tbl_add,{info,#tbl_item_id})
	end
	local AddExp = 0
	if #tbl_add > 0 then
		local _,BreakInfo = g_BreakItemMgr:CanBreak(ItemName,ItemType)
		local nTotalExp = BreakItemDB.GetBreakExpByChar(nCharID)
		if not nTotalExp then
			AddExp = BreakInfo("AddExp")
			StmtOperater._AddBreakExp:ExecStat(nCharID,BreakInfo("AddExp"))
			if not (g_DbChannelMgr:LastAffectedRowNum() > 0) then
				CancelTran()
				return
			end
		else
			if nTotalExp < BreakInfo("TopExp") then
				if nTotalExp + BreakInfo("AddExp") < BreakInfo("TopExp") then
					AddExp = BreakInfo("AddExp")
				else
					AddExp = BreakInfo("TopExp") - nTotalExp
				end
				if 0 < AddExp then
					StmtOperater._UpBreakExp:ExecStat(AddExp,nCharID)
					if not (g_DbChannelMgr:LastAffectedRowNum() > 0) then
						CancelTran()
						return
					end
				end
			end	
		end
	end
	local tblRes = {}
	tblRes.tbl_del = tbl_del
	tblRes.tbl_add = tbl_add
	tblRes.AddExp = AddExp
	return tblRes
end

function BreakItemDB.GMAddBreakItemExp(data)
	local nAddExp = data.m_nExp
	local nCharID = data.m_nCharID
	local nTotalExp = BreakItemDB.GetBreakExpByChar(nCharID)
	if not nTotalExp then
			StmtOperater._AddBreakExp:ExecStat(nCharID,nAddExp)
			if not (g_DbChannelMgr:LastAffectedRowNum() > 0) then
				CancelTran()
				return
			end
			return true
	end
	StmtOperater._UpBreakExp:ExecStat(nAddExp,nCharID)
	if not (g_DbChannelMgr:LastAffectedRowNum() > 0) then
		CancelTran()
		return
	end
	return true
end

------------------------------------------------------------------------------------------------------
SetDbLocalFuncType(BreakItemDB.CanBreakItem)
SetDbLocalFuncType(BreakItemDB.BreakItem)
SetDbLocalFuncType(BreakItemDB.GMAddBreakItemExp)
return BreakItemDB