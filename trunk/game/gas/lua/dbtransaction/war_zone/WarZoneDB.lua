gac_gas_require "war/WarZoneBasic"
gac_gas_require "relation/tong/TongMgr"

local g_TongMgr = CTongMgr:new()
local WarZoneBasic = CWarZoneBasic:new()
local StmtContainer = class()
local g_ItemInfoMgr = CItemInfoMgr:new()

--占领小队等级限制
local OCCUPY_LEVEL_SELF_LINE_2 = 5
local OCCUPY_LEVEL_SELF_LINE_1 = 6
local OCCUPY_LEVEL_ENEMY_LINE_1 = 8
local OCCUPY_LEVEL_ENEMY_LINE_2 = 10

local StmtDef=
{
 "CreateNewWarZone",
 "insert into tbl_war_zone(sc_uId, wz_uId, wz_dtCreateTime) values(?,?,now())"
}
DefineSql(StmtDef,StmtContainer)

local StmtDef=
{
 "GetWarZoneCount",
 "select count(*) from tbl_war_zone"
}
DefineSql(StmtDef,StmtContainer)

local StmtDef=
{
 "GetWarZoneInfo",
 "select wz_uId, sc_uId from tbl_war_zone"
}
DefineSql(StmtDef,StmtContainer)

local StmtDef=
{
 "GetWarZoneSceneId",
 "select sc_uId from tbl_war_zone where wz_uId = ?"
}
DefineSql(StmtDef,StmtContainer)

local StmtDef=
{
	"GetStationOwnerTong",
	[[
	select 
		t_uId 
	from
		tbl_war_zone_station 
	where
		wz_uId = ?
		and wzs_uIndex = ?
	]]
}
DefineSql(StmtDef,StmtContainer)

local StmtDef=
{
	"DeleteStationOwner",
	"delete from tbl_war_zone_station where t_uId = ?"
}
DefineSql(StmtDef,StmtContainer)

local StmtDef=
{
 "SetStationOwner",
 [[
 replace into  
 		tbl_war_zone_station(wz_uId, wzs_uIndex, t_uId, wzs_dtOccupyTime) 
 		values(?,?,?,now())
 ]]
}
DefineSql(StmtDef,StmtContainer)

local StmtDef=
{
 "SetSentryOwner",
 [[
 replace into  
 		tbl_war_zone_sentry(wz_uId, wzs_uIndex, wzs_uCamp) 
 		values(?,?,?)
 ]]
}
DefineSql(StmtDef,StmtContainer)

local StmtDef=
{
	"GetSentryOwnerCamp",
	[[
	select 
		wzs_uCamp 
	from
		tbl_war_zone_sentry 
	where
		wz_uId = ?
		and wzs_uIndex = ?
	]]
}
DefineSql(StmtDef,StmtContainer)


local StmtDef=
{
	"GetStationTongInfo",
	[[
	select 
		wzs_uIndex, t_uId
	from
		tbl_war_zone_station 
	where
		wz_uId = ?
	]]
}
DefineSql(StmtDef,StmtContainer)

local StmtDef=
{
	"GetStationTongNameInfo",
	[[
	select 
		wzs_uIndex, t_sName
	from
		tbl_war_zone_station as wzs, tbl_tong as tong
	where
		wz_uId = ?
		and wzs.t_uId = tong.t_uId
	]]
}
DefineSql(StmtDef,StmtContainer)

local StmtDef=
{
	"GetStationCampInfo",
	[[
	select 
		station.wzs_uIndex, tong.t_uCamp
	from
		tbl_war_zone_station as station, tbl_tong as tong
	where
		station.t_uId = tong.t_uId
		and wz_uId = ?
	]]
}
DefineSql(StmtDef,StmtContainer)


local StmtDef=
{
	"GetSentryCampInfo",
	[[
	select 
		wzs_uIndex, wzs_uCamp
	from
		tbl_war_zone_sentry 
	where
		wz_uId = ?
	]]
}
DefineSql(StmtDef,StmtContainer)

local StmtDef=
{
	"GetTongStation",
	[[
	select 
		warZone.sc_uId, warZone.wz_uId, station.wzs_uIndex
	from
		tbl_war_zone as warZone,tbl_war_zone_station as station
	where
		warZone.wz_uId = station.wz_uId
		and station.t_uId = ?
	]]
}
DefineSql(StmtDef,StmtContainer)

local StmtDef=
{
	"GetTongStationOnly",
	[[
	select 
		wzs_uIndex, wz_uId
	from
		tbl_war_zone_station 
	]]
}
DefineSql(StmtDef,StmtContainer)


local StmtDef=
{
	"GetTongWzAndWzs",
	[[
	select 
		wz_uId, wzs_uIndex
	from
		tbl_war_zone_station
	where
		t_uId = ?
	]]
}
DefineSql(StmtDef,StmtContainer)

local StmtDef=
{
	"GetTongStationAndCamp",
	[[
	select 
		tong.t_uCamp, warZone.wz_uId, station.wzs_uIndex
	from
		tbl_war_zone as warZone,tbl_war_zone_station as station,tbl_tong as tong
	where
		warZone.wz_uId = station.wz_uId
		and station.t_uId = tong.t_uId
		and station.t_uId = ?
	]]
}
DefineSql(StmtDef,StmtContainer)

local StmtDef=
{
	"_GetPlayerLevel",
	[[
	select 
		basic.cb_uLevel
	from
		tbl_char_basic as basic, tbl_sort_level_by_camp as sort
	where
		basic.cs_uId = sort.cs_uId
		order by slbc_uOrder asc
	]]
}
DefineSql(StmtDef,StmtContainer)

local StmtDef=
{
 "SetStationTransferTime",
 [[
 replace into  
 		tbl_station_transfer_time(t_uId,  stt_lastTime) 
 		values(?, now())
 ]]
}
DefineSql(StmtDef,StmtContainer)

local StmtDef=
{
 "IsInTransferCD",
 "select count(*) from tbl_station_transfer_time where t_uId = ? and (unix_timestamp(now()) - unix_timestamp(stt_lastTime)) < 60"
}
DefineSql(StmtDef,StmtContainer)

local StmtDef=
{
 "GetWarZoneServerId",
 "select tbl_scene.sc_uServerId from tbl_war_zone, tbl_scene where tbl_war_zone.sc_uId = tbl_scene.sc_uId and tbl_war_zone.wz_uId = ?"
}
DefineSql(StmtDef,StmtContainer)

local WarZone = CreateDbBox(...)

function WarZone.GetTongStationOnly()
	local result = StmtContainer.GetTongStationOnly:ExecStat()
	local resNum = result:GetRowNum()
	local resTbl = {}
	if resNum > 0 then
		for i = 0, resNum - 1 do
			table.insert(resTbl, {result:GetData(i, 0), result:GetData(i,1)})
		end
	end
	return resTbl
end
----创建新的战区
--function WarZone.CreateNewWarZone(newId)
--	--print"添加新战场"
--	local data =
--	{
--		["SceneName"] = "花语平原" .. newId,
--		["Proc"] = newId
--	}
--	local SceneMgrDB = RequireDbBox("SceneMgrDB")
--	local sceneId = SceneMgrDB._CreateScene( data )
--	if sceneId == nil then
--		CancelTran()
--		return false
--	end
--	StmtContainer.CreateNewWarZone:ExecSql("", sceneId,newId)
--	if g_DbChannelMgr:LastAffectedRowNum() > 0 then
--		return true
--	else
--		CancelTran()
--		return false
--	end
--end

function WarZone.GetWarZoneServerId(warZoneId)
	local result = StmtContainer.GetWarZoneServerId:ExecSql('n', warZoneId)
	if result:GetRowNum() > 0 then
		local serverId = result:GetData(0,0)
		result:Release()
		return serverId
	end
	result:Release()
end

function WarZone.GetWarZoneCount()
	local result = StmtContainer.GetWarZoneCount:ExecSql('n')
	local count = result:GetData(0,0)
	result:Release()
	return count
end

--服务器启动时候调用
--local WAR_ZONE_COUNT = 6 --预计开启的战区数目
function WarZone.GetWarZoneInfoOnGasStart()
--	local curCount = WarZone.GetWarZoneCount()
--	assert(curCount <= WAR_ZONE_COUNT, "战区只能增加数量,不能减少.")
--	for i= curCount + 1, WAR_ZONE_COUNT do
--		WarZone.CreateNewWarZone(i)
--	end
	local result = StmtContainer.GetWarZoneInfo:ExecSql('nn')
	local warZoneIdTbl = {}
	local StationTongAndBuildInfoTbl = {}
	for i = 0, result:GetRowNum() -1 do
		warZoneIdTbl[result:GetData(i,0)] = result:GetData(i,1)
		StationTongAndBuildInfoTbl[result:GetData(i,0)] = WarZone.GetWarZoneStationTongAndBuildInfo({["WarZoneId"] = result:GetData(i,0)})
	end
	result:Release()
	local WarZoneInfoTbl = {}
	WarZoneInfoTbl.WarZoneTbl = warZoneIdTbl
	WarZoneInfoTbl.WorldLevel = WarZone.GetPlayerLevel()
	WarZoneInfoTbl.StationTongAndBuildInfoTbl = StationTongAndBuildInfoTbl
	return WarZoneInfoTbl
end

function WarZone.GetStationOwnerTong(warZoneId, stationIndex)
	local result = StmtContainer.GetStationOwnerTong:ExecSql('n', warZoneId, stationIndex)
	if result:GetRowNum() == 0 then
		return 0
	end
	return result:GetData(0,0)
end

function WarZone.GetSentryOwnerCamp(warZoneId, stationIndex)
	local result = StmtContainer.GetSentryOwnerCamp:ExecSql('n', warZoneId, stationIndex)
	if result:GetRowNum() == 0 then
		return 0
	end
	return result:GetData(0,0)
end

--比较帮会驻地在数据的位置和内存中的位置是否一直
--参数position 为nil 代表内存中 在副本里
function WarZone.IsSamePosition(position, tongId)
	local result = StmtContainer.GetTongStation:ExecSql("nnn", tongId)
	local isSame = false
	if  result:GetRowNum() == 0 then
		if not position then
			isSame = true
		end 
	else
		if position 
			and position.m_WarZoneId == result:GetData(0, 1) 
			and position.m_StationId == result:GetData(0, 2) then
				isSame = true
		end
	end
	result:Release()
	return isSame
end

function WarZone.SetStationTransferTime(tongId)
	StmtContainer.SetStationTransferTime:ExecSql("", tongId)
end

--检测驻地是否刚搬迁过
function WarZone.IsInTransferCD(tongId)
	local result = StmtContainer.IsInTransferCD:ExecSql("", tongId)
	local count = result:GetData(0,0)
	result:Release()
	return count > 0
end

function WarZone.IsCanRetreatStation(data)
	local playerId = data["uPlayerId"]
	local TongBasicDB = RequireDbBox("GasTongBasicDB")
	local tongId = TongBasicDB.GetTongID(playerId)
	if tongId ~= data["tongId"] then
		return false
	end
	local GasTongBasicDB = RequireDbBox("GasTongBasicDB")
	if not TongBasicDB.JudgePurview(playerId, "ApplyRetreat") then
		return false, 9006
	end
	local sceneType, sceneId, serverId, warZoneId, stationIndex =  WarZone.GetTongStationInfo(data)
	if sceneType == 7 then
		return true, serverId, warZoneId, stationIndex
	else
		return false, 9416
	end
end

local g_TongMgr = g_TongMgr or CTongMgr:new()

function WarZone.IsCanOccupyStation(data)
	local uPlayerId = data["PlayerId"]
	local uWarZoneId = data["WarZoneId"]
	local uStationId = data["StationId"]
	local TongBasicDB = RequireDbBox("GasTongBasicDB")
	local tongId = TongBasicDB.GetTongID(uPlayerId)
	if tongId == 0 then
		return false, 9401
	end
	if tongId ~= data["TongId"] then
		return false
	end
	
	local tongType = TongBasicDB.GetTongType(tongId)
	if tongType ~= g_TongMgr:GetTongTypeByName("精英战斗") then
		return false, 9169
	end
	
	if not TongBasicDB.JudgePurview(uPlayerId, "WarManage") then
		return false, 9006
	end
	local oldTongId = WarZone.GetStationOwnerTong(uWarZoneId, uStationId)
	if oldTongId ~= 0 then --有人占领,需将其打退
		return false, 9403
	end
	local TongBasicDB = RequireDbBox("GasTongBasicDB")
	local defaultCamp = math.floor(uStationId / 100)
	local ownCamp = TongBasicDB.GetTongCampByID(tongId)
	local stationLine = WarZoneBasic:GetStationWarLine(uStationId)
	local curSceneType, sceneId, serverId, curWarZoneId, curStationId =  WarZone.GetTongStationInfo({["tongId"] = tongId})
	
	local nRelativeLine
	if ownCamp == defaultCamp then
		nRelativeLine = 3 - stationLine
	else
		nRelativeLine = 2 + stationLine
	end
	
	local level = TongBasicDB.GetTongLevel(tongId)
	if nRelativeLine == 1 then
		if level < OCCUPY_LEVEL_SELF_LINE_2 then
			return false, 9289, OCCUPY_LEVEL_SELF_LINE_2
		end
	elseif nRelativeLine == 2 then
		if level < OCCUPY_LEVEL_SELF_LINE_1 then
			return false, 9289, OCCUPY_LEVEL_SELF_LINE_1
		end
	elseif nRelativeLine == 3 then
		if level < OCCUPY_LEVEL_ENEMY_LINE_1 then
			return false, 9289, OCCUPY_LEVEL_ENEMY_LINE_1
		end
	elseif nRelativeLine == 4 then
		if level < OCCUPY_LEVEL_ENEMY_LINE_2 then
			return false, 9289, OCCUPY_LEVEL_ENEMY_LINE_2
		end
	end
	
	local ownRelativeLine = WarZone.GetTongRelativeLine(tongId)
	
	if curSceneType == 6 then
		if ownRelativeLine + 1 == nRelativeLine then
			return true
		else
			return false, 9449
		end
	elseif curSceneType == 7 then
		if curWarZoneId == uWarZoneId then
			if ownRelativeLine + 1 >= nRelativeLine then
				local nCurStationCamp = math.floor(curStationId / 100)
				local nStationCamp = math.floor(uStationId / 100)
				if ownRelativeLine == 4 then
					if nStationCamp == ownCamp or nStationCamp == nCurStationCamp then
						return true
					else
						return false, 9449
					end
				elseif ownRelativeLine == 3 then
					if nRelativeLine == 3 then
						return true
					elseif nRelativeLine == 4 then
						if nStationCamp == nCurStationCamp then
							return true
						else
							return false, 9449
						end
					else
						return true
					end
				else
					return true
				end
			else
				return false, 9449
			end
		else
			return false, 9412
		end
	end
end

function WarZone.ClearTongStationInfo(tongId)
	--写LOG
	local oldWarZoneId, oldStationId = WarZone.GetTongWzAndWzs(tongId)
	if not oldWarZoneId then
		oldWarZoneId, oldStationId = 0, 0
	end
	local g_LogMgr = RequireDbBox("LogMgrDB")
	g_LogMgr.TongStationMoveLog(tongId, 0, 6, oldWarZoneId, oldStationId, 0, 0)

	StmtContainer.DeleteStationOwner:ExecStat(tongId)
	return (g_DbChannelMgr:LastAffectedRowNum() > 0)
end

function WarZone.ExchangeStation(data)
	local playerId = data["PlayerId"]
	local warZoneId = data["WarZoneId"]
	local stationId = data["StationId"]
	local tongId = data["TongId"]
	local activeBuildingHpTbl = data["ActiveBuildingHpTbl"]
	local passiveBuildingHpTbl = data["PassiveBuildingHpTbl"]
	local targetTongId = WarZone.GetStationOwnerTong(warZoneId, stationId)
	assert(targetTongId == data["TargetTongId"], "挑战期间不能有驻地变迁的操作")
	if targetTongId ~= 0 then
		--写LOG
		local g_LogMgr = RequireDbBox("LogMgrDB")
		g_LogMgr.TongStationMoveLog(targetTongId, playerId, 2, warZoneId, stationId, 0, 0)
	
		StmtContainer.DeleteStationOwner:ExecSql('', targetTongId)
		if not (g_DbChannelMgr:LastAffectedRowNum() > 0) then
			return false
		end
		if passiveBuildingHpTbl then
			local CTongBuildingBox = RequireDbBox("GasTongbuildingDB")
			CTongBuildingBox.UpdateTongBuildingLifeInfo({["BuildingHpTbl"] = passiveBuildingHpTbl})
		end
	end
	local CTongMarketBox = RequireDbBox("GasTongMarketDB")
	CTongMarketBox.ClearStationTime(tongId)
	local argTbl = {}
	argTbl.TongId = targetTongId
	argTbl.WarZoneId = warZoneId
	argTbl.StationId = stationId
	argTbl.DropResourceTbl = {}
	local serverId = WarZone.GetWarZoneServerId(warZoneId)
	Db2GasCall("RetreatStation", argTbl, serverId)
	
	--写LOG
	local oldWarZoneId, oldStationId = WarZone.GetTongWzAndWzs(tongId)
	if not oldWarZoneId then
		oldWarZoneId, oldStationId = 0, 0
	end
	local g_LogMgr = RequireDbBox("LogMgrDB")
	g_LogMgr.TongStationMoveLog(tongId, playerId, 1, oldWarZoneId, oldStationId, warZoneId, stationId)
	
	return WarZone.OccupyStation(warZoneId, stationId, tongId, activeBuildingHpTbl)
end

-- 占领失败返回 false , 失败msgId  , 成功返回 true
function WarZone.OccupyStationRpc(data)
	local uPlayerId = data["PlayerId"]
	local uWarZoneId = data["WarZoneId"]
	local uStationId = data["StationId"]
	local tongId = data["TongId"]
	local buildingHpTbl = data["BuildingHpTbl"]
	local isCanOccupy, msgId= WarZone.IsCanOccupyStation(data)
	if not isCanOccupy then
		return false, msgId
	end
	local ex = RequireDbBox("Exchanger") 
	local sMsg = "2022"
	local CTongBasicBox = RequireDbBox("GasTongBasicDB")
	if not CTongBasicBox.AddTongLogs(tongId,sMsg, g_TongMgr:GetLogType("战争")) then
		return false
	end
	
	--写LOG
	local oldWarZoneId, oldStationId = WarZone.GetTongWzAndWzs(tongId)
	if not oldWarZoneId then
		oldWarZoneId, oldStationId = 0, 0
	end
	local g_LogMgr = RequireDbBox("LogMgrDB")
	g_LogMgr.TongStationMoveLog(tongId, uPlayerId, 3, oldWarZoneId, oldStationId, uWarZoneId, uStationId)
	
	return WarZone.OccupyStation(uWarZoneId, uStationId, tongId, buildingHpTbl, 1)
end

function WarZone.OccupyStation(uWarZoneId, uStationId, uTongId, buildingHpTbl, Sign) --Sign有值的话表示是主动占领，没有值是驻地交换的占领
	local oldSceneType, oldSceneId, oldServerId, oldWarZoneId, oldStationId =  WarZone.GetTongStationInfo({["tongId"] = uTongId})
	if not oldSceneType then--无此帮会
		CancelTran()
		return false
	end
	local oldData = {}
	oldData.SceneType = oldSceneType
	oldData.SceneId = oldSceneId
	oldData.ServerId = oldServerId
	if oldSceneType == 7 then
		oldData.WarZoneId = oldWarZoneId
		oldData.StationId = oldStationId
	end
	if oldData.ServerId ~= 0 then
		oldData.TongId = uTongId
		Db2GasCall("DeleteOldStation", oldData, oldData.ServerId)
	end
	
	StmtContainer.DeleteStationOwner:ExecSql('', uTongId)
	StmtContainer.SetStationOwner:ExecSql('',uWarZoneId, uStationId, uTongId)
	assert(g_DbChannelMgr:LastAffectedRowNum() > 0)
	
	--当驻地易主时,查看tbl_tong_station_time和tbl_tong_line_time
	local CTongMarketBox = RequireDbBox("GasTongMarketDB")
	local parameters = {}
	parameters["tongId"] = uTongId
	parameters["stationId"] = uStationId
	CTongMarketBox.JudgeStationTime(parameters)
	
	local SceneMgrDB = RequireDbBox("SceneMgrDB")
	SceneMgrDB.DeleteTongScene(uTongId)
	local CTongBuildingBox = RequireDbBox("GasTongbuildingDB")
	if buildingHpTbl then
		CTongBuildingBox.UpdateTongBuildingLifeInfo({["BuildingHpTbl"] = buildingHpTbl})
	end
	local TongBasicDB = RequireDbBox("GasTongBasicDB")
	local TongName = TongBasicDB.GetTongNameById(uTongId)
	local newData = {}
	newData.TongId = uTongId
	newData.BuildingTbl = CTongBuildingBox.GetBuildInfo(uTongId):ToTable()
	newData.Camp = TongBasicDB.GetTongCampByID(uTongId)
	newData.WarZoneId = uWarZoneId
	newData.StationId = uStationId
	newData.TongName = TongName
	if Sign then
		local ChannelDB = RequireDbBox("ChannelDB")
		local Flag, memberSet = TongBasicDB.GetTongOnlineMemberInfo({["uTongID"] = uTongId})
		if Flag then
			newData.MemberTbl = memberSet:ToTable()
		end
	end
	local serverId = WarZone.GetWarZoneServerId(uWarZoneId)
	local result = StmtContainer.GetWarZoneSceneId:ExecStat(uWarZoneId)
	if result:GetRowNum() > 0 then
		newData.SceneId = result:GetData(0,0)
	end
	Db2GasCall("CreateNewStation", newData, serverId)
end

--玩家主动撤退
function WarZone.PlayerRetreatStation(data)
	local CTongMarketBox = RequireDbBox("GasTongMarketDB")
	local playerId = data["PlayerId"]
	local isSucceed, msgId, warZoneId, stationId = WarZone.IsCanRetreatStation({["tongId"] = data["TongId"], ["uPlayerId"] = data["PlayerId"]})
	if not isSucceed then
		return false, playerId, msgId
	end
	local serverId = msgId
	local dropResourceTbl = WarZone.RetreatStation(data["TongId"], data["BuildingHpTbl"])
	if dropResourceTbl then
		CTongMarketBox.ClearStationTime(data["TongId"])
		local argTbl = {}
		argTbl.TongId = data["TongId"]
		argTbl.WarZoneId = warZoneId
		argTbl.StationId = stationId
		argTbl.DropResourceTbl = dropResourceTbl
		Db2GasCall("RetreatStation", argTbl, serverId)
		
		--写LOG
		local g_LogMgr = RequireDbBox("LogMgrDB")
		g_LogMgr.TongStationMoveLog(data["TongId"], playerId, 5, warZoneId, stationId, 0, 0)

		return true, playerId
	end
end

function WarZone.RetreatStationRpc(data)
	local CTongMarketBox = RequireDbBox("GasTongMarketDB")
	local playerId = data["PlayerId"]
	local uTongId = data["TongId"]
	local CurPosition = data["CurPosition"]
	local buildingHpTbl = data["BuildingHpTbl"]
	--local currentHpTbl = data["currentHpTbl"]
	if not WarZone.IsSamePosition(CurPosition, uTongId) then
		return false
	end
	local CTongBasicBox = RequireDbBox("GasTongBasicDB")
	local dropResourceTbl = WarZone.RetreatStation(uTongId, buildingHpTbl)
	local ChannelDB = RequireDbBox("ChannelDB")
	local Flag, memberSet = CTongBasicBox.GetTongOnlineMemberInfo({["uTongID"] = uTongId})
	local ex = RequireDbBox("Exchanger")
	local sMsg = "2021"
	if not CTongBasicBox.AddTongLogs(uTongId,sMsg, g_TongMgr:GetLogType("战争")) then
		return false
	end

	--写LOG
	local g_LogMgr = RequireDbBox("LogMgrDB")
	g_LogMgr.TongStationMoveLog(uTongId, playerId, 4, CurPosition.m_WarZoneId, CurPosition.m_StationId, 0, 0)
	
	if dropResourceTbl and Flag and memberSet then
		CTongMarketBox.ClearStationTime(uTongId)
		local argTbl = {}
		argTbl.TongId = uTongId
		argTbl.WarZoneId = CurPosition.m_WarZoneId
		argTbl.StationId = CurPosition.m_StationId
		argTbl.DropResourceTbl = dropResourceTbl
		argTbl.MemberTbl = memberSet:ToTable()
		local serverId = WarZone.GetWarZoneServerId(CurPosition.m_WarZoneId)
		Db2GasCall("RetreatStation", argTbl, serverId)
		return true
	else
		return false
	end
end



local BaseCampTbl = {}
BaseCampTbl[1] = "艾米大本营"
BaseCampTbl[2] = "神圣大本营"
BaseCampTbl[3] = "修斯大本营"

function WarZone.RetreatStation(uTongId, buildingHpTbl)
	--local oldSceneType, sceneId, serverId, warZoneId, stationIndex = WarZone.GetTongStationInfo({["tongId"] = uTongId})
	StmtContainer.DeleteStationOwner:ExecSql('', uTongId)
	if not (g_DbChannelMgr:LastAffectedRowNum() > 0) then
		return
	end
	local result = {}
	local CTongBuildingBox = RequireDbBox("GasTongbuildingDB")
	CTongBuildingBox.UpdateTongBuildingLifeInfo({["BuildingHpTbl"] = buildingHpTbl})
	local CTongBasicBox = RequireDbBox("GasTongBasicDB")
	local nCamp = CTongBasicBox.GetTongCampByID(uTongId)
	local DbBuildingTbl = CTongBuildingBox.GetBuildInfo(uTongId)
	for i =1,DbBuildingTbl:GetRowNum() do
		local buildingId = DbBuildingTbl(i,1)
		local hp = DbBuildingTbl(i,5)
		if hp <= 0.5 then
			if DbBuildingTbl(i,2) == BaseCampTbl[nCamp] then
				if not CTongBuildingBox.SetBuildingLifeInfo(buildingId, 0.01) then
					CancelTran()
					return
				end
			else
				if not CTongBuildingBox.DestroyBuilding({["uTongID"] = uTongId, ["build_id"] = buildingId}) then
					CancelTran()
					return
				end
			end
			
			local NeedRes = g_ItemInfoMgr:GetItemInfo(g_ItemInfoMgr:GetTongItemBigID(), DbBuildingTbl(i,2),"NeedRes")
			result[buildingId] = {}
			result[buildingId].resNum = NeedRes * 0.2
			result[buildingId].x = DbBuildingTbl(i,3)
			result[buildingId].y = DbBuildingTbl(i,4)
		else
			CTongBuildingBox.SetBuildingLifeInfo(buildingId, hp - 0.5)
		end
	end
	local TechDB = RequireDbBox("TongTeachDB")
	TechDB.PauseRDTeach({["TongID"] = uTongId})
	--WarZone.SetStationTransferTime(uTongId)
	return result
end

--获取指定战区内驻地归属表   所属帮会 = 第二个返回值[驻地编号]   所属帮会 为空则是无帮会占领
function WarZone.GetStationTongInfo(data)
	local uWarZoneId = data["WarZoneId"]
	local result = StmtContainer.GetStationTongInfo:ExecSql("nn", uWarZoneId)
	local infoTbl = {}
	for i = 0, result:GetRowNum() -1 do
		infoTbl[result:GetData(i,0)] = result:GetData(i,1)
	end
	result:Release()
	return infoTbl
end

function WarZone.GetStationTongNameInfo(data)
	local uWarZoneId = data["WarZoneId"]
	local result = StmtContainer.GetStationTongNameInfo:ExecStat(uWarZoneId)
	local infoTbl = {}
	for i = 0, result:GetRowNum() -1 do
		infoTbl[result:GetData(i,0)] = result:GetData(i,1)
	end
	result:Release()
	return infoTbl
end

--获得所有战区内驻地归属表
function WarZone.GetAllStationTongInfo(data)
	local uWarZoneId = data["WarZoneId"]
	local result
	local infoTbl = {}
	for index = 1, 6 do
		infoTbl[index] = {}
		result = StmtContainer.GetStationTongInfo:ExecSql("nn", index)
		for i = 0, result:GetRowNum() -1 do
			infoTbl[index][result:GetData(i,0)] = result:GetData(i,1)
		end
		result:Release()
	end
	return infoTbl
end

function WarZone.GetStationCampInfo(data)
	local uWarZoneId = data["WarZoneId"]
	local result = StmtContainer.GetStationCampInfo:ExecSql("nn", uWarZoneId)
	local infoTbl = {}
	for i = 0, result:GetRowNum() -1 do
		infoTbl[result:GetData(i,0)] = result:GetData(i,1)
	end
	result:Release()
	return infoTbl
end

--获取指定战区内哨所归属表   所属阵营 = 第二个返回值[哨所编号]   所属阵营 为空则是默认的阵营
function WarZone.GetSentryCampInfo(data)
	local uWarZoneId = data["WarZoneId"]
	local result = StmtContainer.GetSentryCampInfo:ExecSql("nn", uWarZoneId)
	local infoTbl = {}
	for i = 0, result:GetRowNum() -1 do
		infoTbl[result:GetData(i,0)] = result:GetData(i,1)
	end
	result:Release()
	return infoTbl
end

--这个函数只在场景场景时候 加载帮会建筑调用, 里面包括了 维修计算, 别的场合没必要
function WarZone.GetWarZoneStationTongAndBuildInfo(data)
	local stationTongInfo = WarZone.GetStationTongInfo(data)
	local stationCampInfo = WarZone.GetStationCampInfo(data)
	local stationTongNameInfo = WarZone.GetStationTongNameInfo(data)
	local result = {}
	result["stationTongInfo"] = stationTongInfo
	result["stationCampInfo"] = stationCampInfo
	result["stationTongNameInfo"] = stationTongNameInfo
	result["stationBuilding"] = {}
	local CTongBuildingBox = RequireDbBox("GasTongbuildingDB")
	for stationIndex, tongId in pairs(stationTongInfo) do
		local buildinginfo = CTongBuildingBox.GetBuildInfo(tongId)
		local tbl = {}
		for i =1,buildinginfo:GetRowNum() do
			table.insert(tbl,{buildinginfo(i,1),buildinginfo(i,2),buildinginfo(i,3),buildinginfo(i,4),buildinginfo(i,5),buildinginfo(i,6),buildinginfo(i,7),buildinginfo(i,8),buildinginfo(i,9)})
		end
		result["stationBuilding"][stationIndex] = tbl
		for _, v in pairs(result["stationBuilding"][stationIndex]) do
			--建筑维修
			v[5] = v[5] + CTongBuildingBox.RepairSingleBuilding(v[1], v[5])
		end
	end
	return result
end

function WarZone.GetTongWzAndWzs(tongId)
	local result = StmtContainer.GetTongWzAndWzs:ExecSql("nn", tongId)
	if result:GetRowNum() == 0 then
		result:Release()
		return
	else
		local wzId, wzsId = result:GetData(0,0), result:GetData(0,1)
		result:Release()
		return wzId, wzsId
	end
end

function WarZone.GetTongStationInfo(data)
	local tongId = data["tongId"]
	local SceneMgrDB = RequireDbBox("SceneMgrDB")
	local result = StmtContainer.GetTongStation:ExecSql("nnn", tongId)
	if result:GetRowNum() == 0 then
		result:Release()
		local sceneId, serverId = SceneMgrDB.GetOrCreateTongScene(tongId, data["IsSetServerId"])
		if sceneId then
			return 6, sceneId, serverId
		else
			return
		end
	else
		local sceneId, warZoneId, stationId = result:GetData(0,0),result:GetData(0,1),result:GetData(0,2)
		result:Release()
		
		local serverId = SceneMgrDB.GetSceneServerPos(sceneId)
		return 7, sceneId, serverId, warZoneId, stationId 
	end
end

function WarZone.GetTongStationInfoById(data)
	local tongId = data["tongId"]
	local SceneMgrDB = RequireDbBox("SceneMgrDB")
	local result = StmtContainer.GetTongStation:ExecSql("nnn", tongId)
	if result:GetRowNum() == 0 then
		result:Release()
		local sceneId, serverId = SceneMgrDB.GetOrCreateTongScene(tongId, data["IsSetServerId"])
		local sceneName = SceneMgrDB._GetSceneNameById(sceneId)
		if sceneId then
			return 6, sceneId, serverId, sceneName
		else
			return
		end
	else
		local sceneId, warZoneId, stationId = result:GetData(0,0),result:GetData(0,1),result:GetData(0,2)
		local sceneName = SceneMgrDB._GetSceneNameById(sceneId)
		result:Release()
		local serverId = SceneMgrDB.GetSceneServerPos(sceneId)
		return 7, sceneId, serverId, sceneName, warZoneId, stationId 
	end
end

function WarZone.GetTongStationServer(tongId)
	local SceneMgrDB = RequireDbBox("SceneMgrDB")
	local result = StmtContainer.GetTongStation:ExecStat(tongId)
	if result:GetRowNum() == 0 then
		local sceneId, serverId = SceneMgrDB.GetOrCreateTongScene(tongId, true)
		return serverId
	else
		local sceneId = result:GetData(0,0)
		local serverId = SceneMgrDB.GetSceneServerPos(sceneId)
		return serverId
	end
end

function WarZone.StationMoveGetTongStationInfo(data)
	local serverId
	local activeOrPassive = data["ActiveOrPassive"]
	if activeOrPassive == "Passive" then
		serverId = WarZone.GetTongStationServer(data["TargetTongId"])
	else
		serverId = WarZone.GetTongStationServer(data["TongId"])
	end
	local argTbl = {}
	argTbl.TongId = data["TongId"]
	argTbl.WarZoneId = data["WarZoneId"]
	argTbl.StationId = data["StationId"]
	argTbl.PlayerId = data["PlayerId"]
	argTbl.StationMoveType = data["StationMoveType"]
	argTbl.TargetTongId = data["TargetTongId"]
	argTbl.CurPosition = data["CurPosition"]
	argTbl.ActiveOrPassive = data["ActiveOrPassive"]
	if data["ActiveBuildingHpTbl"] then
		argTbl.ActiveBuildingHpTbl = data["ActiveBuildingHpTbl"]
	end
	Db2GasCall("TongStationMove", argTbl, serverId)
end

function WarZone.GetTongStationInfoByName(data)
	local sTongName = data["sTongName"]
	local nCamp = data["uCamp"]
	
	local GasTongBasicDB = RequireDbBox("GasTongBasicDB")
	local TongId, Camp = GasTongBasicDB.GetTongIDAndCampByNameRpc({["TongName"] = sTongName})
	if TongId == 0 then
		return false, 9020
	end
	if nCamp ~= Camp then
		return false, 9021
	end
	
	local parameter = {}
	parameter["tongId"] = TongId
	parameter["IsSetServerId"] = true
	local sceneType, sceneId, serverId, warZoneId, stationId = WarZone.GetTongStationInfo(parameter)
	return true, sceneType, sceneId, serverId, warZoneId, stationId, TongId
end

function WarZone.GetTongStationAndCamp(data)
	local tongId = data["uTongId"]
	local result = StmtContainer.GetTongStationAndCamp:ExecSql("nnn", tongId)
	if result:GetRowNum() == 0 then
		result:Release()
		local tong_box = RequireDbBox("GasTongBasicDB")
		local tongCamp = tong_box.GetTongCampByID(tongId)
		return 6, tongCamp
	else
		local camp, warZoneId, stationId = result:GetData(0,0), result:GetData(0,1), result:GetData(0,2)
		result:Release()
		return 7, camp, warZoneId, stationId 
	end
end

function WarZone.GetTongStationRpc(data)
	local tongId = data["uTongId"]
	local line = WarZone.GetTongRelativeLine(tongId)
	if line == 0 then
		return line
	end
	
	local result = StmtContainer.GetTongStationAndCamp:ExecSql("nnn", tongId)
	if result:GetRowNum() > 0 then
		local warZoneId, stationId = result:GetData(0,1), result:GetData(0,2)
		result:Release()
		return line, warZoneId, stationId
	end
end

--获取帮会相对 的战线值 返回 0 - 4 (nil 帮会不存在)0是副本 
function WarZone.GetTongRelativeLine(tongId)
	local sceneType, sceneId, serverId, warZoneId, stationId = WarZone.GetTongStationInfo({["tongId"] = tongId})
	if not sceneType then
		return
	end
	if sceneType == 6 then
		return 0
	end
	local tong_box = RequireDbBox("GasTongBasicDB")
	local tongCamp = tong_box.GetTongCampByID(tongId)
	local defCamp =  WarZoneBasic:GetStationDefaultCamp(stationId)
	local line = WarZoneBasic:GetStationWarLine(stationId)
	if defCamp == tongCamp then
		return 3 - line
	else
		return 2 + line
	end
end

function WarZone.GetPlayerLevel(data)
	local result = StmtContainer._GetPlayerLevel:ExecSql("n")
	local num = result:GetRowNum()
	if num < 100 then
		result:Release()
		return 40
	end
	
	local level = result:GetData(99, 0)
	if level < 40 then
		result:Release()
		return 40
	end
	return level
end

return WarZone
