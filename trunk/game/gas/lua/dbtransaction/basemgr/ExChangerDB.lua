local StmtContainer = class()

local StmtDef = {
		"Get_CharId_by_CharName",
		[[	select cs_uId from tbl_char where c_sName = ?
		]]
}
DefineSql(StmtDef, StmtContainer)

local StmtDef = {
		"Get_CharName_by_CharID",
		[[	select c_sName from tbl_char where cs_uId = ? 
		]]
}
DefineSql( StmtDef, StmtContainer )

local StmtDef = 
{
		"Get_CharNameAndLevel_by_CharID",
		[[
			select c_sName, cb_uLevel
			from tbl_char_basic, tbl_char
			where tbl_char_basic.cs_uId = tbl_char.cs_uId
			and tbl_char.cs_uId = ?
		]]
}
DefineSql ( StmtDef, StmtContainer )

local StmtDef = 
{
		"_GetUserIDByCharId",
		[[
			select us_uId from tbl_char_static where cs_uId = ?
		]]
}
DefineSql ( StmtDef, StmtContainer )
local StmtDef = 
{
		"_GetPlayerIp",
		[[
			select uo.uo_sIp
			from tbl_char_static as cs, tbl_user_online as uo
			where uo.us_uId = cs.us_uId
			and cs.cs_uId = ?
		]]
}
DefineSql ( StmtDef, StmtContainer )
local Exchanger = CreateDbBox(...)

function Exchanger.getPlayerIp(char_id)
	local query_result = StmtContainer._GetPlayerIp:ExecStat(char_id)
	if query_result:GetRowNum() == 0 then 
		return "127.0.0.1"
	end
	return query_result(1,1)
end
function Exchanger.getPlayerIdByName(name)
	local query_result = StmtContainer.Get_CharId_by_CharName:ExecSql('n', name)
	if (0 == query_result:GetRowNum()) then
		--print("cs_UId is " .. query_result[1][1])
		query_result:Release()
		return 0
	end
	--print("cs_UId is " .. query_result[1][1])  --to be remove
	local id = query_result:GetNumber(0,0)
	query_result:Release()

	return id
end


function Exchanger.getPlayerNameById(char_id)
	local query_result = StmtContainer.Get_CharName_by_CharID:ExecSql('s[18]', char_id )
	if (0 == query_result:GetRowNum()) then
		query_result:Release()
		return ""
	end
	local name = query_result:GetString(0,0)
	query_result:Release()
	return name
end

function Exchanger.getPlayerNameAndLevelById(char_id)
	local query_result = StmtContainer.Get_CharNameAndLevel_by_CharID:ExecStat(char_id)
	if 0 == query_result:GetRowNum() then
		return false
	end
	
	local sName, nLevel = query_result:GetString(0, 0), query_result:GetNumber(0, 1)
	return true, sName, nLevel
end

function Exchanger.GetUserIDByCharId(char_id)
	local query_result = StmtContainer._GetUserIDByCharId:ExecStat(char_id)
	if 0 == query_result:GetRowNum() then
		return
	end
	return query_result(1,1)
end

return Exchanger