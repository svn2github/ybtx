local MaxServerNum = 15

local os = os
local ShowErrorMsgAndExit = ShowErrorMsgAndExit
local LogDBName = DeployConfig.MySqlDatabase .. "_log"
local GetCurrentRevision = GetCurrentRevision
local StmtOperater = {}		--用来挂sql语句的table
local GasMainDB = class()
local GasMainSql = CreateDbBox(...)

local StmtDef =
{
	"GetServerIdList",
	"select s_uId from tbl_server"
}
DefineSql(StmtDef,StmtOperater)

local StmtDef =
{
	"GetOtherServerOnlineInfo",
	[[
		select
			c.cs_uId, c.c_sName
		from 
			tbl_char_online as co, tbl_char as c
		where
			co.cs_uId = c.cs_uId and 
			co.co_uOnServerId <> ?
	]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef =
{
	"GetOtherServerIdList",
	"select s_uId from tbl_server where s_uId <> ?"
}
DefineSql(StmtDef,StmtOperater)

local StmtDef =
{
	"GetNowTime",
	"select unix_timestamp(now())"
}
DefineSql(StmtDef,StmtOperater)

local StmtDef =
{
	"GetServerAttribute",
	"select s_uId,sa_bCanLogin,sa_bNoFbScene,sa_bNoMainScene from tbl_server_attribute"
}
DefineSql(StmtDef,StmtOperater)


local ServerAttribute = nil

function GasMainSql.GetServerAttribute()
	if ServerAttribute then
		return ServerAttribute
	end
	local result = StmtOperater.GetServerAttribute:ExecSql("n")
	ServerAttribute = {}
	for i = 0, result:GetRowNum() - 1 do
		local temp = {}
		temp.IsCanLogin = result:GetData(i,1) == 1
		temp.IsNoFbScene = result:GetData(i,2) == 1
		temp.IsNoMainScene = result:GetData(i,3) == 1
		ServerAttribute[result:GetData(i,0)] = temp
	end
	return ServerAttribute
end


local ServerList = nil
function GasMainSql.GetServerList()
	if ServerList then
		return ServerList
	end
	local result = StmtOperater.GetServerIdList:ExecSql("n")
	ServerList = result:GetCol(0)
	result:Release()
	return ServerList
end


local OtherServerList = nil
function GasMainSql.GetOtherServerList()
	if OtherServerList then
		return OtherServerList
	end
	local result = StmtOperater.GetOtherServerIdList:ExecStat(g_CurServerId)
	OtherServerList = result:GetCol(0)
	result:Release()
	return OtherServerList
end

local StmtDef =
{
	"GetServerVersion",
	[[
	select * from tbl_server_version
	]]
}
DefineSql(StmtDef,StmtOperater) 

local StmtDef =
{
	"InsertServerVersion",
	[[
	insert into  tbl_server_version set sv_uVersion = ?
	]]
}
DefineSql(StmtDef,StmtOperater) 

local StmtDef =
{
	"SetServerVersion",
	[[
	update tbl_server_version set sv_uVersion = ?
	]]
}
DefineSql(StmtDef,StmtOperater) 

local StmtDef = {
    "GetCSMCfgInfoInDB",
    [[
        select count(*) from tbl_market_cfg_info 
    ]]
}   
DefineSql(StmtDef,StmtOperater) 

local StmtDef = {
    "GetCSMPurchasingCfgInfoInDB",
    [[
        select count(*) from tbl_market_purchasing_cfg_info 
    ]]
}   
DefineSql(StmtDef,StmtOperater) 

local StmtDef = {
    "GetContractManuCfgInfoInDB",
    [[
        select count(*) from tbl_contract_manufacture_cfg_info 
    ]]
}DefineSql(StmtDef,StmtOperater)


local StmtDef = 
{
	"InsertCpuStatus",
	"insert into tbl_cpu_status(s_uId, cs_uMaxThreadUsage, cs_uAllUsage) values(?,?,?)"
}
DefineSql(StmtDef, StmtOperater)

local StmtDef = 
{
	"DelUserOnlineByServerId",
	"delete from tbl_user_online where uo_uOnServerId = ?"
}
DefineSql(StmtDef, StmtOperater)

local StmtDef = 
{
	"DelCharOnlineByServerId",
	"delete from tbl_char_online where co_uOnServerId = ?"
}
DefineSql(StmtDef, StmtOperater)


function GasMainSql.UpdateServerVersion()
	local curVersion = GetCurrentRevision()
	local versionRet = StmtOperater.GetServerVersion:ExecStat()
	local versionNum  = versionRet:GetRowNum() 
	local updateServerFlag = false
	local csmCfgInfo = StmtOperater.GetCSMCfgInfoInDB:ExecStat()
	local csmInfoCount = csmCfgInfo:GetData(0,0)
	local csmPurchasingCfgInfo = StmtOperater.GetCSMPurchasingCfgInfoInDB:ExecStat()
	local csmPurchasingInfoCount = csmPurchasingCfgInfo:GetData(0,0)
	local sql = ""
	local updateContractManuCfgInfo = false
	local updateCSMCfgInfoFlag = false
	local updateCSMPurchasingCfgInfoFlag = false	
	local OperateRoomDB = RequireDbBox("OperateRoomRpcDB")
	local bUpCleanupItemCfg = OperateRoomDB.HaveItemTypeOrderInfo()
	if (versionNum > 0 ) then
	    local versionInDB = versionRet:GetData(0, 0)
	    if versionInDB ~= curVersion then
            updateServerFlag = true
            updateCSMCfgInfoFlag = true
            bUpCleanupItemCfg = true
            sql = StmtOperater.SetServerVersion
        end
    else
        updateServerFlag = true
        updateCSMCfgInfoFlag = true
        bUpCleanupItemCfg = true
        sql = StmtOperater.InsertServerVersion
	end
	if csmInfoCount == 0 then
	   updateCSMCfgInfoFlag = true 
	end
	
	if csmPurchasingInfoCount == 0 then
	   updateCSMPurchasingCfgInfoFlag = true 
	end
	
	local contractManuCout = StmtOperater.GetContractManuCfgInfoInDB:ExecStat()
	if contractManuCout:GetData(0,0) == 0 then
	    updateContractManuCfgInfo = true
    end
	local ret = true
	if updateServerFlag == true then
	    sql:ExecStat(curVersion)
    	if g_DbChannelMgr:LastAffectedRowNum() < 1 then
            ret = false
            CancelTran()
            return ret 
        end
   end
   if updateCSMCfgInfoFlag == true then
      local ConsignmentDB = RequireDbBox("ConsignmentDB")
      ConsignmentDB.ReadCfgToCSMDBTable()
      if g_DbChannelMgr:LastAffectedRowNum() < 1 then
          ret = false
          CancelTran()
          return ret
      end
	end
	
	 if updateCSMPurchasingCfgInfoFlag == true then
			local PurchasingAgentDB = RequireDbBox("PurchasingAgentDB")
      PurchasingAgentDB.ReadCfgToCSMDBTable()
      if g_DbChannelMgr:LastAffectedRowNum() < 1 then
          ret = false
          CancelTran()
          return ret
      end
	end
	if bUpCleanupItemCfg then
		OperateRoomDB.InitItemTypeOrderDB()
	end

	if updateContractManuCfgInfo == true or updateServerFlag == true then
	    local ContractManufactureDB = RequireDbBox("ContractManufactureDB")
        ContractManufactureDB.ReadLiveSkillCfgInfoToDB()
        if g_DbChannelMgr:LastAffectedRowNum() < 1 then
            ret = false
            CancelTran()
            return ret
        end
	end

	return ret 
end

function GasMainSql.GasStartUpFinalize(data)
	-------不要写任何代码---------
	-------不要写任何代码---------
	-------不要写任何代码---------
	-------不要写任何代码---------
	-------不要写任何代码---------
end

function 	GasMainSql.GasStartUp(data)
	local dbNow = StmtOperater.GetNowTime:ExecSql("n")
	local sub = os.time() - dbNow:GetData(0,0)
		
	dbNow:Release()
	if math.abs(sub) > 3 then
		local err = "服务器时间与数据库时间相差过大\n"
		if sub > 0 then
			err = err .. "请将系统时间 调慢 " .. sub .. " 秒"
		else
			err = err .. "请将系统时间 调快 " .. -sub .. " 秒"
		end
		ShowErrorMsgAndExit(err)
	end
	local MultiServerDB = RequireDbBox("MultiServerDB")
	local SceneMgrDB = RequireDbBox("SceneMgrDB")
	local WarZoneDB = RequireDbBox("WarZoneDB")
	local AuctionAssignDB = RequireDbBox("AuctionAssignDB")
	
	SceneMgrDB.SetStartMark(true)  --放在最前面
	
	MultiServerDB.ClearServerData(g_CurServerId)
	
  local upVersionNoret = GasMainSql.UpdateServerVersion()
	if upVersionNoret == false then
	local updateSerMsg="更新服务器版本出错"
  	ShowErrorMsgAndExit(updateSerMsg)
	end
	--读取GM关闭的活动
	local GMDB = RequireDbBox("GMDB")
	local closedActionRes = GMDB.CheckClosedAction()
	
	local sceneTbl = SceneMgrDB._GetMainScene()
	local warZoneRes = WarZoneDB.GetWarZoneInfoOnGasStart()
	local auctionRes = AuctionAssignDB.ReturnAllAuctionMoney()
	local serverAttribute = GasMainSql.GetServerAttribute()
	if not( serverAttribute[g_CurServerId] and serverAttribute[g_CurServerId].IsNoFbScene ) then
		StmtOperater.InsertCpuStatus:ExecStat(g_CurServerId, 0,0)
	end
	
	local serverTbl = MultiServerDB.GetServerTbl()
	local serverMark = {}
	for _, v in pairs(serverTbl) do
		serverMark[v[1]] = true
	end
	for i = 1, MaxServerNum do
		if not serverMark[i] then
			MultiServerDB.ClearServerData(i)
			StmtOperater.DelUserOnlineByServerId:ExecStat(i)
			StmtOperater.DelCharOnlineByServerId:ExecStat(i)
		end
	end
	
		--获取当前在线的角色  (解决中途重启服务器 内存里没有在线状态的bug)
	local onlineInfo = StmtOperater.GetOtherServerOnlineInfo:ExecStat(g_CurServerId)
	Db2CallBack("SetOnlineInfo", onlineInfo)
	
	

	SceneMgrDB.SetStartMark(false) --放在这个函数最后处
	return sceneTbl,warZoneRes,serverTbl,closedActionRes,auctionRes,serverAttribute
end 	

local StmtDef = 
{
	"_UpdateServerTime",
	[[
		replace into tbl_var_server(sv_sVarName,sv_sVarValue,sv_dtDate, sv_uServerId) values(?,"",now(), ?)
	]]
}
DefineSql(StmtDef,StmtOperater)
--记录服务器宕机时间
function 	GasMainSql.ServerTimeUpdateTick(data)
	StmtOperater._UpdateServerTime:ExecSql('',"ServerShutTime", data["CurServerId"])
end 	

--初始化ERating配置
local StmtDef = 
{
	"_InitEratingConfig_stmt",
	[[
		select sc_sVarName, sc_sVarValue from tbl_conf_server where sc_sVarName like "AGIP%"
	]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef = 
{
	"_InitAllowedGmsIP_stmt",
	[[
		select wsc_sValue from tbl_web_server_conf where wsc_sName = 'GMS_ALLOWED_IP'
	]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef = 
{
	"_GetServerType",
	[[
		select sc_sVarValue from tbl_conf_server where sc_sVarName = "ServerType"
	]]
}
DefineSql(StmtDef,StmtOperater)

function 	GasMainSql.InitGasConf(data)
	local result1 = StmtOperater._InitEratingConfig_stmt:ExecStat()

	local result2 = StmtOperater._InitAllowedGmsIP_stmt:ExecStat()
	
	local result3	= StmtOperater._GetServerType:ExecStat()
	
	return {result1, result2,result3}
end	

------------------------------------------------------------------------------------------
local StmtDef = 
{
	"_SelectMaxItemId",

	"select max(ii_uId) from " .. LogDBName .. ".tbl_item_id"

}
DefineSql(StmtDef,StmtOperater)

local StmtDef = 
{
	"_DelNoUseItemId",

	"delete from " .. LogDBName .. ".tbl_item_id where ii_uId < ?"

}
DefineSql(StmtDef,StmtOperater)

--每隔2分钟清tbl_item_id表里非最大物品Id的其他数据。
function 	GasMainSql.DelNoUseItemId(data)
	local res = StmtOperater._SelectMaxItemId:ExecStat()
	local max_item_id = 0 
	if res:GetRowNum() > 0 then
		max_item_id = res:GetData(0,0)
		StmtOperater._DelNoUseItemId:ExecStat(max_item_id)
	end
end
------------------------------------------------------------------------------------------
local StmtDef = 
{
	"_SaveServerOnlineNum",
	[[
		replace into ONLINE_STAT values(1,?,now())
	]]
}
DefineSql(StmtDef,StmtOperater)

function GasMainSql.UpdateOnlineStat()
	local LogoutSql = RequireDbBox("LogoutServerDB")
	local online_num = LogoutSql.GetOnlineUserNum()
	StmtOperater._SaveServerOnlineNum:ExecStat(online_num)
end
------------------------------------------------------------------------------------------
function GasMainSql.SaveServiceOnlineUserInfo()
	local LogMgr = RequireDbBox("LogMgrDB")
	LogMgr.SaveServiceOnlineUserInfo()
end


local StmtDef = 
{
	"_UpdateCupStatus",
	"update tbl_cpu_status set cs_uMaxThreadUsage = ?,cs_uAllUsage = ? where s_uId = ?"
}
DefineSql(StmtDef,StmtOperater)


function GasMainSql.UpdateCupStatus(data)
	StmtOperater._UpdateCupStatus:ExecStat(data["MaxThreadUsage"], data["AllUsage"], g_CurServerId)
end


SetDbLocalFuncType(GasMainSql.GasStartUpFinalize)
SetDbLocalFuncType(GasMainSql.UpdateCupStatus)


return GasMainSql
