
local StmtOperater = {}	
local event_type_tbl = event_type_tbl

--闭包开始
local TongNeedFireActivityDB = CreateDbBox(...)

------------------------------------
local StmtDef = {
    	"_GetTongNeedFireInfo",
    	[[ 
    		select count(*) from tbl_tong_needfire where t_uId = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_SaveTongNeedFireInfo",
    	[[ 
    		insert into tbl_tong_needfire values(?,?) 
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

--@brief 开启篝火
function TongNeedFireActivityDB.OpenTongNeedFireActivityDB(data)
	local uCharId = data["uCharId"]
	local uServerId = data["uServerId"]
	local tong_box = RequireDbBox("GasTongBasicDB")
	local uTongId = tong_box.GetTongID(uCharId)
	if 0 == uTongId then
		return false,351000
	end
	local Count = StmtOperater._GetTongNeedFireInfo:ExecStat(uTongId)
	if Count:GetData(0,0) > 0 then
		return false,351002
	else
		local online_member = StmtOperater._GetOnlineTongMember:ExecStat(uTongId)
		StmtOperater._SaveTongNeedFireInfo:ExecStat(uTongId,uServerId)
		return g_DbChannelMgr:LastAffectedRowNum() > 0,online_member
	end 
end
------------------------------------------------------
--@brief 检查篝火是否开启
function TongNeedFireActivityDB.CheckNeedFireIsOpenDB(data)
	local uCharId = data["uCharId"]
	local tong_box = RequireDbBox("GasTongBasicDB")
	local uTongId = tong_box.GetTongID(uCharId)
	if 0 == uTongId then
		return false,351000
	end
	local Count = StmtOperater._GetTongNeedFireInfo:ExecStat(uTongId)
	local res = StmtOperater._GetCharFirewoodInfo:ExecStat(uCharId)
	local uLeftTime = 0
	if res:GetRowNum() > 0 then
		if res:GetData(0,0) < 180 then
			uLeftTime = 180 - res:GetData(0,0)
		end
	end
	return Count:GetData(0,0) > 0,uLeftTime
end
-------------------------------------------------------
local StmtDef = {
    	"_SaveCharFirewoodInfo",
    	[[ 
    		insert into tbl_char_firewood values(?,now(),?) 
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_GetCharFirewoodInfo",
    	[[ 
    		select now() - cf_dtDateTime,cf_uAddTimes from tbl_char_firewood where cs_uId = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )


local StmtDef = {
    	"_UpdateCharFirewoodInfo",
    	[[ 
    		update tbl_char_firewood set cf_dtDateTime = now(),cf_uAddTimes = cf_uAddTimes + 1 where cs_uId = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_GetOnlineTongMember",
    	[[ 
    		select mt.cs_uId from tbl_member_tong mt,tbl_char_online co where mt.cs_uId = co.cs_uId and mt.t_uId = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )	
	
--@brief 添加木柴
function TongNeedFireActivityDB.AddNeedFireItemDB(data)
	local uCharId = data["uCharId"]
	local uRoomIndex = data["uRoomIndex"]
	local uPos = data["uPos"]
	local sItemName = data["sItemName"]
	local uFirewoodExp = data["uFirewoodExp"]
	
	local res = StmtOperater._GetCharFirewoodInfo:ExecStat(uCharId)
	if res:GetRowNum() > 0 then
		if res:GetData(0,0) - 180 < 0 then
			return false
		elseif res:GetData(0,1) >= 5 then
			return false,351008
		else
			StmtOperater._UpdateCharFirewoodInfo:ExecStat(uCharId)
			if g_DbChannelMgr:LastAffectedRowNum() == 0	then
				return false
			end
		end
	else
		StmtOperater._SaveCharFirewoodInfo:ExecStat(uCharId,1)
		if g_DbChannelMgr:LastAffectedRowNum() == 0	then
			return false
		end
	end
	
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	local sItemType, sItemName1, uItemId = g_RoomMgr.GetOneItemByPosition(uCharId,uRoomIndex,uPos)
	if sItemName ~= sItemName1 then
		return false
	end

	local DelRet = g_RoomMgr.DelItemByPos(uCharId,uRoomIndex,uPos,1,event_type_tbl["篝火活动"])
	if IsNumber(DelRet) then
		CancelTran()
		return false
	end
	
	local tong_box = RequireDbBox("GasTongBasicDB")
	local uTongId = tong_box.GetTongID(uCharId)
	local online_member = StmtOperater._GetOnlineTongMember:ExecStat(uTongId)
	return true,online_member
end
-------------------------------------------------------
function TongNeedFireActivityDB.TongMemberAddExp(data)
	local memberLevelTbl = {}
	for i = 1,#data["memberAddExpTbl"] do
		local uCharId = data["memberAddExpTbl"][i] 
		local param = {}
		param["char_id"] = uCharId
		param["addExp"] = data["addExp"]
		param["addExpType"] = event_type_tbl["篝火活动"]
		local RoleQuestDB = RequireDbBox("RoleQuestDB")
		local CurLevel,LevelExp = RoleQuestDB.AddExp(param)
		if CurLevel then
			local AddExpTbl = {}
			AddExpTbl["Level"] = CurLevel
			AddExpTbl["Exp"] = LevelExp
			AddExpTbl["AddExp"] = data["addExp"]
			AddExpTbl["uInspirationExp"] = 0
			memberLevelTbl[uCharId] = AddExpTbl
		end
	end
	return memberLevelTbl
end
------------------------------------------------------
local StmtDef = {
    	"_DelTongNeedFireInfo",
    	[[ 
    		delete from tbl_tong_needfire
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_GetCharNeedFireInfo",
    	[[ 
    		select 
    			mt.cs_uId 
    		from 
    			tbl_tong_needfire tn,tbl_member_tong mt,tbl_char_online co 
    		where 
    			tn.t_uId = mt.t_uId and mt.cs_uId = co.cs_uId 
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_DelCharNeedFireInfo",
    	[[ 
    		delete from tbl_char_firewood
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

--@brief 活动结束后清空佣兵小队篝火活动相关信息表
function TongNeedFireActivityDB.CleanTongNeedFire(data)
	local res = StmtOperater._GetCharNeedFireInfo:ExecStat()
	StmtOperater._DelTongNeedFireInfo:ExecStat()
	StmtOperater._DelCharNeedFireInfo:ExecStat()
	Db2GasCall("CleanTongNeedFireTick", {})
	return res
end

return TongNeedFireActivityDB
