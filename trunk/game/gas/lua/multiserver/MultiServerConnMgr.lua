
Gas2GasDef = {}

local tblErrMap = {}
tblErrMap[-1] = "rpc函数id错误"
tblErrMap[-2] = "验证码校验出错，可能出现网络消息错误"
tblErrMap[-3] = "数据格式错误"
tblErrMap[-4] = "没有找到该rpc处理函数"
tblErrMap[-5] = "没有Gac2Gas"

function Gas2GasDef:OnError(num, fun)
--[[
		-1 ：表示id错误
		-2 ：表示验证码错误
		-3 ：表示数据格式错误
		-4 ：表示没有找到处理函数
		-5 ：表示返回的错误
		>0 ：表示用户自己返回的参数
--]]
	if num > 0 then
		error(fun .. " rpc call error: " .. "rpc函数不允许有返回值" )
		return false
	end
			
	error(fun .. " rpc call error: " .. tblErrMap[num] )
	return false
end

Gas2GasCall = {}

g_MultiServerConnList = {}
g_OtherServerConnList = {}
g_StartConnTime = {}
g_OtherServerShutDownList = {}--记录其他正常关服的服务器
g_ConnOtherServerTickTbl = {}
g_MultiServReConnTickTime = 5000 --单位毫秒

function Gas2GasDef:TestMultiRpc(conn, str)
	print("send from " .. str)
	local addr = CAddress:new()
	conn:GetLocalAddress(addr)
	print(addr:GetAddress(),addr:GetPort())
	--Gas2GasCall:TestMultiRpc(gpipe,"aaaaaaaaaa")
end

function Gas2GasDef:SendSuccConn(conn, ServerId)
	g_OtherServerConnList[ServerId] = conn
end

function Gas2GasDef:ShutDownNotifyOtherServer(conn, ServerId)
	g_OtherServerShutDownList[ServerId] = true
end

function IsServerShutDownState(ServerId)
	return g_OtherServerShutDownList[ServerId]
end

function ResetServerShutDownState(ServerId)
	g_OtherServerShutDownList[ServerId] = nil
end

function SetOtherServConnByServId(ServId,Conn)
	g_OtherServerConnList[ServId] = Conn
end

function GetServerConn(ServId)
	return g_OtherServerConnList[ServId]
end

--这个函数是为了配合 Gas2GasAutoCall 使用
function GetServerAutoConn(ServId)
	if ServId == g_CurServerId then
		return g_CurServerId
	end
	return g_OtherServerConnList[ServId]
end

local gas_path = GetLoadPath("gas")
function CreateListenOfLocalAddr(ip,port)
	g_LocalAddrListen = CMultiServerConn:new(false)
	g_LocalAddrListen:Listen(ip,port)
	g_LocalAddrListen:RegistCall("gas", "multiserver/Gas2GasProList", "Gas2GasCall", "IPipe")
	g_LocalAddrListen:RegistDef("gas", "multiserver/Gas2GasProList", "Gas2GasDef", "IPipe")
end

function ClearListenOfLocalAddr()
	if g_LocalAddrListen then
		g_LocalAddrListen:ShutDown()
		g_LocalAddrListen = nil
	end
end

function CreateConnToTargetServer(targetserverid,ip,port)
	local MultiServerConn = CMultiServerConn:new(false)
	MultiServerConn.m_TargetServerId = targetserverid
	g_StartConnTime[targetserverid] = os.time()
	MultiServerConn:Connect(ip,port)
	MultiServerConn:RegistCall("gas", "multiserver/Gas2GasProList", "Gas2GasCall", "IPipe")
	MultiServerConn:RegistDef("gas", "multiserver/Gas2GasProList", "Gas2GasDef", "IPipe")
	g_MultiServerConnList[targetserverid] = MultiServerConn
end

function ClearConnOfOtherServer()
	if g_MultiServerConnList then
		for serverId, Conn in pairs(g_MultiServerConnList) do
			Conn:ShutDown()
		end
		g_MultiServerConnList = nil
	end
end

function ShutDownNotifyOtherServer()
	if g_OtherServerConnList then
		for serverId, Conn in pairs(g_OtherServerConnList) do
			Gas2GasCall:ShutDownNotifyOtherServer(Conn,g_CurServerId)
		end
	end
end

function ClearConnOtherServerTick()
	for servid, tick in pairs(g_ConnOtherServerTickTbl) do
		UnRegisterTick(tick)
		g_ConnOtherServerTickTbl[servid] = nil
	end
	g_ConnOtherServerTickTbl = {}
end






------------------------以下是一些为了使用方便做的封装------------------------
Gas2GasAutoCall = {}

local mt = {__index = function(t, k)
		return function(autoTbl, conn, ...)
			if not (Gas2GasDef[k] and Gas2GasCall[k]) then
				LogErr("未定义Gas2Gas函数", k)
			end
			if conn == g_CurServerId then
				Gas2GasDef[k](Gas2GasDef, conn, ...)
			else
				Gas2GasCall[k](Gas2GasCall, conn, ...)
			end
		end
	end
	}
setmetatable(Gas2GasAutoCall, mt)