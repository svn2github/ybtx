
local CallSequenceId = 0
local CallGasTbl = {}

local function PackArg(...)
	local arg = {...}
	arg.n = select("#", ...)
	return arg
end

function CallGas(Conn, funName, callback, timeOutCallBack, time, ...)
	local tbl = {}
	
	local function retFun (...)
		UnRegisterTick(tbl.Tick)
		if callback then
			callback(...)
		end
		CallGasTbl[tbl.CallId] = nil
	end
	
	local function OnTimeOutTick()
		UnRegisterTick(tbl.Tick)
		if timeOutCallBack then
			timeOutCallBack()
		end
		CallGasTbl[tbl.CallId] = nil
	end
	
	tbl.RetFun = retFun
	tbl.CallId = CallSequenceId
	tbl.Tick = RegisterTick("CallGasTimeOutTick", OnTimeOutTick, time * 1000)
	CallGasTbl[CallSequenceId] = tbl
	local sendData = {}
	sendData.Arg = PackArg(...)
	sendData.FunName = funName
	sendData.CallId = CallSequenceId
	SendDataToServer(Conn, "CallGas", sendData)
	CallSequenceId = CallSequenceId + 1
end

--关服调用
function ClearCallGasTick()
	local count = 0
	for _, tbl in pairs(CallGasTbl) do
		UnRegisterTick(tbl.Tick)
		CallGasTbl[tbl.CallId] = nil
		count = count + 1
	end
	if count > 1000 then
		LogErr("关服时候CallGasTbl中残余内容比预计的多,可能有内存泄露", "残余内容 " .. count .. " 条")
	end
end


function ReceiveData.CallGas(serverId, data)
	local funName = data.FunName
	local arg = data.Arg
	assert(CallGasDef[funName])
	local result = PackArg(CallGasDef[funName]( unpack(arg, 1, arg.n) ))
	
	local sendData = {}
	sendData.FunResult = result
	sendData.CallId = data.CallId
	SendDataToServer(GetServerConn(serverId), "RetCallGas", sendData)
end

function ReceiveData.RetCallGas(serverId, data)
	local callId = data.CallId
	local arg = data.FunResult
	if CallGasTbl[callId] then
		CallGasTbl[callId].RetFun( unpack(arg, 1, arg.n) )
	end
end

