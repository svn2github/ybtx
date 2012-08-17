local StmtOperater = {}	
gac_gas_require "item/item_info_mgr/ItemInfoMgr"
gac_gas_require "item/store_room_cfg/StoreRoomCfg"
local g_ItemInfoMgr = CItemInfoMgr:new()			
local g_StoreRoomIndex = g_StoreRoomIndex  		
local g_IsSlotRight = g_IsSlotRight   
  			
local CAppendBagMgrDB = CreateDbBox(...)
--------------------------------------------------附加背包----------------------------
local StmtDef=
{
    "_GetSlotInfo",
    [[
    	select 
    		bag.ibiu_uBagSlotIndex,bag.is_uId,bag.ibiu_uRoomIndex,
    		item.is_uType,item.is_sName
    	from 
    		tbl_item_bag_in_use as bag join tbl_item_static as item 
    			on bag.is_uId = item.is_uId 
    	where bag.cs_uId =? 
    		and bag.ibiu_uBagSlotIndex  =? 
    	order by 
    		bag.ibiu_uRoomIndex;
    ]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef=
{
    "_GetAllRoomUse",
    [[
    	select 
    		ibiu_uRoomIndex
    	from 
    		tbl_item_bag_in_use
    	where cs_uId =? 
    		and ibiu_uRoomIndex >= ?
    	order by 
    		ibiu_uRoomIndex;
    ]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef=
{
    "_AddBagInUse",
    "insert into tbl_item_bag_in_use (cs_uId,is_uId,ibiu_uBagSlotIndex,ibiu_uRoomIndex) values(?,?,?,?)"
}
DefineSql(StmtDef,StmtOperater)

local StmtDef=
{
    "_SelectPosByRoom",
    "select gir_uPos from tbl_grid_in_room where cs_uId = ? and gir_uRoomIndex = ? order by gir_uPos"
}
DefineSql(StmtDef,StmtOperater)

local StmtDef=
{
   "_ReplaceBag",
   [[
    update 
    	tbl_item_in_grid as room 
    	join tbl_grid_in_room as igp
    		on igp.gir_uGridID = room.gir_uGridID
    	join tbl_item_bag_in_use as bag 
    		on igp.cs_uId = bag.cs_uId
    set 
    	room.is_uId = ?,bag.is_uId = ?
    where 
    	igp.cs_uId = ?
    	and igp.gir_uRoomIndex = ?
    	and igp.gir_uPos = ? 
    	and bag.ibiu_uBagSlotIndex = ? 
   ]]
}
DefineSql(StmtDef,StmtOperater)


local StmtDef=
{
    "_ChangeRoom",
    "update tbl_grid_in_room set gir_uPos = ? where cs_uId = ? and gir_uRoomIndex = ? and gir_uPos = ?"
}
DefineSql(StmtDef,StmtOperater)
local StmtDef=
{
    "_UpdateItemGridInfo",
    [[
    	update tbl_grid_info 
    	set is_uType= ?,is_sName = ?
    	where gir_uGridID = ?
    ]]
}
DefineSql(StmtDef,StmtOperater)
function CAppendBagMgrDB.UseBag(nCharID,nCharLevel,nRoom,nPos,nSlot)
	local nFirstSlot,nLastSlot = g_IsSlotRight(nSlot)
	if nFirstSlot == nil then
		return -1
	end
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	local nBagBigID,nBagIndex,nBagItemID = g_RoomMgr.GetOneItemByPosition(nCharID,nRoom,nPos)

	if nBagBigID == nil then
		return 0
	end
	if g_ItemInfoMgr:IsPlayerBag(nBagBigID) == false then
		return 2
	end
	if nCharLevel < (g_ItemInfoMgr:GetItemInfo( nBagBigID,nBagIndex,"BaseLevel" ) or 1) then
		return 4
	end
	if g_RoomMgr.CountItemLimitByID(nBagItemID) > 0 then
		--禁止使用
		return 6
	end
	local nGridID = g_RoomMgr.GetGridID(nCharID,nRoom,nPos)
	local SlotUse = StmtOperater._GetSlotInfo:ExecSql("nnnns[32]",nCharID,nSlot)
	if SlotUse:GetRowNum() == 0 then
		SlotUse:Release()
		--这个巢没有被使用，查找哪个room没有被使用过
		local nRoomUsed = -1
		local RoomIndexUse = StmtOperater._GetAllRoomUse:ExecSql("n",nCharID,g_StoreRoomIndex.SlotRoomBegin)
		for i=g_StoreRoomIndex.SlotRoomBegin ,g_StoreRoomIndex.SlotRoomLast do
			local nRunNum = 0
			if RoomIndexUse ~= nil then
				local num = RoomIndexUse:GetRowNum()
				for n=1 ,num do
					if RoomIndexUse:GetData(n-1,0) == i then
						break;
					end
					nRunNum = nRunNum + 1
				end
				if nRunNum == RoomIndexUse:GetRowNum() then
					nRoomUsed = i
					break
				end
			end
		end
		RoomIndexUse:Release()
		assert(nRoomUsed >= g_StoreRoomIndex.SlotRoomBegin and nRoomUsed <= g_StoreRoomIndex.SlotRoomLast)
		StmtOperater._AddBagInUse:ExecSql("",nCharID,nBagItemID,nSlot,nRoomUsed) --背包是不可以叠加的，所有可以用查出的物品id
		if not g_RoomMgr.MoveItemFromPackage(nCharID,nBagItemID,"notCallBack") then
			return 0
		end
		return {nRoom,nPos,nSlot,nRoomUsed}
	else 
		--如果背包就在要交换的包内是不行交换的
		local uSlotIndex,uBagID,uBagRoomIndex,uBagBigID,uBagIndex = SlotUse:GetData(0,0),SlotUse:GetData(0,1),SlotUse:GetData(0,2),SlotUse:GetData(0,3),SlotUse:GetData(0,4)
    		SlotUse:Release()
		if nRoom == uBagRoomIndex then
			return 3
		end
		--这个巢有背包，看新包是否能容纳下原来的东西
		local AllPos = StmtOperater._SelectPosByRoom:ExecSql("n",nCharID,uBagRoomIndex)
		if AllPos == nil then
			return 
		end
		local BagRoom = g_ItemInfoMgr:GetItemInfo( nBagBigID,nBagIndex,"BagRoom" )
		if AllPos:GetRowNum() > BagRoom then
			AllPos:Release()
			return 4
		end

		local n,nLow,nLast = 1,1,AllPos:GetRowNum()
		--最后一个格子的nPos比新背包最后一个格子大，才循环
		while nLast > 0 and BagRoom < AllPos:GetData(nLast-1,0)  do
			--查找第一个空的格子。nLow 表示那个格子，顺序进行从1,2,3 ...BagRoom
			while nLow <= BagRoom do
				if nLow < AllPos:GetData(nLast-1,0) then
					--print("ChangRoom,nLast,nLow,nCharID,nRoom,AllPos[nLast][1]",nLast,nLow,nCharID,nRoom,AllPos[nLast][1])
					StmtOperater._ChangeRoom:ExecSql("",nLow,nCharID,uBagRoomIndex,AllPos:GetData(nLast-1,0))
					break
				else
					n = n + 1
				end
				nLow = nLow + 1
			end
			nLast = nLast - 1
		end
		AllPos:Release()
		--交换背包
		StmtOperater._ReplaceBag:ExecSql("",uBagID,nBagItemID,nCharID,nRoom,nPos,uSlotIndex)
		if g_DbChannelMgr:LastAffectedRowNum() == 0 then
			CallDbTrans()
			return 0
		end
		if not (nBagBigID == uBagBigID and nBagIndex == uBagIndex) then
			StmtOperater._UpdateItemGridInfo:ExecStat(uBagBigID,uBagIndex,nGridID)
			if g_DbChannelMgr:LastAffectedRowNum() == 0 then
				CallDbTrans()
				return 0
			end
		end
		return {nRoom,nPos,nSlot,0}
	end
end 
-------------------------------------------------------------------------------------------
local StmtDef=
{
    "_MoveBagInUse",
    "delete from tbl_item_bag_in_use where cs_uId =? and ibiu_uRoomIndex = ?;" 
}
DefineSql(StmtDef,StmtOperater)

local StmtDef=
{
    "_SelectBagTypeBySlot",
    [[ 
   	select 
   		item.is_uType,item.is_sName,bag.is_uId,bag.ibiu_uRoomIndex
    from 
    	tbl_item_bag_in_use as bag 
    	join 
    	tbl_item_static as item 
    		on bag.is_uId = item.is_uId 
    where bag.cs_uId = ? 
    	and bag.ibiu_uBagSlotIndex = ?;
    ]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef=
{
    "_GetCountFromRoom",
    [[
    	select count(*) from tbl_grid_in_room
     	where  cs_uId = ? and gir_uRoomIndex = ?;
    ]] 
}
DefineSql(StmtDef,StmtOperater)

--[[
	从附栏上取下背包
--]]
function CAppendBagMgrDB.FetchBag(nCharID,nSlot,nRoom,nPos)
	local nFirstSlot,nLastSlot = g_IsSlotRight(nSlot)
	if nFirstSlot == nil then
		return -1
	end
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	if g_RoomMgr.CheckRoomAndPos(nCharID,nRoom,nPos) == false then
		return -1
	end
	
	local BagType = StmtOperater._SelectBagTypeBySlot:ExecSql("ns[32]nn",nCharID,nSlot)
	if BagType == nil then
		return 0
	end
	if BagType:GetRowNum() == 0 then
		BagType:Release()
		return 0
	end
	
	
	local nBagBigID,nBagIndex,nBagItemID,nBagUseRoom = BagType:GetData(0,0),BagType:GetData(0,1),BagType:GetData(0,2),BagType:GetData(0,3)
	BagType:Release()
	if nBagUseRoom == nRoom then
		return 1
	end
	local DesPosItemCount = g_RoomMgr.GetCountByPosition(nCharID,nRoom,nPos)
	if DesPosItemCount > 0 then
		return 2
	end
	--判断是否为空包，查询附栏的背包是否装有东西
	local SrcRoomCount = StmtOperater._GetCountFromRoom:ExecStat(nCharID,nBagUseRoom)
	if SrcRoomCount ~= nil then
		if SrcRoomCount:GetRowNum()>0 then
			if SrcRoomCount:GetData(0,0) > 0 then
				SrcRoomCount:Release()
				return 3
			end
		end
		SrcRoomCount:Release()
	end
	
	StmtOperater._MoveBagInUse:ExecSql("",nCharID,nBagUseRoom)
	local nGridID = g_RoomMgr.GetGridID(nCharID,nRoom,nPos)
	if not g_RoomMgr.PutIntoRoom(nCharID,nGridID,nBagItemID) then
		CancelTran()
		return 0
	end
	return {nSlot,nRoom,nPos,nGridID}
end
--------------------------------------------------------------------------------------------------
--[[
	从附栏上直接删除背包

--]]
function CAppendBagMgrDB.DelBagInUse(nCharID,nSlot)
	local nFirstSlot,nLastSlot = g_IsSlotRight(nSlot)
	if nFirstSlot == nil then
		return -1
	end
	local BagType = StmtOperater._SelectBagTypeBySlot:ExecSql("ns[32]nn",nCharID,nSlot)
	if BagType == nil then
		return 0
	end
	if BagType:GetRowNum() == 0 then
		BagType:Release()
		return 0
	end
	
	local nBagBigID,nBagIndex,nBagItemID,nBagUseRoom = BagType:GetData(0,0),BagType:GetData(0,1),BagType:GetData(0,2),BagType:GetData(0,3)
	BagType:Release()
	--判断是否为空包，查询附栏的背包是否装有东西
	local SrcRoomCount = StmtOperater._GetCountFromRoom:ExecSql("n",nCharID,nBagUseRoom)
	if SrcRoomCount ~= nil then
		if SrcRoomCount:GetRowNum()>0 then
			if SrcRoomCount:GetData(0,0) > 0 then
				SrcRoomCount:Release()
				return 1
			end
		end
		SrcRoomCount:Release()
	end
	StmtOperater._MoveBagInUse:ExecSql("",nCharID,nBagUseRoom)
	return {nSlot}
end
------------------------------------------------------------------------------------
--从数据里面取出两个包裹的信息
local StmtDef=
{
	"_Select2BagInfo",
	[[
		select 
			is_uId, ibiu_uBagSlotIndex, ibiu_uRoomIndex 
		from 
			tbl_item_bag_in_use 
		where 
			cs_uId = ? and ibiu_uBagSlotIndex in (?, ?)
	]]
		
}
DefineSql(StmtDef,StmtOperater)

--重新插入两个包裹的信息
local StmtDef=
{
	"_Insert2BagInfo",
	[[
		replace into tbl_item_bag_in_use values (?, ?, ?, ?), (?, ?, ?, ?)
	]]
		
}
DefineSql(StmtDef,StmtOperater)

local StmtDef=
{
    "_MoveBagSlot",
    "update tbl_item_bag_in_use set ibiu_uBagSlotIndex = ? where cs_uId = ? and ibiu_uBagSlotIndex = ?"
}

DefineSql(StmtDef,StmtOperater)

function CAppendBagMgrDB.Change2BagInUse(nCharID,nASlot,nBSlot)
	local nFirstSlot,nLastSlot = g_IsSlotRight(nASlot)
	if nFirstSlot == nil then
		return -1
	end
	
	nFirstSlot,nLastSlot = g_IsSlotRight(nBSlot)
	if nFirstSlot == nil then
		return -1
	end
	
	if nASlot == nBSlot then
		return 0
	end
	
	--必须判断附栏上是否有东西
	local ABagType = StmtOperater._SelectBagTypeBySlot:ExecSql("ns[32]nn",nCharID,nASlot)
	if ABagType == nil then
		return 0
	end
	if ABagType:GetRowNum() == 0 then
		ABagType:Release()
		return 0
	end
	--第2个巢是空的，是被允许的，那就把第一个巢的位置改一下
	local BBagType = StmtOperater._SelectBagTypeBySlot:ExecSql("ns[32]nn",nCharID,nBSlot)
	if BBagType == nil then
		StmtOperater._MoveBagSlot:ExecSql("",nBSlot,nCharID,nASlot)
	elseif BBagType:GetRowNum() == 0 then
		BBagType:Release()
		StmtOperater._MoveBagSlot:ExecSql("",nBSlot,nCharID,nASlot)
	else	
		local ARoomIndex,BRoomIndex = ABagType:GetData(0,3),BBagType:GetData(0,3)
		local BagInfo = StmtOperater._Select2BagInfo:ExecStat(nCharID,nASlot,nBSlot)
		StmtOperater._Insert2BagInfo:ExecStat(nCharID,BagInfo(2,1),BagInfo(1,2),BagInfo(2,3),
								nCharID,BagInfo(1,1),BagInfo(2,2),BagInfo(1,3))
		ABagType:Release()
		BBagType:Release()
	end
	

	return {nASlot,nBSlot}
end

return CAppendBagMgrDB