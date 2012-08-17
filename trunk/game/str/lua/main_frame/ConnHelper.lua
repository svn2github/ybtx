local ConnSucMsg = MsgTbl.ConnSucMsg
local count = 0
--以同步方式创建一个连接
function SyncConnect(Addr)
	assert(Addr)
	--创建连接

	local Conn = ConnMgrStress:Connect(Addr)
	Conn.m_Co = coroutine.running()
	
	WaitForMsg(ConnSucMsg)
	
	return Conn
end

--断开一个连接
function SyncDisConnect(Conn, UserName)
	if not IsCppBound(Conn) then
		return	
	end
	if UserName then
		count = count + 1
		print(count,UserName)
	end
	Conn:ShutDown()
	return
end

--断线，等待一段时间重连
function ReConnect(case_data, time, addr)
	local Conn = case_data.m_Conn
	
	SyncDisConnect(Conn)
	case_data.m_Conn = nil
	
	TestSleep(time)
	
	Conn = SyncConnect(addr)
	case_data.m_Conn = Conn
	
	return Conn
end
