local LogErr = LogErr
local LogDBName = DeployConfig.MySqlDatabase .. "_log."

local MaxServerNum = 15
local SeparateTable = 
{
	"tbl_arg_common",
	"tbl_msg_common",
	"tbl_long_string_value",
}

local StmtOperater = class()
local StmtOperater = {}

local StmtDef = 
{
	"ClearMarkBySrcServer",
	"delete from tbl_msg_unready_mark where s_uSrcServer = ?"
}
DefineSql(StmtDef, StmtOperater)

local StmtDef = 
{
	"ClearMarkByDisServer",
	"delete from tbl_msg_unready_mark where s_uDisServer = ?"
}
DefineSql(StmtDef, StmtOperater)

local StmtDef = 
{
	"RemoveUnreadyMarkById",
	"delete from tbl_msg_unready_mark where s_uDisServer = ? and mc_uId = ?"
}
DefineSql(StmtDef, StmtOperater)

local StmtDef = 
{
	"SetUnreadyMarkById",
	"insert into tbl_msg_unready_mark values(?, ?, ?)"
}
DefineSql(StmtDef, StmtOperater)

local StmtDef =
{
	"GetMinUnreadyId",
	[[
	select min(mc_uId), s_uSrcServer from tbl_msg_unready_mark where s_uDisServer = ? group by s_uDisServer
	]]
}
DefineSql(StmtDef, StmtOperater)

local StmtDef =
{
	"lock",
	[[
	select * from tbl_msg_lock_mark where s_uId = ? for update
	]]
}
DefineSql(StmtDef, StmtOperater)




local function  BatchDefineSql(tbl)  --为了方便, 重写了函数 不改名
	for i = 1, MaxServerNum do
		local str = tbl[2]
		for _, v in pairs(SeparateTable) do
			str = string.gsub(str, v, LogDBName .. v ..i)
		end
		if str ==  tbl[2] then
			DefineSql(tbl, StmtOperater)
			break
		else
			DefineSql({tbl[1] .. i, str},  StmtOperater)
		end
	end
end


local StmtDef = 
{
	"InsertPair",
	"insert into tbl_arg_common(ac_sKey, ac_nKey, ac_sValue, ac_nValue, ac_acOwner) values(?,?,?,?,?)"
}
BatchDefineSql(StmtDef)

local StmtDef = 
{
	"GetPairs",
	"select ac_uId, ac_sKey, ac_nKey, ac_sValue, ac_nValue from tbl_arg_common where ac_acOwner = ?"
}
BatchDefineSql(StmtDef)

local StmtDef =
{
	"GetMemberTable",
	"select ac_uId from tbl_arg_common where ac_sValue is null and ac_nValue is null and ac_acOwner = ?"
}
BatchDefineSql(StmtDef)

local StmtDef =
{
	"DeleteMember",
	"delete from tbl_arg_common where ac_acOwner = ?"
}
BatchDefineSql(StmtDef)

local StmtDef =
{
	"DeleteArgId",
	"delete from tbl_arg_common where ac_uId = ?"
}
BatchDefineSql(StmtDef)

local StmtDef =
{
	"GetArgUseCount",
	"select count(*) from tbl_msg_common where ac_uId = ?"
}
BatchDefineSql(StmtDef)

local StmtDef =
{
	"InsertServerMsg",
	"insert into tbl_msg_common(mc_sFunName, ac_uId) values(?,?)"
}
BatchDefineSql(StmtDef)


local StmtDef =
{
	"GetServerMsg",
	[[
		select mc_sFunName, ac_uId from tbl_msg_common where mc_uId <= ? order by mc_uId
	]]
}
BatchDefineSql(StmtDef)


local StmtDef =
{
	"DeleteAllServerMsg",
	"delete from tbl_msg_common"
}
BatchDefineSql(StmtDef)

local StmtDef =
{
	"DeleteAllServerArg",
	"delete from tbl_arg_common"
}
BatchDefineSql(StmtDef)

local StmtDef =
{
	"DeleteAllLongStringValue",
	"delete from tbl_long_string_value"
}
BatchDefineSql(StmtDef)

local StmtDef =
{
	"DeleteReadyServerMsg",
	"delete from tbl_msg_common where mc_uId <= ?"
}
BatchDefineSql(StmtDef)



local StmtDef =
{
	"ClearArg",
	"delete from tbl_arg_common where ac_uId < ? "
}
BatchDefineSql(StmtDef)

local StmtDef =
{
	"ClearLongStrValue",
	[[
	delete from tbl_long_string_value
	where ac_uId not in(select ac_uId from tbl_arg_common)
	]]
}
BatchDefineSql(StmtDef)


local StmtDef =
{
	"InsertLongStrValue",
	"insert into tbl_long_string_value(ac_uId, lsv_sValue) values(?,?)"
}
BatchDefineSql(StmtDef)

local StmtDef =
{
	"DelLongStrValue",
	"delete from tbl_long_string_value where ac_uId = ?"
}
BatchDefineSql(StmtDef)

local StmtDef =
{
	"GetLongStrValue",
	"select lsv_sValue from tbl_long_string_value where ac_uId = ?"
}
BatchDefineSql(StmtDef)

local StmtDef =
{
	"GetMaxMsgId",
	"select max(mc_uId) from tbl_msg_common"
}
BatchDefineSql(StmtDef)


local ServerMsgDB = CreateDbBox(...)


local function ExecStat(name, serverId, ...)
	if serverId then
		assert(StmtOperater[name .. serverId])
		return StmtOperater[name .. serverId]:ExecStat(...)
	else
		return StmtOperater[name]:ExecStat(...)
	end
end


local function SavePair(serverId,key, value, ownerId)
	local sKey, nKey, sValue, nValue
	local keyType, valueType = type(key), type(value)
	if keyType == "string" then
		sKey = key
	else
		nKey = key
	end
	local isLongStr = false
	if valueType == "string" then
		if string.len(value) < 32  then
			sValue = value
		else
			isLongStr = true
		end
	elseif valueType == "number" then
		nValue = value
	else
		assert(valueType == "table", "不支持的参数数据类型" .. valueType)
	end
	--print("insert ", sKey, nKey, sValue, nValue, ownerId)
	ExecStat("InsertPair",serverId, sKey, nKey, sValue, nValue, ownerId)
	assert(g_DbChannelMgr:LastAffectedRowNum() > 0)
	local argId = g_DbChannelMgr:LastInsertId()
	if isLongStr then
		ExecStat("InsertLongStrValue", serverId, argId, value)
	elseif valueType == "table" then
		for _key, _value in pairs(value) do
			SavePair(serverId,_key, _value, argId)
		end
	end
	
	return argId
end

function ServerMsgDB.CreateArg(serverId, argTbl)
	if argTbl then
		assert(type(argTbl) == "table", " 参数必须为table")
		return SavePair(serverId,nil, argTbl, nil)
	end
end

function ServerMsgDB.GetArg(serverId, argId)
	if argId then
		local result = ExecStat("GetPairs", serverId, argId)
		local argTbl = {}
		for i = 0, result:GetRowNum() -1 do
			local acId, sKey, nKey, sValue, nVale = result:GetData(i,0), result:GetData(i,1), result:GetData(i,2), result:GetData(i,3), result:GetData(i,4)
			if not sValue and not nVale then
				local longStr = ExecStat("GetLongStrValue", serverId, acId)
				if longStr:GetRowNum() == 0 then --为table
					argTbl[sKey or nKey] = ServerMsgDB.GetArg(serverId, acId)
				else
					argTbl[sKey or nKey] = longStr:GetData(0,0)
				end
			else
				argTbl[sKey or nKey] = sValue or nVale
			end
		end
		result:Release()
		return argTbl
	end
end


local function DeleteArgTbl(serverId, argId)
	local result = ExecStat("GetMemberTable", serverId,argId)
	for i = 0, result:GetRowNum() -1 do
		DeleteArgTbl(serverId, result:GetData(i,0))
		ExecStat("DelLongStrValue", serverId, result:GetData(i,0))
	end
	result:Release()
	ExecStat("DeleteMember", serverId, argId)
	--ExecStat("DeleteArgId", serverId, argId)
end

function ServerMsgDB.DeleteArg(serverId, argId)
		DeleteArgTbl(serverId, argId)
		ExecStat("DeleteArgId",serverId,argId)
end

function ServerMsgDB.InsertServerMsg(serverId, funName, argId, isReady)
	ExecStat("InsertServerMsg",serverId, funName, argId)
	if not(g_DbChannelMgr:LastAffectedRowNum() > 0)  then
		local msg = "参数列表: " .. tostring(serverId) .. " " .. tostring(funName) .. " " .. tostring(argId) .. " ".. tostring(isReady)
		LogErr("tbl_msg_common 插入操作失败", msg)
	end
	local msgId = g_DbChannelMgr:LastInsertId()
	if isReady == 0 then --此服务器第一次调用
		StmtOperater.SetUnreadyMarkById:ExecStat(serverId, g_CurServerId, msgId)
	end
	return msgId
end

local function GetMaxMsgId()
	local result = ExecStat("GetMaxMsgId", g_CurServerId)
	if result:GetRowNum() > 0 then
		return result:GetData(0,0)
	end
end

local function GetMinUnreadyId()
	local result = StmtOperater.GetMinUnreadyId:ExecStat(g_CurServerId)
	if result:GetRowNum() > 0 then
		return result:GetData(0,0), result:GetData(0,1)
	end
end

function ServerMsgDB.GetDbCall(data)
	local msgId = data["msgId"]
	local msgTbl = {}
	local minArgId
	
	local maxReadyId = GetMaxMsgId() or -1
	local minUnreadyId, srcServerId = GetMinUnreadyId()
	if minUnreadyId then
		if maxReadyId > 0 and maxReadyId - minUnreadyId > 1500 then
			local msg = string.format("加锁服务器:%d 被锁服务器:%d 相差id: %d", srcServerId, g_CurServerId, maxReadyId - minUnreadyId)
			LogErr("最新的msgId 与 被锁住的最小id 相差过大, 可能是一次添加的msg过多, 或加锁服务器宕机没解锁, 为不卡住Db2Gas机制,这里强行解锁 (参看详细消息)", msg)
			ServerMsgDB.RemoveUnreadyMarkById(g_CurServerId, minUnreadyId)
			minUnreadyId, srcServerId = GetMinUnreadyId()
			if minUnreadyId then
				maxReadyId = minUnreadyId - 1
			end
		else
			maxReadyId = minUnreadyId - 1
		end
	end
	
	if maxReadyId >= 0 then
		local result = ExecStat("GetServerMsg",g_CurServerId, maxReadyId)
		ExecStat("DeleteReadyServerMsg",g_CurServerId, maxReadyId)
		if result:GetRowNum() > 0 then
			for i = 0, result:GetRowNum() -1 do
				local funName = result:GetData(i,0)
				local argId = result:GetData(i,1)
				local argTbl = ServerMsgDB.GetArg(g_CurServerId, argId)
				table.insert(msgTbl, {funName,argTbl})
				ServerMsgDB.DeleteArg(g_CurServerId, argId)
			end
			minArgId = result:GetData(0,1)
		end
	end

	
	return  msgTbl, minArgId
end

function ServerMsgDB.RemoveUnreadyMarkById(DisServerId, msgId)
	StmtOperater.RemoveUnreadyMarkById:ExecStat(DisServerId, msgId)
end

--根据arg的添加方式可知 ac_uId 小于 msg_common表中使用的最小ac_uId 的所有数据都为不在使用的, 可以清理掉
--故而简化了之前清理arg的方式,  并可保证无用arg不会累积
function ServerMsgDB.ClearArg(data)
	ExecStat("ClearArg",g_CurServerId, data["MinArgId"])
	ExecStat("ClearLongStrValue", g_CurServerId)
end

--服务器启动调用
function ServerMsgDB.ClearServerMsg(serverId)
	ExecStat("DeleteAllServerMsg",serverId)
--	local minUnreadyId = GetMinUnreadyId()
--	ServerMsgDB.ClearArg({["MinArgId"] = minUnreadyId})
	ExecStat("DeleteAllServerArg",serverId)
	ExecStat("DeleteAllLongStringValue",serverId)
	StmtOperater.ClearMarkBySrcServer:ExecStat(serverId)
	StmtOperater.ClearMarkByDisServer:ExecStat(serverId)
end

--function ServerMsgDB.TestDbCall(data)
--	if data[1] == 1 then
--		--StmtOperater.lock:ExecStat(2)
--		--StmtOperater.lock:ExecStat(1)
--		Db2GasCall("TestDbCall",{1,2}, 1)
--	else
--		--StmtOperater.lock:ExecStat(1)
--		print ("+++++++++++++++ CharLogout")
--		for i = 1, 2000 do
--			Db2GasCall("CharLogout", {1, "aaaaaa"}, 1) 
--		end
--		--StmtOperater.lock:ExecStat(2)
--		print("+++++++++++++++ CharLogout ----------- over")
--	end
--end


SetDbLocalFuncType(ServerMsgDB.GetDbCall)
SetDbLocalFuncType(ServerMsgDB.ClearArg)
return ServerMsgDB
