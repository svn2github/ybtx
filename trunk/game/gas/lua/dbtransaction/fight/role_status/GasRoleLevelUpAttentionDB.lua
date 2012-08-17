
local CGasRoleLevelUpSql = class()
local os = os
local MaxLevel			= 150

local CGasRoleLevelUpDB = CreateDbBox(...)
--------------------------------------------------------
local StmtDef =
{
	"_GetPlayerLevelUpTime",
	"select unix_timestamp(clt_uLevelUpTime) from tbl_char_levelup_time where cs_uId = ? and clt_uLevel = ?"
}
DefineSql(StmtDef,CGasRoleLevelUpSql)

--@brief 获得角色升级信息
function CGasRoleLevelUpDB.GetPlayerLevelUpTime(CharID, LevelNum)
	local result = CGasRoleLevelUpSql._GetPlayerLevelUpTime:ExecStat(CharID, LevelNum)
	if(result:GetRowNum() == 0) then
		result:Release()
		return nil
	end
	
	local data = result:GetData(0,0)
	result:Release()
	return data
end
----------------------------------------------------------
local StmtDef =
{
	"_SavePlayerLevelUpTime",
	"insert into tbl_char_levelup_time(cs_uId, clt_uLevelUpTime, clt_uLevel) values(?,now(),?)"
}
DefineSql(StmtDef,CGasRoleLevelUpSql)

--@brief 记录玩家升级信息
function CGasRoleLevelUpDB.SavePlayerLevelUpTime(data)
	local CharID = data.CharID
	local LevelNum = data.LevelNum
	CGasRoleLevelUpSql._SavePlayerLevelUpTime:ExecStat(CharID, LevelNum)
end


return CGasRoleLevelUpDB