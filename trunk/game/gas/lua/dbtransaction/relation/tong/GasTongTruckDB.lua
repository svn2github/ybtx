local StmtOperater = {}	

local CTongTruckBox = CreateDbBox(...)
-----------------------------------------------------------------------
local StmtDef = {
    	"_SaveTruckInfo",
    	[[ 
    	  insert ignore into tbl_truck_tong (cs_uId,tt_sName,tt_uResNum)values(?,?,?)
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_QueryTruckInfo",
    	[[ 
    	  select tt_sName,tt_uResNum from	tbl_truck_tong where cs_uId = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_DelTruckInfo",
    	[[ 
    	  delete from	tbl_truck_tong where cs_uId = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

function CTongTruckBox.SaveTruckInfo(data)
	local CharID = data["char_id"] 
	local TruckName = data["TruckName"]
	local Resource = data["Resource"] 
	local result = StmtOperater._QueryTruckInfo:ExecStat(CharID)
	if result:GetRowNum() > 0 then
		StmtOperater._DelTruckInfo:ExecStat(CharID)
	end
	StmtOperater._SaveTruckInfo:ExecStat(CharID,TruckName,Resource)
	return g_DbChannelMgr:LastAffectedRowNum() > 0
end

function CTongTruckBox.GetTruckInfo(data)
	local CharID = data["char_id"]
	local TruckName, Resource
	local result = StmtOperater._QueryTruckInfo:ExecStat(CharID)
	if result:GetRowNum() > 0 then
		TruckName = result:GetData(0,0)
		Resource = result:GetData(0,1)
	end
	StmtOperater._DelTruckInfo:ExecStat(CharID)
	return TruckName, Resource
end

return CTongTruckBox