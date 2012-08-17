cfg_load "appellation/AppellationText_Common"
lan_load "appellation/Lan_AppellationText_Common"

local CGasAppellationAndMoralSql = class()
local AppellationText_Common = AppellationText_Common
local Lan_AppellationText_Common = Lan_AppellationText_Common
local MemH64 = MemH64
local GasAppellationAndMoralDB = CreateDbBox(...)

---------------------------------------------------------------------------
local StmtDef =
{
	"_SavePlayerAppellation",
	"replace into tbl_char_appellation(cs_uId,ca_uAppellationId1,ca_uAppellationId2) values(?,?,?) "
}
DefineSql( StmtDef , CGasAppellationAndMoralSql )


--@brief 保存玩家称谓
--@param char_id:角色Id
--@param appellationText:称谓内容
function GasAppellationAndMoralDB.SavePlayerAppellation(data)
	local char_id = data["char_id"]
	local appellationId1 = data["appellationId1"]
	local appellationId2 = data["appellationId2"]
	
	CGasAppellationAndMoralSql._SavePlayerAppellation:ExecSql("",char_id,appellationId1,appellationId2)
	return g_DbChannelMgr:LastAffectedRowNum() > 0
end

---------------------------------------------------------------------------
local StmtDef =
{
	"_GetPlayerAppellationInfo",
	"select ca_uAppellationId1,ca_uAppellationId2 from tbl_char_appellation where cs_uId = ?"
}
DefineSql( StmtDef , CGasAppellationAndMoralSql )

--@brief 查询玩家当前所使用的称谓
--@param char_id:角色Id
--@return 角色称谓
function GasAppellationAndMoralDB.GetPlayerAppellationInfo(data)
	local char_id = data["char_id"]
	local appellationId1,appellationId2 = 0,0
	local res = CGasAppellationAndMoralSql._GetPlayerAppellationInfo:ExecSql("s[32]",char_id)
	if res:GetRowNum() > 0 then
		appellationId1 = res:GetData(0,0)
		appellationId2 = res:GetData(0,1)
	end
	
	return {appellationId1,appellationId2}
end
---------------------------------------------------------------------------
local StmtDef =
{
	"_AddNewAppellation",
	"insert ignore into tbl_char_appellationList(cs_uId,ca_uAppellation) values(?,?) "
}
DefineSql( StmtDef , CGasAppellationAndMoralSql )


--@brief 玩家新增称谓
--@param char_id:角色Id
--@param appellationId:称谓Id
function GasAppellationAndMoralDB.AddNewAppellation(data)
	local char_id = data["char_id"]
	local AppellationText = data["AppellationText"]
	local isUsed = false
	local appellationId = AppellationText_Common(AppellationText,"ID")
	if nil == appellationId then
		return
	end
	CGasAppellationAndMoralSql._AddNewAppellation:ExecSql("",char_id,appellationId)
	return g_DbChannelMgr:LastAffectedRowNum() > 0,appellationId
end
---------------------------------------------------------------------------
local StmtDef =
{
	"_GetCharAllAppellationInfo",
	"select ca_uAppellation from tbl_char_appellationList where cs_uId = ?"
}
DefineSql( StmtDef , CGasAppellationAndMoralSql )

--@brief 查询玩家拥有的所有称谓
--@param char_id:角色Id
--@return 角色所有称谓
function GasAppellationAndMoralDB.GetCharAllAppellationInfo(data)
	local char_id = data["char_id"]
	local appellationList = {}
	local res = CGasAppellationAndMoralSql._GetCharAllAppellationInfo:ExecSql("n",char_id)
	if res and res:GetRowNum() > 0 then
		for i = 1,res:GetRowNum() do
			table.insert(appellationList,res:GetData(i-1,0))
		end
	end
	return appellationList
end
---------------------------------------------------------------------------
local StmtDef =
{
	"_DelAppellation",
	"delete from tbl_char_appellationList where cs_uId = ? and ca_uAppellation = ? "
}
DefineSql( StmtDef , CGasAppellationAndMoralSql )

--@brief 删除玩家的转职称谓
--@param char_id:角色Id
function GasAppellationAndMoralDB.DelAppellation(data)
	local char_id = data["char_id"]
	local AppellationText = data["AppellationText"]
	local appellationId = AppellationText_Common(AppellationText,"ID")
	if nil == appellationId then
		return
	end
	CGasAppellationAndMoralSql._DelAppellation:ExecSql("",char_id,appellationId)
end

---------------------------------------------------------------------------
local StmtDef =
{
	"_QueryPlayerAppellation",
	"select count(*) from tbl_char_appellationList where cs_uId = ? and ca_uAppellation = ?"
}
DefineSql( StmtDef , CGasAppellationAndMoralSql )

--@brief 查询玩家是否有某个称号
--@param char_id:角色Id
--@param AppellationText:称号
--@return 玩家是否有这个称号
function GasAppellationAndMoralDB.QueryPlayerAppellation(data)
	local charId ,sAppellationText = data["char_id"], data["AppellationText"]
	local appellationId = AppellationText_Common(sAppellationText,"ID")
	if nil == appellationId then
		return false
	end
	local res = CGasAppellationAndMoralSql._QueryPlayerAppellation:ExecStat(charId, appellationId)
	return res:GetData(0,0) > 0
end



SetDbLocalFuncType(GasAppellationAndMoralDB.SavePlayerAppellation)
SetDbLocalFuncType(GasAppellationAndMoralDB.AddNewAppellation)
return GasAppellationAndMoralDB
