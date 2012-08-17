cfg_load "gm/SysSoulPearl_Common"
gac_gas_require "item/item_info_mgr/ItemInfoMgr"


local LogErr = LogErr

local buchang_event = event_type_tbl["gm补偿"]
local SysSoulPearl_Common = SysSoulPearl_Common
local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
local g_ItemInfoMgr = g_ItemInfoMgr or CItemInfoMgr:new()
local StmtOperater = {}		--用来挂sql语句的table
local BuChangSql = CreateDbBox(...)

local StmtDef =
{
	"GetItemRange",
	"select ifnull(min(it_id), 0), ifnull(max(it_id),0) from tbl_item_temp"
}
DefineSql(StmtDef,StmtOperater)


local StmtDef =
{
	"GetItemInfoSpecifyRange",
	"select cs_uId, it_uType, it_sName from tbl_item_temp where it_id >= ? and it_id <= ?"
}
DefineSql(StmtDef,StmtOperater)



local StmtDef =
{
	"DeleteItemInfoSpecifyRange",
	"delete from tbl_item_temp where it_id >= ? and it_id <= ?"
}
DefineSql(StmtDef,StmtOperater)




function BuChangSql.GetBuChangRange()
	local info = {}
	local itemRet = StmtOperater.GetItemRange:ExecStat()
	info.item = {}
	info.item["MinId"] = itemRet:GetData(0,0)
	info.item["MaxId"] = itemRet:GetData(0,1)
	return info
end

function BuChangSql.DoItem(data)
	local MinId = data["MinId"]
	local MaxId = data["MaxId"]
	local ret = StmtOperater.GetItemInfoSpecifyRange:ExecStat(MinId, MaxId)
	local ItemBuffer = {}
	local lastCharId = nil
	local count = 0
	local sysMailExecutor = RequireDbBox("SysMailDB")
	for i = 0, ret:GetRowNum() -1 do
		local charId = ret:GetData(i, 0)
		local itemType = ret:GetData(i, 1)
		local itemName = ret:GetData(i, 2)
		if charId ~= lastCharId then
			if next(ItemBuffer) then
				sysMailExecutor.SendSysMail("1014", lastCharId, "1014", "", ItemBuffer, 0, buchang_event)
				count = count + #ItemBuffer
				ItemBuffer = {}
			end
			lastCharId = charId
		end
		local itemId = BuChangSql.CreateItem(charId, itemType, itemName)
		if not itemId then
			assert(false, "物品创建失败 " .. charId .. " " .. itemType .. " " ..  itemName )
		end
		table.insert(ItemBuffer,{itemId})
	end
	if next(ItemBuffer) then
		sysMailExecutor.SendSysMail("1014", lastCharId, "1014", "", ItemBuffer, 0, buchang_event)
		count = count + #ItemBuffer
		ItemBuffer = {}
	end
	assert(count == ret:GetRowNum(), "添加物品数不对!!!")
	StmtOperater.DeleteItemInfoSpecifyRange:ExecStat(MinId, MaxId)
	
	return data["Index"]
end



function BuChangSql.CreateItem(charId, itemType, itemName)
	local params = {}
	params.m_nCharID = charId
	params.m_nType = itemType
	params.m_nBindingType = g_ItemInfoMgr:GetItemInfo(itemType,itemName,"BindingStyle") or 0
	params.m_sCreateType = buchang_event
	if itemType == 19 and SysSoulPearl_Common(itemName) then
		params.m_sName = itemName .. ":" .. SysSoulPearl_Common(itemName, "Value")
	else
		params.m_sName = itemName
	end
	return g_RoomMgr.CreateItem(params)
end



-------------------------------------------------------------
local StmtDef =
{
	"GetCharAndItemID",
	"select cs_uId,is_uId from tbl_char_mail_item_temp where cmit_uId >= ? and cmit_uId <= ?"
}
DefineSql(StmtDef,StmtOperater)

local StmtDef =
{
	"DeleteItemIDByRange",
	"delete from tbl_char_mail_item_temp where cmit_uId >= ? and cmit_uId <= ?"
}
DefineSql(StmtDef,StmtOperater)
local StmtDef =
{
	"GetItemIDRange",
	"select ifnull(min(cmit_uId), 0), ifnull(max(cmit_uId),0) from tbl_char_mail_item_temp"
}
DefineSql(StmtDef,StmtOperater)

function BuChangSql.DoItemByID(data)
	local MinId = data["MinId"]
	local MaxId = data["MaxId"]
	local ret = StmtOperater.GetCharAndItemID:ExecStat(MinId, MaxId)
	local ItemBuffer = {}
	local lastCharId = nil
	local count = 0
	local sysMailExecutor = RequireDbBox("SysMailDB")
	for i = 0, ret:GetRowNum() -1 do
		local charId = ret:GetData(i, 0)
		local itemId = ret:GetData(i, 1)
		if charId ~= lastCharId then
			if next(ItemBuffer) then
				sysMailExecutor.SendSysMail("1014", lastCharId, "1014", "", ItemBuffer, 0, buchang_event)
				count = count + #ItemBuffer
				ItemBuffer = {}
			end
			lastCharId = charId
		end
		table.insert(ItemBuffer,{itemId})
	end
	if next(ItemBuffer) then
		sysMailExecutor.SendSysMail("1014", lastCharId, "1014", "", ItemBuffer, 0, buchang_event)
		count = count + #ItemBuffer
		ItemBuffer = {}
	end
	assert(count == ret:GetRowNum(), "添加物品数不对!!!")
	StmtOperater.DeleteItemIDByRange:ExecStat(MinId, MaxId)
	
	return data["Index"]
end

function BuChangSql.GetBuChangItemIDRange()
	local info = {}
	local itemRet = StmtOperater.GetItemIDRange:ExecStat()
	info.item = {}
	info.item["MinId"] = itemRet:GetData(0,0)
	info.item["MaxId"] = itemRet:GetData(0,1)
	return info
end

return BuChangSql
