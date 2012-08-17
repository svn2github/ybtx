gas_require "e_protocol/EProtocolInc"
gas_require "e_protocol/LuaInt2Byte"
etc_gas_require "GasConfig"

function EProtocol:Ctor()
	self.m_bConnected 				=  false						
	self.m_Pipe 					= g_App:GetIPipeWithBufferSize(self, 1024*1024*10,1024*1024*10,false)
	self.m_nSendIBPayNum 			= 0 --一次最多发送的消费数据条数
	self.m_nSendMoneyExchangeNum 	= 0	--一次最多发送的金钱流通数据条数
	self.m_tblIBPayDetail = {}
	self.m_tblMoneyExchangeDetail = {}
end

--消息批量处理
function EProtocol:RecvDataEnd()
	if # self.m_tblIBPayDetail > 0 then
		CExchangeMoney.ERating_SendIBPayMsg_Succ(self.m_tblIBPayDetail)
		self.m_tblIBPayDetail = {}
	end
	if # self.m_tblMoneyExchangeDetail > 0 then
		CExchangeMoney.ERating_UserCashExchange_Succ(self.m_tblMoneyExchangeDetail)
		self.m_tblMoneyExchangeDetail = {}
	end
end

local function _GetDefaultGameID()
	local GBSAuthType = tonumber(GasConfig.GBSAuthType)
	if GBSAuthType == 0 or 1 == GBSAuthType then
		return tonumber(ERatingConfig.AGIP_GameID)
	elseif GBSAuthType == 2 then
		return tonumber(ERatingConfig.AGIP_YYGameID)
	elseif GBSAuthType == 3 then
		return tonumber(ERatingConfig.AGIP_SinaGameID)
	end
end

local function _InitMsgHead(MsgID, TotalSize, acr,gameid)
	g_AGIP_Sequence_ID = g_AGIP_Sequence_ID + 1
	WriteUnShortNum(acr, TotalSize)
	local gameid  = gameid or _GetDefaultGameID()
	local remain_packages = 0
	acr:Write("C", ERatingConfig.AGIP_Version)
	acr:Write("C", remain_packages)
	WriteUnIntNum(acr, MsgID)
	WriteUnIntNum(acr, g_AGIP_Sequence_ID)
	WriteUnIntNum(acr, gameid)
	WriteUnIntNum(acr, ERatingConfig.AGIP_Gateway_id)
	return g_AGIP_Sequence_ID
end

local function _AddCheckSum(buf,size)
	local buf_size =  size + 2
	local resv = 0
	WriteUnShortNum(buf, resv)
	local check_sum = Crc16(buf,buf_size)
	WriteUnShortNum(buf, check_sum)
end

--[[
	连接AGIP服务器
--]]
local function _Connect(Tick, data)
	local addr= CAddress:new()
	local ip = ERatingConfig.AGIP_PRTL_IP
	local port = ERatingConfig.AGIP_PRTL_Port
	data.m_ServerId = tonumber(ERatingConfig.AGIP_Gateway_id)*100 + 1
	addr:SetAddress(ip)
	addr:SetPort(port)
	data.m_Pipe:Connect(addr)
	local msg = "connect to " .. ip .. ":" .. port
	g_EPrtlLog:Write(msg)
end
function EProtocol:Connect()
	if (GasConfig.SkipGBSAuth == 0) then
		g_AGIP_Connect_tick = RegisterTick("EProtocol:Connect",_Connect, 5*1000,self)
	end
end

--[[
	断开AGIP服务器
--]]
function EProtocol:Shutdown()
	self.m_bShuttedDown = true
	for i, v in pairs(g_tbl_AGIP_Tick) do
		g_tbl_AGIP_Tick[i] = nil
		UnRegisterTick(v)
	end
	if self.m_bConnected and self.m_bBinded then
		self:_UnBind()
	end

	if self.m_bConnected then
		self.m_Pipe:ShutDown()
		self.m_bConnected = nil
	end
	
	if self.m_Pipe and IsCppBound(self.m_Pipe) then
		self.m_Pipe:SetHandler(nil)
		self.m_Pipe=nil
	end
	g_EPrtlLog:Write("shutdown")
	UnRegisterTick(g_AGIP_Connect_tick)
	g_AGIP_Connect_tick = nil
end

--[[
	绑定AGIP服务器
--]]
local function _Bind_Callback(data, buf, sequence_id)
	local eprtl = data[1]
	local ReturnCode = ReadIntNum(buf)
	g_EPrtlLog:Write("bind callback with code: " .. ReturnCode)
	if ReturnCode == 1 then
		print("------------------------------EratingBindSucc------------------------------")
		eprtl.m_bBinded = true
		eprtl.m_uGateWay = ReadIntNum(buf)
		g_ep_shake_hand_tick = RegClassTick("EPShakeHand", eprtl._ShakeHand, 20*1000, eprtl)
		g_ep_data_report_tick = RegClassTick("GatewayDataReport", eprtl.GatewayDataReport, 15*1000, eprtl)
		return
	end
end

function EProtocol:_Bind(server_id)
	assert(self.m_bConnected)
	self.m_bBinded = false
	local cmd_size = 84
	local gate_way_code = ERatingConfig.AGIP_Gateway_Code
	local gate_way_pass = ERatingConfig.AGIP_Gateway_Passwd
	local mac_addr = "000000000000"
	local str_size = cmd_size + size_msg_checksum + size_msg_header
	local acr = CreateCBufAcr(CreateCBuf(str_size+1))
	acr:Seek( "begin" )

	local sequence_id = _InitMsgHead(0x10000001,str_size,acr)
	_write_size_str_to_buf(acr, gate_way_code, 32)
	_write_size_str_to_buf(acr, gate_way_pass, 32)
	_write_size_str_to_buf(acr, mac_addr, 12)
	local reconnect_flag = 1
	local pad = 0
	acr:Write("cc", reconnect_flag, pad)
	local pad2 = 0
	WriteUnShortNum(acr, pad2)

	WriteUnIntNum(acr, server_id)
	
	_AddCheckSum(acr,str_size-size_msg_checksum)
	acr:Seek( "begin" )
	local res = self.m_Pipe:Send(acr, str_size)
	g_tbl_AGIP_CallBack[sequence_id] = {_Bind_Callback, {self, sequence_id} }
	g_EPrtlLog:Write("bind")
end


--[[
	握手包，每20s发送一次。
	是tick触发，在Bind里面
--]]
local function _ShakeHand_Callback(data, buf, sequence_id)
end

function EProtocol:_ShakeHand()
	local cmd_size = 0
	local str_size = cmd_size + size_msg_checksum + size_msg_header
	local acr = CreateCBufAcr(CreateCBuf(str_size+1))
	local sequence_id = _InitMsgHead(0x10001001,str_size,acr)
	
	_AddCheckSum(acr,str_size-size_msg_checksum)
	acr:Seek( "begin" )
	
	local res = self.m_Pipe:Send(acr, str_size)
	g_tbl_AGIP_CallBack[sequence_id] = {_ShakeHand_Callback, {self, sequence_id}}
end


--[[
	解除绑定包
--]]
local function _UnBind_Callback(data, buf, sequence_id)
	local eprtl = data[1]
	local ReturnCode = ReadIntNum(buf)
	if ReturnCode == 1 then
		eprtl.m_bBinded = false
		return
	end
end

function EProtocol:_UnBind()
	UnRegisterTick(g_ep_shake_hand_tick)
	UnRegisterTick(g_ep_data_report_tick)
	g_ep_shake_hand_tick = nil
	g_ep_data_report_tick = nil
	local cmd_size = 4
	local str_size = cmd_size + size_msg_checksum + size_msg_header
	local acr = CreateCBufAcr(CreateCBuf(str_size+1))
	local sequence_id = _InitMsgHead(0x10000002,str_size,acr)
	WriteUnIntNum(acr, self.m_uGateWay)
	
	_AddCheckSum(acr,str_size-size_msg_checksum)
	acr:Seek( "begin" )
	local res = self.m_Pipe:Send(acr, str_size)
	
	g_tbl_AGIP_CallBack[sequence_id] = {_UnBind_Callback, self}
end


--[[
	玩家登陆
--]]
local function UserLoginCallback(data, buf, sequence_id)
	local ReturnCode = ReadIntNum(buf)
	if not g_tbl_AGIP_Tick[sequence_id] then
		g_EPrtlLog:Write(string.format("sid: %d, retcode: %d, %s", sequence_id, ReturnCode, "Login TimeoutCallback"))
		return
	end
	UnRegisterTick(g_tbl_AGIP_Tick[sequence_id])
	g_tbl_AGIP_Tick[sequence_id] = nil
	local eprtl = data[1]
	local callback_func = data[2]
	g_EPrtlLog:Write(string.format("sid: %d, retcode: %d, %s", sequence_id, ReturnCode, "Login Callback"))
	if ReturnCode == 1 then
		local user_id = ReadUnIntNum(buf) 
		local user_type = ReadByteNum(buf) 
		local adult_flag = ReadByteNum(buf) 
		local user_class = ReadByteNum(buf) 
		local pad = ReadByteNum(buf) 
		local user_point = ReadIntNum(buf) 
		local promoter_id = ReadUnIntNum(buf) 
		local user_flag = ReadUnIntNum(buf) 
		local activation_code = _read_size_str_from_buf(buf,42)
		local tbl_user_info = {user_id, user_type, adult_flag, user_class, user_role_count, user_point, promoter_id, user_flag,activation_code}

		callback_func(UserLoginRetCode.LoginSucc, "", tbl_user_info)
	else
		callback_func(UserLoginRetCode.LoginFail,ReturnCode)
	end
end


local function LocalUserAuth(Tick, data)
	local local_auth_func, sequence_id = data[1], data[2]
	UnRegisterTick(g_tbl_AGIP_Tick[sequence_id])
	g_tbl_AGIP_Tick[sequence_id] = nil
	local_auth_func(UserLoginRetCode.LoginFail, "time out")
	g_EPrtlLog:Write(string.format("sid: %d, retcode: %d, %s", sequence_id, 0, "Login timeout"))
end

function EProtocol:UserLogin(data, callback)
	if not (self.m_bConnected and self.m_bBinded) then
		local callback_fun = callback[1]
		callback_fun(UserLoginRetCode.LocalLogin,500)
	end
	local username, password, password_type, user_port, user_ip, matrix_password, matrix_coord,gameid,DistStr,IDCode = unpack(data)
	password_type = password_type or 1
	if matrix_password and matrix_password ~= "" then
		password_type = 3
	end
	local pad1 = 0
	local AD_ID = DistStr or ""
	local cmd_size = 0
	local str_size = 0
	local acr 
	local sequence_id
	local IDCode = IDCode or ""
	local GBSAuthType = tonumber(GasConfig.GBSAuthType)
	if GBSAuthType == 0 then
		--专服
		if gameid ~= tonumber(ERatingConfig.AGIP_GameID) then
			LogErr("专服gameid错误","gameid:" .. gameid)
		end
		local gate_id = ERatingConfig.AGIP_Gateway_id		
		local sInfo = "<GWID>" .. gate_id .. "<GameID>" .. gameid .. "<UserIP4>" .. user_ip .. "<Port>" .. user_port .. "<PW>" .. password .. "<MAC><UN>" .. username .. "<UID><ADID>" .. AD_ID
		
		pad1 = 0
		cmd_size = string.len(sInfo)
		
		str_size = cmd_size + size_msg_checksum + size_msg_header
		
		acr = CreateCBufAcr(CreateCBuf(str_size+1))
		acr:Seek( "begin" )
	
		sequence_id = _InitMsgHead(0x10003801,str_size,acr)

		acr:Write("s", sInfo)
	elseif GBSAuthType == 1 and gameid == tonumber(ERatingConfig.AGIP_GameID) then
		pad1 = 0
		cmd_size = 228
		str_size = cmd_size + size_msg_checksum + size_msg_header
		acr = CreateCBufAcr(CreateCBuf(str_size+1))
		acr:Seek( "begin" )
	
		sequence_id = _InitMsgHead(0x10003307,str_size,acr,gameid)
	
		_write_size_str_to_buf(acr, username, 32)
		_write_size_str_to_buf(acr, password, 32)
	
		WriteByteNum(acr, password_type)
		WriteByteNum(acr, pad1)
		WriteUnShortNum(acr, user_port)
		WriteUnIntNum(acr, user_ip)

		_write_size_str_to_buf(acr, matrix_password, 32)
		_write_size_str_to_buf(acr, matrix_coord, 16)
	
		local mac = ""
		local SN1 = ""
		local SN2 = ""
		_write_size_str_to_buf(acr, mac, 12)
		_write_size_str_to_buf(acr, AD_ID,32)
		_write_size_str_to_buf(acr, SN1, 32)
		_write_size_str_to_buf(acr, SN2, 32)
	else
		--yy登陆
		local gate_id = ERatingConfig.AGIP_Gateway_id		
		local sInfo = "<GWID>" .. gate_id .. "<GameID>" .. gameid .. "<UserIP4>" .. user_ip .. "<Port>" .. user_port .. "<PW>" .. password .. "<MAC><UN>" .. username .. "<UID><HWSN1><HWSN2><ADID>" .. AD_ID .. "<IDCode>" .. IDCode
		
		pad1 = 0
		cmd_size = string.len(sInfo)
		
		str_size = cmd_size + size_msg_checksum + size_msg_header
		
		acr = CreateCBufAcr(CreateCBuf(str_size+1))
		acr:Seek( "begin" )
	
		sequence_id = _InitMsgHead(0x10003802,str_size,acr,gameid)

		acr:Write("s", sInfo)
	
	end
	_AddCheckSum(acr,str_size-size_msg_checksum)
	acr:Seek( "begin" )
	
	local ret_size = self.m_Pipe:Send(acr, str_size)
	if ret_size ~= str_size then
		LogErr("erating消息发送错误", "UserLogin--str_size:" .. str_size .. "ret_size:" .. ret_size)
	end
	
	g_tbl_AGIP_CallBack[sequence_id] = {UserLoginCallback, {self, callback[1]}}
	g_tbl_AGIP_Tick[sequence_id] = RegisterTick("EProtocol:UserLogin", LocalUserAuth, 120*1000, {callback[2], sequence_id})
	g_EPrtlLog:Write(string.format("sid: %d, retcode: %d,gameid:%d, %s", sequence_id, 0,gameid, username .. " Login " ))
end

--[[
	创建角色
--]]
local function _CreateRole_Callback(data, buf, sequence_id)
	local ReturnCode = ReadIntNum(buf)
	if not g_tbl_AGIP_Tick[sequence_id] then
		g_EPrtlLog:Write(string.format("sid: %d, retcode: %d, %s", sequence_id, ReturnCode, "CreateRole TimeoutCallback"))
		return
	end
	UnRegisterTick(g_tbl_AGIP_Tick[sequence_id])
	g_tbl_AGIP_Tick[sequence_id] = nil
	local eprtl = data[1]
	local callback_func = data[2][1]
	local callback_data = data[2][2]
	local role_id = 0
	g_EPrtlLog:Write(string.format("sid: %d, retcode: %d, %s", sequence_id, ReturnCode, "CreateRole callback"))
	if ReturnCode == 1 then
		role_id = ReadUnIntNum(buf) 
	end
	callback_func(role_id,ReturnCode, callback_data,ReturnCode)
end

local function _CreateRoleTimeOut(Tick, data)
	local userinfo, callback, sequence_id = data[1], data[2], data[3]
	UnRegisterTick(g_tbl_AGIP_Tick[sequence_id])
	g_tbl_AGIP_Tick[sequence_id] = nil
	callback[1](0, callback[2], 602)
	g_EPrtlLog:Write(string.format("sid: %d, retcode: %d, %s", sequence_id, 0, "CreateRole timeout"))
end

function EProtocol:CreateRole(data, callback)
	if not (self.m_bConnected and self.m_bBinded) then
		return
	end
	local id, name, gender, occupation, level, ip, port, community_id = unpack(data)
	local pad = 0
	local cmd_size = 48
	local str_size = cmd_size + size_msg_checksum + size_msg_header
	local acr = CreateCBufAcr(CreateCBuf(str_size+1))
	acr:Seek( "begin" )

	local sequence_id = _InitMsgHead(0x10003102,str_size,acr)

	WriteUnIntNum(acr, id)
	_write_size_str_to_buf(acr, name, 32)
	WriteByteNum(acr, gender)
	WriteByteNum(acr, occupation)
	WriteUnShortNum(acr, Level)
	WriteUnIntNum(acr, ip)
	WriteUnShortNum(acr, port)
	WriteUnByteNum(acr,community_id)
	WriteUnByteNum(acr, pad)
	
	_AddCheckSum(acr,str_size-size_msg_checksum)
	acr:Seek( "begin" )
	local res = self.m_Pipe:Send(acr, str_size)
	if res ~= str_size then
		LogErr("erating消息发送错误", "CreateRole--str_size:" .. str_size .. "res:" .. res)
	end
	g_tbl_AGIP_CallBack[sequence_id] = {_CreateRole_Callback, {self, callback}}
	g_tbl_AGIP_Tick[sequence_id] = RegisterTick("EProtocol:CreateRole", _CreateRoleTimeOut, 20*1000, {data, callback, sequence_id})
	g_EPrtlLog:Write(string.format("sid: %d, retcode: %d, %s", sequence_id, 0, id .. " CreateRole " .. name ))
end


--[[
	玩家登陆游戏
--]]
local function _User_EnterGame_Callback(data, buf, sequence_id)
	local ReturnCode = ReadIntNum(buf)
	if not g_tbl_AGIP_Tick[sequence_id] then
		g_EPrtlLog:Write(string.format("sid: %d, retcode: %d, %s", sequence_id, ReturnCode, "EnterGame TimeoutCallback"))
		return
	end
	UnRegisterTick(g_tbl_AGIP_Tick[sequence_id])
	g_tbl_AGIP_Tick[sequence_id] = nil
	local tbl_balance_info = {}
	g_EPrtlLog:Write(string.format("sid: %d, retcode: %d, %s", sequence_id, ReturnCode, "EnterGame callback"))
	if ReturnCode == 1 then
		local balance_count = ReadIntNum(buf)
		for i = 1, balance_count do
			local subject_id = ReadIntNum(buf)
			local amount = ReadIntNum(buf)
			tbl_balance_info[subject_id] = amount
		end
		data[1](tbl_balance_info)
		return
	end
	data[2](ReturnCode)
end

local function _EnterGameTimeOut(Tick, data)
	local info, callback, sequence_id = data[1], data[2], data[3]
	UnRegisterTick(g_tbl_AGIP_Tick[sequence_id])
	g_tbl_AGIP_Tick[sequence_id] = nil
	callback[2]("time out")
	g_EPrtlLog:Write(string.format("sid: %d, retcode: %d, %s", sequence_id, 0, "EnterGame timeout"))
end

function EProtocol:UserEnterGame(data, callback)
	if not (self.m_bConnected and self.m_bBinded) then
		callback[2]("time out")
		return
	end
--	local rating_id = self.m_uGateWay
	local server_id = self.m_ServerId
	local user_id, role_id, role_level, user_port, user_ip,gender,occupation,community = unpack(data)
	local cmd_size = 26*4
	local str_size = cmd_size + size_msg_checksum + size_msg_header
	local acr = CreateCBufAcr(CreateCBuf(str_size+1))
	acr:Seek( "begin" )

	local sequence_id = _InitMsgHead(0x10003106,str_size,acr)

	WriteUnIntNum(acr, server_id)
	WriteUnIntNum(acr, user_id)
	WriteUnIntNum(acr, role_id)
--	WriteUnIntNum(acr, rating_id)
	WriteUnShortNum(acr, role_level)
	local corps = 0
	WriteUnByteNum(acr, gender)
	WriteUnByteNum(acr, occupation)
	WriteUnIntNum(acr, corps)
	WriteUnShortNum(acr, community)
	WriteUnShortNum(acr, user_port)
	WriteUnIntNum(acr, user_ip)
	
	local mac = ""
	local SN1 = ""
	local SN2 = ""
	_write_size_str_to_buf(acr, mac, 12)
	_write_size_str_to_buf(acr, SN1, 32)
	_write_size_str_to_buf(acr, SN2, 32)

	_AddCheckSum(acr,str_size-size_msg_checksum)
	acr:Seek( "begin" )

	local ret_size = self.m_Pipe:Send(acr, str_size)
	if ret_size ~= str_size then
		LogErr("erating消息发送错误", "UserEnterGame--str_size:" .. str_size .. "ret_size:" .. ret_size)
	end
	g_tbl_AGIP_CallBack[sequence_id] = {_User_EnterGame_Callback, callback}
	g_tbl_AGIP_Tick[sequence_id] = RegisterTick("EProtocol:UserEnterGame", _EnterGameTimeOut, 20*1000, {data, callback, sequence_id})
	local msg = "userid: " .. user_id .. " roleid: " .. role_id .. " EnterGame"
	g_EPrtlLog:Write(string.format("sid: %d, retcode: %d, %s", sequence_id, 0, msg))
end

--[[
	玩家登出游戏
--]]
local function _User_Logout_Callback(data, buf, sequence_id)
	local ReturnCode = ReadIntNum(buf)
	if not g_tbl_AGIP_Tick[sequence_id] then
		g_EPrtlLog:Write(string.format("sid: %d, retcode: %d, %s", sequence_id, ReturnCode, "Logout TimeoutCallback"))
		return
	end
	local tbl_balance_info = {}
	local callback = data[1]
	UnRegisterTick(g_tbl_AGIP_Tick[sequence_id])
	g_tbl_AGIP_Tick[sequence_id] = nil
	g_EPrtlLog:Write(string.format("sid: %d, retcode: %d, %s", sequence_id, ReturnCode, "Logout callback"))
	if ReturnCode == 1 then
		local balance_count = ReadIntNum(buf)
		for i = 1, balance_count do
			local subject_id = ReadIntNum(buf)
			local amount = ReadIntNum(buf)
			tbl_balance_info[subject_id] = amount
		end
		callback(true)
		return
	end
	callback(false,ReturnCode)
end

local function _LogoutTimeOut(Tick, data)
	local info, callback, sequence_id = data[1], data[2], data[3]
	UnRegisterTick(g_tbl_AGIP_Tick[sequence_id])
	g_tbl_AGIP_Tick[sequence_id] = nil
	g_EPrtlLog:Write(string.format("sid: %d, retcode: %d, %s", sequence_id, 0, "Logout timeout"))
end

function EProtocol:UserLogout(data, callback)
	if not (self.m_bConnected and self.m_bBinded) then
		return
	end
	local rating_id = self.m_uGateWay
	local user_id, role_id, role_accupation, role_level, money1, money2, experience = unpack(data)
	local logout_flag = 1

	local cmd_size = 32
	local str_size = cmd_size + size_msg_checksum + size_msg_header
	local acr = CreateCBufAcr(CreateCBuf(str_size+1))
	acr:Seek( "begin" )

	local sequence_id = _InitMsgHead(0x10003303,str_size,acr)

	WriteUnIntNum(acr, user_id)
	WriteUnIntNum(acr, role_id)
	WriteByteNum(acr, logout_flag)
	WriteByteNum(acr, role_accupation)
	WriteUnShortNum(acr, role_level)
	WriteUnIntNum(acr, rating_id)
	WriteUnIntNum(acr, money1)
	WriteUnIntNum(acr, money2)
	WriteLongLongNum(acr, experience)

	_AddCheckSum(acr,str_size-size_msg_checksum)
	acr:Seek( "begin" )
		
	self.m_Pipe:Send(acr, str_size)

	g_tbl_AGIP_CallBack[sequence_id] = {_User_Logout_Callback, {callback, user_id}}
	g_tbl_AGIP_Tick[sequence_id] = RegisterTick("EProtocol:UserLogout", _LogoutTimeOut, 20*1000, {data, callback, sequence_id})
	local msg = "userid: " .. user_id .. " roleid: " .. role_id .. " Logout"
	g_EPrtlLog:Write(string.format("sid: %d, retcode: %d, %s", sequence_id, 0, msg))
end


--[[
	网关数据上报[系统向玩家赠送行为]
--]]
local function _GatewayDataReport_Callback(data, buf, sequence_id)
	--print("in _GatewayDataReport_Callback")
end


function EProtocol:GatewayDataReport()
	if not (self.m_bConnected and self.m_bBinded) then
		return
	end
	local cmd_size = 16
	local ServerID = self.m_ServerId
	local DataCount = 1
	local DataType = 1
	local str_size = cmd_size + size_msg_checksum + size_msg_header
	
	g_OnlineUserNum = g_OnlineUserNum or {}
	
	for i,v in pairs(g_OnlineUserNum) do
			local gameid,current_char_num = i,v
			local acr = CreateCBufAcr(CreateCBuf(str_size+1))
			acr:Seek( "begin" )
		
			local sequence_id = _InitMsgHead(0x10002003,str_size,acr,gameid)
			WriteUnIntNum(acr, ServerID)
			WriteIntNum(acr, DataCount)
			WriteIntNum(acr, DataType)
			WriteIntNum(acr, current_char_num)
		
			_AddCheckSum(acr,str_size-size_msg_checksum)
			acr:Seek( "begin" )
				
			self.m_Pipe:Send(acr, str_size)

			g_tbl_AGIP_CallBack[sequence_id] = {_GatewayDataReport_Callback}
			local msg = "gate way data report gameid:" .. gameid
			g_EPrtlLog:Write(string.format("sid: %d, retcode: %d, %s", sequence_id, 0, msg))
	end
	local function global_user_num(tblUserNum)
		g_OnlineUserNum = {}
		for i=1,tblUserNum:GetRowNum() do
			g_OnlineUserNum[tblUserNum(i,2)] = tblUserNum(i,1)
		end
	end
	CallDbTrans("LogoutServerDB", "GetGlobalUserNum", global_user_num, {})
end

--[[
	角色改名
--]]
local function _RenameRole_Callback(data, buf, sequence_id)
	UnRegisterTick(g_tbl_AGIP_Tick[sequence_id])
	g_tbl_AGIP_Tick[sequence_id] = nil
	g_tbl_AGIP_CallBack[sequence_id] = nil

	local eprt = data[1]
	local param = data[2]
	local callback = data[3]
	
	local ReturnCode = ReadIntNum(buf)
	local msg = "Rename role callback"
	g_EPrtlLog:Write(string.format("sid: %d, retcode: %d, %s", sequence_id, ReturnCode, msg))
	if ReturnCode == 1 then
		callback(true)
		return
	end
	callback(false,ReturnCode)
end

local function _RenameRoleTickOut(Tick, data)
	local param, callback, sequence_id = unpack(data)
	UnRegisterTick(g_tbl_AGIP_Tick[sequence_id])
	g_tbl_AGIP_Tick[sequence_id] = nil
	callback(false, "timeout")
	g_tbl_AGIP_CallBack[sequence_id] = nil
	local msg = "Rename role timeout"
	g_EPrtlLog:Write(string.format("sid: %d, retcode: %d, %s", sequence_id, 0, msg))
end

function EProtocol:RenameRole(data, callback)
	if not (self.m_bConnected and self.m_bBinded) then
		callback(true)
		return
	end
	
	local userid = data['userid']
	local roleid = data['roleid']
	local rolename = data['newname']
	local cmd_size = 40
	local str_size = cmd_size + size_msg_checksum + size_msg_header

	local acr = CreateCBufAcr(CreateCBuf(str_size+1))
	acr:Seek( "begin" )

	local sequence_id = _InitMsgHead(0x10003107,str_size,acr)
	
	WriteUnIntNum(acr, userid)
	WriteUnIntNum(acr, roleid)
	_write_size_str_to_buf(acr, rolename, 32)

	_AddCheckSum(acr,str_size-size_msg_checksum)
	acr:Seek( "begin" )
	
	self.m_Pipe:Send(acr, str_size)

	g_tbl_AGIP_CallBack[sequence_id] = {_RenameRole_Callback, {self, data, callback}}
	g_tbl_AGIP_Tick[sequence_id] = RegisterTick("EProtocol:RenameRole", _RenameRoleTickOut, 20*1000, {data, callback, sequence_id})
	local msg = "user: " .. "Rename role " .. roleid .. " new name is: " .. rolename
	g_EPrtlLog:Write(string.format("sid: %d, retcode: %d, %s", sequence_id, 0, msg))
end

--[[
	活动礼品查询
--]]
local function __ActivityItemQuery_callback(data,buf,sequence_id)
	g_tbl_AGIP_CallBack[sequence_id] = nil
	local ReturnCode = ReadIntNum(buf)
	if not g_tbl_AGIP_Tick[sequence_id] then
		g_EPrtlLog:Write(string.format("sid: %d, retcode: %d, %s", sequence_id, ReturnCode, "ActivityItemQuery TimeoutCallback"))
		return
	end	
	UnRegisterTick(g_tbl_AGIP_Tick[sequence_id])
	g_tbl_AGIP_Tick[sequence_id] = nil
	local user_id, role_id,activity_id = unpack(data[2])
	local callback_func = data[3]
	local ItemInfo = {}
	g_EPrtlLog:Write(string.format("sid: %d, retcode: %d, %s", sequence_id, ReturnCode, "ActivityItemQuery Callback"))
	if ReturnCode == 1 then
		local item_count = ReadIntNum(buf)
		for i=1,item_count do
			local activity_id = ReadUnIntNum(buf)
			local ItemNameI = _read_size_str_from_buf(buf,32)
			local item_left_num = ReadIntNum(buf)
			local begin_time = ReadUnIntNum(buf)
			local end_time = ReadUnIntNum(buf)
			table.insert(ItemInfo,{activity_id,ItemNameI,item_left_num,begin_time,end_time})
		end
	end
	callback_func(ItemInfo)
end

local function _ActivityItemQuery_timeout(Tick, data)
	local sequence_id = data[3]
	UnRegisterTick(g_tbl_AGIP_Tick[sequence_id])
	g_tbl_AGIP_Tick[sequence_id] = nil
	local callback_func = data[2]
	g_EPrtlLog:Write(string.format("sid: %d, retcode: %d, %s", sequence_id, 0, "ActivityItemQuery timeout"))
end
function EProtocol:ActivityItemQuery(data, callback)
	if not (self.m_bConnected and self.m_bBinded) then
		return
	end
	local user_id, role_id, activity_id,role_level = unpack(data)
	local cmd_size = 20
	local str_size = cmd_size + size_msg_checksum + size_msg_header
	local acr = CreateCBufAcr(CreateCBuf(str_size+1))
	acr:Seek( "begin" )

	local sequence_id = _InitMsgHead(0x10003509,str_size,acr)
	WriteUnIntNum(acr, user_id)
	WriteUnIntNum(acr, role_id)
	WriteUnIntNum(acr, activity_id)
	WriteUnShortNum(acr, role_level)
	local pad1, pad2 = 0, 0
	WriteUnShortNum(acr, pad1)
	WriteUnIntNum(acr, pad2)
	
	_AddCheckSum(acr,str_size-size_msg_checksum)
	acr:Seek( "begin" )

	self.m_Pipe:Send(acr, str_size)

	g_tbl_AGIP_CallBack[sequence_id] = {__ActivityItemQuery_callback, {self, data, callback}}
	g_tbl_AGIP_Tick[sequence_id] = RegisterTick("EProtocol:ActivityItemQuery", _ActivityItemQuery_timeout, 20*1000, {data, callback, sequence_id})
	g_EPrtlLog:Write(string.format("sid: %d, retcode: %d, %s", sequence_id, 0, "user_id:"..user_id .. "role_id:".. role_id .. " ActivityItemQuery:" ..activity_id))
end

local function __UserItemMinus_callback(data,buf,sequence_id)
	UnRegisterTick(g_tbl_AGIP_Tick[sequence_id])
	g_tbl_AGIP_Tick[sequence_id] = nil
	g_tbl_AGIP_CallBack[sequence_id] = nil
	local callback_succ_func = data[3][1]
	local ReturnCode = ReadIntNum(buf)
	g_EPrtlLog:Write(string.format("sid: %d, retcode: %d, %s", sequence_id, ReturnCode," UserItemMinus callback"))

	if ReturnCode == 1 or ReturnCode == -1461 or ReturnCode == -1457 or ReturnCode == -1450 then
		callback_succ_func()
		return
	end
	local callback_fail_func = data[3][2]
	callback_fail_func(ReturnCode)
end

local function _UserItemMinus_timeout(Tick, data)
	local sequence_id = data[3]
	UnRegisterTick(g_tbl_AGIP_Tick[sequence_id])
	g_tbl_AGIP_Tick[sequence_id] = nil
	g_tbl_AGIP_CallBack[sequence_id] = nil
	local callback_func = data[2][2]
	g_EPrtlLog:Write(string.format("sid: %d, retcode: %d, %s", sequence_id, 0," UserItemMinus timeout"))
end
function EProtocol:UserItemMinus(data, callback)
	if not (self.m_bConnected and self.m_bBinded) then
		return
	end
	local user_id, role_id, activity_id,item_code,item_num = unpack(data)
	local cmd_size = 52
	local str_size = cmd_size + size_msg_checksum + size_msg_header
	local acr = CreateCBufAcr(CreateCBuf(str_size+1))
	acr:Seek( "begin" )

	local sequence_id = _InitMsgHead(0x10003505,str_size,acr)
	local gateway_id = ERatingConfig.AGIP_Gateway_id
	WriteUnIntNum(acr, gateway_id)
	WriteUnIntNum(acr, user_id)
	WriteUnIntNum(acr, role_id)
	WriteUnIntNum(acr, activity_id)
	_write_size_str_to_buf(acr, item_code, 32)
	WriteIntNum(acr, item_num)
	_AddCheckSum(acr,str_size-size_msg_checksum)
	acr:Seek( "begin" )

	self.m_Pipe:Send(acr, str_size)

	g_tbl_AGIP_CallBack[sequence_id] = {__UserItemMinus_callback, {self, data, callback}}
	g_tbl_AGIP_Tick[sequence_id] = RegisterTick("EProtocol:UserItemMinus", _UserItemMinus_timeout, 20*1000, {data, callback, sequence_id})
	g_EPrtlLog:Write(string.format("sid: %d, retcode: %d, %s", sequence_id, 0, "role_id:".. role_id .. "UserItemMinus" .. activity_id))
end


-------------------------------------------------------------
-------------------下面的功能在道具商城用--------------------
-------------------------------------------------------------
--[[
	道具商城购买物品
--]]
local function _BuyProduct_callback(data, buf, sequence_id)
	UnRegisterTick(g_tbl_AGIP_Tick[sequence_id])
	g_tbl_AGIP_Tick[sequence_id] = nil
	g_tbl_AGIP_CallBack[sequence_id] = nil
	local callback_func = data[3][1]
	local detail_id = data[2][1]
	local ReturnCode = ReadIntNum(buf)
	if ReturnCode == 1 or ReturnCode == -1460 then
		--返回-1460，同一流水号重复发送
		local cost_money = ReadIntNum(buf)
		callback_func(detail_id)
		return
	end
	local callback_errfunc = data[3][2]
	callback_errfunc(detail_id,ReturnCode)
end

local function _BuyProduct_timeout(Tick, data)
	local sequence_id = data[3]
	UnRegisterTick(g_tbl_AGIP_Tick[sequence_id])
	g_tbl_AGIP_Tick[sequence_id] = nil
	g_tbl_AGIP_CallBack[sequence_id] = nil
	local callback_func = data[2][2]
	callback_func("timeout",data[1][1])
end

function EProtocol:UserBuyProduct(data, callback)
	if not (self.m_bConnected and self.m_bBinded) then
		return
	end
	local userid,roleid,sex,class,level,
				ratingid,IB_Code,package_flag,nCount,ip,
				subjectid,audit_flag,price,dis_price,buy_time,detail_id = unpack(data)
									
	local cmd_size = 80
	local str_size = cmd_size + size_msg_checksum + size_msg_header
	local acr = CreateCBufAcr(CreateCBuf(str_size+1))
	acr:Seek( "begin" )

	local sequence_id = _InitMsgHead(0x10003700, str_size, acr)
	WriteLongLongNum(acr, detail_id)
	WriteUnIntNum(acr, userid)
	WriteUnIntNum(acr, roleid)
	WriteUnByteNum(acr, sex)
	WriteUnByteNum(acr, class)
	WriteUnShortNum(acr, level)
	WriteUnIntNum(acr, ratingid)
	_write_size_str_to_buf(acr, GB2U8(IB_Code), 32)
	WriteUnByteNum(acr, package_flag)
	local pad = 0
	WriteUnByteNum(acr, pad)
	WriteUnShortNum(acr, nCount)
	WriteUnIntNum(acr, buy_time)
	WriteUnIntNum(acr, ip)
	WriteShortNum(acr, subjectid)
	WriteShortNum(acr, audit_flag)
	WriteUnIntNum(acr, price)
	WriteUnIntNum(acr, dis_price)
	_AddCheckSum(acr,str_size-size_msg_checksum)

	acr:Seek( "begin" )
	self.m_Pipe:Send(acr, str_size)
	g_tbl_AGIP_CallBack[sequence_id] = {_BuyProduct_callback, {self, data, callback}}
	g_tbl_AGIP_Tick[sequence_id] = RegisterTick("EProtocol:UserBuyProduct", _BuyProduct_timeout, 20*1000, {{detail_id}, callback, sequence_id})
end
--[[
	用户消耗IB物品
--]]
local function _UseProduct_callback(data, buf, sequence_id)
	UnRegisterTick(g_tbl_AGIP_Tick[sequence_id])
	g_tbl_AGIP_Tick[sequence_id] = nil
	g_tbl_AGIP_CallBack[sequence_id] = nil
	local detail_id, user_id, rating_id, subject_id, audit_flag, amount = unpack(data)
	local eprtl = data[1]
	local user_id, role_id = unpack(data[2])
	local callback_func = data[3][1]
	local ReturnCode = ReadIntNum(buf)
	if ReturnCode == 1  then
		callback_func()
		return
	end

	local callback_errfunc = data[3][2]
	callback_errfunc(ReturnCode)
end

local function _UseProduct_timeout(Tick, data)
	UnRegisterTick(g_tbl_AGIP_Tick[sequence_id])
	g_tbl_AGIP_Tick[sequence_id] = nil
	g_tbl_AGIP_CallBack[sequence_id] = nil
	local callback_func = data[2][2]
	callback_func("error")
end


function EProtocol:UserUseProduct(data, callback)
	if not (self.m_bConnected and self.m_bBinded) then
		return
	end
	local userid, roleid, purchaseid,rolegender, 
			roleoccupation, rolelevel = unpack(data)
	local cmd_size = 20
	local str_size = cmd_size + size_msg_checksum + size_msg_header
	local acr = CreateCBufAcr(CreateCBuf(str_size+1))
	acr:Seek( "begin" )

	local sequence_id = _InitMsgHead(0x10003501, str_size, acr)
	WriteUnIntNum(acr, userid)
	WriteUnIntNum(acr, roleid)
	WriteLongLongNum(acr, purchaseid)
	WriteByteNum(acr, rolegender)
	WriteByteNum(acr, roleoccupation)
	WriteUnShortNum(acr, rolelevel)

	_AddCheckSum(acr,str_size-size_msg_checksum)

	acr:Seek( "begin" )
	self.m_Pipe:Send(acr, str_size)
	g_tbl_AGIP_CallBack[sequence_id] = {_UseProduct_callback, {self, data, callback}}
	g_tbl_AGIP_Tick[sequence_id] = RegisterTick("EProtocol:UseProduct", _UseProduct_timeout, 20*1000, {data, callback, sequence_id})
end


--金钱流通
local function _UserCashExchange_callback(data, buf, sequence_id)
	UnRegisterTick(g_tbl_AGIP_Tick[sequence_id])
	g_tbl_AGIP_Tick[sequence_id] = nil
	g_tbl_AGIP_CallBack[sequence_id] = nil
	local param = data[2]
	local detail_id,src_user_id,src_char_id,
				dst_user_id,dst_char_id,subject_id,
				flag,amount,time
				 = unpack(param)
	local ReturnCode = ReadIntNum(buf)
	if ReturnCode ~= 0 and ReturnCode ~= -500 then
		table.insert(g_EProtocol.m_tblMoneyExchangeDetail,detail_id)
	end
	g_EProtocol.m_nSendMoneyExchangeNum = g_EProtocol.m_nSendMoneyExchangeNum -1
	if g_EProtocol.m_nSendMoneyExchangeNum <= 0 then
		CExchangeMoney.SendMoneyExchangeInfo()
	end
end

local function _UserCashExchange_timeout(Tick, data)
	g_EProtocol.m_nSendMoneyExchangeNum = g_EProtocol.m_nSendMoneyExchangeNum -1
	if g_EProtocol.m_nSendMoneyExchangeNum <= 0 then
		CExchangeMoney.SendMoneyExchangeInfo()
	end
	local sequence_id = data[2]
	UnRegisterTick(g_tbl_AGIP_Tick[sequence_id])
	g_tbl_AGIP_Tick[sequence_id] = nil
	g_tbl_AGIP_CallBack[sequence_id] = nil
end

function EProtocol:UserCashExchange(data, callback)
	if not (self.m_bConnected and self.m_bBinded) then
		g_EProtocol.m_nSendMoneyExchangeNum = g_EProtocol.m_nSendMoneyExchangeNum -1
		return
	end
	local detail_id,src_user_id,src_char_id,
				dst_user_id,dst_char_id,subject_id,
				flag,amount,time
				 = unpack(data)
	local cmd_size = 48
	local str_size = cmd_size + size_msg_checksum + size_msg_header
	local acr = CreateCBufAcr(CreateCBuf(str_size+1))
	acr:Seek( "begin" )

	local sequence_id = _InitMsgHead(0x10003709,str_size,acr)
	WriteLongLongNum(acr, detail_id)
	WriteUnIntNum(acr, src_user_id)
	WriteUnIntNum(acr, src_char_id)
	WriteUnIntNum(acr, dst_user_id)
	WriteUnIntNum(acr, dst_char_id)
	WriteUnShortNum(acr, subject_id)
	WriteUnShortNum(acr, flag)
	WriteIntNum(acr, amount)
	WriteUnIntNum(acr, time)
	local type = 1
	WriteIntNum(acr, type)
	local gameid  = _GetDefaultGameID()
	WriteUnIntNum(acr, gameid)
	WriteUnIntNum(acr, gameid)
	_AddCheckSum(acr,str_size-size_msg_checksum)

	acr:Seek( "begin" )
	self.m_Pipe:Send(acr, str_size)
	g_tbl_AGIP_CallBack[sequence_id] = {_UserCashExchange_callback, {self, data}}
	g_tbl_AGIP_Tick[sequence_id] = RegisterTick("EProtocol:UserCashExchange", _UserCashExchange_timeout, 20*1000, {data, sequence_id})
end

--[[
	道具消费
--]]
local function _IBPay_callback(data, buf, sequence_id)
	g_tbl_AGIP_CallBack[sequence_id] = nil
	local ReturnCode = ReadIntNum(buf)
	if not g_tbl_AGIP_Tick[sequence_id] then
		g_EPrtlLog:Write(string.format("sid: %d, retcode: %d, %s", sequence_id, ReturnCode, "UserIBPay TimeoutCallback"))
		return
	end	
	UnRegisterTick(g_tbl_AGIP_Tick[sequence_id])
	g_tbl_AGIP_Tick[sequence_id] = nil
	local params = data[2]
	local user_id,role_id,role_gender,role_occupation,
				role_level,rating_id,IB_Code,package_flag,count,
				user_ip,subject_id, audit_flag, price,discount_price,time,detail_id
				 = unpack(params)
	local charge_info = {}
	g_EPrtlLog:Write(string.format("sid: %d, retcode: %d, %s", sequence_id, ReturnCode, "UserIBPay callback"))
	if ReturnCode ~= 0 and  ReturnCode ~= -500 then
		table.insert(g_EProtocol.m_tblIBPayDetail,{detail_id,ReturnCode})
	end
	g_EProtocol.m_nSendIBPayNum = g_EProtocol.m_nSendIBPayNum -1
	if g_EProtocol.m_nSendIBPayNum <= 0 then
		CExchangeMoney.SendIBPayInfo()
	end
end

local function _IBPay_timeout(Tick, data)
	g_EProtocol.m_nSendIBPayNum = g_EProtocol.m_nSendIBPayNum -1
	if g_EProtocol.m_nSendIBPayNum <= 0 then
		CExchangeMoney.SendIBPayInfo()
	end
	local sequence_id = data[2]
	UnRegisterTick(g_tbl_AGIP_Tick[sequence_id])
	g_tbl_AGIP_Tick[sequence_id] = nil
	g_EPrtlLog:Write(string.format("sid: %d, retcode: %d, %s", sequence_id, 0, "UserIBPay timeout"))
end

function EProtocol:UserIBPay(data)
	if not (self.m_bConnected and self.m_bBinded) then
		g_EProtocol.m_nSendIBPayNum = g_EProtocol.m_nSendIBPayNum -1
		return
	end
	local user_id,role_id,role_gender,role_occupation,
				role_level,rating_id,IB_Code,package_flag,count,
				user_ip,subject_id, audit_flag, price,discount_price,time,detail_id
				 = unpack(data)
	
	local cmd_size = 80
	local str_size = cmd_size + size_msg_checksum + size_msg_header
	local acr = CreateCBufAcr(CreateCBuf(str_size+1))
	acr:Seek( "begin" )

	local sequence_id = _InitMsgHead(0x10003701,str_size,acr)
	WriteLongLongNum(acr, detail_id)
	WriteUnIntNum(acr, user_id)
	WriteUnIntNum(acr, role_id)
	WriteUnByteNum(acr, role_gender)
	WriteUnByteNum(acr, role_occupation)
	WriteUnShortNum(acr, role_level)
	WriteUnIntNum(acr, rating_id)
	_write_size_str_to_buf(acr, IB_Code, 32)
	WriteUnByteNum(acr, package_flag)
	local pad = 0
	WriteByteNum(acr, pad)
	WriteUnShortNum(acr, count)
	WriteIntNum(acr, time)
	WriteUnIntNum(acr, user_ip)
	WriteUnShortNum(acr, subject_id)
	WriteUnShortNum(acr, audit_flag)
	WriteIntNum(acr,	price)
	WriteIntNum(acr, discount_price)
	_AddCheckSum(acr,str_size-size_msg_checksum)

	acr:Seek( "begin" )
	local ret_size = self.m_Pipe:Send(acr, str_size)
	if ret_size ~= str_size then
		LogErr("erating消息发送错误", "IBPay--str_size:" .. str_size .. "ret_size:" .. ret_size)
	end
	g_tbl_AGIP_CallBack[sequence_id] = {_IBPay_callback, {self, data}}
	g_tbl_AGIP_Tick[sequence_id] = RegisterTick("EProtocol:UserIBPay", _IBPay_timeout, 20*1000, {data, sequence_id})
	g_EPrtlLog:Write(string.format("sid: %d, retcode: %d, %s", sequence_id, 0, "role_id:".. role_id .. "UserIBPay"))
end

--[[
	道具消耗
--]]
local function _IBUse_callback(data, buf, sequence_id)
	UnRegisterTick(g_tbl_AGIP_Tick[sequence_id])
	g_tbl_AGIP_Tick[sequence_id] = nil
	g_tbl_AGIP_CallBack[sequence_id] = nil
	local callback_func = data[3][1]
	local ReturnCode = ReadIntNum(buf)
	if ReturnCode == 1 then
		return
	end
end

local function _IBUse_timeout(Tick, data)
	UnRegisterTick(g_tbl_AGIP_Tick[sequence_id])
	g_tbl_AGIP_Tick[sequence_id] = nil
	g_tbl_AGIP_CallBack[sequence_id] = nil
end

function EProtocol:UserIBUse(data, callback)
	if not (self.m_bConnected and self.m_bBinded) then
		return
	end
	local user_id,role_id,detail_id,role_gender,role_occupation,
				role_level
				 = unpack(data)
	local cmd_size = 24
	local str_size = cmd_size + size_msg_checksum + size_msg_header
	local acr = CreateCBufAcr(CreateCBuf(str_size+1))
	acr:Seek( "begin" )

	local sequence_id = _InitMsgHead(0x10003703,str_size,acr)
	WriteUnIntNum(acr, user_id)
	WriteUnIntNum(acr, role_id)
	WriteLongLongNum(acr, detail_id)
	WriteByteNum(acr, role_gender)
	WriteByteNum(acr, role_occupation)
	WriteUnShortNum(acr, role_level)
	WriteUnIntNum(acr, os.time())
	_AddCheckSum(acr,str_size-size_msg_checksum)

	acr:Seek( "begin" )
	self.m_Pipe:Send(acr, str_size)
	g_tbl_AGIP_CallBack[sequence_id] = {_IBUse_callback, {self, data, callback}}
	g_tbl_AGIP_Tick[sequence_id] = RegisterTick("EProtocol:UserIBUse", _IBUse_timeout, 10*1000, {data, callback, sequence_id})
end


--[[
	玩家余额查询
--]]
function _GetBalanceInfoCallback(data, buf, sequence_id)
	g_tbl_AGIP_CallBack[sequence_id] = nil
	local ReturnCode = ReadIntNum(buf)
	if not g_tbl_AGIP_Tick[sequence_id] then
		g_EPrtlLog:Write(string.format("sid: %d, retcode: %d, %s", sequence_id, ReturnCode, "GetBalance TimeoutCallback"))
		return
	end	
	UnRegisterTick(g_tbl_AGIP_Tick[sequence_id])
	g_tbl_AGIP_Tick[sequence_id] = nil
	local callback_func = data[1]
	g_EPrtlLog:Write(string.format("sid: %d, retcode: %d, %s", sequence_id, ReturnCode, "GetBalance callback"))

	if ReturnCode == 1 then
		local BalanceCount = ReadIntNum(buf)
		local left_ticket = 0
		if BalanceCount > 0 then 
			for i =1,BalanceCount do
				local subject_id  = ReadIntNum(buf)
				local amount      = ReadIntNum(buf)
				local left_amount = ReadIntNum(buf)
				if subject_id == 3 then
					left_ticket = left_amount
					break
				end
			end
		end
		callback_func(left_ticket)
		return
	end
	local callback_error_func = data[2]
	callback_error_func(ReturnCode)
end

function _GetBalanceInfo_Timeout(Tick, data)
	local callback_func,sequence_id = unpack(data)
	UnRegisterTick(g_tbl_AGIP_Tick[sequence_id])
	g_tbl_AGIP_Tick[sequence_id] = nil
	callback_func[2]("timeout")
	g_EPrtlLog:Write(string.format("sid: %d, retcode: %d, %s", sequence_id, 0, "GetBalance timeout"))
end

function EProtocol:GetUserBalanceInfo(data, callback)
	if not (self.m_bConnected and self.m_bBinded) then
		return
	end

	local userid,role_id,rating_id = unpack(data)
	local cmd_size = 12
	local str_size = cmd_size + size_msg_checksum + size_msg_header

	local acr = CreateCBufAcr(CreateCBuf(str_size+1))
	acr:Seek( "begin" )

	local sequence_id = _InitMsgHead(0x10003402,str_size,acr)
	WriteUnIntNum(acr, userid)
	WriteUnIntNum(acr, role_id)
	WriteUnIntNum(acr, rating_id)
	_AddCheckSum(acr,str_size-size_msg_checksum)
	acr:Seek( "begin" )

	local ret_size = self.m_Pipe:Send(acr, str_size)
	if ret_size ~= str_size then
		LogErr("erating消息发送错误", "IBPay--str_size:" .. str_size .. "ret_size:" .. ret_size)
	end
	g_tbl_AGIP_CallBack[sequence_id] = {_GetBalanceInfoCallback, callback}
	g_tbl_AGIP_Tick[sequence_id] = RegisterTick("EProtocol:GetUserBalanceInfo", _GetBalanceInfo_Timeout, 10*1000, {callback,sequence_id})
	g_EPrtlLog:Write(string.format("sid: %d, retcode: %d, %s", sequence_id, 0, "role_id:" .. role_id .. "GetBalanceInfo"))
end

--[[
	玩家领取余额
--]]
function _DrawBalanceCallback(data, buf, sequence_id)
	g_tbl_AGIP_CallBack[sequence_id] = nil
	local ReturnCode = ReadIntNum(buf)
	if not g_tbl_AGIP_Tick[sequence_id] then
		g_EPrtlLog:Write(string.format("sid: %d, retcode: %d, %s", sequence_id, ReturnCode, "DrawBalance TimeoutCallback"))
		return
	end	
	UnRegisterTick(g_tbl_AGIP_Tick[sequence_id])
	g_tbl_AGIP_Tick[sequence_id] = nil
	local params = data[2]
	local detail_id,userid,roleid,ratingid,subject_id,amount = unpack(params)
	local callback_func = data[1][1]
	g_EPrtlLog:Write(string.format("sid: %d, retcode: %d, %s", sequence_id, ReturnCode, "DrawBalance callback"))
	if ReturnCode == 1 or ReturnCode == -1460 or ReturnCode == -1282 then
		callback_func(ReturnCode,detail_id,roleid)
		return
	end
	local callback_error_func = data[1][2]
	callback_error_func(ReturnCode,detail_id,roleid)
end

function _DrawBalance_Timeout(Tick, data)
	local callback_func,sequence_id = unpack(data)
	UnRegisterTick(g_tbl_AGIP_Tick[sequence_id])
	g_tbl_AGIP_Tick[sequence_id] = nil
	g_EPrtlLog:Write(string.format("sid: %d, retcode: %d, %s", sequence_id, 0, "DrawBalance timeout"))
end
function EProtocol:DrawBalance(data, callback)
	if not (self.m_bConnected and self.m_bBinded) then
		return
	end

	local detail_id,userid,roleid,ratingid,subject_id,amount = unpack(data)
	local cmd_size = 28
	local str_size = cmd_size + size_msg_checksum + size_msg_header

	local acr = CreateCBufAcr(CreateCBuf(str_size+1))
	acr:Seek( "begin" )

	local sequence_id = _InitMsgHead(0x10003406,str_size,acr)
	WriteLongLongNum(acr, detail_id)
	WriteUnIntNum(acr, userid)
	WriteUnIntNum(acr, roleid)
	WriteUnIntNum(acr, ratingid)
	WriteIntNum(acr, subject_id)
	WriteIntNum(acr, amount)
	_AddCheckSum(acr,str_size-size_msg_checksum)
	acr:Seek( "begin" )

	local ret_size = self.m_Pipe:Send(acr, str_size)
	if ret_size ~= str_size then
		LogErr("erating消息发送错误", "IBPay--str_size:" .. str_size .. "ret_size:" .. ret_size)
	end
	g_tbl_AGIP_CallBack[sequence_id] = {_DrawBalanceCallback, {callback,data}}
	g_tbl_AGIP_Tick[sequence_id] = RegisterTick("EProtocol:DrawBalance", _DrawBalance_Timeout, 30*1000, {callback,sequence_id})
	g_EPrtlLog:Write(string.format("sid: %d, retcode: %d, %s", sequence_id, 0, "roleid:" .. roleid .. "DrawBalance"))
end

--对账
function _GWBalanceReportCallback(data, buf, sequence_id)
	UnRegisterTick(g_tbl_AGIP_Tick[sequence_id])
	g_tbl_AGIP_Tick[sequence_id] = nil
	g_tbl_AGIP_CallBack[sequence_id] = nil
	local params = data[2]
	local audit_id,subject_id,output,consume,left_money,audit_time = unpack(params)
	local callback_func = data[1][1]
	local ReturnCode = ReadIntNum(buf)
	g_EPrtlLog:Write(string.format("sid: %d, retcode: %d, %s", sequence_id, ReturnCode, "GWBalanceReport callback"))
	if ReturnCode == 1 or ReturnCode == -1460 or ReturnCode == -1282 then
		callback_func(audit_id)
		return
	end
end

function _GWBalanceReport_Timeout(Tick, data)
	local callback_func,sequence_id = unpack(data)
	UnRegisterTick(g_tbl_AGIP_Tick[sequence_id])
	g_tbl_AGIP_Tick[sequence_id] = nil
	g_tbl_AGIP_CallBack[sequence_id] = nil
	--callback_func[2]("timeout")
	g_EPrtlLog:Write(string.format("sid: %d, retcode: %d, %s", sequence_id, 0, "GWBalanceReport timeout"))
end

function EProtocol:GWBalanceReport(data, callback)
	if not (self.m_bConnected and self.m_bBinded) then
		return
	end

	local audit_id,subject_id,output,consume,left_money,audit_time,game_id = unpack(data)
	local cmd_size = 20
	local str_size = cmd_size + size_msg_checksum + size_msg_header

	local acr = CreateCBufAcr(CreateCBuf(str_size+1))
	acr:Seek( "begin" )

	local sequence_id = _InitMsgHead(0x10004201,str_size,acr,game_id)
	WriteUnIntNum(acr, audit_time)
	WriteIntNum(acr, subject_id)
	WriteUnIntNum(acr, output)
	WriteUnIntNum(acr, consume)
	WriteIntNum(acr, left_money)
	_AddCheckSum(acr,str_size-size_msg_checksum)
	acr:Seek( "begin" )

	local ret_size = self.m_Pipe:Send(acr, str_size)
	if ret_size ~= str_size then
		LogErr("erating消息发送错误", "IBPay--str_size:" .. str_size .. "ret_size:" .. ret_size)
	end
	g_tbl_AGIP_CallBack[sequence_id] = {_GWBalanceReportCallback, {callback,data}}
	g_tbl_AGIP_Tick[sequence_id] = RegisterTick("EProtocol:GWBalanceReport", _GWBalanceReport_Timeout, 30*1000, {callback,sequence_id})
	g_EPrtlLog:Write(string.format("sid: %d, retcode: %d, %s", sequence_id, 0, "GWBalanceReport"))
end
-----------------------------------------------------------------------
--------------------------------------回调函数-------------------------
-----------------------------------------------------------------------
function EProtocol:OnConnected()
	print("+++++++++++++++++++EProtocol:OnConnected+++++++++++++++++++++");
	self.m_bConnected = true
	self:_Bind(self.m_ServerId)
	UnRegisterTick(g_AGIP_Connect_tick)
	g_AGIP_Connect_tick = nil
	local msg = "EProtocol:OnConnected"
	g_EPrtlLog:Write(msg)
	if g_CurServerId == 1 and GasConfig.EratingIBPaySend == 1 then
			CExchangeMoney.SendMoneyExchangeInfo()
			CExchangeMoney.SendIBPayInfo()
	end
end

function EProtocol:OnConnectFailed(eReason)
	print("+++++++++++++++++++EProtocol:OnConnectFailed+++++++++++++++++++++");
	self.m_bConnected = false
	local msg = "EProtocol:OnConnectFailed"
	g_EPrtlLog:Write(msg)
end

function EProtocol:OnDisconnected(eReason,bGracefully)
	local msg = "EProtocol:OnDisconnected"
	g_EPrtlLog:Write(msg)

	if self.m_bShuttedDown then
		return
	end

	print("+++++++++++++++++++EProtocol:OnDisconnected+++++++++++++++++++++");
	UnRegisterTick(g_ep_shake_hand_tick)
	g_ep_shake_hand_tick = nil
	UnRegisterTick(g_ep_data_report_tick)
	g_ep_data_report_tick = nil
	self.m_bBinded = false
	self.m_bConnected = false
	self:Connect()
end

function EProtocol:OnAccepted(pPipe)
end

function EProtocol:OnDataReceived()
	local size = self.m_Pipe:GetRecvDataSize()
	if size == 0 then
		return
	end
	local obj = self.m_Pipe:GetRecvData()
	local clone_buf_acr = CreateCBufAcr( CreateCBuf(size) )
	clone_buf_acr:Copy( obj, size )
	clone_buf_acr:Seek("begin")
	
	if (not self.m_bConnected) then
		return
	end
	
	local seek_size = 0
	while (size > 11) do
		local CommandSize = ReadUnShortNum(clone_buf_acr)
		clone_buf_acr:Seek("begin")
		clone_buf_acr:Seek(seek_size)
		if (size < CommandSize) or CommandSize == 0 then
			break
		end

		local cmd_buf = CreateCBufAcr( CreateCBuf(CommandSize) )
		cmd_buf:Copy(clone_buf_acr, CommandSize)
		cmd_buf:Seek("begin")

		CommandSize = ReadUnShortNum(cmd_buf)
		local Version = ReadUnByteNum(cmd_buf)
		assert(Version == tonumber(ERatingConfig.AGIP_Version))
		local Remain_Package = cmd_buf:Read("C")
		local Command_id = ReadUnIntNum(cmd_buf)
		local sequence_id = ReadUnIntNum(cmd_buf)
		local game_id = ReadUnIntNum(cmd_buf)
		local gateway_id = ReadUnIntNum(cmd_buf)
		if g_tbl_AGIP_CallBack[sequence_id] then
			g_tbl_AGIP_CallBack[sequence_id][1](g_tbl_AGIP_CallBack[sequence_id][2], cmd_buf, sequence_id)
			g_tbl_AGIP_CallBack[sequence_id] = nil
		end
		seek_size = seek_size + CommandSize
		size = size - CommandSize
		clone_buf_acr:Seek("begin")
		clone_buf_acr:Seek(seek_size)
	end
	self:RecvDataEnd()
	self.m_Pipe:PopRecvData(seek_size)
	clone_buf_acr:Seek("begin")
end

function EProtocol:OnDataSent(uSentSize)
end

g_EProtocol = g_EProtocol or EProtocol:new()
