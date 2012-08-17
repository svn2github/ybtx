gas_require "multiserver/TransferDataInc"

local function date_type(v)
	local sType = type(v)
	if sType == "string" and string.len(v) >= 256 then
		return "lstring"
	end
	return sType
end

local function SendTable(Conn, data)
	for key, value in pairs(data) do
		local keyType, valueType = date_type(key), date_type(value)
		assert(g_SendPairKeyType[keyType],"SendDataToServer 不支持以 " .. keyType .. " 为key的数据")
		assert(g_SendPairValueType[valueType], "SendDataToServer 不支持以 " .. valueType .. " 为value的数据")
		local rpcFunName = g_PairRpcDef[keyType][valueType]
		if valueType == "table" then
			Gas2GasCall[rpcFunName](Gas2GasCall, Conn, g_CurServerId, key)
			SendTable(Conn, value)
		else
			Gas2GasCall[rpcFunName](Gas2GasCall, Conn, g_CurServerId, key, value)
		end
		
	end
	Gas2GasCall:SendTableEnd(Conn, g_CurServerId)
end


function SendDataToServer(Conn, receiveFunName, data)
	if not IsCppBound(Conn) then
		return
	end
	assert(type(data) == "table" and type(receiveFunName) == "string")
	Gas2GasCall:SendDataBegin(Conn, g_CurServerId, receiveFunName)
	SendTable(Conn, data)
	Gas2GasCall:SendDataEnd(Conn, g_CurServerId)
end

function ReceivePair(serverId, key, value)
	local stack = ReceiveData[serverId].stack
	local curTbl = stack[#stack]
	if value ~= nil then
		curTbl[key] = value
	else  --新一层table
		curTbl[key] = {}
		table.insert(stack, curTbl[key])
	end
	
	--ReceiveData[serverId].count = ReceiveData[serverId].count + 1
end

function Gas2GasDef:SendTableEnd(Conn, serverId)
	table.remove(ReceiveData[serverId].stack)
	--ReceiveData[serverId].count = ReceiveData[serverId].count + 1
end


function Gas2GasDef:SendDataBegin(Conn, serverId, funName)
	if ReceiveData[serverId] then
		LogErr("上次发送的数据未正常结束")
	end
	local tbl = {}
	tbl = {}
	tbl.FunName = funName
	tbl.data = {}
	tbl.stack = {}
	--tbl.count = 0
	table.insert(tbl.stack, tbl.data)
	ReceiveData[serverId] = tbl
end

function Gas2GasDef:SendDataEnd(Conn,serverId)
	local tbl = ReceiveData[serverId]
	assert(tbl,"异常的数据传输结束消息")
	assert(#tbl.stack == 0, "收到结束消息时 添加数据的栈还没完全退出")
	ReceiveData[serverId] = nil
	ReceiveData[tbl.FunName](serverId, tbl.data)
	
--	print(tbl.FunName .. " 发送数据使用了 " .. tbl.count  .. " 条消息")
end


--定义Rpc函数
for keyType, v in pairs(g_PairRpcDef) do
	for valueType, funName in pairs(v) do
		local str = 
		[[
			function Gas2GasDef:%s(Conn, serverId, key, value)
				ReceivePair(serverId, key, value)
			end 
		]]
		str = string.format(str, funName)
		loadstring(str)()
	end
end

