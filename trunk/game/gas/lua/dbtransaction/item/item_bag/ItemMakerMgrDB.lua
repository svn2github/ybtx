
local StmtOperater = {}	
local ItemMakerDB = CreateDbBox(...)

local StmtDef=
{
    "_GetItemMakerByChar",
    [[
    	select 
    		c.c_sName,im.is_uId
    	from 
    		tbl_char as c,tbl_item_maker as im,tbl_item_in_grid as isr,tbl_grid_in_room as igp
    		where c.cs_uId = im.cs_uId
    		and im.is_uId = isr.is_uId
    		and igp.gir_uGridID = isr.gir_uGridID
    		and igp.cs_uId = ?
    union
    	select 
    		c.c_sName,im.is_uId
    	from tbl_char as c,tbl_item_maker as im,tbl_item_equip as ie
    	where c.cs_uId = im.cs_uId
    		and im.is_uId = ie.is_uId
    		and ie.cs_uId = ?
    ]]
}
DefineSql(StmtDef,StmtOperater)
function ItemMakerDB.GetItemMakerByChar(nCharID)
	local query_result = StmtOperater._GetItemMakerByChar:ExecStat(nCharID,nCharID)
	return query_result
end

function ItemMakerDB.SendItemMakerByTblIDs(nCharID,item_ids,traget_id)
	local query_string = "select m.is_uId,c.c_sName from tbl_item_maker as m,tbl_char as c where c.cs_uId = m.cs_uId and is_uId in ("
	local item_list = ""
	for i =1, #item_ids-1 do
		item_list = item_list .. item_ids[i] .. ", "
	end
	if #item_ids > 0 then
		item_list = item_list .. item_ids[#item_ids] .. ")"
	end
	query_string = query_string .. item_list
	local _, query_result = g_DbChannelMgr:TextExecute(query_string)
	Db2CallBack("RetItemMakerName",nCharID,query_result)
	if traget_id then
		Db2CallBack("RetItemMakerName",traget_id,query_result)
	end
end

local StmtDef = {
	"_AddItemMaker",
	[[
		replace into tbl_item_maker(is_uId,cs_uId) 
		values(?,?)
	]]
}
DefineSql( StmtDef, StmtOperater )
function ItemMakerDB.AddItemMaker(uPlayerID,ItemID)
	StmtOperater._AddItemMaker:ExecStat(ItemID,uPlayerID)
	return g_DbChannelMgr:LastAffectedRowNum() > 0 
end

return ItemMakerDB