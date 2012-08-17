gac_gas_require "activity/item/ItemUseInfoMgr"
gac_gas_require "item/item_info_mgr/ItemInfoMgr"
gac_gas_require "item/store_room_cfg/StoreRoomCfg"
local g_StoreRoomIndex = g_StoreRoomIndex
local g_ItemInfoMgr = CItemInfoMgr:new()	

local g_ItemUseInfoMgr = g_ItemUseInfoMgr
local os = os

local StmtOperater = class()

local StmtDef = 
{
	"UseItem",
	"replace into tbl_item_cool_down(cs_uId, icd_sName, icd_dataUseTime) values(?,?,now())"
}
DefineSql(StmtDef,StmtOperater)

local StmtDef = 
{
	"SaveItemCoolDownInfo",
	"replace into tbl_item_cool_down(cs_uId, icd_sName, icd_dataUseTime) values(?,?,from_unixtime(unix_timestamp(now()) - ?))"
}
DefineSql(StmtDef,StmtOperater)

local StmtDef = 
{
	"GetItemCoolDownInfo",
	"select icd_sName, unix_timestamp(now()) - unix_timestamp(icd_dataUseTime) from tbl_item_cool_down where cs_uId = ?"
}
DefineSql(StmtDef,StmtOperater)


local StmtDef =
{
	"DelItemCoolDownInfo",
	"delete from tbl_item_cool_down where cs_uId = ? and icd_sName = ?"
}
DefineSql(StmtDef,StmtOperater)



local UseSpecialItemDB = CreateDbBox(...)

function UseSpecialItemDB.UseItem(data)
	local charId = data["CharId"]
	local itemName = data["ItemName"]
	StmtOperater.UseItem:ExecSql("", charId, itemName)
end

function UseSpecialItemDB.SaveSpecialItemCoolDown(charId, coolDownInfo)
	if coolDownInfo then
		local now = os.time()
		for name, time in pairs(coolDownInfo) do
			if g_ItemUseInfoMgr:GetItemUseInfo(name) and (now - time) * 1000 < g_ItemUseInfoMgr:GetItemUseInfo(name,"CoolDownTime") - 2 then
				StmtOperater.SaveItemCoolDownInfo:ExecStat(charId, name, now - time)
			end			
		end
	end
end

function UseSpecialItemDB.GetItemCoolDownInfo(charId)
	local result = StmtOperater.GetItemCoolDownInfo:ExecStat(charId)
	local coolDownInfo = {}
	for i = 0, result:GetRowNum()-1 do
		local name = result:GetData(0,0)
		local spaceTime = result:GetData(i, 1)
		if g_ItemUseInfoMgr:GetItemUseInfo(name) and tonumber(g_ItemUseInfoMgr:GetItemUseInfo(name,"CoolDownTime")) and tonumber(g_ItemUseInfoMgr:GetItemUseInfo(name,"CoolDownTime")) > spaceTime * 1000 then
			coolDownInfo[name] = spaceTime
		else
			StmtOperater.DelItemCoolDownInfo:ExecStat(charId, name)
		end
	end
	return coolDownInfo
end

--删除物品预处理, 优先消耗传入pos上的物品（右键使用的位置）
--不够的数量从其他格上消耗，把准备删除的物品位置和ID存入表中返回
--@return 删除的物品所在位置和物品Id表 ItemPosTbl,ItemIdTbl
local function GetConsumeItemByPos(nCharID, nBigID, nIndex, nCount, nRoom, nPos, ConsumeItemTbl)
	if not g_ItemInfoMgr:CheckType( nBigID,nIndex ) then
		return 
	end
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	local function AddItemPosTbl(Room, Pos, Count)
		local tbl = {}
		local ItemIdResult = g_RoomMgr.SelectAllItemIDByPos(nCharID,Room,Pos)
		Count = Count or ItemIdResult:GetRowNum()
		tbl["nRoomIndex"] = Room
		tbl["nPos"] = Pos
		tbl["nCount"] = Count
		for i=1,Count do
			table.insert(tbl, ItemIdResult(i,1))
		end
		table.insert(ConsumeItemTbl, tbl)
	end
	
	--pos上的物品优先消耗
	local PosCount = g_RoomMgr.GetCountByPosition(nCharID,nRoom,nPos)
	if PosCount >= nCount then
		AddItemPosTbl(nRoom, nPos, nCount)
		return ConsumeItemTbl
	else
		AddItemPosTbl(nRoom, nPos, PosCount)
		local FoldLimit = g_ItemInfoMgr:GetItemInfo( nBigID,nIndex,"FoldLimit" ) or 1
		
		local nSum = nCount - PosCount
		local RoomPosCount = g_RoomMgr.GetPosAndCountByType(nCharID,nBigID,nIndex,g_StoreRoomIndex.PlayerBag,FoldLimit+1)
		
		for n=1, #RoomPosCount do
			local GetRoom,GetPos,GetFoldCount = RoomPosCount[n][1],RoomPosCount[n][2],RoomPosCount[n][3]
			if GetRoom ~= nRoom or (GetRoom == nRoom and GetPos ~= nPos) then
				if nSum <= 0 then
					break
				elseif nSum - GetFoldCount >= 0 then
					nSum = nSum - GetFoldCount
					AddItemPosTbl(GetRoom, GetPos, GetFoldCount)
				else
					AddItemPosTbl(GetRoom, GetPos, nSum)
					GetFoldCount = nSum
					nSum = 0
				end
			end
		end
	end
	--返回被删除的物品信息
	return ConsumeItemTbl
end

local function GetConsumeItemById(nCharID, nBigID, nIndex, nCount, ConsumeItemTbl)
	
	if not g_ItemInfoMgr:CheckType( nBigID,nIndex ) then
		return 
	end
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	local function AddItemPosTbl(Room, Pos, Count)
		local tbl = {}
		local ItemIdResult = g_RoomMgr.SelectAllItemIDByPos(nCharID,Room,Pos)
		Count = Count or ItemIdResult:GetRowNum()
		tbl["nRoomIndex"] = Room
		tbl["nPos"] = Pos
		tbl["nCount"] = Count
		for i=1,Count do
			table.insert(tbl, ItemIdResult(i,1))
		end
		table.insert(ConsumeItemTbl, tbl)
	end
	
	local FoldLimit = g_ItemInfoMgr:GetItemInfo( nBigID,nIndex,"FoldLimit" ) or 1
	local RoomPosCount = g_RoomMgr.GetPosAndCountByType(nCharID,nBigID,nIndex,g_StoreRoomIndex.PlayerBag,FoldLimit+1)

	local nSum = nCount
	
	for n=1, #RoomPosCount do
		local GetRoom,GetPos,GetFoldCount = RoomPosCount[n][1],RoomPosCount[n][2],RoomPosCount[n][3]
		if nSum <= 0 then
			break
		elseif nSum - GetFoldCount >= 0 then
			nSum = nSum - GetFoldCount
			AddItemPosTbl(GetRoom, GetPos, GetFoldCount)
		else
			AddItemPosTbl(GetRoom, GetPos, nSum)
			GetFoldCount = nSum
			nSum = 0
		end
	end
	
	return ConsumeItemTbl
end

--返回物品数量列表
function UseSpecialItemDB.GetItemListNum(data)
	local CharId = data["nCharId"]
	local RoomIndex = data["RoomIndex"]
	local Pos = data["Pos"]
	local result = {}
	local ConsumeItemTbl = {}
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	for i,v in ipairs(data) do
		local num = g_RoomMgr.GetItemNum(v)
		if v["ItemNum"] > num then
			return false
		end
		if v["ItemName"] == data["ItemName"] then
			ConsumeItemTbl = GetConsumeItemByPos(v["nCharId"], v["ItemType"], v["ItemName"], v["ItemNum"], RoomIndex, Pos, ConsumeItemTbl)
		else
			ConsumeItemTbl = GetConsumeItemById(v["nCharId"], v["ItemType"], v["ItemName"], v["ItemNum"], ConsumeItemTbl)
		end
	end
	return ConsumeItemTbl
end

--检测ItemId对应的物品是否在相应的格子上
function UseSpecialItemDB.CheckItemType(data)
	local ConsumeItemTbl,AddItemsTbl
	local PlayerId = data["PlayerId"]
	local ConsumeTbl = data["ConsumeTbl"]
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	ConsumeItemTbl = {}
	for i,v in pairs(ConsumeTbl) do
		local result = {}
		for j = 1, #v do
			local PosInfo = g_RoomMgr.GetRoomIndexAndPosByItemId(PlayerId,v[j])
			if PosInfo then
				if PosInfo(1,1) ~= v.nRoomIndex or PosInfo(1,2) ~= v.nPos then
					return false,"物品移动过"
				end
			else
				return false,"物品已经不存在"
			end
		end
	end
	return true
end

SetDbLocalFuncType(UseSpecialItemDB.GetItemListNum)

return UseSpecialItemDB
