gas_require "dbtransaction/CallDbTransDefInc"

local DbProcessResult = DbProcessResult

--没第serverId 代表全服务器都发

local server_check
local server_count = 0

function Db2GasCall(funName, argTbl, ...)
	local arg = {...}
	local ServerMsgDB = RequireDbBox("ServerMsgDB")
	local GasMainDB = RequireDbBox("GasMainDB")
	
	if server_check == nil then
		local DbServerList = GasMainDB.GetServerList()
		server_check = {}
		server_count = 0
		assert( next(DbServerList), "GasMainDB.GetServerList 获得的服务器列表为空!!!!!!")
		for _,id in pairs(DbServerList) do
			server_check[id] = true
			server_count = server_count + 1
		end
		assert( server_count ~= 0 and next(server_check), "server_check 为空!!!!!  服务器数为:" .. server_count)
	end
	
	local serverList = {}
	if #arg == 0 then
		serverList = GasMainDB.GetServerList()
	else
		for i = 1, #arg do
			if arg[i] then
				if not server_check[arg[i]] then
					local str = " 当前存在的服务器:"
					for serverId in pairs(server_check) do
						str = str .. serverId .. ","
					end 
					LogErr("Db2GasCall 向不存在的服务器发消息",  "要发送的服务器Id:" .. arg[i] .. str)
				end
				table.insert(serverList, arg[i])
			end
		end
	end
	
	local sendCount = 0
	for _,serverId in ipairs(serverList) do
		sendCount = sendCount + 1
		local argId = ServerMsgDB.CreateArg(serverId, argTbl)
		local isReady = 0
		local isFirst = true

		local co = coroutine.running()
		local processResult = DbProcessResult[co]
		if not processResult then
			processResult = {}
		end
		if processResult["FlushDbCall"] and
			processResult["FlushDbCall"][serverId] then
			isReady = 1
			isFirst = false
		end

		local msgId = ServerMsgDB.InsertServerMsg(serverId, funName, argId, isReady)
		if isFirst then
			if processResult["FlushDbCall"] == nil then
				processResult["FlushDbCall"] = {}
			end
			processResult["FlushDbCall"][serverId] =  msgId
		end
	end
	
	if (#arg == 0 and sendCount ~= server_count)
		or (#arg ~= 0 and sendCount ~= #arg)then
		LogErr("Db2GasCall 实际发送的服务器数 与参数不一致")
	end
end



function Db2CallBack(callBackName, ...)
	local arg = {...}
	arg.n = select("#", ...)
	assert(type(callBackName) == "string" and callBackName ~= "", "函数名需要为字符串")
	local co = coroutine.running()
	local processResult = DbProcessResult[co]
	if not processResult then
		processResult = {}
		DbProcessResult[co] = processResult
	end
	local tbl = processResult["Db2CallBack"]
	if tbl == nil then
		tbl = {}
		processResult["Db2CallBack"] = tbl
	end
	table.insert(tbl, {callBackName, arg})
end

local function Db2GacBaseFun(typeName, charId, funcName, ...)
	local arg = {...}
	arg.n = select("#", ...)
	local co = coroutine.running()
	if not DbProcessResult[co] then
		DbProcessResult[co] = {}
	end
	if not DbProcessResult[co]["Db2GacById"] then
		DbProcessResult[co]["Db2GacById"] = {}
	end
	if not DbProcessResult[co]["Db2GacById"][typeName] then
		DbProcessResult[co]["Db2GacById"][typeName] = {}
	end
	local tbl = DbProcessResult[co]["Db2GacById"][typeName][charId]
	if not tbl then
		tbl = {}
		local ClientMsgDB = RequireDbBox("ClientMsgDB")
		tbl.MsgId = ClientMsgDB.GetMsgState(charId, typeName)
		tbl.MsgList = {}
		DbProcessResult[co]["Db2GacById"][typeName][charId] = tbl
	end
	
	table.insert(tbl.MsgList, {funcName, arg})
end

Db2GacById = {}
local mt = {}
mt.__index = function (t, k)
	return function(_, typeName, charId, ...)
		assert(type(typeName) == "string", "调用Db2GacById 第一个参数必须是表示消息分类的 字符串")
		assert(type(charId) == "number")
		assert(type(k) == "string")
		Db2GacBaseFun(typeName, charId, k, ...)
	end
end

setmetatable(Db2GacById, mt)


function DbMsgToConnById(typeName, charId, msgId, ...)
	assert(type(typeName) == "string", "调用DbMsgToConnById 第一个参数必须是表示消息分类的 字符串")
	assert(type(charId) == "number")
	assert(type(msgId) == "number")
	Db2GacBaseFun(typeName, charId, msgId, ...)
end

