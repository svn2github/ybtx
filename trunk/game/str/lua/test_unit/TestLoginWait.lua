--排队等待
--测试这个时候记得将GasConfig.lua里面LogouTime改成10000或者以下，不然排队等待的时间会很长s

local TestLoginWaitCase = TestCase("TestLoginWait", true)  --我们不使用默认的初始化

local LoginSucMsg = MsgTbl.LoginSucMsg
local LoginWaitingMsg = MsgTbl.LoginWaitingMsg
local DelWaitingMsg = MsgTbl.DelWaitingMsg
local UpdateWaitingMsg = MsgTbl.UpdateWaitingMsg
local RepeatLoginMsg = MsgTbl.RepeatLoginMsg

local function GetRandWaitTime()
	local MinTime = 1
	local MaxTime = 10

	return math.random(MinTime,MaxTime) * 1000
end

function TestLoginWaitCase:SetUp(case_data)
	local Conn = SyncConnect(ConnAddr)
	case_data.m_Conn = Conn	
end

function TestLoginWaitCase:test_CancelWait(case_data)
	local Conn = case_data.m_Conn

	local user_name = case_data.m_UserName
	
	local msg = SyncCheckUser(Conn, user_name)
	--我们这里要测试的是登录排队，如果能够登录成功，那么等待一段时间之后断线重连
	if msg == LoginSucMsg or msg == RepeatLoginMsg then
		local waited_time = GetRandWaitTime()

		Conn = ReConnect(case_data, waited_time, ConnAddr)

		return
	end
	
	--取消排队
	assert(IsCppBound(Conn))
	StressRpcToServer("CancelLoginWaiting", Conn)
	
	--如果是排队登录了，我们就可以测试了
	while true do
		local LastWaitingID = Conn.m_WaitingID
		assert(LastWaitingID)
		
		local msg =	WaitForMsg(UpdateWaitingMsg, DelWaitingMsg, LoginSucMsg)
		if msg == UpdateWaitingMsg then
			assert(Conn.m_WaitingID < LastWaitingID)
		else
			break
		end
	end
	
	local waited_time = GetRandWaitTime()
	Conn = ReConnect(case_data, waited_time, ConnAddr)
end

function TestLoginWaitCase:test_UpdateWaiting(case_data)
	local Conn = case_data.m_Conn
	local user_name = case_data.m_UserName
	
	local msg = SyncCheckUser(Conn, user_name)
	--我们这里要测试的是登录排队，如果能够登录成功，那么等待一段时间之后断线重连
	if msg == LoginSucMsg or msg == RepeatLoginMsg then
		local waited_time = GetRandWaitTime()
		Conn = ReConnect(case_data, waited_time, ConnAddr)
		return
	end
	
	while true do
		local LastWaitingID = Conn.m_WaitingID
		assert(LastWaitingID)
		local waited_msg = WaitForMsg(UpdateWaitingMsg, LoginSucMsg)
		if waited_msg == LoginSucMsg then
			local waited_time = GetRandWaitTime()
			Conn = ReConnect(case_data, waited_time, ConnAddr)
			return
		else
			local CurWaitingID = Conn.m_WaitingID
			assert(CurWaitingID < LastWaitingID)
		end
	end
end

function TestLoginWaitCase:TearDown(case_data)
	SyncDisConnect(case_data.m_Conn)
	case_data.m_Conn = nil
end

return TestLoginWaitCase