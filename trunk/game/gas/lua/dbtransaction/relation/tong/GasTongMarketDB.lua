cfg_load "tong/TongStartTime_Common"
local StmtOperater = {}	
gac_gas_require "framework/common/CMoney"
local g_MoneyMgr = CMoney:new()
local LogErr = LogErr
local TongStartTime_Common = TongStartTime_Common
local event_type_tbl = event_type_tbl
local GetStaticTextServer = GetStaticTextServer
local TransportStartTime = GetCfgTransformValue(false, "TongStartTime_Common", "佣兵团资源运输开始时间", "OpenTime") --资源运输开始时间
local TransportTimeLength = TongStartTime_Common("资源运输时长", "OpenTime") * 60 * 1000
------------------------------------
--闭包开始
local CTongMarketBox = CreateDbBox(...)

local StationModulus = {} --驻地系数
StationModulus[0] = 1
StationModulus[1] = 2
StationModulus[2] = 2.2
StationModulus[3] = 2.4
StationModulus[4] = 2.6

local StationProfferModulus = {} --驻地团贡奖励系数
StationProfferModulus[0] = 1
StationProfferModulus[1] = 1.2
StationProfferModulus[2] = 1.4
StationProfferModulus[3] = 1.6
StationProfferModulus[4] = 1.8

local StationLineModulus = {} --驻地团贡奖励系数
StationLineModulus[1] = 100
StationLineModulus[2] = 200
StationLineModulus[3] = 300
StationLineModulus[4] = 400
--------------------------------------------------------------------------------
local StmtDef = {
    	"_GetOderInfoByTongID",
    	[[ 
    		select tm_uPrice,tm_uBuyNumber from tbl_tong_market  where t_uId = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_GetOderInfo",
    	[[ 
    		select T.t_sName,M.tm_uPrice,M.tm_uBuyNumber from tbl_tong_market as M,tbl_tong as T
    	 where M.t_uId = T.t_uId 
    	 and T.t_uId = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_GetAllOderInfo",
    	[[ 
    		select T.t_sName,M.t_uId,M.tm_uPrice,M.tm_uBuyNumber from tbl_tong_market as M,tbl_tong as T
    	 where M.t_uId = T.t_uId
    	 and T.t_uCamp = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_AddOrderInfo",
    	[[ 
    		replace into tbl_tong_market(t_uId,tm_uPrice,tm_uBuyNumber) values(?,?,?)
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_DeleteOrderInfo",
    	[[ 
    		delete from tbl_tong_market where t_uId = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_DelTongOrder",
    	[[ 
    		delete from tbl_tong_market where t_uId = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_GetPlayerLevel",
    	[[ 
    		select cb_uLevel from tbl_char_basic
    		where cs_uId = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )


--------------------------------------------RPC调用相关-------------------------------------------------
local StmtDef = {
    "_AddStationTime",
    "insert into tbl_tong_station_time (t_uId, tst_uStationId, tst_uWsId) values(?,?,?)"
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    "_GetStationInfo",
    "select tst_uStationId from tbl_tong_station_time where t_uId = ?"
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    "_GetTongByStaion",
    "select t_uId from tbl_tong_station_time where tst_uStationId = ? and tst_uWsId = ?"
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    "_GetStatTongInfo",
    "select t_uId from tbl_tong_station_time"
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    "_ChangeStationInfo",
    "update tbl_tong_station_time set tst_uStationId = ?, tst_uWsId = ? where t_uId = ?"
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    "_ClearStationTime",
    "delete from tbl_tong_station_time"
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    "_DeleteStationTime",
    "delete from tbl_tong_station_time where tst_uStationId = ? and t_uId = ?"
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    "_AddLineTime",
    "insert into tbl_tong_line_time (t_uId, tlt_uLineId, tlt_uWsId) values(?,?,?)"
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    "_GetLineTime",
    "select tlt_uTime from tbl_tong_line_time where tlt_uLineId = ? and t_uId = ?"
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    "_GetLineInfo",
    "select tlt_uLineId from tbl_tong_line_time where t_uId = ?"
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    "_UpdateLineWar",
    "update tbl_tong_line_time set tlt_uWsId = ? where t_uId = ?"
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    "_UpdateLineTime",
    "update tbl_tong_line_time set tlt_uTime = ? + 1 where tlt_uLineId = ? and t_uId = ?"
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    "_GetLineWarZone",
    "select count(*) from tbl_tong_line_time where t_uId = ? and tlt_uWsId  = ?"
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    "_ChangeLineInfo",
    "update tbl_tong_line_time set tlt_uLineId = ?, tlt_uWsId = ? where t_uId = ?"
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    "_ClearLineTime",
    "delete from tbl_tong_line_time"
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    "_DeleteLineTime",
    "delete from tbl_tong_line_time where tlt_uLineId = ? and t_uId = ?"
}    
DefineSql ( StmtDef, StmtOperater )


local StmtDef = {
    "_DeleteLineTimeById",
    "delete from tbl_tong_line_time where t_uId = ?"
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    "_DeleteStatTimeById",
    "delete from tbl_tong_station_time where t_uId = ?"
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    "_GetLineAndTime",
    "select tlt_uLineId, tlt_uTime from tbl_tong_line_time where t_uId = ?"
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    "GetQueryInfo",
    [[select tst.t_uId, tlt.tlt_uLineId, tlt.tlt_uTime from tbl_tong_station_time tst,
    tbl_tong_line_time tlt where tst.t_uId = tlt.t_uId]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    "_ResetLineInfo",
    "update tbl_tong_line_time set tlt_uTime = 0 where tlt_uLineId = ? and t_uId = ?"
}    
DefineSql ( StmtDef, StmtOperater )


local StmtDef = {
    "_ResetStationInfo",
    "update tbl_tong_station_time set tst_uTime = 0 where  tst_uStationId = ? and  t_uId = ?"
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
   "_UpdateStationTime",
    "update tbl_tong_station_time set tst_uTime = ? + 1 where tst_uStationId = ? and t_uId = ?"
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
   "_AddIssueRecord",
    "replace into tbl_tong_issue_record (t_uId, tir_uNum) values (?,?)"
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
   "_GetIssueRecord",
    "select tir_uNum from tbl_tong_issue_record where t_uId = ?"
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
   "_ClearIssueRecord",
    "delete from tbl_tong_issue_record"
}    
DefineSql ( StmtDef, StmtOperater )



--------------------------------------------RPC调用相关-------------------------------------------------
local function GetLineNum(lineId, tongId)
	local result = StmtOperater._GetLineTime:ExecStat(lineId, tongId)
	if result:GetRowNum() > 0 then
		local transTime = result:GetData(0,0)
		if transTime < StationLineModulus[lineId] then
			return true
		end
	end
end

--[征粮令用]
function CTongMarketBox.AddStationTime(parameters)
	local CTongMarketBox = RequireDbBox("GasTongBasicDB")
	local tongId = parameters["tongId"]
	local result = StmtOperater._GetStationInfo:ExecStat(tongId)
	local res = StmtOperater._GetLineInfo:ExecStat(tongId)
	local stationId = parameters["uStationId"]
	local warZoneId = parameters["warZoneId"]
	local data = {}
	data["TongID"] = tongId
	local camp = CTongMarketBox.GetTongCampByIDRpc(data)
	local lineId = parameters["uLineId"]
	local resultNum = result:GetRowNum()
	local resNum = res:GetRowNum()
	if resultNum ~= 0 and resNum ~= 0 then
		for i = 0, resultNum - 1 do
			if result:GetData(i, 0) == stationId then
				if GetLineNum(lineId, tongId) then
					return 9349
				end
			end
		end
		for i = 0, resNum - 1 do
			if res:GetData(i, 0) == lineId then
				if GetLineNum(lineId, tongId) then
					return 9349
				end
			end
		end
		StmtOperater._ResetStationInfo:ExecStat(stationId, tongId)
		StmtOperater._ResetLineInfo:ExecStat(lineId, tongId)
	else
		local recordTbl = StmtOperater._GetIssueRecord:ExecStat(tongId)
		if recordTbl:GetRowNum() > 0 and recordTbl:GetData(0,0) >= 5 then
			return 9373
		end
		StmtOperater._AddStationTime:ExecStat(tongId, stationId, warZoneId)
		StmtOperater._AddLineTime:ExecStat(tongId, lineId, warZoneId)
	end
	
	if not (g_DbChannelMgr:LastAffectedRowNum() > 0) then
		CancelTran()
		return
	end
end

function CTongMarketBox.JudgeStationTime(parameters)
	local CTongMarketBox = RequireDbBox("GasTongBasicDB")
	local tongId = parameters["tongId"]
	local stationId = parameters["stationId"]
	local WarZone = RequireDbBox("WarZoneDB")
	local result = StmtOperater._GetStationInfo:ExecStat(tongId)
	local res = StmtOperater._GetLineAndTime:ExecStat(tongId)
	local data = {}
	local time = 0 
	local lineId = 0
	if res:GetRowNum() > 0 then
		time = res:GetData(0,1)
		lineId = res:GetData(0,0)
	end
	data["tongId"] = tongId
	if result:GetRowNum() > 0 then
		local resStation = result:GetData(0,0)
		if resStation ~= stationId then
			local sceneType, sceneId, serverId, warZoneId, stationId = WarZone.GetTongStationInfo(data)
			local line = WarZone.GetTongRelativeLine(tongId)
			if time >= StationLineModulus[line] then
				StmtOperater._DeleteStationTime:ExecStat(resStation, tongId)
				StmtOperater._DeleteLineTime:ExecStat(lineId, tongId)
				return 
			end
			StmtOperater._ChangeStationInfo:ExecStat(stationId, warZoneId, tongId)
			StmtOperater._ChangeLineInfo:ExecStat(line,warZoneId, tongId)
		end
	end 
end

function CTongMarketBox.ClearStationTime(tongId)
	local result = StmtOperater._GetStationInfo:ExecStat(tongId)
	local res = StmtOperater._GetLineInfo:ExecStat(tongId)
	if result:GetRowNum() > 0 and res:GetRowNum() > 0 then
		StmtOperater._DeleteStatTimeById:ExecStat(tongId)
		StmtOperater._DeleteLineTimeById:ExecStat(tongId)
	end
end

local function GetOffsetSecond(strTime)
	local index1 = string.find(strTime, ":")
	local index2 = string.find(strTime, ":", index1 + 1) or 0
	local hour = tonumber(string.sub(strTime, 1, index1 - 1))
	local min = tonumber(string.sub(strTime, index1 + 1, index2 -1))
	local sec = 0
	if index2 ~= 0 then
		sec = tonumber(string.sub(strTime, index2 + 1, -1))
	end
	assert(hour and min and sec, "时间格式错误")
	return (hour * 60 + min) * 60  + sec
end

local function IsInOpenTime(data, length)
	local curDate =  os.date("*t")
	local todayOffsetSecond = (curDate.hour * 60 + curDate.min) * 60 + curDate.sec
	local offset = GetOffsetSecond(data.time[1])
	curDate.wday = ((curDate.wday == 1) and 7 or curDate.wday-1)  --将系统星期换成填表星期
	local wdayTbl = data.wday
	for _, wday in pairs(wdayTbl) do
		local endOffset = offset + length / 1000
		local day = (curDate.wday >= wday and curDate.wday - wday) or curDate.wday + 7 - wday --跨越天数
		local curOffsetSecond = todayOffsetSecond + day * 86400
		if curOffsetSecond >= offset and curOffsetSecond < endOffset then
				return true, endOffset - curOffsetSecond
		end
	end
	return false
end

local function GetData(tongTbl)
	local CTongMarketBox = RequireDbBox("GasTongBasicDB")
	local WarZone = RequireDbBox("WarZoneDB")
	local restbl = {}
	for _, tongId in pairs(tongTbl) do
		local res = StmtOperater._GetLineAndTime:ExecStat(tongId)
		if res:GetRowNum() > 0 then
			for i = 0, res:GetRowNum() - 1 do
				local lineId = res:GetData(i, 0)
				local time = res:GetData(i, 1)
				if time < StationLineModulus[lineId] then
					local resStat = StmtOperater._GetStationInfo:ExecStat(tongId)
					if resStat:GetRowNum() > 0 then
						local data = {}
						data["TongID"] = tongId
						local camp = CTongMarketBox.GetTongCampByIDRpc(data)
						local wzId, wzsId = WarZone.GetTongWzAndWzs(tongId)
						local stationId = resStat:GetData(0,0)
						if wzsId ~= nil and stationId == wzsId then
							table.insert(restbl,{stationId, camp, wzId})
						end
					end
				end
			end
		end
	end
	return restbl
end

function CTongMarketBox.GetStationTimeInfo(data)
	local WarZone = RequireDbBox("WarZoneDB")
	local CTongBasicBox = RequireDbBox("GasTongBasicDB")
	local tongTbl = CTongMarketBox.GetStatTongInfo()
	local resTbl = {}
	local dataTbl = {}
	local temptbl = {}
	local tbl = WarZone.GetTongStationOnly() --其他帮会驻地(花语平原驻地)
	local SceneMgrDB = RequireDbBox("SceneMgrDB")
	local sceneType, sceneId, serverId, warZoneId, stationId = WarZone.GetTongStationInfo({["tongId"] = data["tongId"]}) --自己驻地
	
	local sceneName = nil
	
	if sceneType then
		sceneName = SceneMgrDB._GetSceneNameById(sceneId)
	end
	local result = GetData(tongTbl) --征粮令
	if result then
		for _, resStatTbl in pairs(result) do
			local index = resStatTbl[1]..","..resStatTbl[3]
			dataTbl[index] = true
			table.insert(temptbl,resStatTbl)
		end
	end
	local isOpenTime, length = IsInOpenTime(TransportStartTime, TransportTimeLength)
	if isOpenTime then
		if tbl then  --所有
			for _, infoTbl in pairs(tbl) do  --stationId驻地, warId战线
				local tempStationId = infoTbl[1]
				local wzId = infoTbl[2]
				local Index = tempStationId..","..wzId
				if dataTbl[Index] ~= true then
					dataTbl[Index] = true
					local tempIdTbl = StmtOperater._GetTongByStaion:ExecStat(tempStationId, wzId)
					local camp = 0
					if tempIdTbl:GetRowNum() > 0 then
						local id = tempIdTbl:GetData(0,0)
						local data = {}
						data["TongID"] = id
						camp = CTongBasicBox.GetTongCampByIDRpc(data)
					end
					table.insert(temptbl,{tempStationId, camp, wzId})
				end
			end
		end
	end
	return sceneName, sceneType,stationId, temptbl,warZoneId
end

function CTongMarketBox.GetFbStationInfo(data)
	local WarZone = RequireDbBox("WarZoneDB")
	local sceneType, sceneId, serverId, sceneName, warZoneId, stationId = WarZone.GetTongStationInfoById(data) --自己驻地
	return sceneName
end

function CTongMarketBox.DeleteIssueInfo()
	StmtOperater._ClearLineTime:ExecStat()
	StmtOperater._ClearStationTime:ExecStat()
	StmtOperater._ClearIssueRecord:ExecStat()
end

function CTongMarketBox.GetStatTongInfo()
	local result = StmtOperater._GetStatTongInfo:ExecStat()
	local resTbl = {}
	if result:GetRowNum() > 0 then
		for i = 0, result:GetRowNum() - 1 do
			table.insert(resTbl, result:GetData(i,0))
		end
	end
	return resTbl
end

function CTongMarketBox.GetStationTime(parameters)
	local lineId  = parameters["uLineId"]
	local tongId = parameters["uTongID"]
	local WarZone = RequireDbBox("WarZoneDB")
	local result = StmtOperater._GetLineTime:ExecStat(lineId, tongId)
	local statRes = StmtOperater._GetStationInfo:ExecStat(tongId) 
	local stationId = nil
	if statRes:GetRowNum() > 0 then
		stationId = statRes:GetData(0,0)
	end
	if lineId ~= 0  then 
		if result:GetRowNum() > 0 then
			local transTime = result:GetData(0,0)
			if transTime >= StationLineModulus[lineId] then
				return 9347
			end
			if transTime + 1 >= StationLineModulus[lineId] then
				local recordTbl = StmtOperater._GetIssueRecord:ExecStat(tongId)
				local num = 1
				if recordTbl:GetRowNum() > 0 then
					num = recordTbl:GetData(0,0) + 1
				end
				StmtOperater._AddIssueRecord:ExecStat(tongId, num)
				StmtOperater._DeleteLineTime:ExecStat(lineId, tongId)
				StmtOperater._DeleteStationTime:ExecStat(stationId, tongId)
				return
			end
			local data = {}
			data["tongId"] = tongId
			
			local sceneType, sceneId, serverId, warZoneId, stationId = WarZone.GetTongStationInfo(data)
			local warRes = StmtOperater._GetLineWarZone:ExecStat(tongId, warZoneId)
			if warRes:GetData() == 0 then
				StmtOperater._UpdateLineWar:ExecStat(warZoneId, tongId)
			end
			StmtOperater._UpdateLineTime:ExecStat(transTime , lineId, tongId)
		end
	end
end


--[出售信息]
function CTongMarketBox.SellTongForageInfoDB(parameters)
	local GasTongBasicDB = RequireDbBox("GasTongBasicDB")
	local WarZoneDB = RequireDbBox("WarZoneDB")
	local tongId = parameters["uTongID"]
	local SellResNum = parameters["SellResNum"]
	local level = parameters["Level"]
	local state = parameters["state"]
	local proffer = 0 
	if not tongId then   --自然Npc
		proffer = SellResNum * level *5 /10
		return {true, SellResNum, proffer}
	end
	if state ~= "Transporting" then
		local res = StmtOperater._GetStationInfo:ExecStat(tongId)
		if res:GetRowNum() <= 0 then
			return {false, 9042}
		end
	end
	local parameter = {}
	parameter["uTongID"] = tongId
	parameter["uResource"] = SellResNum
	parameter["nEventType"] = event_type_tbl["佣兵小队粮草出售"]
	local nAddResource = GasTongBasicDB.GetTongResource(parameter["uTongID"])
	local uMaxCapbility = GasTongBasicDB.GetUpperLimitResource(parameter["uTongID"])
	local nLine = WarZoneDB.GetTongRelativeLine(tongId)
	local profferModules = 0
	if nLine == nil then
		profferModules = 1
	else
		profferModules = StationProfferModulus[nLine]
	end
	
	if nAddResource + SellResNum <= uMaxCapbility then
		proffer = SellResNum * level * 5 * profferModules/10
		return {true, SellResNum,proffer}
	else
		proffer = (uMaxCapbility - nAddResource)* level * 5 * profferModules/10
		return {true, uMaxCapbility - nAddResource,proffer}
	end
end

--【出售粮草】
function CTongMarketBox.SellTongResourceDB(parameters)
	local AreaFbPointDB = RequireDbBox("AreaFbPointDB")
	local tongOtherDB = RequireDbBox("GasTongOtherDB")
	local tong_box = RequireDbBox("GasTongBasicDB")
	local WarZoneDB = RequireDbBox("WarZoneDB")
	local TongID = parameters["TongID"]
	local uPlayerID = parameters["uPlayerID"]
	local uSellNum = parameters["uSellNum"]
	local uCamp = parameters["uCamp"]
	local sceneName = parameters["sceneName"]
	local state = parameters["state"]
	local camp = tong_box.GetTongCampByIDRpc(parameters)
	local uTongID = tong_box.GetTongID(uPlayerID)
	local nLine = WarZoneDB.GetTongRelativeLine(TongID)
	local CurLevel = 0
	local proffer = 0
	local profferFlag = false
	local tempdata = {}
	tempdata["type"] = 1
	tempdata["charId"] = parameters["uPlayerID"]
	local res = StmtOperater._GetPlayerLevel:ExecStat(uPlayerID)
	if res:GetRowNum() > 0 then
		CurLevel = res:GetData(0,0)
	end
	if TongID == 0 then
		tempdata["nAddResource"] = uSellNum
		proffer = uSellNum * CurLevel *5 /10
		profferFlag = AreaFbPointDB.AddAreaFbPointByType(uPlayerID,proffer,10,sceneName,nil,event_type_tbl["佣兵小队粮草出售"])
		tongOtherDB.AddCharData(tempdata)
		return true, 0, profferFlag, proffer
	end
	if camp ~= uCamp then
		return false, 9018
	end 
	local prameter = {}
	prameter["uTongID"] = TongID
	prameter["uResource"] = uSellNum
	prameter["nEventType"] = event_type_tbl["佣兵小队粮草出售"]
	local nFlag, nAddResource = tong_box.AddTongResource(prameter)
	tempdata["nAddResource"] = nAddResource
	if nFlag then
		proffer = nAddResource * CurLevel *  5 * StationProfferModulus[nLine]/10
		if nAddResource > 0 then
			profferFlag = AreaFbPointDB.AddAreaFbPointByType(uPlayerID,proffer,10,sceneName,nil,event_type_tbl["佣兵小队粮草出售"])
			if state ~= "Transporting" then  --加军资运输次数
				local data = {}
				data["uLineId"] = nLine
				data["uTongID"] = TongID
				local tempres = CTongMarketBox.GetStationTime(data)
				if IsNumber(tempres) then
					return false, tempres
				end
			end
		end
		tongOtherDB.AddCharData(tempdata)
		return true, uSellNum - nAddResource, profferFlag, proffer
	else
		CancelTran()
		return false, 9027
	end
end

--【查询发布征粮令信息】
function CTongMarketBox.GetQueryInfo(data)
	local camp = data[1]
	local CTongBasicBox = RequireDbBox("GasTongBasicDB")
	local WarZone = RequireDbBox("WarZoneDB")
	local result = StmtOperater.GetQueryInfo:ExecStat()
	if result:GetRowNum() <= 0 then
		return 
	end
	local resTbl = {}
	for i = 0, result:GetRowNum() - 1 do
		local tongId = result:GetData(i, 0)
		local tongName = CTongBasicBox.GetTongNameById(tongId)
		local tongCamp = CTongBasicBox.GetTongCampByID(tongId)
		
		local warZoneId, stationId = WarZone.GetTongWzAndWzs(tongId)
		local lineId = result:GetData(i, 1)
		local time = result:GetData(i, 2)
		if tongCamp == camp then
			table.insert(resTbl, {tongName, warZoneId, stationId, lineId, time})
		end
	end
	return resTbl
end

function CTongMarketBox.SetFetchTimes(data)	
	local CTongBasicBox = RequireDbBox("GasTongBasicDB")
	local charId = data["charId"]
	local num = data["num"]
	local tongId = CTongBasicBox.GetTongID(charId)
	local lineRes = StmtOperater._GetLineInfo:ExecStat(tongId)
	local statRes = StmtOperater._GetStationInfo:ExecStat(tongId)
	local line, stationId = 0,0
	if lineRes:GetRowNum() > 0 then
		line = lineRes:GetData(0,0)
	end
	if statRes:GetRowNum() > 0 then
		stationId = statRes:GetData(0,0)
	end
	if not StationLineModulus[line] then
		return 9048
	end
	if num + 1 >= StationLineModulus[line] then
		return "Over Max Value"
	end
	StmtOperater._UpdateLineTime:ExecStat(num, line, tongId)
	StmtOperater._UpdateStationTime:ExecStat(num, stationId, tongId)
	if g_DbChannelMgr:LastAffectedRowNum() > 0 then
		return "Fetch Succ"
	else
		return "Fetch Fail"
	end
end

---------------------------------
return CTongMarketBox
