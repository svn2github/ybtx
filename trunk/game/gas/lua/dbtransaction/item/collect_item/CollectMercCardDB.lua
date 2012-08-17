gac_gas_require "item/item_info_mgr/ItemInfoMgr"
local StmtContainer = class()
local event_type_tbl = event_type_tbl

local CollectMercCardDB = CreateDbBox(...)

--========================================================================
local StmtDef = {
			"_QueryAllMercCard",
			--查询角色全部修行塔卡牌
			[[ 
				select mec_uCardId from tbl_merc_educate_card where cs_uId = ?
			]]
}
DefineSql (StmtDef,StmtContainer)

--查询角色全部修行塔卡牌
function CollectMercCardDB.QueryAllMercCard(nCharID)
	CollectMercCardDB.InitMercCard(nCharID)
	local res = StmtContainer._QueryAllMercCard:ExecStat(nCharID)
	local ResTbl = {}
	if res then
		local row = res:GetRowNum()
		for i = 1, row do
			ResTbl[res:GetData(i-1,0)] = true
		end
		res:Release()
		return ResTbl
	end
end
--========================================================================
local StmtDef = {
			"_QueryMercCard",
			[[ 
				select count(*) from tbl_merc_educate_card where cs_uId = ? and mec_uCardId = ?
			]]
}
DefineSql (StmtDef,StmtContainer)

function CollectMercCardDB.QueryMercCard(nCharID, CardId)
	local res = StmtContainer._QueryMercCard:ExecStat(nCharID, CardId)
	return res:GetData(0,0) > 0
end
--========================================================================
local StmtDef = {
			"_insertMercCard",
			[[ 
				insert into tbl_merc_educate_card(cs_uId,mec_uCardId) values(?,?)
			]]
}
DefineSql (StmtDef,StmtContainer)

function CollectMercCardDB.InsertMercCard(nCharID, CardId)
	if CollectMercCardDB.QueryMercCard(nCharID, CardId) then
		return false
	end
	StmtContainer._insertMercCard:ExecStat(nCharID,CardId)
	return true
end

function CollectMercCardDB.DelItem(data)
	local result = {}
	local PlayerId = data["PlayerId"]
	local ItemRoomIndex = data["RoomIndex"]
	local ItemPos = data["Pos"]
	local ItemNum = data["ItemNum"]
	local CardId = data["CardId"]
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	local DelRet = g_RoomMgr.DelItemByPos(PlayerId,ItemRoomIndex,ItemPos,ItemNum,event_type_tbl["物品使用"])
	if IsNumber(DelRet) then
		return 1
	end
	DelRet.m_nRoomIndex = ItemRoomIndex
	DelRet.m_nPos = ItemPos
	
	if not CollectMercCardDB.InsertMercCard(PlayerId, CardId) then
		CancelTran()
		return 2
	end
	table.insert(result,DelRet)
	return result, CardId
end

function CollectMercCardDB.InitMercCard(nCharID)
	if CollectMercCardDB.QueryMercCard(nCharID, 1) then
		return
	end
	for i = 1, 30 do
		StmtContainer._insertMercCard:ExecStat(nCharID,i)
	end
end

SetDbLocalFuncType(CollectMercCardDB.DelItem)

return CollectMercCardDB
