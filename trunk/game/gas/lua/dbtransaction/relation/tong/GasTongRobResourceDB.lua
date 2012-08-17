cfg_load "fb_game/FbRobRes_Server"
cfg_load "int_obj/IntObj_Common"
local FbRobRes_Server = FbRobRes_Server
local g_ItemInfoMgr = CItemInfoMgr:new()
local GetCfgTransformValue = GetCfgTransformValue
local LogErr = LogErr
local StmtRobResource = class()
local event_type_tbl = event_type_tbl
local RobResource = CreateDbBox(...)

local StmtDef=
{
 "AddRobResTong",
 "replace into tbl_tong_rob_resource(trr_sObjName, trr_uId) values(?,?)"
}
DefineSql(StmtDef,StmtRobResource)


local StmtDef=
{
 "GetRobResTong",
 "select trr_uId from tbl_tong_rob_resource where trr_sObjName = ?"
}
DefineSql(StmtDef,StmtRobResource)

local StmtDef=
{
 "GetRobResObjById",
 "select trr_sObjName from tbl_tong_rob_resource where trr_uId = ?"
}
DefineSql(StmtDef,StmtRobResource)

local StmtDef=
{
 "ClearRobResTong",
 "delete from tbl_tong_rob_resource"
}
DefineSql(StmtDef,StmtRobResource)

local StmtDef=
{
 "GetRobResWin",
 "select t_sName, trr_sObjName, t.t_uCamp  from tbl_tong_rob_resource trr, tbl_tong t where t.t_uId = trr.trr_uId"
}
DefineSql(StmtDef,StmtRobResource)

local StmtDef=
{
 "GetRobResDataTong",
 "select * from tbl_tong_rob_resource"
}
DefineSql(StmtDef,StmtRobResource)

local StmtDef=
{
 	"UpdateRobResTong",
	"update tbl_tong_rob_resource set trr_uId = ? where trr_sObjName = ?"
}
DefineSql(StmtDef,StmtRobResource)

local StmtDef=
{
 	"UpdateRobResTongById",
	"update tbl_tong_rob_resource set trr_sObjName = ? where trr_uId = ?"
}
DefineSql(StmtDef,StmtRobResource)

local StmtDef=
{
 	"DeleteRobDataById",
	"delete from tbl_tong_rob_resource where trr_sObjName = ?"
}
DefineSql(StmtDef,StmtRobResource)


local StmtDef=
{
 	"AddSignUpTong",
	"insert into tbl_tong_rob_res(trr_sObjName, trr_uId, trr_sName, trr_uTongCamp) values(?,?,?,?)"
}
DefineSql(StmtDef,StmtRobResource)

local StmtDef = 
{
 	"GetTongNum",
	"select count(trr_uId) from tbl_tong_rob_res where trr_sObjName = ? and trr_uTongCamp = ?"
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
 	"SelectSignUpTongName",
	"select trr_uId from tbl_tong_rob_res where trr_sObjName = ?"
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
 	"SelectAll",
	"select trr_uId from tbl_tong_rob_res"
}
DefineSql(StmtDef,StmtRobResource) 

local StmtDef=
{
 	"SelectAllObj",
	"select trr_sObjName from tbl_tong_rob_res"
}
DefineSql(StmtDef,StmtRobResource) 

local StmtDef=
{
 	"DeleteObjInfo",
	"delete from tbl_tong_rob_res where trr_sObjName = ?"
}
DefineSql(StmtDef,StmtRobResource) 

local StmtDef=
{
 	"DeleteResourceInfo",
	"delete from tbl_tong_rob_resource where trr_sObjName = ?"
}
DefineSql(StmtDef,StmtRobResource)

local StmtDef=
{
 	"DeleteTongInfo",
	"delete from tbl_tong_rob_res where trr_sName = ?"
}
DefineSql(StmtDef,StmtRobResource)

local StmtDef=
{
 	"DeleteTongAllInfo",
	"delete from tbl_tong_rob_res"
}
DefineSql(StmtDef,StmtRobResource)

local StmtDef=
{
 	"_DeleteTongById",
	"delete from tbl_tong_rob_res where trr_uId = ?"
}
DefineSql(StmtDef,StmtRobResource)


local StmtDef=
{
 "AddTempRobResTong",
 "insert into tbl_tong_rob_resource_temp(trrt_uId, trrt_nCampId, trrt_sObjName) values(?,?,?)"
}
DefineSql(StmtDef,StmtRobResource)

local StmtDef=
{
 "GetTempRobResTong",
 "select trrt_uId, trrt_nCampId from tbl_tong_rob_resource_temp where trrt_sObjName = ?"
}
DefineSql(StmtDef,StmtRobResource)

local StmtDef=
{
 "GetAllTempRobResTong",
 "select * from tbl_tong_rob_resource_temp"
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
 "GetSameTempRobResTongCount",
 "select count(*) from tbl_tong_rob_resource_temp where trrt_sObjName = ?"
}
DefineSql(StmtDef,StmtRobResource)

local StmtDef=
{
 "GetTempRobResObj",
 "select trrt_sObjName from tbl_tong_rob_resource_temp"
}
DefineSql(StmtDef,StmtRobResource)

local StmtDef=
{
 	"UpdateTempRobResTong",
	"update tbl_tong_rob_resource_temp set trrt_uId = ? ,trrt_nCampId = ? where trrt_sObjName = ?"
}
DefineSql(StmtDef,StmtRobResource)

local StmtDef=
{
 	"DeleteTempRobResTong",
	"delete from tbl_tong_rob_resource_temp where trrt_uId = ?"
}
DefineSql(StmtDef,StmtRobResource)

local StmtDef=
{
 	"DeleteTempRobResData",
	"delete from tbl_tong_rob_resource_temp"
}
DefineSql(StmtDef,StmtRobResource)

local StmtDef=
{
 	"DeleteTempRobResTongInfo",
	"delete from tbl_tong_rob_resource_temp where trrt_uId = ? and trrt_sObjName = ?"
}
DefineSql(StmtDef,StmtRobResource)

local StmtDef=
{
 	"AddSignUpTeamAndTime",
	"insert into tbl_tong_rob_res_time(trrti_sObjName,trrti_uCamp,trrti_uId, trrti_uServerId, trrti_dtSignTime) values(?,?,?,?,DATE_FORMAT(now(),'%Y%m%d'))"
}
DefineSql(StmtDef,StmtRobResource)

local StmtDef=
{
 	"SelectSignUpTeamAndTime",
	"select trrti_dtSignTime from tbl_tong_rob_res_time where trrti_uId = ? and trrti_uServerId = ? and trrti_uCamp = ? and trrti_sObjName = ?"
}
DefineSql(StmtDef,StmtRobResource)

local StmtDef=
{
 	"UpdateSignUpTeamAndTime",
	"update tbl_tong_rob_res_time set trrti_uId = ? where trrti_uServerId = ? and trrti_dtSignTime = ? and trrti_uCamp = ? and trrti_sObjName = ?"
}
DefineSql(StmtDef,StmtRobResource)

local StmtDef = 
{
	"GetSignUpTeamTimeCount",
	[[
	select 
		trrti_uId
	from 
		tbl_tong_rob_res_time
	where 
		trrti_uServerId = ? and trrti_dtSignTime = DATE_FORMAT(now(),"%Y%m%d")
	]]
}
DefineSql(StmtDef,StmtRobResource)

local StmtDef=
{
 	"DeleteTongInfoByServerId",
	"delete from tbl_tong_rob_res_time where trrti_uId = ? and trrti_uServerId = ? and trrti_dtSignTime = DATE_FORMAT(now(),'%Y%m%d')"
}
DefineSql(StmtDef,StmtRobResource)

local StmtDef=
{
 	"UpdateSignTong",
	"update tbl_tong_rob_res set trr_uId = ? ,trr_sName = ? where trr_sObjName = ? and trr_uTongCamp = ?" 
}
DefineSql(StmtDef,StmtRobResource)


local StmtDef = {
  	"_AddGetProfferTime",
  	"insert into tbl_tong_get_proffer_time(cs_uId, tgpt_sTime) values (?,now())"
}    
DefineSql ( StmtDef, StmtRobResource )

local StmtDef = {
  	"_GetProfferTime",
  	"select count(*) from tbl_tong_get_proffer_time where cs_uId = ?"
}    
DefineSql ( StmtDef, StmtRobResource )

local StmtDef = {
  	"_DelGetProfferTime",
  	"delete from tbl_tong_get_proffer_time"
}    
DefineSql ( StmtDef, StmtRobResource )

local function GetRobResTong(objName)
	local result = StmtRobResource.GetRobResTong:ExecStat(objName)
	local tongId 
	if result:GetRowNum() > 0 then
		tongId = result:GetData(0,0)
	end
	return tongId
end

local function GetRobResDataTong()
	local result = StmtRobResource.GetRobResWin:ExecStat()
	return result
end


function RobResource.GetRobResTong(parameters)
	local objName = parameters["objName"]
	return GetRobResTong(objName)
end

function RobResource.DeleteTongInfoByServerId(serverId)
	local result = StmtRobResource.GetSignUpTeamTimeCount:ExecStat(serverId)
	local num = result:GetRowNum()
	for i = 0, num - 1 do
		local tongId = result:GetData(i,0)
		local objName = StmtRobResource.SelectObjNameByTongId:ExecStat(tongId)
		local tempNum = objName:GetRowNum()
		if tempNum > 0  then
			objName = objName:GetData(0,0)
			StmtRobResource.DeleteObjInfo:ExecStat(objName)
		end
	end
	StmtRobResource.DeleteTongInfoByServerId:ExecStat(serverId)
end

function RobResource.GetRobResObjById(tongId)
	local result = StmtRobResource.GetRobResObjById:ExecStat(tongId)
	local objName = nil
	if result:GetRowNum() > 0 then
		objName = result:GetData(0,0)
	else
		objName = ""
	end
	return objName
end

function RobResource.GetProffer(data)
	local AreaFbPointDB = RequireDbBox("AreaFbPointDB")
	local tongdb = RequireDbBox("GasTongBasicDB")
	local PlayerId = data["charId"]
	local objName = data["objName"]
	--local getTime = data["getTime"]
	local sceneName = data["sceneName"]
	local result = StmtRobResource.GetRobResTong:ExecStat(objName)
	local proffer = FbRobRes_Server(objName, "Modulus") 
	local PlayerTongId = tongdb.GetTongID(PlayerId)
	local tongId = 0
	if result:GetRowNum() > 0 then
		tongId = result:GetData(0,0)
		local tong_id,tong_name,tong_pos = tongdb.GetTongID(PlayerId)
		if tongId == tong_id then
			local res = StmtRobResource._GetProfferTime:ExecStat(PlayerId)
			local count = res:GetData(0,0)
			if count > 0 then
				return {false, 9473}
			end
			StmtRobResource._AddGetProfferTime:ExecStat(PlayerId)
			if not (g_DbChannelMgr:LastAffectedRowNum()>0)  then
				CancelTran()
				return
			end
	
			local AddProffer = proffer *100* 0.1 *60 /20
			if AddProffer ~= 0 then
				AreaFbPointDB.AddAreaFbPointByType(PlayerId,AddProffer,10,"抢夺资源点",nil,event_type_tbl["抢夺资源点加国贡"])
			end
			if not (g_DbChannelMgr:LastAffectedRowNum()>0)  then
	  		CancelTran()
	  		return
	  	end
			return {true, AddProffer}
		end
		return {false, 9472}
	end
	return {false,9472}
end

function RobResource.AddTempRobResTong(parameter)
	local tongId = parameter["tongId"]
	local campType = parameter["PlayerCamp"]
	local objName = StmtRobResource.SelectObjNameByTongId:ExecStat(tongId)   --报名点
	if objName:GetRowNum() > 0 then
		objName = objName:GetData(0,0)
	else
		return
	end
	local result = StmtRobResource.GetTempRobResTong:ExecStat(objName)
	if result:GetRowNum() > 0 then
		StmtRobResource.UpdateTempRobResTong:ExecStat(tongId, campType, objName)
	else
		StmtRobResource.AddTempRobResTong:ExecStat(tongId, campType, objName)
	end 
end

function RobResource.DeleteTempRobResData()
	local DestorySceneDB = RequireDbBox("SceneMgrDB")
	local GasRobRes = RequireDbBox("GasTongRobResDB")
	local res = StmtRobResource.GetAllTempRobResTong:ExecStat()
	if res:GetRowNum() > 0 then
		StmtRobResource.DeleteTempRobResData:ExecStat()
	end
	GasRobRes.DeleteSceneInfo()
	return DestorySceneDB.DeleteSceneByType()
end

function RobResource.DealRobData(objName, tongId)
	local result = StmtRobResource.GetRobResTong:ExecStat(objName)
	if result:GetRowNum() > 0 then
		StmtRobResource.DeleteRobDataById:ExecStat(objName)
	end
	StmtRobResource.AddRobResTong:ExecStat(objName, tongId)
end

--获得(添加)资源点
function RobResource.AddRobResTong(parameter)
	local GasTongRobResDB = RequireDbBox("GasTongRobResDB")
	local TongOtherDB = RequireDbBox("GasTongOtherDB")
	local CLogMgrDB = RequireDbBox("LogMgrDB")
	local tongId = parameter["tongId"]
	local campType = parameter["PlayerCamp"]
	local PlayerIdTbl = parameter["PlayerTbl"]
	local state = parameter["state"]
	local flag = parameter["flag"]
	local objName = StmtRobResource.SelectObjNameByTongId:ExecStat(tongId)
	if objName:GetRowNum() > 0 then
		objName = objName:GetData(0,0)
	else
		return
	end
	local tempResult = StmtRobResource.GetTempRobResTong:ExecStat(objName)
	if tempResult:GetRowNum() > 0 then
		StmtRobResource.DeleteTempRobResTong:ExecStat(tongId)
	end
	local GasTongBasicDB = RequireDbBox("GasTongBasicDB")
	local tongName = GasTongBasicDB.GetTongNameById(tongId)
	local parameters = {}
	parameters["nEventType"] = event_type_tbl["佣兵小队抢夺资源点活动"]
	local result = StmtRobResource.GetRobResTong:ExecStat(objName)
	--local result = StmtRobResource.GetRobResTong:ExecStat(objName)
	if result:GetRowNum() > 0 then
		StmtRobResource.DeleteRobDataById:ExecStat(objName)
	end
	StmtRobResource.AddRobResTong:ExecStat(objName, tongId)
	
	local data = {}
	data["tongId"] = tongId
	TongOtherDB.AddTongData(data)
	CLogMgrDB.SaveTongRobResourceSucc(tongId)
	if flag then
		GasTongRobResDB.UpdateRobResState(parameter)
	end
	if not tongName then
		LogErr("占领资源点后出现错误","资源点名:"..objName.."小队id:"..tongId)
	end
	return tongName
end


--记录报名帮会名
function RobResource.SelectSignUpTongName(parameters)
	local CTongBasicBox = RequireDbBox("GasTongBasicDB")
	local signUpId = parameters["signUpId"]
	local restbl = {}
	local result = StmtRobResource.SelectSignUpTongName:ExecStat(signUpId)
	local winTongId = GetRobResTong(signUpId)
	local winTongName = CTongBasicBox.GetTongNameById(winTongId)
	local num = result:GetRowNum()
	if num > 0 then
		for i=0, result:GetRowNum()- 1 do
			local tongId = result:GetData(i,0)
			local exploitPoint = CTongBasicBox.GetTongExploit(tongId)
			local tongName = CTongBasicBox.GetTongNameById(tongId)
			local tongCamp = CTongBasicBox.GetTongCampByID(tongId)
			table.insert(restbl,{tongName, tongCamp, exploitPoint,winTongName})
		end
	else
		table.insert(restbl,{tongName, tongCamp, exploitPoint,winTongName})
	end
	return restbl
end

local function UpdateSignUpTeamAndTime(objName, campType, oldtongId, serverId, newtongId)
	local result = StmtRobResource.SelectSignUpTeamAndTime:ExecStat(oldtongId, serverId, campType,objName)
	local signTime 
	if result:GetRowNum() <= 0 then
		StmtRobResource.AddSignUpTeamAndTime:ExecStat(objName, campType,newtongId,serverId)
		return
	end
	signTime = result:GetData(0,0)
	StmtRobResource.UpdateSignUpTeamAndTime:ExecStat(newtongId, serverId, signTime,campType,objName)
end

function RobResource.GetObjName(parameters)
	local charId = parameters["charId"]
	local tong_box = RequireDbBox("GasTongBasicDB")
	local tong_id,tong_name,tong_pos = tong_box.GetTongID(charId)
	local result = StmtRobResource.SelectObjNameByTongId:ExecStat(tong_id)
	local num = result:GetRowNum()
	if num > 0 then
		local objName = result:GetData(0,0)
		return objName
	end
end

--删除报名信息
function RobResource.DeleteObjInfo(parameters)
	local restbl = {}
	local charId = parameters["charId"]
	local tong_box = RequireDbBox("GasTongBasicDB")
	local tong_id,tong_name,tong_pos = tong_box.GetTongID(charId)
	local result = StmtRobResource.SelectObjNameByTongId:ExecStat(tong_id)
	local num = result:GetRowNum()
	if num > 0 then
		local objName = result:GetData(0,0)
		StmtRobResource.DeleteObjInfo:ExecStat(objName)
	end
end

--删除资源信息信息
function RobResource.DeleteResourceInfo(parameters)
	local charId = parameters["charId"]
	local tong_box = RequireDbBox("GasTongBasicDB")
	local tong_id,tong_name,tong_pos = tong_box.GetTongID(charId)
	local result = StmtRobResource.SelectObjNameByTongId:ExecStat(tong_id)
	local num = result:GetRowNum()
	if num > 0 then
		local objName = result:GetData(0,0)
		--local resourceName = IntObj_Common(objName, "ActionArg")
		StmtRobResource.DeleteResourceInfo:ExecStat(objName)
	end
end


function RobResource.DeleteTongAllInfo()
	local result = StmtRobResource.SelectAll:ExecStat()
	local num = result:GetRowNum()
	if num > 0 then
		StmtRobResource.DeleteTongAllInfo:ExecStat()
	else 
		return 
	end
end


--删除帮会报名信息
function RobResource.DeleteTongInfo(data)
	local tongName = data["tongName"]
	StmtRobResource.DeleteTongInfo:ExecStat(tongName)
end

function RobResource.SelectAllInfo()
	local haveTong = StmtRobResource.SelectAll:ExecStat()
	local haveNum = haveTong:GetRowNum()
	local tbl = {}
	if haveNum ~= nil and haveNum > 0 then
		for v = 0, haveNum - 1 do
			local haveTongId = haveTong:GetData(v, 0)
			table.insert(tbl,haveTongId)
		end 
		return tbl
	else
		return 
	end
end

function RobResource.SelectAllTong()

	local result = StmtRobResource.SelectAll:ExecStat()
	local tong_box = RequireDbBox("GasTongBasicDB")
	local joinActionDB = RequireDbBox("JoinActionDB")
	local num = result:GetRowNum()
	local tbl = {}
	local resTbl = {}
	if num > 0 then
		for i = 0, num - 1 do
			local tongId = result:GetData(i,0)
			local objName = StmtRobResource.SelectObjNameByTongId:ExecStat(tongId)
			if objName:GetRowNum() > 0 then
				objName = objName:GetData(0,0)
			end
			table.insert(resTbl, {tongId,objName})
		end
	end
	local objTbl = {}
	for _, res in pairs(resTbl) do
		local name = res[2]
		local tongId = res[1]
		if objTbl[name] then
			local charTbl = tong_box.GetTongOnlineCharId(tongId) 
			for _, charId in pairs(charTbl) do 
				local data = {}
				if charId then
					data["CharId"] = charId
					data["ActionName"] = "抢夺资源点"
					data["IsAlarm"] = true
					local isSucceed, args = joinActionDB.JoinActionInner(data)
					break
				end
			end
		else
			objTbl[name] = true
			local charTbl = tong_box.GetTongOnlineCharId(tongId)    
			for _, charId in pairs(charTbl) do
				local data = {}
				if charId then
					data["CharId"] = charId
					data["ActionName"] = "抢夺资源点"
					data["IsAlarm"] = true
					local isSucceed, args = joinActionDB.JoinActionInner(data)
					table.insert(tbl, {isSucceed, args})
					break
				end
			end
		end
	end
	return tbl
end

local function DeleteTongById(tongId)
	StmtRobResource._DeleteTongById:ExecStat(tongId) 
	if not (g_DbChannelMgr:LastAffectedRowNum()>0)  then
		CancelTran()
		LogErr("删除抢夺资源点报名失败","佣兵团"..tongId)
		return
	end
	local tongRobResDB = RequireDbBox("GasTongRobResDB")
	local tongdb = RequireDbBox("GasTongBasicDB")
	local state = tongRobResDB.GetRobResActionState()
	return true,state
end

function RobResource.DeleteTongById(data)
	local playerId = data["charId"]
	local tong_box = RequireDbBox("GasTongBasicDB")
	local tongRobResDB = RequireDbBox("GasTongRobResDB")
	local CLogMgrDB = RequireDbBox("LogMgrDB")
	local tongId = tong_box.GetTongID(playerId)
	local onlineMember = tong_box.GetTongOnlineCharId(tongId)
	local objName = StmtRobResource.SelectObjNameByTongId:ExecStat(tongId)
	if objName:GetRowNum() <= 0 then
		return
	end
	objName = objName:GetData(0,0)
	local data = {}
	data["objName"] = objName
	local sceneId, serverId  = tongRobResDB.GetSceneInfoOnly(data)
	local flag,state = DeleteTongById(tongId)
	if flag then
		CLogMgrDB.SaveTongResourceSigUp(tongId,playerId,-1,objName)
	end
	return flag, state,sceneId, serverId, onlineMember
end

function RobResource.DeleteInfoByTongId(data)
	local tongId = data["tongId"]
	local flag = DeleteTongById(tongId)
	return flag
end

local function SelectObjNameByTongId(tongId)
	local result = StmtRobResource.SelectObjNameByTongId:ExecStat(tongId) 
	local num = result:GetRowNum()
	if num > 0 then
		local objName = result:GetData(0,0)
		return true,objName
	end
	return false
end

function RobResource.SelectObjNameByTongId(data)
	local tong_box = RequireDbBox("GasTongBasicDB")
	local tongId = data["tongId"]
	local charId = data["charId"]
	local flag, objName = SelectObjNameByTongId(tongId)
	if charId then
		local purview = tong_box.JudgePurview(charId, "CancelSignResource")
		return flag, objName, purview
	else
		return flag, objName
	end
--	local tong_box = RequireDbBox("GasTongBasicDB")
--	local tongId = tong_box.GetTongID(playerId)

end

function RobResource.SelectObjNameByCharId(data)
	local charId = data["charId"]
	local tong_box = RequireDbBox("GasTongBasicDB")
	local tongId = tong_box.GetTongID(charId)
	local flag, objName = SelectObjNameByTongId(tongId)
	return flag, objName
end


function RobResource.GetRobResDataTong()
	return GetRobResDataTong()
end

function RobResource.DelGetProfferTime()
	StmtRobResource._DelGetProfferTime:ExecStat() 
end


SetDbLocalFuncType(RobResource.DeleteTongById)
SetDbLocalFuncType(RobResource.SelectObjNameByTongId)
SetDbLocalFuncType(RobResource.SelectSignUpTongName)

return RobResource
