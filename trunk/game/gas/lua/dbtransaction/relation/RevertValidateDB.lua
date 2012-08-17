local StmtOperater = {}	
local RevertValidateBox = CreateDbBox(...)
local StmtDef = {
    	"_CountValidateInfo",
    	[[ 
    		select count(*) from tbl_revert_validate 
    		where cs_uInviter = ? and cs_uInvitee = ? and rv_uFun = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_AddValidateInfo",
    	[[ replace into tbl_revert_validate(cs_uInviter, cs_uInvitee,rv_uFun ) values(?,?,?) ]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_DelValidateInfo",
    	[[ 
    		delete from  tbl_revert_validate 
    		where cs_uInviter = ? and cs_uInvitee = ? and rv_uFun = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )
local StmtDef = {
    	"_DelAllValidateInfoByCharID",
    	[[ 
    		delete from  tbl_revert_validate 
    		where cs_uInvitee = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_GetValidateInfoByInviter",
    	[[ 
    		select cs_uInviter from  tbl_revert_validate 
    		where cs_uInvitee = ? and rv_uFun = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )
function RevertValidateBox.GetValidateInfoByInviter(InviteeID,uFun)
 	local result = StmtOperater._GetValidateInfoByInviter:ExecStat(InviteeID,uFun)
  
  return result
end

function RevertValidateBox.AddValidateInfo(InviterID,InviteeID,uFun)
	local ex = RequireDbBox("Exchanger")
  if not (ex.getPlayerNameById(InviterID) ~= "" and ex.getPlayerNameById(InviteeID) ~= "") then
  	CancelTran()
  	return 
  end
 	StmtOperater._AddValidateInfo:ExecStat(InviterID,InviteeID,uFun)
  if not (g_DbChannelMgr:LastAffectedRowNum()>0)  then
  	CancelTran()
  	return 
  end
  return true
end

function RevertValidateBox.DelValidateInfo(InviterID,InviteeID,uFun)
 	StmtOperater._DelValidateInfo:ExecStat(InviterID,InviteeID,uFun)
  if not (g_DbChannelMgr:LastAffectedRowNum()>0)  then
  	CancelTran()
  	return false  
  end
  return true
end

function RevertValidateBox.DelAllValidateInfoByCharID(InviteeID)
 	StmtOperater._DelAllValidateInfoByCharID:ExecStat(InviteeID)
end

function RevertValidateBox.HaveValidateInfo(InviterID,InviteeID,uFun)
 	local tblCount = StmtOperater._CountValidateInfo:ExecStat(InviterID,InviteeID,uFun)
  return tblCount:GetData(0,0) > 0
end

return RevertValidateBox