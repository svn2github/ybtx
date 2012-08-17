cfg_load "fb_game/FbRobRes_Server" 
cfg_load "tong/TongStartTime_Common"
gac_gas_require "relation/tong/TongMgr"
gac_gas_require "framework/common/CMoney"
local StmtRobResource = class()
local FbRobRes_Server = FbRobRes_Server
local g_MoneyMgr = CMoney:new()
local MemH64 = MemH64
local TongStartTime_Common = TongStartTime_Common
local GetCfgTransformValue = GetCfgTransformValue
local StartSignTime = GetCfgTransformValue(false, "TongStartTime_Common", "抢夺资源点报名时间", "OpenTime")  --玩家可以开始报名的时间(分)
local WaitJointTime = TongStartTime_Common("抢夺资源点报名时长", "OpenTime")   		--可以继续报名的时间(分)
local WaitEnterSceneTime = TongStartTime_Common("抢夺资源点进场时长", "OpenTime")	--玩家可以进场的时间(秒)
local GameLength = TongStartTime_Common("抢夺资源点游戏时长", "OpenTime")
local event_type_tbl = event_type_tbl
local g_TongMgr = CTongMgr:new()
local GasRobRes = CreateDbBox(...)


local StmtDef=
{
 	"SelectAll",
	"select trr_uId from tbl_tong_rob_res"
}
DefineSql(StmtDef,StmtRobResource) 

local StmtDef=
{
 	"_SelectObjName",
	"select trr_sObjName from tbl_tong_rob_res"
}
DefineSql(StmtDef,StmtRobResource) 

local StmtDef=
{
 	"SelectObjNameByTongId",
	"select trr_sObjName from tbl_tong_rob_res where trr_uId = ?"
}
DefineSql(StmtDef,StmtRobResource)

local StmtDef=
{
 "GetSameTempRobResTongCount",
 "select count(*) from tbl_tong_rob_resource_temp where trrt_sObjName = ?"
}
DefineSql(StmtDef,StmtRobResource)

local StmtDef=
{
 "GetIdByObjName",
 "select trrt_uId from tbl_tong_rob_resource_temp where trrt_sObjName = ?"
}
DefineSql(StmtDef,StmtRobResource)

local StmtDef=
{
 "AddRobResTong",
 "insert into tbl_tong_rob_resource(trr_sObjName, trr_uId) values(?,?)"
}
DefineSql(StmtDef,StmtRobResource)


local StmtDef=
{
 "GetRobResTong",
 "select count(*) from tbl_tong_rob_resource where trr_uId = ?"
}
DefineSql(StmtDef,StmtRobResource)

local StmtDef=
{
 	"UpdateRobResTong",
	"update tbl_tong_rob_resource set trr_sObjName = ? where trr_uId = ?"
}
DefineSql(StmtDef,StmtRobResource)

local StmtDef=
{
 	"UpdateSignTong",
	"update tbl_tong_rob_res set trr_uId = ? ,trr_sName = ? where trr_sObjName = ? and trr_uTongCamp = ?" 
}
DefineSql(StmtDef,StmtRobResource)


local StmtDef=
{
 "_AddSceneInfo",
 "insert into tbl_tong_rob_scene(trse_sObjName, trse_uSceneId, trse_uServerId) values(?, ?, ?)"
}
DefineSql(StmtDef,StmtRobResource)

local StmtDef=
{
 "_GetSceneInfo",
 "select trse_uSceneId, trse_uServerId from tbl_tong_rob_scene where trse_sObjName = ?"
}
DefineSql(StmtDef,StmtRobResource)

local StmtDef=
{
 "_UpdateSceneInfo",
 "update tbl_tong_rob_scene set trse_uSceneId = ?,trse_uServerId = ? where trse_sObjName = ?"
}
DefineSql(StmtDef,StmtRobResource)

local StmtDef=
{
 "_GetAllSceneInfo",
 "select trse_uSceneId, trse_uServerId from tbl_tong_rob_scene"
}
DefineSql(StmtDef,StmtRobResource)

local StmtDef=
{
 "_GetSceneObj",
 "select trse_sObjName, trse_uSceneId from tbl_tong_rob_scene"
}
DefineSql(StmtDef,StmtRobResource)

local StmtDef=
{
 "_DeleteSceneInfo",
 "delete from tbl_tong_rob_scene"
}
DefineSql(StmtDef,StmtRobResource)

local StmtDef=
{
 "_AddRobResState",
 "insert into tbl_tong_rob_state(trs_sState) values(?)"
}
DefineSql(StmtDef,StmtRobResource)

local StmtDef=
{
 "_UpdateRobResState",
 "update tbl_tong_rob_state set trs_sState = ?"
}
DefineSql(StmtDef,StmtRobResource)

local StmtDef=
{
 "_GetRobResState",
 "select trs_sState from tbl_tong_rob_state"
}
DefineSql(StmtDef,StmtRobResource)

local StmtDef=
{
 "_DeleteRobResState",
 "delete from tbl_tong_rob_state"
}
DefineSql(StmtDef,StmtRobResource)

local StmtDef=
{
 	"_SelectAllTongId",
	"select trr_uId from tbl_tong_rob_res"
}
DefineSql(StmtDef,StmtRobResource)

local StmtDef=
{
 	"SelectOtherTongId",
	"select trr_uId from tbl_tong_rob_res where trr_sObjName = ? and trr_uTongCamp = ?"
}
DefineSql(StmtDef,StmtRobResource)

local StmtDef=
{
 	"AddSignUpTong",
	"insert into tbl_tong_rob_res(trr_sObjName, trr_uId, trr_sName, trr_uTongCamp) values(?,?,?,?)"
}
DefineSql(StmtDef,StmtRobResource)

local StmtDef=
{
 	"_GetHeaderAwardInfo",
	"select count(*) from tbl_tong_header_award_time where t_uId = ?"
}
DefineSql(StmtDef,StmtRobResource)

local StmtDef=
{
 	"_UpdateHeaderAwardInfo",
	"replace into tbl_tong_header_award_time(t_uId, cs_uId, that_dtTime) values(?,?,now())"
}
DefineSql(StmtDef,StmtRobResource)

local StmtDef=
{
 	"_AddRobResLog",
	"insert into tbl_tong_rob_res_log(t_uId, trrl_sObjName) values(?,?)"
}
DefineSql(StmtDef,StmtRobResource)

local StmtDef=
{
 	"_GetRobResLog",
	"select count(*) from tbl_tong_rob_res_log where t_uId = ? and trrl_sObjName = ?"
}
DefineSql(StmtDef,StmtRobResource)

local StmtDef=
{
 	"_DelHeaderAwardInfo",
	"delete from tbl_tong_header_award_time"
}
DefineSql(StmtDef,StmtRobResource)

local StmtDef=
{
 	"_AddClearAwardTime",
	"insert into tbl_tong_res_award_time values(now())"
}
DefineSql(StmtDef,StmtRobResource)

local StmtDef=
{
 	"_GetClearAwardTime",
	"select unix_timestamp(rat_dtTime) from tbl_tong_res_award_time"
}
DefineSql(StmtDef,StmtRobResource)

local StmtDef=
{
 	"_UpdateClearAwardTime",
	"Update tbl_tong_res_award_time set rat_dtTime = now()"
}
DefineSql(StmtDef,StmtRobResource)

local StmtDef=
{
 	"_DelClearAwardTime",
	"delete from tbl_tong_res_award_time"
}
DefineSql(StmtDef,StmtRobResource)

local function GetInfoForLog()
	local tong_box = RequireDbBox("GasTongBasicDB")
	local result = StmtRobResource._SelectAllTongId:ExecStat() 
	if result:GetRowNum() > 0 then
		for i = 0, result:GetRowNum() - 1 do
			local tongId = result:GetData(i,0)
			local res = StmtRobResource.SelectObjNameByTongId:ExecStat(tongId) 
			if res:GetRowNum() > 0 then
				local objName = res:GetData(0,0)
				local count = StmtRobResource._GetRobResLog:ExecStat(tongId, objName)
				if count:GetData(0,0) ~= 0 then
					return 
				end
				StmtRobResource._AddRobResLog:ExecStat(tongId, objName)
				local sMsg = "2035_objname:" .. objName
 				tong_box.AddTongLogs(tongId,sMsg, g_TongMgr:GetLogType("战争"))
			end
		end
	end
end

local function SelectObjName()
	local result = StmtRobResource._SelectObjName:ExecStat()
	local tbl = {}
	if result:GetRowNum() > 0 then
		for i = 0 , result:GetRowNum() - 1 do
			if not tbl[result:GetData(i,0)] then
				tbl[result:GetData(i,0)] = true
			end
		end
	end
	return tbl
end

local function AddSceneInfo(objName, sceneName)

	local SceneMgrDB = RequireDbBox("SceneMgrDB")
	local otherArg = {}
	otherArg["objName"] = objName
	local data = {}
	data["SceneName"] = sceneName
	data["OtherArg"] = otherArg
	--检测此obj是否有对应的 场景
	local res = StmtRobResource._GetSceneInfo:ExecStat(objName)
	if res:GetRowNum() <= 0 then
		local sceneId, serverId = SceneMgrDB._CreateScene(data)
		StmtRobResource._AddSceneInfo:ExecStat(objName, sceneId, serverId)
		return sceneId, serverId
	end
end

local function CreateScene()
	local objNameTbl = SelectObjName()
	local tbl = {}
	for objName, _ in pairs(objNameTbl) do
		local sceneId, serverId = AddSceneInfo(objName, "抢夺资源点")
		table.insert(tbl, {sceneId, serverId})
	end
	return tbl
end

local function GetSceneInfo(objName)

	local result = StmtRobResource._GetSceneInfo:ExecStat(objName)
	local num = result:GetRowNum()
	if num > 0 then
		local sceneId = result:GetData(0,0)
		local serverId = result:GetData(0,1)
		return sceneId,serverId
	end
end

local function GetAllSceneInfo()
	local result = StmtRobResource._GetAllSceneInfo:ExecStat()
	local num = result:GetRowNum()
	if num > 0 then
		local sceneId = result:GetData(0,0)
		local serverId = result:GetData(0,1)
		return sceneId,serverId
	end
end


local function UpdateSceneInfo(objName, sceneName)
	local SceneMgrDB = RequireDbBox("SceneMgrDB")
	local otherArg = {}
	otherArg["objName"] = objName
	local data = {}
	data["SceneName"] = sceneName
	data["OtherArg"] = otherArg
	local sceneId, serverId = SceneMgrDB._CreateScene(data)
	StmtRobResource._UpdateSceneInfo:ExecStat(sceneId, serverId, objName)
	return sceneId, serverId
end

local function SelectAllTongId()
	local tong_box = RequireDbBox("GasTongBasicDB")
	local result = StmtRobResource._SelectAllTongId:ExecStat()
	local tbl = {}
	local num = result:GetRowNum() 
	if num > 0 then
		for i = 0, num - 1 do
			local tongId = result:GetData(i,0)
			local charTbl = tong_box.GetTongOnlineByPos(tongId) 
			for _, charId in pairs(charTbl) do
				table.insert(tbl, charId)
			end
		end
	end
	return tbl
end

--local function GetSceneInfo(objName)
--	local result = StmtRobResource._GetSceneInfo(objName)
--	local num = result:GetRowNum()
--	if num > 0 then
--		local sceneId = result:GetData(0,0)
--		return sceneId
--	end
--end

local function SelectSceneInfo()
	local tong_box = RequireDbBox("GasTongBasicDB")
	local SceneMgrDB = RequireDbBox("SceneMgrDB")
	local GasTongRobResDB = RequireDbBox("GasTongRobResourceDB")
	local result = StmtRobResource._SelectAllTongId:ExecStat()
	local tbl = {}
	local num = result:GetRowNum() 
	if num > 0 then
		for i = 0, num - 1 do
			local tongId = result:GetData(i,0)
			local camp = tong_box.GetTongCampByID(tongId)
			local data = {}
			data["tongId"] = tongId
			local flag, objName = GasTongRobResDB.SelectObjNameByTongId(data)
			local sceneId, serverId = GetSceneInfo(objName)
			local charTbl = tong_box.GetTongOnlineCharId(tongId) 
			for _, charId in pairs(charTbl) do
				table.insert(tbl, {charId, sceneId, serverId, camp})
			end
		end
	end
	return tbl
end

local function AddRobResState(state)
	local res = StmtRobResource._GetRobResState:ExecStat()
	if res:GetRowNum() > 0 then
		StmtRobResource._UpdateRobResState:ExecStat(state)
		return 
	end
	StmtRobResource._AddRobResState:ExecStat(state)
end

--local function JudgeRobResState(state)
--	local res = StmtRobResource._GetRobResState:ExecStat()
--	if res:GetRowNum() > 0 then
--		StmtRobResource._UpdateRobResState:ExecStat(state)
--		return 
--	end
--	StmtRobResource._AddRobResState:ExecStat(state)
--end

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
	local curDate = os.date("*t")
	local week = curDate.wday - 1
	for _, i in pairs(data.wday) do
		if week == i then
			local hour = curDate.hour
			local minute = curDate.min
			local second = curDate.sec
			local todayCurSecond = (hour * 60 + minute) * 60 + second 
			local offSet = GetOffsetSecond(data.time[1])
			local endOffSet = offSet + length * 60
			if todayCurSecond >= offSet and todayCurSecond <= endOffSet then
				return true, endOffSet - todayCurSecond
			end
		end
	end
	return false
end

local function GetSignOverTime(strTime, strTime1)
	local index1 = string.find(strTime, ":")
	local index2 = string.find(strTime, ":", index1 + 1) or 0
	local hour = tonumber(string.sub(strTime, 1, index1 - 1))
	local min = tonumber(string.sub(strTime, index1 + 1, index2 -1))
	if min + strTime1 >= 60 then
		hour = (min + strTime1) / 60 + hour
		min = (min + strTime1) % 60
	else
		hour = hour
		min = min + strTime1
	end
	local str = math.floor(hour)..":"..math.floor(min)
	return str
end

local function IsOpen()

	local RobResource = RequireDbBox("GasTongRobResourceDB")
	local chardb = RequireDbBox("CharacterMediatorDB")
	local isOpenLastTime, lengthBegin = IsInOpenTime(StartSignTime, (WaitJointTime + GameLength + WaitEnterSceneTime/60))
	local type = nil
	local tbl = {}
	if not isOpenLastTime then
		StmtRobResource._DeleteSceneInfo:ExecStat()
		StmtRobResource._DeleteRobResState:ExecStat()
		RobResource.DeleteTongAllInfo()
		
		return 
	end
	local objNameTbl = StmtRobResource._GetSceneObj:ExecStat()
	if objNameTbl:GetRowNum() > 0 then
		for i = 0, objNameTbl:GetRowNum() - 1 do
			local objName = objNameTbl:GetData(i,0)
			local sceneOldId = objNameTbl:GetData(i,1)
			local param = {}
			param["sceneId"] = sceneOldId
			if not chardb.IsSceneExist(param) then
				local sceneId ,serverId = UpdateSceneInfo(objName, "抢夺资源点")
				table.insert(tbl,{sceneId, serverId, objName})
			end
		end
	end
	
	local enterTime = {}
	local isOpenSignTime, lengthSign = IsInOpenTime(StartSignTime, WaitJointTime)
	if isOpenSignTime then
		type = "Signning" 
		enterTime = GetSignOverTime(StartSignTime.time[1],WaitJointTime)
		AddRobResState(type)
		return tbl, enterTime, type
	end
	
	local isOpenWaitingTime, lengthWait = IsInOpenTime(StartSignTime, WaitJointTime + WaitEnterSceneTime/ 60)
	if isOpenWaitingTime then
		type = "Waiting" 
		enterTime = GetSignOverTime(StartSignTime.time[1],WaitJointTime + WaitEnterSceneTime/ 60)
		AddRobResState(type)
		return tbl, enterTime, type
	end
	
	local isOpenPlayTime, lengthPlay = IsInOpenTime(StartSignTime, WaitJointTime + GameLength +WaitEnterSceneTime/ 60)
	if isOpenPlayTime then
		type = "Playing" 
		enterTime = GetSignOverTime(StartSignTime.time[1],WaitJointTime + GameLength + WaitEnterSceneTime/ 60)
		AddRobResState(type)
		return tbl, enterTime, type
	end
end

local function GetRobResState()
	--local sceneId, serverId, enterTime, state = GetRobResState()
	local result = StmtRobResource._GetRobResState:ExecStat()
	local num = result:GetRowNum()
	if num > 0 then
		local state = result:GetData(0,0)
		return state
	end
end


local function UpdateRobResState(state)
	local GasTongRobResDB = RequireDbBox("GasTongRobResourceDB")
	local sta = GetRobResState()
	if sta ~= state then
		StmtRobResource._UpdateRobResState:ExecStat(state)
	end
	if state == "Playing" then
		local tbl = SelectAllTongId()
		return tbl
	end
	
	if state == "Waiting" then
		--获得tbl_tong_rob_res表中的objName和tongId
		GetInfoForLog()
		local tbl = SelectAllTongId()
		--创建场景
		local sceneInfo = CreateScene()
		return tbl,sceneInfo
	end
	

	if state == "Ending" then
		GasTongRobResDB.DeleteTongAllInfo()
	end
end

local function DeleteRobResState()
	StmtRobResource._DeleteRobResState:ExecStat()
end

function GasRobRes.AddRobResState(parameters)
	local state = parameters["state"]
	AddRobResState(state)
end

function GasRobRes.GetRobResState()
	local tbl, enterTime, type = IsOpen()
--	local state = GetRobResState()
--	local res = {}
--	if state == "Playing" then
--		res = SelectSceneInfo()
--	end
	return {tbl, enterTime, type}
end

function GasRobRes.GetRobResActionState()
	local state = GetRobResState()
	return state
end

function GasRobRes.UpdateRobResState(parameters)
	local state = parameters["state"]
	local tbl, sceneInfo = UpdateRobResState(state)
	return tbl, sceneInfo
end

function GasRobRes.DeleteRobResState(parameters)
	DeleteRobResState()
end

function GasRobRes.SelectAllTongId()
	local charTbl = SelectAllTongId()
	return charTbl
end

function GasRobRes.AddSceneInfo(parameters)
	local objName = parameters["objName"]
	local sceneName = parameters["sceneName"]
	AddSceneInfo(objName, sceneName)
end

--服务器启动时检测
function GasRobRes.SelectSceneInfo()
	local sceneInfo = SelectSceneInfo()
	return sceneInfo
end

--主动进场
function GasRobRes.GetSceneInfo(parameters)
	local tong_box = RequireDbBox("GasTongBasicDB")
	local SceneMgrDB = RequireDbBox("SceneMgrDB")
	local GasTongRobResDB = RequireDbBox("GasTongRobResourceDB")
	local chardb = RequireDbBox("CharacterMediatorDB")
	local charId = parameters["charId"]
	local tempobj = parameters["objName"]
	local isFlag = parameters["flag"]
	local pos =	tong_box.GetPositionById(charId)
	if pos < 2 then
		return false, 9484
	end
 	local tongId = tong_box.GetTongID(charId)
	local data = {}
	data["tongId"] = tongId
	local flag, objName = GasTongRobResDB.SelectObjNameByTongId(data)
	if not objName then
		return false, 9475
	end
	local sceneId, serverId = GetSceneInfo(objName)
	local state = StmtRobResource._GetRobResState:ExecStat()
	if state:GetRowNum() > 0 then
		state = state:GetData(0,0)
	end
	if isFlag then
		if objName == tempobj then
			return true, {sceneId, serverId, true, state, nil}
		else
			return false,9477
		end
	else
		if objName then
			return true, {sceneId, serverId, true, state, nil}
		end
	end
end

function GasRobRes.GetSceneInfoOnly(parameters)
	local objName = parameters["objName"]
	local sceneId, serverId = GetSceneInfo(objName)
	return sceneId, serverId
end

local function JoinCondition(playerId, objName)
	local tong_box = RequireDbBox("GasTongBasicDB")
	local tongId, tongName, tongPos = tong_box.GetTongID(playerId)
	local tongLevel = tong_box.GetTongLevelAndHonor(tongId)
	local minLevel,maxLevel = FbRobRes_Server(objName, "MinLevel"), FbRobRes_Server(objName, "MaxLevel")
	local charTbl = tong_box.GetTongOnlineByPos(tongId) 
	local purview = tong_box.JudgePurview(playerId, "SignUpResource")
	local exploit = tong_box.GetTongExploit(tongId)
	local camp = tong_box.GetCampByCharID(playerId)
	return tongId, tongName, tongPos, tongLevel, charTbl, minLevel, maxLevel, purview, exploit, camp
end

local function SelectAll(objName, tongId)
	local GasTongRobResDB = RequireDbBox("GasTongRobResourceDB")
	local result = StmtRobResource.SelectAll:ExecStat()
	local num = result:GetRowNum()
	if num ~= nil and num > 0 then
		for i = 0, num - 1 do
			local haveTongId = result:GetData(i, 0)
			if haveTongId == tongId then
				local data = {}
				data["tongId"] = tongId
				local flag, ObjName = GasTongRobResDB.SelectObjNameByTongId(data)
				if ObjName ~= objName then
					return {9456}
				end
			end
		end 
	end
end

function GasRobRes.JudgeRobResSign(parameters)
	local joinAction = RequireDbBox("JoinActionDB")
	local tong_box = RequireDbBox("GasTongBasicDB")
	local chardb_box = RequireDbBox("CharacterMediatorDB")
	local CLogMgrDB = RequireDbBox("LogMgrDB")
	local tongbuild_box = RequireDbBox("GasTongbuildingDB")
	local serverId = parameters["serverId"]
	local playerId = parameters["charId"]
	local objName = parameters["objName"]
	local tong_id, tong_name, tong_pos, tongLevel, charTbl, minLevel, maxLevel , purview, exploit, campType = JoinCondition(playerId, objName)
	local sceneName = parameters["sceneName"]
	local res = StmtRobResource.SelectOtherTongId:ExecStat(objName, campType)
	local msgTbl = SelectAll(objName, tong_id)
	if msgTbl then
		return msgTbl
	end
	if tong_id == 0 then  --是否加入帮会
		return {9450}
	end
	
	if tongLevel < minLevel or tongLevel > maxLevel then
		return {9479}
	end
	
	if not purview then  --是否有报名权限
		return {9451}
	end
	
	if res:GetRowNum() >= 0 then
		if res:GetRowNum() == 0 then
			StmtRobResource.AddSignUpTong:ExecStat(objName, tong_id, tong_name, campType)  --未报名,将其报名
			--local sceneId, serverId = AddSceneInfo(objName, sceneName)
			
			local sMsg = "2033_objname:" .. objName
 			tong_box.AddTongLogs(tong_id,sMsg, g_TongMgr:GetLogType("战争"))
 			CLogMgrDB.SaveTongResourceSigUp(tong_id,playerId,exploit or 0,objName)
			return {9453, charTbl, nil}
		end
		local tempTongId = res:GetData(0,0)
		if tempTongId == tong_id then
			return {9454}
		end
		local oldTbl = {}
		oldTbl["tongId"] = tempTongId
		local oldTongExploit = tong_box.GetTongExploit(oldTbl["tongId"])
		if not exploit then
			exploit = 0 
		end 
		if not oldTongExploit then
			oldTongExploit = 0 
		end 
		if exploit <= oldTongExploit then
			return {9455}
		else
			local sMsg = "2033_objname:" .. objName
 			tong_box.AddTongLogs(tong_id,sMsg, g_TongMgr:GetLogType("战争"))
 			CLogMgrDB.SaveTongResourceSigUp(tong_id,playerId,exploit or 0,objName)
			return {true, tong_box.GetTongNameById(tempTongId)}
		end
	end 
end

function GasRobRes.UpdateJoinInfo(parameters)
	local tong_box = RequireDbBox("GasTongBasicDB")
	local playerId = parameters["charId"]
	local objName = parameters["objName"]
	local tong_id, tong_name, tong_pos, tongLevel, charTbl, minLevel, maxLevel , purview, exploit, campType  = JoinCondition(playerId, objName)
	local res = StmtRobResource.SelectOtherTongId:ExecStat(objName, campType)
	if res:GetRowNum() > 0 then
		local oldTongId = res:GetData(0,0)
		local oldTongPlayerTbl = tong_box.GetTongOnlineCharId(oldTongId)  
		--local sceneId, serverId = AddSceneInfo(objName, "抢夺资源点")
		StmtRobResource.UpdateSignTong:ExecStat(tong_id, tong_name, objName, campType)
		local sMsg = "2034_" .. tong_name .. ",objname:" .. objName
 		tong_box.AddTongLogs(oldTongId,sMsg, g_TongMgr:GetLogType("战争"))
		--return {9453, charTbl, oldTongPlayerTbl,sceneId, serverId}
		return {9453, charTbl, oldTongPlayerTbl}
	end
end

local function FormitLeaveFb()
	local TongOtherDB = RequireDbBox("GasTongOtherDB")
	local chardb = RequireDbBox("CharacterMediatorDB")
	local tongdb = RequireDbBox("GasTongBasicDB")
	local GasTongRobResDB = RequireDbBox("GasTongRobResourceDB")
	local CLogMgrDB = RequireDbBox("LogMgrDB")
	local result = StmtRobResource._GetSceneObj:ExecStat()
	local tbl = {}
	
	if result:GetRowNum() > 0 then
		for i = 0, result:GetRowNum()  - 1 do
			local objName = result:GetData(i,0)
			local sceneId = result:GetData(i,1)
			local param = {}
			param["sceneId"] = sceneId
			if chardb.IsSceneExist(param) then
				local objNameNum = StmtRobResource.GetSameTempRobResTongCount:ExecStat(objName)
				if objNameNum:GetRowNum() > 0 then
					local	Tempnum = objNameNum:GetData(0,0)
					if Tempnum <= 0 then
						table.insert(tbl,{false, nil, sceneId})
					end
					local result = StmtRobResource.GetIdByObjName:ExecStat(objName)
					if result:GetRowNum() > 0 then
						local id = result:GetData(0,0)
						local name = tongdb.GetTongNameById(id)
						table.insert(tbl,{true,name,sceneId})
						GasTongRobResDB.DealRobData(objName, id)
						CLogMgrDB.SaveTongRobResourceSucc(id)
						local data = {}
						data["tongId"] = id
						TongOtherDB.AddTongData(data)
					end
				end
			end
		end
		return tbl
	end
end


function GasRobRes.FormitLeaveFb()
	local tbl = FormitLeaveFb()
	return tbl
end

function GasRobRes.GetHeaderAward(data)
	local tongdb = RequireDbBox("GasTongBasicDB")
	local GasTongRobResDB = RequireDbBox("GasTongRobResourceDB")
	local PlayerId = data["charId"]
	local objName = data["objName"]
--	local getTime = data["getTime"]
	local sceneName = data["sceneName"]
	local tong_id, tong_name, tong_pos, tongLevel, charTbl, minLevel, maxLevel , purview, exploit, campType = JoinCondition(PlayerId, objName)
	local getPurview = tongdb.JudgePurview(PlayerId, "GetHeaderAward")
	local data = {}
	data["objName"] = objName
	local tongId = GasTongRobResDB.GetRobResTong(data)
	if tongId == tong_id then
		--查询领取时间
		local result = StmtRobResource._GetHeaderAwardInfo:ExecStat(tong_id)
		if not getPurview then
			return {false, 9480}
		end
		if result:GetData(0,0) == 0 then
			
			StmtRobResource._UpdateHeaderAwardInfo:ExecStat(tong_id, PlayerId)
			--加非绑定金钱
			local money = FbRobRes_Server(objName, "AwardMoney")
			local fun_info = g_MoneyMgr:GetFuncInfo("RobResourceAward")
			local MoneyManagerDB = RequireDbBox("MoneyMgrDB")
			MoneyManagerDB.AddMoney(fun_info["FunName"],fun_info["HeaderAward"], PlayerId, money, nil, event_type_tbl["资源点队长奖励"])  --log类型
			return {true, money}
		else
			return {false, 9481}
		end
	else 
		return {false, 9483}
	end
end

function GasRobRes.DeleteSceneInfo()
	StmtRobResource._DeleteSceneInfo:ExecStat()
end

function GasRobRes.DelHeaderAwardInfo()
	local GasTongRobResDB = RequireDbBox("GasTongRobResourceDB")
	GasTongRobResDB.DelGetProfferTime()
	StmtRobResource._DelHeaderAwardInfo:ExecStat()
	local res = StmtRobResource._GetClearAwardTime:ExecStat()
	if res:GetRowNum() > 0 then
		StmtRobResource._UpdateClearAwardTime:ExecStat()
	else
		StmtRobResource._AddClearAwardTime:ExecStat()
	end
end


function GasRobRes.StartClearAwardInfo()
	local GasTongRobResDB = RequireDbBox("GasTongRobResourceDB")
	local res = StmtRobResource._GetClearAwardTime:ExecStat()
	if res:GetRowNum() > 0 then
		local clearTime = res:GetData(0,0)
		if os.time() - clearTime >= 24 * 60 * 60 then
			GasTongRobResDB.DelGetProfferTime()
			StmtRobResource._DelHeaderAwardInfo:ExecStat()
			StmtRobResource._DelClearAwardTime:ExecStat()
		end
	end
end

SetDbLocalFuncType(GasRobRes.GetSceneInfo)
SetDbLocalFuncType(GasRobRes.GetHeaderAward)

return GasRobRes
