gac_gas_require "relation/tong/TongMgr"

local g_TongMgr = g_TongMgr or CTongMgr:new()
local DragonCaveMgr = class()
local event_type_tbl = event_type_tbl
local g_ItemInfoMgr = g_ItemInfoMgr
local g_StoreRoomIndex	= g_StoreRoomIndex
local DragonCaveDB = CreateDbBox(...)
local LogErr = LogErr


local StmtDef = {

	"AddDragonCaveInfo",
	"insert into tbl_scene_dragoncave(t_uId, sc_uId) values(?, ?)"
}
DefineSql(StmtDef,DragonCaveMgr)

local StmtDef = {

	"ReceiveSceneTbl",
	[[
		select 
			tbl_scene.sc_uServerId,
			tbl_scene_dragoncave.sc_uId
		from 
			tbl_scene, 
			tbl_scene_dragoncave 
		where
			tbl_scene.sc_uId = tbl_scene_dragoncave.sc_uId and
			tbl_scene_dragoncave.t_uId = ? 
	]]
}
DefineSql(StmtDef,DragonCaveMgr)

local StmtDef = {

	"CheckPlayerState",
	"select unix_timestamp(sc_sFinishTime),unix_timestamp(now()) from tbl_tong_dragoncave"
}
DefineSql(StmtDef,DragonCaveMgr)

local StmtDef = {

	"CheckTongState",
	"select unix_timestamp(sc_sFinishTime),unix_timestamp(now()) from tbl_tong_dragoncavetong"
}
DefineSql(StmtDef,DragonCaveMgr)

local StmtDef = {

	"CheckPlayerSceneState",
	"select unix_timestamp(sc_sFinishTime) from tbl_tong_dragoncave where cs_uId = ?"
}
DefineSql(StmtDef,DragonCaveMgr)

local StmtDef = {

	"CheckTongSceneState",
	"select unix_timestamp(sc_sFinishTime) from tbl_tong_dragoncavetong where t_uId = ?"
}
DefineSql(StmtDef,DragonCaveMgr)

local StmtDef = {

	"GetSceneInfo",
	[[
		select 
			tbl_scene.sc_uServerId,
			tbl_scene_dragoncave.sc_uId
		from 
			tbl_scene, 
			tbl_scene_dragoncave 
		where
			tbl_scene.sc_uId = tbl_scene_dragoncave.sc_uId and
			tbl_scene_dragoncave.t_uId = ? and tbl_scene.sc_sSceneName = ? 
	]]
}
DefineSql(StmtDef,DragonCaveMgr)


local StmtDef = {

	"AddDragonCaveTong",
	"insert into tbl_tong_dragoncavetong(t_uId, sc_sFinishTime) values(?, now())"
}
DefineSql(StmtDef,DragonCaveMgr)

local StmtDef = {

	"AddDragonCaveBuilding",
	"insert into tbl_building_dragon(bt_uId, t_uId, bt_buildTime) values(?,?, now())"
}
DefineSql(StmtDef,DragonCaveMgr)


local StmtDef = {

	"AddDragonCaveList",
	[[
		insert ignore into 
			tbl_tong_dragoncave
		select
			cs_uId, now()
		from
			tbl_member_tong
		where
			t_uId = ?
	]]
}
DefineSql(StmtDef,DragonCaveMgr)


local StmtDef = {

	"CheckBuildingState",
	"select unix_timestamp(bt_buildTime),unix_timestamp(now()) from tbl_building_dragon"
}
DefineSql(StmtDef,DragonCaveMgr)

local StmtDef = {

	"CheckDragonBuildingState",
	"select bt_uId,t_uId,unix_timestamp(bt_buildTime),unix_timestamp(now()) from tbl_building_dragon"
}
DefineSql(StmtDef,DragonCaveMgr)

local StmtDef = {

	"selcetBuilding",
	"select unix_timestamp(bt_buildTime) from tbl_building_dragon where bt_uId = ?"
}
DefineSql(StmtDef,DragonCaveMgr)

local StmtDef = {

	"DeleteDragonCave",
	"delete from tbl_tong_dragoncave"
}
DefineSql(StmtDef,DragonCaveMgr)

local StmtDef = {

	"DeleteDragonCaveTong",
	"delete from tbl_tong_dragoncavetong"
}
DefineSql(StmtDef,DragonCaveMgr)

local StmtDef = {

	"DeleteDragonBuilding",
	"delete from tbl_building_tong where bt_uId = ?"
}
DefineSql(StmtDef,DragonCaveMgr)

local StmtDef = {
    	"SelectBuildingInfo",
    	"select bt_sBuildName,bt_uLevel from tbl_building_tong  where bt_uId = ? and t_uId = ?"
}    
DefineSql ( StmtDef, DragonCaveMgr )

function DragonCaveDB.GetDragonCaveSceneState(data)
	local CharId = data["charId"]
	local TongId = data["TongId"]
	local PlayerTbl = DragonCaveMgr.CheckPlayerSceneState:ExecStat(CharId)
	local TongTbl = DragonCaveMgr.CheckTongSceneState:ExecStat(TongId)
	if PlayerTbl:GetRowNum() > 0 or TongTbl:GetRowNum() > 0 then
		PlayerTbl:Release()
		TongTbl:Release()
		return true
	else
		local TongBasicDB = RequireDbBox("GasTongBasicDB")
--		local player_pos = TongBasicDB.GetPositionById(CharId)
		return false
	end
end
function DragonCaveDB.GetDragonCaveScene(data)
	local SceneMgrDB = RequireDbBox("SceneMgrDB")	
	local TongBasicDB = RequireDbBox("GasTongBasicDB")
	local CharId = data["charId"]
	local TongId = data["TongId"]
	local SceneName = data["SceneName"]
	local result = DragonCaveMgr.GetSceneInfo:ExecStat(TongId, SceneName)
	local Camp = TongBasicDB.GetTongCampByID(TongId)
	local PlayerTbl = DragonCaveMgr.CheckPlayerSceneState:ExecStat(CharId)
	local TongTbl = DragonCaveMgr.CheckTongSceneState:ExecStat(TongId)
	if PlayerTbl:GetRowNum() > 0 or TongTbl:GetRowNum() > 0 then
		PlayerTbl:Release()
		TongTbl:Release()
		return true
	else
		local TongBasicDB = RequireDbBox("GasTongBasicDB")
--		local player_pos = TongBasicDB.GetPositionById(CharId)
		local OtherArg = {}
		OtherArg["m_TongId"] = TongId
		OtherArg["m_Camp"] = Camp
		if result:GetRowNum() > 0 then
			local ServerId, SceneId = result:GetData(0,0), result:GetData(0,1)
		  result:Release()
		  return false,ServerId, SceneId
		else
			local dataTbl = {}
			dataTbl["SceneName"] = SceneName
			dataTbl["OtherArg"] = OtherArg
			local SceneId, ServerId = SceneMgrDB._CreateScene(dataTbl)
			if not SceneId then --场景失败
				LogErr("龙穴活动场景创建失败!", "小队Id："..TongId,"场景名："..SceneName)
				return
			end
			DragonCaveMgr.AddDragonCaveInfo:ExecStat(TongId, SceneId)
			return false,ServerId, SceneId
		end
	end
end

local function SendItem(playerId,BigID,ItemName,Count, sceneName)
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	local sysMailExecutor = RequireDbBox("SysMailDB")
	local AddItemsTbl = {}		
	local params= {}
	params.m_nType = BigID
	params.m_sName = ItemName
	params.m_nBindingType = g_ItemInfoMgr:GetItemInfo(BigID,ItemName,"BindingStyle") or 0
	params.m_nCharID = playerId
	params.m_sCreateType = event_type_tbl["龙穴活动奖励"]
	for j=1, Count do
		local itemID = g_RoomMgr.CreateItem(params)
		if not itemID then
			LogErr("龙穴奖励 帮会物品添加出现其他错误", playerId)
		end
		table.insert(AddItemsTbl,{itemID})
	end
	if AddItemsTbl[1] then
 		local sendRet = sysMailExecutor.SendSysMail(1014, playerId, 8001, 8002, AddItemsTbl, 0, event_type_tbl["龙穴活动奖励"])
		if sendRet == false then
			LogErr("龙穴奖励 帮会物品奖励邮件出错", playerId)
		end
	end
	return true
end

local function AddDragonCaveBuilding(building_id,TongId)
	local buildingTbl = {}
	DragonCaveMgr.AddDragonCaveBuilding:ExecStat(building_id,TongId)
	buildingTbl = DragonCaveMgr.selcetBuilding:ExecStat(building_id)
	if buildingTbl:GetRowNum() > 0 then
		local buildingTime = os.date("*t", buildingTbl:GetData(0,0))
		buildingTbl:Release()
		return buildingTime.wday
	end
end
function DragonCaveDB.SendAwardToTong(data)
	local AreaFbPointDB = RequireDbBox("AreaFbPointDB")
	local GasTongBasicDB = RequireDbBox("GasTongBasicDB")
	local SceneMgrDB = RequireDbBox("SceneMgrDB")	
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	local cdepotMgr = RequireDbBox("GasCDepotMgrDB")
	local TongId = data["TongId"]
	local SceneName = data["SceneName"]
	local SceneId = data["SceneId"]
	local TongAwardId = data["TongAwardId"]
	local TongAwardName = data["TongAward"]
	local TongAwardCount = data["TongAwardCount"]
	local AwardId = data["AwardId"]
	local AwardName = data["Award"]
	local AwardCount = data["AwardCount"]
	local TongInitiator = GasTongBasicDB.GetTongInitiator(TongId)
	local SceneTbl = {}
  local TongTbl = DragonCaveMgr.CheckTongSceneState:ExecStat(TongId)
  if TongTbl:GetRowNum() > 0 then
  	return false
  end
  DragonCaveMgr.AddDragonCaveTong:ExecStat(TongId)
  DragonCaveMgr.AddDragonCaveList:ExecStat(TongId)
	local params= {}
	params.m_nType = TongAwardId
	params.m_sName = TongAwardName
	params.m_nCharID = TongInitiator
	params.m_nBindingType = g_ItemInfoMgr:GetItemInfo(TongAwardId, TongAwardName, "BindingStyle")
	params.m_sCreateType = event_type_tbl["龙穴活动奖励"]
	local tbl_item_id = {}
	for i=1,TongAwardCount do
		local building_id = GasTongBasicDB.AddTongBuild(TongId, TongAwardName, 0, 0, 3, 0)
		local item_id = g_RoomMgr.CreateItem(params)
		table.insert(tbl_item_id,item_id)
		AddDragonCaveBuilding(building_id,TongId)
		local g_LogMgr = RequireDbBox("LogMgrDB")
		g_LogMgr.TongItemProduceEventLog(TongId,TongAwardName,3,0)
	end	
	local nRes = cdepotMgr.AutoAddItems(GasTongBasicDB.GetTongDepotID(TongId),TongAwardId, TongAwardName,tbl_item_id,5)
	if IsNumber(nRes) then
		if nRes == 3 then
			local mailResult = SendItem(TongInitiator, TongAwardId, TongAwardName, TongAwardCount, SceneName)
			if not mailResult then
				LogErr("龙穴奖励 帮会物品奖励邮件出错", TongInitiator)
			end	
		else
			LogErr("龙穴奖励 帮会物品添加出现其他错误", TongId)
		end	
	end
	local Members = GasTongBasicDB.GetTongOnlineCharId(TongId)
	for _, CharId in pairs(Members) do
		local CharSceneId = SceneMgrDB.GetPlayerCurScene(CharId)
		if CharSceneId == SceneId then
			local params= {}
			params.nCharID = CharId
			params.nStoreMain = g_StoreRoomIndex.PlayerBag
			params.nBigID = AwardId
			params.nIndex = AwardName
			params.nCount = AwardCount
			params.m_nBindingType = g_ItemInfoMgr:GetItemInfo(AwardId, AwardName, "BindingStyle")
			params.createType = event_type_tbl["龙穴活动奖励"]
			local g_RoomMgr = RequireDbBox("GasRoomMgrDB")	
			local itemTbl = g_RoomMgr.AddItem(params)
			if IsNumber(itemTbl) then
				if itemTbl == 47 then
					local mailResult = SendItem(CharId, AwardId, AwardName, AwardCount, SceneName)
					if not mailResult then
						LogErr("龙穴奖励 背包面发邮件也失败!", CharId)
					end	
				else
					LogErr("龙穴奖励  添加物品出现 其他错误", CharId  .. "  " .. itemTbl)
				end	
			end
		end
	end

	local SceneResult = DragonCaveMgr.ReceiveSceneTbl:ExecStat(TongId)
	return SceneResult
end

function DragonCaveDB.DeleteDragonBuilding(data)
	local buildingTbl = DragonCaveMgr.CheckDragonBuildingState:ExecStat()
	local DragonBuildingTbl = {}
	if buildingTbl:GetRowNum()> 0 then
		for i=0,buildingTbl:GetRowNum()-1 do
			local buildingId = buildingTbl:GetData(i,0)
			local TongId = buildingTbl:GetData(i,1)
			local FinishTime,CurTime = os.date("*t", buildingTbl:GetData(i,2)),os.date("*t",buildingTbl:GetData(i,3))
			local build_name = DragonCaveMgr.SelectBuildingInfo:ExecStat(buildingId,TongId)
			if (CurTime.yday - FinishTime.yday) >= 7 then
				DragonCaveMgr.DeleteDragonBuilding:ExecStat(buildingId)
				table.insert(DragonBuildingTbl,{TongId,buildingId})
				local sBuildName = build_name:GetData(0,0)
				local sBuildLevel = build_name:GetData(0,1)
				local g_LogMgr = RequireDbBox("LogMgrDB")
				g_LogMgr.TongBuildingEventLog(TongId,sBuildName,sBuildLevel,2,0,0)
			end
		end
		buildingTbl:Release()
		return DragonBuildingTbl
	end
end

function DragonCaveDB.CheckDragonCaveList(data)
	local result = DragonCaveMgr.CheckPlayerState:ExecStat()
	local tongTbl = DragonCaveMgr.CheckTongState:ExecStat()
	local buildingTbl = DragonCaveMgr.CheckDragonBuildingState:ExecStat()
	if buildingTbl:GetRowNum()> 0 then
		for i=0,buildingTbl:GetRowNum()-1 do
			local buildingId = buildingTbl:GetData(i,0)
			local TongId = buildingTbl:GetData(i,1)
			local FinishTime,CurTime = os.date("*t", buildingTbl:GetData(i,2)),os.date("*t",buildingTbl:GetData(i,3))
			local build_name = DragonCaveMgr.SelectBuildingInfo:ExecStat(buildingId,TongId)
			if (CurTime.yday - FinishTime.yday) >= 7 then
				DragonCaveMgr.DeleteDragonBuilding:ExecStat(buildingId)
				local sBuildName = build_name:GetData(0,0)
				local sBuildLevel = build_name:GetData(0,1)
				local g_LogMgr = RequireDbBox("LogMgrDB")
				g_LogMgr.TongBuildingEventLog(TongId,sBuildName,sBuildLevel,2,0,0)
			end
		end
		buildingTbl:Release()
	end
	if result:GetRowNum() > 0 then
		local FinishTime,CurTime = os.date("*t", result:GetData(0,0)),os.date("*t",result:GetData(0,1)) 
		local Finishwday = (FinishTime.wday == 1 and 7 ) or FinishTime.wday - 1
		local Curwday= (CurTime.wday == 1 and 7 ) or CurTime.wday - 1
		if Finishwday == 7 and Curwday ~= 7 then
			DragonCaveMgr.DeleteDragonCave:ExecStat()
		elseif Finishwday ~= 7 and (Curwday - Finishwday) ~= (CurTime.yday - FinishTime.yday) then
			DragonCaveMgr.DeleteDragonCave:ExecStat()
		end
		result:Release()
	end
	if tongTbl:GetRowNum() > 0 then
		local FinishTime,CurTime = os.date("*t", tongTbl:GetData(0,0)),os.date("*t",tongTbl:GetData(0,1)) 
		local Finishwday = (FinishTime.wday == 1 and 7 ) or FinishTime.wday - 1
		local Curwday= (CurTime.wday == 1 and 7 ) or CurTime.wday - 1
		if Finishwday == 7 and Curwday ~= 7 then
			DragonCaveMgr.DeleteDragonCaveTong:ExecStat()
		elseif Finishwday ~= 7 and (Curwday - Finishwday) ~= (CurTime.yday - FinishTime.yday) then
			DragonCaveMgr.DeleteDragonCaveTong:ExecStat()
		end
		tongTbl:Release()
	end
end	
	
function DragonCaveDB.DeleteDragonCave(data)
	DragonCaveMgr.DeleteDragonCave:ExecStat()
	DragonCaveMgr.DeleteDragonCaveTong:ExecStat()
end

SetDbLocalFuncType(DragonCaveDB.SendAwardToTong)
SetDbLocalFuncType(DragonCaveDB.DeleteDragonCave)

return DragonCaveDB
