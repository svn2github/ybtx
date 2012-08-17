gas_require "e_protocol/EProtocolInc"
gas_require "e_protocol/LuaInt2Byte"
gac_gas_require "framework/common/StringSplit"
etc_gas_require "GasConfig"

------------------------------------------------------------
local function _InitMsgHead(MsgID, TotalSize, acr,g_AGIP_Sequence_ID)
	WriteUnShortNum(acr, TotalSize)
	local gameid  = tonumber(ERatingConfig.AGIP_GameID)
	local remain_packages = 0
	acr:Write("C", ERatingConfig.AGIP_Version)
	acr:Write("C", remain_packages)
	WriteUnIntNum(acr, MsgID)
	WriteUnIntNum(acr, g_AGIP_Sequence_ID)
	WriteUnIntNum(acr, gameid)
	WriteUnIntNum(acr, ERatingConfig.AGIP_Gateway_id)
end
function EProMsgRecvHandler:Ctor()
	self.m_bListening = false
end

function EProMsgRecvHandler:OnConnected()
	self.m_bConnected = true
end

function EProMsgRecvHandler:OnAccepted(pPipe)
end

function EProMsgRecvHandler:_Auth()
	self.m_Pipe:GetRemoteAddress(addr)
	self.m_RemoteIp = addr:GetAddress()
	if not g_AllowedEProMsgSender then
		return false
	end
	return g_AllowedEProMsgSender[self.m_RemoteIp]
end

function EProMsgRecvHandler:OnListenFaild(reason)
end

function EProMsgRecvHandler:OnDisconnected(eReason, bGracefully)
end

function EProMsgRecvHandler:OnConnectFailed(eReason, bGracefully)
end

function EProMsgRecvHandler:SetPipe(pPipe)
	self.m_Pipe = pPipe
end

function EProMsgRecvHandler:OnDataSent(uSentSize)
end

--【网关连接】
function EProMsgRecvHandler:_Bind(data,sequence_id)
--	if not self.m_bConnected then
--		return
--	end
	local result_code = data.m_ResultCode
	local cmd_size = 4
	local str_size = cmd_size + size_msg_checksum + size_msg_header
	
	local acr = CreateCBufAcr(CreateCBuf(str_size+1))
	acr:Seek( "begin" )

	_InitMsgHead(0x20000001,str_size,acr,sequence_id)
	WriteIntNum(acr, result_code)
	_AddCheckSum(acr,cmd_size+ size_msg_header)
	acr:Seek( "begin" )
	
	self.m_Pipe:Send(acr, str_size)
end

local function _Bind(buf,sequence_id,obj)
	local gateway_code 				= 	_read_size_str_from_buf(buf,32)
	local gateway_password 		= 	_read_size_str_from_buf(buf,32)
	local mac 								= 	_read_size_str_from_buf(buf,12)
	local reconnect_flag 			= 	ReadByteNum(buf)
	local pad1 								= 	ReadByteNum(buf)
	local pad2 								= 	ReadUnShortNum(buf)
	local server_id 								= 	ReadUnIntNum(buf)
	obj:_Bind({["m_ResultCode"] = 1},sequence_id)
end

--【网关去连接】
function EProMsgRecvHandler:_UnBind(data,sequence_id)
--	if not self.m_bConnected then
--		return
--	end
	local result_code = data.ResultCode
	local cmd_size = 4
	local str_size = cmd_size + size_msg_checksum + size_msg_header
	
	local acr = CreateCBufAcr(CreateCBuf(str_size+1))
	acr:Seek( "begin" )

	_InitMsgHead(0x20000002,str_size,acr,sequence_id)
	WriteIntNum(acr, result_code)
	_AddCheckSum(acr,cmd_size+ size_msg_header)
	acr:Seek( "begin" )
	
	self.m_Pipe:Send(acr, str_size)
end

local function _UnBind(buf,sequence_id,obj)
	local data = {["ResultCode"] =1}
	local GatewayID = ReadUnIntNum(buf)
	obj:_UnBind(data, sequence_id)
end


--【充值请求】
function EProMsgRecvHandler:UserChargeRes(data,sequence_id)
--	if not self.m_bConnected then
--		return
--	end
	local result_code,user_id,rating_id,subject_id = data.m_ResultCode,data.m_UserID,data.m_RatingID,data.m_SubjectID
	local detail_id,audit_flag = data.m_DetailID,data.m_AuditFlag
	local total_amount = data.m_TotalAmount
	local cmd_size = 28
	local str_size = cmd_size + size_msg_checksum + size_msg_header
	
	local acr = CreateCBufAcr(CreateCBuf(str_size+1))
	acr:Seek( "begin" )

	_InitMsgHead(0x20003401,str_size,acr,sequence_id)
	WriteIntNum(acr, result_code)
	WriteLongLongNum(acr,detail_id)
	WriteUnIntNum(acr, user_id)
	WriteUnIntNum(acr, rating_id)
	WriteUnShortNum(acr, subject_id)
	WriteUnShortNum(acr, audit_flag)
	WriteIntNum(acr, total_amount)
	_AddCheckSum(acr,cmd_size+ size_msg_header)
	acr:Seek( "begin" )
	
	self.m_Pipe:Send(acr, str_size)
end

local function _UserCharge(buf,sequence_id,obj)
	local data = {}
	data.sequence_id = sequence_id
	data.m_DetailID 			= ReadLongLongNum(buf)
	data.m_UserID 				= ReadUnIntNum(buf)
	data.m_RatingID 			=  ReadUnIntNum(buf)
	data.m_SubjectID 			= ReadShortNum(buf)
	data.m_AuditFlag 			= ReadShortNum(buf)
	data.m_Amount 				= ReadIntNum(buf)
	data.m_ChargeTime 		= ReadUnIntNum(buf)
	CGasToolsMall.Local_UserCharge(data,obj)
end

--【充值对单】
function EProMsgRecvHandler:UserChargeAuditRes(data,sequence_id)
--	if not self.m_bConnected then
--		return
--	end
	local result_code,user_id,rating_id,subject_id = data.m_ResultCode,data.m_UserID,data.m_RatingID,data.m_SubjectID
	local tblDetail = data.m_tblDetail
	local audit_count = 0
	if 1 == result_code then
	 	audit_count = tblDetail:GetRowNum()
	end
	local cmd_size = 16+12*audit_count
	local str_size = cmd_size + size_msg_checksum + size_msg_header
	
	local acr = CreateCBufAcr(CreateCBuf(str_size+1))
	acr:Seek( "begin" )

	_InitMsgHead(0x20003404,str_size,acr,sequence_id)
	WriteIntNum(acr, result_code)
	WriteUnIntNum(acr, user_id)
	WriteUnIntNum(acr, rating_id)
	WriteShortNum(acr, subject_id)
	WriteUnShortNum(acr, audit_count)
	for i =1,audit_count do
		WriteLongLongNum(acr,tblDetail(i,1))
		WriteIntNum(acr,tblDetail(i,2))
	end
	_AddCheckSum(acr,cmd_size+ size_msg_header)
	acr:Seek( "begin" )
	
	self.m_Pipe:Send(acr, str_size)
end
local function _UserChargeAudit(buf,sequence_id, obj)
	local data = {}
	data.sequence_id = sequence_id
	data.m_UserID 				= ReadUnIntNum(buf)
	data.m_RatingID 			=  ReadUnIntNum(buf)
	data.m_SubjectID 			= ReadShortNum(buf)
	data.m_AuditCount 		= ReadUnShortNum(buf)
	data.m_BeginDetailID 	= ReadLongLongNum(buf)
	data.m_EndDetailID 		= ReadLongLongNum(buf)
	CGasToolsMall.Local_UserChargeAudit(data,obj)
end

--【消费流水列表查询】
function EProMsgRecvHandler:PayListQueryRes(data,sequence_id)
--	if not self.m_bConnected then
--		return
--	end
	local result_code = data.m_ResultCode
	local tblDetail = data.m_tblRes
	local detail_count = 0
	if 1 == result_code then
	 	detail_count = tblDetail:GetRowNum()
	end
	local total_amount = data.m_MoneyAmount
	
	local cmd_size = 12+8*detail_count
	local str_size = cmd_size + size_msg_checksum + size_msg_header
	
	local acr = CreateCBufAcr(CreateCBuf(str_size+1))
	acr:Seek( "begin" )

	_InitMsgHead(0x20003706,str_size,acr,sequence_id)
	WriteIntNum(acr, result_code)
	WriteIntNum(acr, total_amount)
	WriteUnShortNum(acr, data.m_SubjectID)
	WriteUnShortNum(acr, detail_count)
	for i =1,detail_count do
		WriteLongLongNum(acr,tblDetail(i,1))
	end
	_AddCheckSum(acr,cmd_size+ size_msg_header)
	acr:Seek( "begin" )
	
	self.m_Pipe:Send(acr, str_size)
end
local function _PayListQuery(buf,sequence_id,obj)
	local data = {}
	data.sequence_id = sequence_id
	data.m_DetailType = ReadIntNum(buf)
	data.m_SubjectID 			= ReadShortNum(buf)
	data.m_DetailCount 		= ReadUnShortNum(buf)
	data.m_BeginDetailID 	= ReadLongLongNum(buf)
	CExchangeMoney.Local_PayListQuery(data,obj)
end

--【消费流水索取】
function EProMsgRecvHandler:PayDetailDemandRes(data,sequence_id)
--	if not self.m_bConnected then
--		return
--	end
	local result_code = data.m_ResultCode
	local cmd_size = 4
	local str_size = cmd_size + size_msg_checksum + size_msg_header
	
	local acr = CreateCBufAcr(CreateCBuf(str_size+1))
	acr:Seek( "begin" )

	_InitMsgHead(0x20003707,str_size,acr,sequence_id)
	WriteIntNum(acr, 1)
	_AddCheckSum(acr,cmd_size+ size_msg_header)
	acr:Seek( "begin" )
	
	self.m_Pipe:Send(acr, str_size)
end
local function _PayDetailDemand(buf,sequence_id,obj)
	local data = {}
	data.sequence_id = sequence_id
	data.m_DetailType = ReadIntNum(buf)
	data.m_SubjectID 			= ReadShortNum(buf)
	data.m_DetailCount 		= ReadUnShortNum(buf)
	local tbl_detail = {}
	for i =1,data.m_DetailCount do
		table.insert(tbl_detail,ReadLongLongNum(buf))
	end
	data.m_tblDetail = tbl_detail
	CExchangeMoney.Local_PayDetailDemand(data,obj)
end

--【注册或者修改账hao】
function EProMsgRecvHandler:CreateOrUpdateUserRes(data,sequence_id)
--	if not self.m_bConnected then
--		return
--	end
	local result_code = data.m_ResultCode
	local cmd_size = 4
	local str_size = cmd_size + size_msg_checksum + size_msg_header
	
	local acr = CreateCBufAcr(CreateCBuf(str_size+1))
	acr:Seek( "begin" )

	_InitMsgHead(0x20003001,str_size,acr,sequence_id)
	WriteIntNum(acr, result_code)
	_AddCheckSum(acr,cmd_size+ size_msg_header)
	acr:Seek( "begin" )
	
	self.m_Pipe:Send(acr, str_size)
end
local function _CreateOrUpdateUser(buf,sequence_id,obj)
	local data = {}
	data.UserID					= ReadUnIntNum(buf)
	data.UserName 			= _read_size_str_from_buf(buf,32)
	data.Password 			= _read_size_str_from_buf(buf,32)
	data.m_UserType				= ReadByteNum(buf)
	data.m_UserClass			= ReadByteNum(buf)
	data.m_Pad						= ReadShortNum(buf)
	data.m_UserPoint			= ReadIntNum(buf)
	data.m_PromoterID			= ReadUnIntNum(buf)
	data.m_UserFlag		 		= ReadUnIntNum(buf)
	
	local res = {}
	res.m_ResultCode = 1
		
	gac_gas_require "framework/common/UserNameCheck"
	local flag = CheckUserName(data.UserName)
	if not flag then
		res.m_ResultCode = -100
		obj:CreateOrUpdateUserRes(res,sequence_id)
		return
	end
	
	local function CallBack(Result)
		if not Result then 
			res.m_ResultCode = 0
		end
		if IsNumber(Result) then
			res.m_ResultCode = Result
		end
		obj:CreateOrUpdateUserRes(res,sequence_id)
	end
	CallDbTrans("LoginServerDB", "AuthSuccessAndInsertLocalDB", CallBack, data, key)
end

--g_tbl_AGIP_CMD_Handler[0x10000001] = _Bind
--g_tbl_AGIP_CMD_Handler[0x10000002] = _UnBind
--g_tbl_AGIP_CMD_Handler[0x10003401] = _UserCharge
--g_tbl_AGIP_CMD_Handler[0x10003404] = _UserChargeAudit
--g_tbl_AGIP_CMD_Handler[0x10003706] = _PayListQuery
--g_tbl_AGIP_CMD_Handler[0x10003707] = _PayDetailDemand
--g_tbl_AGIP_CMD_Handler[0x10003001] = _CreateOrUpdateUser


function EProMsgRecvHandler:OnDataReceived()
	print("+++++++++++++++++++EProMsgRecvHandler:OnDataReceived+++++++++++++++++++++");
	local size = self.m_Pipe:GetRecvDataSize()
	if size == 0 then
		return
	end
	local obj = self.m_Pipe:GetRecvData()
	local clone_buf_acr = CreateCBufAcr( CreateCBuf(size) )
	clone_buf_acr:Copy( obj, size )
	clone_buf_acr:Seek("begin")
--	if (not self.m_bConnected) then
--		return
--	end
	local str = ""
	for i = 1, size do
		str = str .. clone_buf_acr:Read("C") .. " "
	end

	clone_buf_acr:Seek("begin")
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
		assert(game_id == tonumber(ERatingConfig.AGIP_GameID))

		local gateway_id = ReadUnIntNum(cmd_buf)
		
		if g_tbl_AGIP_CMD_Handler[Command_id] then
			g_tbl_AGIP_CMD_Handler[Command_id](cmd_buf,sequence_id,self)
		end
		seek_size = seek_size + CommandSize
		size = size - CommandSize
		clone_buf_acr:Seek("begin")
		clone_buf_acr:Seek(seek_size)
	end
	self.m_Pipe:PopRecvData(seek_size)

	clone_buf_acr:Seek("begin")
end

---------------------------------------------------------------------------------


function EProMsgRecv:Ctor()
	self.m_bConnected = false
	self.m_Pipe = g_App:GetIPipe(self)
end

function EProMsgRecv:_Listen()
	if g_CurServerId ~= 1 then
		return
	end
	local addr = CAddress:new()
	local ip = ERatingConfig.AGIP_ListenIp or '127.0.0.1'
	local port = ERatingConfig.AGIP_ListenPort or 10088
	addr:SetAddress(ip);
	addr:SetPort(port)
	self.m_Pipe:Listen(addr)
end

function EProMsgRecv:Listen()
	if g_CurServerId ~= 1 then
		return
	end
	if GasConfig.SkipGBSAuth==1 then
			return
	end
	print("EProMsgRecv Listen")
	self:_Listen()
end

function EProMsgRecv:Shutdown()	
	if self.m_Pipe and IsCppBound(self.m_Pipe) then
		self.m_Pipe:SetHandler(nil)
		self.m_Pipe=nil
	end
end

function EProMsgRecv:OnConnected()
	print("EProMsgRecv OnConnected")
end

function EProMsgRecv:OnAccepted(pPipe)
	local pHandler = EProMsgRecvHandler:new()
	pPipe:SetHandler(pHandler)
	pHandler:SetPipe(pPipe)
	self.m_bConnected = true
end

function EProMsgRecv:OnListenFaild(reason)
	print("Listen Failed", "Reason is " .. reason)
	self:_Listen()
end

function EProMsgRecv:OnDisconnected(eReason, bGracefully)
end

function EProMsgRecv:OnConnectFailed(eReason)
end

function EProMsgRecv:OnDataReceived()
	print("EProMsgRecv:OnDataReceived()")
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
	local str = ""
	for i = 1, size do
		str = str .. clone_buf_acr:Read("C") .. " "
	end
	self.m_Pipe:PopRecvData(size)
end

function EProMsgRecv:OnDataSent(uSentSize)
end




--g_EProMsgRecv = g_EProMsgRecv or EProMsgRecv:new()
