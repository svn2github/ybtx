
local ItemBagLockDB = CreateDbBox(...)

local StmtOperater = {}	

local StmtDef = {
	"_IdentityAffirmSql",
	[[
		select count(*) from tbl_char_static cs,tbl_user_static us where cs.us_uId = us.us_uId and cs_uId =  ? and us_sPassword = ?
	]]
}
DefineSql( StmtDef, StmtOperater )


function ItemBagLockDB.IdentityAffirm(data)
	local charId = data["charId"] 
	local sPassword = data["sPassword"]
	
	local res = StmtOperater._IdentityAffirmSql:ExecStat(charId,sPassword)
	return res:GetData(0,0) > 0
end
---------------------------------------------------------

local StmtDef = {
	"_LockItemBagByTime",
	[[
		replace into tbl_item_store_room_lock values(?,?,now())
	]]
}
DefineSql (StmtDef,StmtOperater)

function ItemBagLockDB.LockItemBagByTime(data)
	local charId = data["charId"]
	local lock_time = data["lock_time"]
	
	StmtOperater._LockItemBagByTime:ExecStat(charId,lock_time)
	return g_DbChannelMgr:LastAffectedRowNum() == 1 
end
---------------------------------------------------------------- 
local StmtDef = {
	"_GetLockTime",
	[[
		select isrl_uTime*60 from tbl_item_store_room_lock where cs_uId = ?;
	]]
}
DefineSql (StmtDef,StmtOperater)

local StmtDef = {
	"_GetPassTime",
	[[
		select unix_timestamp(now()) - unix_timestamp(isrl_dtLockTime) from tbl_item_store_room_lock where cs_uId = ?;
	]]
}
DefineSql (StmtDef,StmtOperater)

local StmtDef = {
	"_DelItemBagLock",
	[[
		delete from tbl_item_store_room_lock where cs_uId = ?;
	]]
}
DefineSql (StmtDef,StmtOperater)

function ItemBagLockDB.JudgeItemBagLock(data)
	local charId = data["char_id"]
	local res = StmtOperater._GetLockTime:ExecStat(charId)
	
	local left_time = 0
	local num = res:GetRowNum()
	if num == 1 then
		local res2 =  StmtOperater._GetPassTime:ExecStat(charId)
		left_time = res:GetData(0,0) - res2:GetData(0,0)
		if left_time <= 59 then
			left_time = 0
			StmtOperater._DelItemBagLock:ExecStat(charId)
		end
	end

	return {num == 1,left_time}
end
---------------------------------------------------------------
local StmtDef = {
	"_HaveItemBagLock",
	[[
		select count(*) from tbl_item_store_room_lock where cs_uId = ?;
	]]
}
DefineSql (StmtDef,StmtOperater)

function ItemBagLockDB.HaveItemBagLock(charId)
	local res = StmtOperater._HaveItemBagLock:ExecStat(charId)
	return res:GetData(0,0) == 1
end
---------------------------------------------------------------
SetDbLocalFuncType(ItemBagLockDB.IdentityAffirm)

return ItemBagLockDB