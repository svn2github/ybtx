local LoginSucMsg = MsgTbl.LoginSucMsg
local DisConnMsg = MsgTbl.DisConnMsg
local CreatePlayerMsg = MsgTbl.CreatePlayerMsg
local LoginWaitingMsg = MsgTbl.LoginWaitingMsg
local UpdateWaitingMsg = MsgTbl.UpdateWaitingMsg
local RepeatLoginMsg = MsgTbl.RepeatLoginMsg
local RetEnterOtherServerMsg = MsgTbl.RetEnterOtherServerMsg

local login_key_num = 0

function CheckUser(Conn, UserName)
	assert(IsCppBound(Conn))
	assert(UserName)
	
	Conn.m_UserName = UserName
	
	local Password_to_MD5 = Conn.m_UserName .. " 111111"	
	--MD5
	local MD5Digester = CMd5Digester:new()
	MD5Digester:Begin()
	MD5Digester:Append(Password_to_MD5, string.len(Password_to_MD5))
	local PasswordMD5 = CreateCBufAcr( CreateCBuf(33) )
	MD5Digester:GetMD5Base16Str(PasswordMD5)
	local strPasswordMD5 = PasswordMD5:ToString()
	
	login_key_num = login_key_num + 1		
	Gac2Gas:CheckUser(Conn, Conn.m_UserName, strPasswordMD5, tostring(login_key_num),"","")
end


--跨服登录
function LoginOtherServer(case_data)
	local Conn = case_data.m_Conn
	local sIP = Conn.m_IP 
	local Port = Conn.m_Port 
	local LoginType = Conn.m_LoginType
	local sKey = Conn.m_sKey 
	local LastMsgStr = Conn.m_LastMsgStr 
	
	local addr = CAddress:new()
	addr:SetAddress(sIP)
	addr:SetPort(tonumber(Port))	
	
	Conn = ReConnect(case_data, 1000, addr)
	assert(IsCppBound(Conn))
	local tempTime = GetProcessTime()
	Gac2Gas:CharEnterOtherServer(Conn, LoginType, sKey, LastMsgStr)
	if case_data.tempTime then
		--print(case_data.m_UserName,"发完消息所用的时间:",GetProcessTime()-case_data.tempTime)
	end
	local waited_msg = WaitForMsg(CreatePlayerMsg, DisConnMsg, RetEnterOtherServerMsg)
	if waited_msg == DisConnMsg then
		print(case_data.m_UserName,"在切场景的时候断开连接")
		return false
	elseif waited_msg == RetEnterOtherServerMsg then
		print(case_data.m_UserName,"切多服后,因为延迟,收到让切回原服的消息.")
		return false, 1
	else
		return true
	end
end

--login 函数的包装
function Login(case_data, login_type)
	local Conn = case_data.m_Conn
	local user_name = case_data.m_UserName
	
	while true do
		local waited_msg = SyncCheckUser(Conn, user_name)
		if waited_msg == LoginSucMsg then
			break;
		elseif waited_msg == LoginWaitingMsg then
			while true do
				local msg = WaitForMsg(LoginSucMsg, UpdateWaitingMsg)
				if msg == LoginSucMsg then
					break
				end
			end			
			break
		else
			TestSleep(2000)
		end
	end

	--创建角色，进入游戏
	SyncGetCharList(Conn)
	if #Conn.m_RoleList == 0 then
		SyncCreateRole(Conn)
	else
		SelectChar(Conn)
		EnterGame(Conn, login_type)
	end
	
	--如果等到的是CreatePlayerMsg消息，证明是同服登录的，否则则是跨服登录
	--进入跨服登录流程
	local waited_msg = WaitForMsg(RetEnterOtherServerMsg, CreatePlayerMsg)
	if waited_msg == CreatePlayerMsg then
		return
	else
		local res = LoginOtherServer(case_data)
		if not res then
			return
		end
	end
	
	
end

--默认的正常登录流程，如果测试用例不是测试跟登录有关的，我们都可以下面函数来进行登录，退出
function DefaultLoginFlow(case_data)
	local user_name = case_data.m_UserName
	
	local Conn = SyncConnect(ConnAddr)		
	print(user_name, Conn)
	case_data.m_Conn = Conn	
	Login(case_data, 0)
end

function DefaultLogoutFlow(case_data)
	SyncDisConnect(case_data.m_Conn)
	case_data.m_Conn = nil
end
