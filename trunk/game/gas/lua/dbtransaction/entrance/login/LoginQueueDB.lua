
local LoginQueueSql = class()
local os = os
local logoutTime = GasConfig.LogoutTime / 1000
local LoginQueueDB = CreateDbBox(...)


--------------------------------------------------------------------------------------
--删除账号下线信息
local StmtDef =
{
	"_DelLogoutUserInfo",
	"delete from tbl_user_logout where us_sName = ? and us_uGameID = ?"
}
DefineSql(StmtDef,LoginQueueSql)

local StmtDef =
{
	"_IsJustLogout",
	"select us_sName from tbl_user_logout where us_sName = ? and us_uGameID = ?"
}
DefineSql(StmtDef,LoginQueueSql)

function LoginQueueDB.DelLogoutUserInfo(data)
	local userName = data["userName"]
	local game_id = data["game_id"]
	LoginQueueSql._DelLogoutUserInfo:ExecSql("",userName,game_id)
--	if g_DbChannelMgr:LastAffectedRowNum() > 0 then
--		LoginQueueDB.UpdateWaitQueue(data)
--	end
end

function LoginQueueDB.IsJustLogout(userName,game_id)
	local result = LoginQueueSql._IsJustLogout:ExecStat(userName,game_id)
	return result:GetRowNum() > 0
end
 
--------------------------------------------------------------------------------------
local StmtDef = 
{
	"_GetServerMostUserNum",
	[[
		select 
			count(uo.us_uId) as userNum
		from 
			tbl_server left join tbl_user_online as uo on(tbl_server.s_uId = uo.uo_uOnServerId)
			group by tbl_server.s_uId 
			order by userNum desc
			limit 1
	]]
}
DefineSql(StmtDef, LoginQueueSql)

local StmtDef =
{
	"_GetOnlineUserNum",
	"select count(*) from tbl_user_online"
}
DefineSql(StmtDef,LoginQueueSql)

local StmtDef =
{
	"_GetTotalOnlineCharNum",
	"select count(*) from tbl_char_online"
}
DefineSql(StmtDef,LoginQueueSql)

local StmtDef =
{
	"_GetOnlineCharNum",
	"select count(*) as serverId from tbl_char_online group by co_uOnServerId order by serverId desc limit 1;"
}
DefineSql(StmtDef,LoginQueueSql)

--五分钟之内下线的账号数
local StmtDef =
{
	"_GetJustLogoutUserNum",
	"select count(*) from tbl_user_logout"
}
DefineSql(StmtDef,LoginQueueSql)

function LoginQueueDB.GetServerMostUserNum()
	local res1 = LoginQueueSql._GetOnlineCharNum:ExecStat()
	local res2 = LoginQueueSql._GetOnlineUserNum:ExecStat()
	local res3 = LoginQueueSql._GetTotalOnlineCharNum:ExecStat()
	local num = 0
	if res1:GetRowNum() > 0 then
		num = res1:GetData(0,0)
	end
	local online_user_num = num + (res2:GetData(0,0) - res3:GetData(0,0))
	return online_user_num
end

function LoginQueueDB.GetJustLogoutUserNum()
	local res = LoginQueueSql._GetJustLogoutUserNum:ExecStat()
	return res:GetData(0,0)
end
-----------------------------------------------------------------------------------------------------------

-- 查询在线账号数
local StmtDef=
{
	"IsUserOnline",
	[[
		select 
			count(*)
		from 
			tbl_user_online as uo, tbl_user_static as us
		where 
			uo.us_uId = us.us_uId and us.us_sName = ? and us_uGameID = ?
	]]
}
DefineSql(StmtDef,LoginQueueSql)

--@brief 检查顶号
local function IsUserOnline(userName,game_id)
	local res = LoginQueueSql.IsUserOnline:ExecStat(userName,game_id)
	return res:GetData(0,0) > 0
end

-----------------------------------------------------------------------------------------------------------
local StmtDef =
{
	"_UpdateUserWaitInfo",
	"update tbl_user_wait_queue set uwq_uOnServerId = ?, uwq_uContextId = ? where us_sName = ?"
}
DefineSql(StmtDef,LoginQueueSql)

local StmtDef =
{
	"_CanceLoginGame",
	"delete from tbl_user_wait_queue where us_sName = ? and uwq_uContextId = ? and us_uGameID = ?"
}
DefineSql(StmtDef,LoginQueueSql)

local StmtDef =
{
	"_GetWaitQueueDbId",
	"select uwq_uId from tbl_user_wait_queue where us_sName = ? and us_uGameID = ?"
}
DefineSql(StmtDef,LoginQueueSql)

local StmtDef =
{
	"_GetUserWaitQueueInfo",
	"select uwq_uId, uwq_uOnServerId, uwq_uContextId from tbl_user_wait_queue where us_sName = ? and us_uGameID = ?"
}
DefineSql(StmtDef,LoginQueueSql)

local StmtDef =
{
	"_GetWaitSequenceId",
	"select count(*) from tbl_user_wait_queue where uwq_uId <= ?"
}
DefineSql(StmtDef,LoginQueueSql)

local StmtDef =
{
	"_GetLastQueueId",
	"select max(uwq_uId) from tbl_user_wait_queue"
}
DefineSql(StmtDef,LoginQueueSql)

function LoginQueueDB.GetWaitQueueDbId(userName,game_id)
	local res = LoginQueueSql._GetWaitQueueDbId:ExecStat(userName,game_id)
	if res:GetRowNum() > 0 then
		return res:GetData(0,0)
	end
end

function LoginQueueDB.GetUserWaitQueueInfo(userName,game_id)
	local res = LoginQueueSql._GetUserWaitQueueInfo:ExecStat(userName,game_id)
	if res:GetRowNum() > 0 then
		return res:GetData(0,0), res:GetData(0,1), res:GetData(0,2)
	end
end

function LoginQueueDB.GetWaitSequenceId(dbId)
	local res = LoginQueueSql._GetWaitSequenceId:ExecStat(dbId)
	return res:GetData(0,0)
end

function LoginQueueDB.GetLastQueueId()
	local res = LoginQueueSql._GetLastQueueId:ExecStat()
	if res:GetRowNum() > 0 then
		return res:GetData(0,0)
	end
end

function LoginQueueDB.GetServerCanLoginNum()
	local maxOnLineUserNum = LoginQueueDB.GetServerMaxOnLineUserNum()
	local mostUserNum = LoginQueueDB.GetServerMostUserNum()
	local justLogoutUserNum = LoginQueueDB.GetJustLogoutUserNum()
	local canLoginNum = maxOnLineUserNum - (mostUserNum + justLogoutUserNum)
	local num = LoginQueueDB.JudgeServerSupport()
	if num > 0 then
		if num > canLoginNum then
			return canLoginNum
		else
			return num
		end
	else
		return 0
	end
end

function LoginQueueDB.CheckUserLoginGame(data)
	--判断是不是gm账号
	local GMDB = RequireDbBox("GMDB")
	local nGmLevel = GMDB.GetGmLevel(data)
	if nGmLevel then
		return true
	end
	local userName = data["user_name"]
	local contextId = data["context_id"]
	local game_id = data["game_id"]
	--重复登录
	if IsUserOnline(userName,game_id) then
		return true
	end
	
	--判断是不是在十分钟之内下线的账号
	if LoginQueueDB.IsJustLogout(userName,game_id) then
		return true
	end
	
	local canLoginNum = LoginQueueDB.GetServerCanLoginNum()
	return canLoginNum > 0
end
---------------------------------------------------------------------------------------
local StmtDef =
{
	"_SelectLoginGameUser",
	"select us_sName, uwq_uOnServerId, uwq_uContextId from tbl_user_wait_queue order by uwq_uId limit ?"
}
DefineSql(StmtDef,LoginQueueSql)


local StmtDef =
{
	"_DelLoginGameUser",
	"delete from tbl_user_wait_queue where us_sName = ? and us_uGameID = ?"
}
DefineSql(StmtDef,LoginQueueSql)


local loginServerId = nil
function LoginQueueDB.UpdateWaitQueue(data)
	local GasMainDB = RequireDbBox("GasMainDB")
	local ServerAttribute = GasMainDB.GetServerAttribute()

	local canLoginNum = LoginQueueDB.GetServerCanLoginNum()
	if canLoginNum > 0 then
		if loginServerId == nil then
			for serverId, v in pairs(ServerAttribute) do	
				if v.IsCanLogin then
					loginServerId = serverId
					break
				end
			end
		end
		assert(loginServerId, "没有登录服务器")
		Db2CallBack("UserAutoLoginGame", canLoginNum, loginServerId)
	end
	
end

--------------------------------------------------------------------------------------
local StmtDef =
{
	"_SelectUserWaitQueueSequence",
	"select us_sName, uwq_uContextId, uwq_uOnServerId from tbl_user_wait_queue where uwq_uId <= ? order by uwq_uId"
}
DefineSql(StmtDef,LoginQueueSql)

--function LoginQueueDB.GetUserWaitQueueSequence(data)
--	local lastId = data[1]
--	local res = LoginQueueSql._SelectUserWaitQueueSequence:ExecStat(lastId)
--	local resTbl = {}
--	for i = 0, res:GetRowNum() -1 do
--		if res:GetData(i, 2) == g_CurServerId then
--			table.insert(resTbl, {res:GetData(i, 0), res:GetData(i, 1), i+1})
--		end
--	end
--	return resTbl
--end
--------------------------------------------------------------------------------------
local StmtDef =
{
	"_SelectUserWaitQueueNum",
	"select count(*) from tbl_user_wait_queue"
}
DefineSql(StmtDef,LoginQueueSql)

-----------------------------------------------------------------------------------
--function LoginQueueDB.CanceLoginGame(data)
--	local userName = data["userName"]
--	local contextId = data["contextId"]
--	local game_id = data["game_id"]
--	LoginQueueSql._CanceLoginGame:ExecStat(userName, contextId,game_id)
--	if g_DbChannelMgr:LastAffectedRowNum() > 0 then
--		Db2GasCall("CanceLoginGame", data, g_CurServerId)
--		local lastId = LoginQueueDB.GetLastQueueId()
--		if lastId then
--			Db2GasCall("UpdateWaitQueueId", {lastId})
--		end
--		return true
--	end
--	return false
--end

------------------------------------------------------------------------------
--gm指令修改单个服务器最大承载人数
local StmtDef = 
{
	"_UpdateServerMaxOnLineUserNum",
	[[
		update tbl_conf_server set sc_sVarValue = ? where sc_sVarName = "SingleServerMaxUserNum"
	]]
}
DefineSql(StmtDef,LoginQueueSql)

function LoginQueueDB.SetMaxOnLineUserNum(data)
	local newMaxOnLineUserNum = data["newMaxOnLineUserNum"]
	local user_name = data["user_name"]
	--判断是不是gm账号
	local GMDB = RequireDbBox("GMDB")
	local nGmLevel = GMDB.GetGmLevel(data)
	if nGmLevel then
		LoginQueueSql._UpdateServerMaxOnLineUserNum:ExecStat(tostring(newMaxOnLineUserNum))
		LoginQueueDB.UpdateWaitQueue(data)
	end
end
-------------------------------------------------------------------------------------
--查询服务器单个进程最大承载人数
local StmtDef = 
{
	"_QueryServerMaxOnLineUserNum",
	[[
		select sc_sVarValue from tbl_conf_server where sc_sVarName = "SingleServerMaxUserNum"
	]]
}
DefineSql(StmtDef,LoginQueueSql)

function LoginQueueDB.GetServerMaxOnLineUserNum()
	local res =	LoginQueueSql._QueryServerMaxOnLineUserNum:ExecStat()
	return tonumber(res:GetData(0,0))
end
----------------------------------------------------------------------------------
--查询服务器最大承载人数
local StmtDef = 
{
	"_QueryAllServerMaxUserNum",
	[[
		select sc_sVarValue from tbl_conf_server where sc_sVarName = "AllServerMaxUserNum"
	]]
}
DefineSql(StmtDef,LoginQueueSql)

function LoginQueueDB.GetAllServerMaxOnLineUserNum()
	local res =	LoginQueueSql._QueryAllServerMaxUserNum:ExecStat()
	return tonumber(res:GetData(0,0))
end
-------------------------------------------------------------------------------
--gm指令修改服务器总承载人数
local StmtDef = 
{
	"_UpdateAllServerMaxUserNum",
	[[
		update tbl_conf_server set sc_sVarValue = ? where sc_sVarName = "AllServerMaxUserNum"
	]]
}
DefineSql(StmtDef,LoginQueueSql)

function LoginQueueDB.SetAllServerMaxUserNum(data)
	local newMaxOnLineUserNum = data["newMaxOnLineUserNum"]
	local user_name = data["user_name"]
	--判断是不是gm账号
	local GMDB = RequireDbBox("GMDB")
	local nGmLevel = GMDB.GetGmLevel(data)
	if nGmLevel then
		LoginQueueSql._UpdateAllServerMaxUserNum:ExecStat(tostring(newMaxOnLineUserNum))
		LoginQueueDB.UpdateWaitQueue(data)
	end
end
-------------------------------------------------------------------------------
--判断是不是超过服务器承载能力
function LoginQueueDB.JudgeServerSupport()
	local all_sever_num_set = LoginQueueDB.GetAllServerMaxOnLineUserNum()
	local res = LoginQueueSql._GetOnlineUserNum:ExecStat()
	local cur_online_user = res:GetData(0,0)
	local justLogoutUserNum = LoginQueueDB.GetJustLogoutUserNum()
	return (all_sever_num_set - cur_online_user - justLogoutUserNum)
end
-------------------------------------------------------------------------------
local StmtDef = 
{
	"_QueryWaitQueueCount",
	[[
		select count(*) from  tbl_user_wait_queue_count
	]]
}
DefineSql(StmtDef,LoginQueueSql)

local StmtDef = 
{
	"_UpdateWaitQueueCount",
	[[
		update tbl_user_wait_queue_count set uwqc_uCount = ? 
	]]
}
DefineSql(StmtDef,LoginQueueSql)


local StmtDef = 
{
	"_SaveWaitQueueCount",
	[[
		insert into tbl_user_wait_queue_count values(?) 
	]]
}
DefineSql(StmtDef,LoginQueueSql)

function LoginQueueDB.SaveWaitQueueCountDB(data)
	local uTotalCount = data["uTotalCount"]
	local res = LoginQueueSql._QueryWaitQueueCount:ExecStat()
	if res:GetData(0,0) == 0 then
		LoginQueueSql._SaveWaitQueueCount:ExecStat(uTotalCount)
	else
		LoginQueueSql._UpdateWaitQueueCount:ExecStat(uTotalCount)
	end
end

SetDbLocalFuncType(LoginQueueDB.CheckUserLoginGame)
--SetDbLocalFuncType(LoginQueueDB.CanceLoginGame)
--SetDbLocalFuncType(LoginQueueDB.GetUserWaitQueueSequence)

return LoginQueueDB