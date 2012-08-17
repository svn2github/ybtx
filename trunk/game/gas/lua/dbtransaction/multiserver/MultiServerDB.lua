
local StmtOperater = class()
local math = math
local LogErr = LogErr

local StmtDef = 
{
	"_GetServerShutTime",
	[[
		select unix_timestamp(sv_dtDate) from tbl_var_server where sv_sVarName = "ServerShutTime" and sv_uServerId = ?
	]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef = 
{
	"_InitServerTime",
	[[
		insert ignore into tbl_var_server(sv_sVarName,sv_sVarValue,sv_dtDate, sv_uServerId) values("ServerShutTime","",now(), ?)
	]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef = 
{
	"_UpdateSomeCharLogOutTime",
	[[
		update 	tbl_char_onlinetime join tbl_char_online co on co.cs_uId = tbl_char_onlinetime.cs_uId 
		set 		co_dtLastLogOutTime = if( co_dtLastLoginTime > from_unixtime(?), co_dtLastLoginTime, from_unixtime(?) )
		where 	co_dtLastLogOutTime <= co_dtLastLoginTime and co_uOnServerId = ?
	]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef = 
{
	"GetServerCount",
	"select count(*) from tbl_server"
}
DefineSql(StmtDef,StmtOperater)

local StmtDef =
{
	"GetServerTbl",
	"select s_uId, s_sClientIp, s_sServerIp,s_uClientPort, s_uServerPort from tbl_server"
}
DefineSql(StmtDef,StmtOperater)

local StmtDef = 
{
	"GetServerPlayerCount",
	[[ 
		select 
			count(*) 
		from 
			tbl_char_online
		where 
			tbl_char_online.co_uOnServerId = ?
	]]
}
DefineSql(StmtDef, StmtOperater)


local StmtDef = 
{
	"GetCpuStatus",
	--"select s_uId, cs_uMaxThreadUsage, cs_uAllUsage from tbl_cpu_status"
	[[
		select s.s_sServerIp, avg(cs.cs_uAllUsage)
		from tbl_server as s ,tbl_cpu_status as cs 
		where s.s_uId = cs.s_uId
		group by s.s_sServerIp
	]]
}
DefineSql(StmtDef, StmtOperater)

local StmtDef = 
{
	"DelCpuStatus",
	"delete from tbl_cpu_status where s_uId not in (select s_uId from tbl_server) or s_uId = ?"
}
DefineSql(StmtDef, StmtOperater)


local StmtDef = 
{
	"GetIdleServerByIp",
	--"select s_uId, cs_uMaxThreadUsage, cs_uAllUsage from tbl_cpu_status"
	[[
		select s.s_uId
		from tbl_server as s ,tbl_cpu_status as cs 
		where s.s_uId = cs.s_uId and s.s_sServerIp = ?
		order by cs.cs_uMaxThreadUsage
		limit 1
	]]
}
DefineSql(StmtDef, StmtOperater)

local StmtDef =
{
	"GetServerIdList",
	"select s_uId from tbl_server"
}
DefineSql(StmtDef,StmtOperater)


--服务器重启时删除该服务器上的上次登录过的账号下线信息
local StmtDef =
{
	"_DelLogoutUserInfo",
	"delete from tbl_user_logout where ul_uOnServerId = ?"
}
DefineSql(StmtDef,StmtOperater)

local StmtDef =
{
	"_DelWaitUserByServerId",
	"delete from tbl_user_wait_queue where uwq_uOnServerId = ?"
}
DefineSql(StmtDef,StmtOperater)

local StmtDef =
{
	"_DelWaitQueueCount",
	"delete from tbl_user_wait_queue_count"
}
DefineSql(StmtDef,StmtOperater)

local StmtDef =
{
	"_DelTongNeedfireByServerId",
	"delete from tbl_tong_needfire where tn_uServerId = ?"
}
DefineSql(StmtDef,StmtOperater)

local StmtDef =
{
	"GetOnlineInfo",
	[[
		select
			c.cs_uId, c.c_sName
		from 
			tbl_char_online as co, tbl_char as c
		where
			co.cs_uId = c.cs_uId  
	]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef =
{
	"GetOnlineInfoByLevel",
	[[
		select
			cb.cs_uId
		from 
			tbl_char_online as co, tbl_char_basic as cb,tbl_member_tong as tmt, tbl_tong as tt
		where
			co.cs_uId = cb.cs_uId and 
			cb.cb_uLevel >= 20 and cb.cs_uId = tmt.cs_uId and tmt.t_uId = tt.t_uId and (tt.t_uType = 1 or tt.t_uType = 3)
	]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef =
{
	"GetOnlineInfoByCamp",
	[[
		select
			cs.cs_uId
		from 
			tbl_char_online as co, tbl_char_static as cs
		where
			co.cs_uId = cs.cs_uId and 
			cs.cs_uCamp = ?
	]]
}
DefineSql(StmtDef,StmtOperater)


local StmtDef = 
{
	"UpdataConfServer",
	[[
		update tbl_conf_server set sc_sVarValue = ? where sc_sVarName = ?
	]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef = 
{
	"InsertConfServer",
	[[
		insert into tbl_conf_server (sc_sVarName, sc_sVarValue) values (?,?)
	]]
}
DefineSql(StmtDef,StmtOperater)


local StmtDef = 
{
	"GetConfServer",
	[[
		select sc_sVarValue from tbl_conf_server where sc_sVarName = ?
	]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef = 
{
	"DelDelaySaveInfo",
	"delete from tbl_delay_save_info where s_uId = ?"
}
DefineSql(StmtDef,StmtOperater)


local StmtDef =
{
	"GetOnlineInfoUsedAct",
	[[
		select
			cb.cs_uId,ifnull(ac_HistoryTimes,0),ifnull(ccp.sc_uId,0), ifnull(cfp.sc_uId,0)
		from 
			tbl_char_online as co, tbl_char_basic as cb
		left join tbl_activity_history_count ahc on ahc.cs_uId = cb.cs_uId 
		and ac_sActivityName = ?
		left join tbl_char_fb_position cfp on cfp.cs_uId = cb.cs_uId 
		left join tbl_char_change_position ccp on ccp.cs_uId = cb.cs_uId 
		where
			co.cs_uId = cb.cs_uId and
			cb.cb_uLevel >= ? and cb.cb_uLevel <= ?
	]]
}
DefineSql(StmtDef,StmtOperater)

local StmtDef =
{
	"GetOnlineInfoByTime",
	[[
			select
			cb.cs_uId,ifnull(ac_sActivityName,"0"),ifnull(ccp.sc_uId,0), ifnull(cfp.sc_uId,0), count(*)
		from 
			tbl_char_online as co, tbl_char_basic as cb
		left join tbl_activity_count ac on ac.cs_uId = cb.cs_uId
		and ac_sActivityName = ? and date(ac_dtTime) = date(now()) 
		left join tbl_char_fb_position cfp on cfp.cs_uId = cb.cs_uId 
		left join tbl_char_change_position ccp on ccp.cs_uId = cb.cs_uId 
		where
			co.cs_uId = cb.cs_uId and
			cb.cb_uLevel >= ? and cb.cb_uLevel <= ? group by cb.cs_uId
	]]
}
DefineSql(StmtDef,StmtOperater)


local MultiServerDB = CreateDbBox(...)

function MultiServerDB.GetServerCount()
	local result = StmtOperater.GetServerCount:ExecSql("n")
	local count = 0
	if result:GetRowNum() > 0 then
		count = result:GetData(0,0)
	end
	result:Release()
	return count
end

function MultiServerDB.GetServerShutTime(serverId)
	local result = StmtOperater._GetServerShutTime:ExecStat(serverId)
	return result:GetData(0,0)
end
 
--计算宕机后未完成的正常逻辑, 删除服务器相关的所有数据.  只做清理指定服务器数据用
function MultiServerDB.ClearServerData(serverId)
	local SceneMgrDB = RequireDbBox("SceneMgrDB")
	local JoinActionDB = RequireDbBox("JoinActionDB")
	local ServerMsgDB = RequireDbBox("ServerMsgDB")
	local ClientMsgDB = RequireDbBox("ClientMsgDB")
	local GasTongRobResourceDB = RequireDbBox("GasTongRobResourceDB")
	--修改宕机后所有在线玩家的离线时间
	StmtOperater._InitServerTime:ExecSql('', serverId)
	local shutTime = MultiServerDB.GetServerShutTime(serverId)
	StmtOperater._UpdateSomeCharLogOutTime:ExecSql('', shutTime, shutTime, serverId)
	
	SceneMgrDB._ClearAllFbScene(serverId)
	
	JoinActionDB.ClearTeamOnServerStartUp(serverId)
	GasTongRobResourceDB.DeleteTongInfoByServerId(serverId)
	StmtOperater._DelLogoutUserInfo:ExecSql('',serverId)
	StmtOperater._DelWaitUserByServerId:ExecSql('',serverId)
	ServerMsgDB.ClearServerMsg(serverId)
	ClientMsgDB.ClearClientMsg(serverId)
	StmtOperater.DelDelaySaveInfo:ExecStat(serverId)
	StmtOperater.DelCpuStatus:ExecStat(serverId)
	if serverId == 1 then
		StmtOperater._DelWaitQueueCount:ExecStat()
	end
	StmtOperater._DelTongNeedfireByServerId:ExecStat(serverId)
end

function MultiServerDB.GetOnlineInfo()
	return StmtOperater.GetOnlineInfo:ExecStat()
end

function MultiServerDB.GetOnlinePlayerInfo()
	local result = StmtOperater.GetOnlineInfoByLevel:ExecStat()
	return result
end

function MultiServerDB.GetOnlinePlayerInfoByCamp(data)
	local result = StmtOperater.GetOnlineInfoByCamp:ExecStat(data[1])
	return result
--	local result = StmtOperater.GetOnlineInfo:ExecStat()
--	local num = result:GetRowNum()
--	local playerId = 0
--	local TongBasicBox = RequireDbBox("GasTongBasicDB")
--	local tbl = {}
--	if num > 0 then
--		for i = 0, num - 1 do
--			playerId = result:GetData(i, 0)
--			local playerCamp = TongBasicBox.GetCampByCharID(playerId)
--			if playerCamp == data[1] then
--				table.insert(tbl, playerId)
--			end
--		end
--	end
--	return tbl
end

function MultiServerDB.GetOnlinePlayerUsedAct(data)
	local maxTimes = data["MaxTimes"]
	local minLevel = data["MinLevel"]
	local maxLevel = data["MaxLevel"]
	local cueOnce = data["CueOnce"]
	local gameName = data["gameName"]
	local ActivityCountDB = RequireDbBox("ActivityCountDB")
	local resTbl = {}
	if cueOnce and cueOnce == 1 then   -- 检测进入一次
		local res = StmtOperater.GetOnlineInfoUsedAct:ExecStat(gameName, minLevel, maxLevel)
		return res, false
	end
	local result = StmtOperater.GetOnlineInfoByTime:ExecStat(maxTimes,minLevel, maxLevel)
	return result, true
end

function MultiServerDB.GetServerTbl()
	local result = StmtOperater.GetServerTbl:ExecStat()
	return result:ToTable(true)
end

function MultiServerDB.GetServerPlayerCount(serverId)
	local result = StmtOperater.GetServerPlayerCount:ExecSql('n', serverId)
	local count = result:GetData(0,0)
	result:Release()
	return count
end

function MultiServerDB.InsertConfServer(name, value)
	StmtOperater.InsertConfServer:ExecStat(name, value)
	return g_DbChannelMgr:LastAffectedRowNum() > 0
end

function MultiServerDB.UpdataConfServer(name, value)
	local oldValue = MultiServerDB.GetConfServer(name)
	if not oldValue then
 		return MultiServerDB.InsertConfServer(name, tostring(value))
	elseif oldValue ~= value then
		StmtOperater.UpdataConfServer:ExecStat(tostring(value), name)
		return g_DbChannelMgr:LastAffectedRowNum() > 0
	end
end

function MultiServerDB.GetConfServer(name, defaultValue)
	local result = StmtOperater.GetConfServer:ExecStat(name)
	if result:GetRowNum() > 0 then
		return result:GetData(0,0)
	else
		if defaultValue and MultiServerDB.InsertConfServer(name, defaultValue) then
			return defaultValue
		end
	end
end


function MultiServerDB.GetIdleServer()
	local GasMainDB = RequireDbBox("GasMainDB")
	local ServerAttribute = GasMainDB.GetServerAttribute()
	
	local serverId
	local result = StmtOperater.GetCpuStatus:ExecStat()
	if result:GetRowNum() > 0 then
		local index = 0
		for i = 1, result:GetRowNum() -1 do
			if result:GetData(i,1) < result:GetData(index,1) then
				index = i
			end
		end
		local serverRet = StmtOperater.GetIdleServerByIp:ExecStat(result:GetData(index, 0))
		serverId = serverRet:GetData(0,0)
	else
		LogErr("没有cpu状态, 采用随机分配副本")
		local serverList = StmtOperater.GetServerIdList:ExecStat()
		while true do
			serverId = serverList:GetData(math.random(0, serverList:GetRowNum() -1) ,0)
			if not(ServerAttribute[serverId] and ServerAttribute[serverId].IsNoFbScene) then
				break
			end
		end
	end

	assert(not(ServerAttribute[serverId] and ServerAttribute[serverId].IsNoFbScene) )
	return serverId
end


return MultiServerDB
