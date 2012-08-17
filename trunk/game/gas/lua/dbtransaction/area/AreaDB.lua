gac_gas_require "areamgr/AreaMgr"
gac_gas_require "activity/quest/QuestMgrInc"

local g_AreaQuestMgr = g_AreaQuestMgr
local g_SceneAreaList = g_SceneAreaList
local QuestState = {
	init	= 1,
	failure	= 2,
	finish	= 3
}
local StmtContainer = class()

local StmtDef = 
{
	"_AddPlayerAreaInfo",
	[[
		insert into tbl_area(cs_uId,a_sAreaName,a_bDiscovery) values ( ?, ?, ?)
	]]
}
DefineSql(StmtDef, StmtContainer)

local StmtDef = 
{
	"_AddPlayerAreaPlace",
	[[
		insert into tbl_player_area(cs_uId,pa_sAreaName) values ( ?, ? )
	]]
}
DefineSql(StmtDef, StmtContainer)

local StmtDef = 
{
	"_PlayerChangeAreaPace",
	[[
		update tbl_player_area set pa_sAreaName = ? where cs_uId = ?
	]]
}
DefineSql(StmtDef, StmtContainer)

--local StmtDef = 
--{
--	"_PlayerOutOfArea",
--	[[
--		update tbl_player_area set pa_sAreaName = ? where cs_uId = ?
--	]]
--}
--DefineSql(StmtDef, StmtContainer)

local StmtDef = 
{
	"_GetPlayerAreaPlace",
	[[
		select pa_sAreaName from tbl_player_area where cs_uId = ?
	]]
}
DefineSql(StmtDef, StmtContainer)

local StmtDef = 
{
	"_GetPlayerAreaInfo",
	[[
		select a_bDiscovery,0 from tbl_area where cs_uId = ? and a_sAreaName = ?
	]]
}
DefineSql(StmtDef, StmtContainer)

local StmtDef = 
{
	"_UpdateDiscoveryAreaInfo",
	[[
		update tbl_area set a_bDiscovery = 1 where cs_uId = ? and a_sAreaName = ?
	]]
}
DefineSql(StmtDef, StmtContainer)

local AreaDB = CreateDbBox(...)

function AreaDB.NewRoleAddPlayerAreaPlace(PlayerId)
	StmtContainer._AddPlayerAreaPlace:ExecSql("",PlayerId, "")
end

function AreaDB.GetPlayerAreaInfo(data)
	local ret = StmtContainer._GetPlayerAreaInfo:ExecSql("nn", data[1],data[2])
	if(ret:GetRowNum() == 0) then
		ret:Release()
		return nil
	end
	local res1 = ret:GetData(0,0)
	local res2 = ret:GetData(0,1)
	ret:Release()
	return {res1, res2}
end

function AreaDB.FirstTimeIntoArea(data)
	local PlayerId, AreaName = data[1],data[2]
	StmtContainer._PlayerChangeAreaPace:ExecSql("",AreaName,PlayerId)
	local ret = AreaDB.GetPlayerAreaInfo(data)
	if ret then
		if ret[1] == 0 then
			StmtContainer._UpdateDiscoveryAreaInfo:ExecSql("",PlayerId, AreaName)
			if g_DbChannelMgr:LastAffectedRowNum() > 0	 then
				return {ret[1],ret[2]}  --返回进入该区域前的区域探索状态，区域任务完成数量
			else
				CancelTran()
				return {false}
			end
		else
			return {ret[1],ret[2]}
		end
	else
		StmtContainer._AddPlayerAreaInfo:ExecSql("",PlayerId, AreaName,1)
		if g_DbChannelMgr:LastAffectedRowNum() > 0	 then
			return {0,0}
		else
			CancelTran()
			return {false}
		end
	end 
end

function AreaDB.GetPlayerSceneAreaInfo(data)
	local PlayerId, SceneName = data[1],data[2]
	local tbl = {}
	for AreaName, v in pairs( g_SceneAreaList[SceneName] ) do
		local Arg = {PlayerId, AreaName}
		local res = AreaDB.GetPlayerAreaInfo(Arg)
		if res then
			table.insert(tbl,{AreaName,res[1],res[2]})
		else
			table.insert(tbl,{AreaName,0,0})
		end
	end
	return tbl
end

function AreaDB.PlayerIntoArea(data)
	local ret = StmtContainer._GetPlayerAreaPlace:ExecSql("s[128]",data[1])
	if(ret:GetRowNum() == 0) then
		StmtContainer._AddPlayerAreaPlace:ExecSql("",data[1], data[2])
	else
		StmtContainer._PlayerChangeAreaPace:ExecSql("", data[2],data[1])
	end
	return g_DbChannelMgr:LastAffectedRowNum() > 0	
end

function AreaDB.PlayerOutFromArea(data)
	local ret = StmtContainer._GetPlayerAreaPlace:ExecSql("s[128]",data[1])
	if(ret:GetRowNum() == 0) then
		StmtContainer._AddPlayerAreaPlace:ExecSql("",data[1], "")
	else
		StmtContainer._PlayerChangeAreaPace:ExecSql("", "",data[1])
	end
	return g_DbChannelMgr:LastAffectedRowNum() > 0	
end

function AreaDB.GetPlayerAreaPlace(PlayerId)
	local ret = StmtContainer._GetPlayerAreaPlace:ExecSql("s[128]",PlayerId)
	local AreaName = nil
	if(ret:GetRowNum() ~= 0) then
		AreaName = ret:GetData(0, 0)		
	end
	ret:Release()
	return AreaName
end

function AreaDB.CheckPlayerIsInArea(PlayerId,AreaName)
	return AreaDB.GetPlayerAreaPlace(PlayerId) == AreaName
end

function AreaDB.Check2PlayerInSameArea(PlayerId1,PlayerId2)
	local AreaName1 = AreaDB.GetPlayerAreaPlace(PlayerId1)
	local AreaName2 = AreaDB.GetPlayerAreaPlace(PlayerId2)
	return (AreaName1~= "") and (AreaName1 == AreaName2)
end

SetDbLocalFuncType(AreaDB.FirstTimeIntoArea)
SetDbLocalFuncType(AreaDB.GetPlayerSceneAreaInfo)
SetDbLocalFuncType(AreaDB.PlayerIntoArea)
SetDbLocalFuncType(AreaDB.PlayerOutFromArea)
return AreaDB
