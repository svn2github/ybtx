

local StmtOperater = {}		--用来挂sql语句的table
--闭包开始
local TeamMarkBox = CreateDbBox(...)

--------------------------------------SQL相关--------------------------------------------

--【获得某小队的所有标记信息】
local StmtDef = {
    	"_GetAllTeamMarkInfoByTeamID",
    	[[ 
    		select 	mt_uMarkType, mt_uTargetType, mt_uTargetID 	from 	tbl_mark_team 	where 	t_uId = ? 
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

--【统计同一个小队的同一标记使用数量】
--条件：小队id、标记类型
local StmtDef = {
    	"_CountByMarkType",
    	[[ 
    		select count(*) from tbl_mark_team where t_uId = ? and mt_uMarkType = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

--【根据小队id和标记的类型修改标记信息】
local StmtDef = {
    	"_UpdateMarkInfoByMarkType",
    	[[ 
    		update 	tbl_mark_team 
    		set 		mt_uTargetType = ?, mt_uTargetID = ?
    		where 	t_uId = ? and mt_uMarkType = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

--【统计同一个小队给某对象标记的次数】
local StmtDef = {
    	"_CountByTarget",
    	[[ 
    		select count(*) from tbl_mark_team where t_uId = ? and mt_uTargetType = ? and mt_uTargetID = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

--【根据小队id和标记的类型修改标记信息】
local StmtDef = {
    	"_UpdateMarkInfoByTargetInfo",
    	[[ 
    		update 	tbl_mark_team 
    		set 		mt_uMarkType = ?
    		where 	t_uId = ? and  mt_uTargetType = ? and mt_uTargetID = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

--【添加标记信息】
local StmtDef = {
    	"_AddMarkInfo",
    	[[ 
    		replace into  tbl_mark_team(t_uId, mt_uMarkType, mt_uTargetType, mt_uTargetID) values(?, ?, ?, ?)
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

--【删除标记信息】
local StmtDef = {
    	"_DelMarkInfoByChar",
    	[[ 
    		delete from tbl_mark_team where mt_uTargetID = ? and mt_uTargetType = ? and t_uId = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )
--------------------------------------数据库操作相关----------------------------------------------------

--【获得某小队的所有标记信息】
function TeamMarkBox.GetAllMarkInfo(uTeamID)
	local query_list = StmtOperater._GetAllTeamMarkInfoByTeamID:ExecStat(uTeamID)
	
	return query_list
end

--【添加标记信息】
function TeamMarkBox.UpdateMarkInfo(uTeamID, uMarkType, uTargetType, uTargetID)
	if 0 == uMarkType then
		StmtOperater._DelMarkInfoByChar:ExecStat(uTargetID,uTargetType,uTeamID)
		return true
	end
	local tblMarkCount = StmtOperater._CountByMarkType:ExecStat(uTeamID, uMarkType)
	local tblTargetCount = StmtOperater._CountByTarget:ExecStat(uTeamID, uTargetType, uTargetID)
	
	if tblMarkCount:GetNumber(0,0) > 0 then
		--说明该小队的此标记已经被用
		if tblTargetCount:GetNumber(0,0) == 0 then
			--说明此小队还没有在该对象上做过标记
			StmtOperater._UpdateMarkInfoByMarkType:ExecStat(uTargetType, uTargetID, uTeamID, uMarkType)
		end
	elseif tblTargetCount:GetNumber(0,0) > 0 then
		--说明此小队已经给此对象做了标记，直接修改此对象的标记类型
		StmtOperater._UpdateMarkInfoByTargetInfo:ExecStat(uMarkType, uTeamID, uTargetType, uTargetID)
	else
		--此小队没有用过此标记且此小队没有在该对象上标记过时，直接add标记信息
		StmtOperater._AddMarkInfo:ExecStat( uTeamID, uMarkType, uTargetType, uTargetID)
	end
	return true
end

--------------------------------RPC相关------------------------------------------------------

--【小队标记】
function TeamMarkBox.UpdateTeamMark(parameters)
	local uPlayerID = parameters.uPlayerID
	local uMarkType = parameters.uMarkType
	local uTargetType = parameters.uTargetType
	local uTargetID = parameters.uTargetID
	
	local team_box = RequireDbBox("GasTeamDB")
	local uTeamID = team_box.GetTeamID(uPlayerID)
	
	if team_box.GetCaptain(uTeamID) ~= uPlayerID then
		--说明不是队长 ，没有权限
		return 
	end
	
	--添加标记信息
	if not TeamMarkBox.UpdateMarkInfo(uTeamID, uMarkType, uTargetType, uTargetID) then
		return 
	end
	--获得所有小队成员
	local members = team_box.GetTeamMembers(uTeamID)
	
	return members
	
end

--返回闭包
return TeamMarkBox

