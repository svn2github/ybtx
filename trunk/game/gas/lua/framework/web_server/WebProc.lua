gas_require "framework/web_server/WebServerInl"
gas_require "e_protocol/LuaInt2Byte"
gas_require "framework/web_server/web_req/gm_web_role_mng"
gas_require "framework/web_server/web_req/gm_npc_manage"
gas_require "framework/web_server/web_req/gm_role_compensate"
gas_require "framework/web_server/web_req/web_shop"
gas_require "framework/web_server/web_req/lan_service"
gac_gas_require "framework/common/StringSplit"

local MAX_BLOCK_SIZE=1024

local responses = {
	[100]= {'Continue', 'Request received, please continue'},
	[101]= {'Switching Protocols',
		  'Switching to new protocol; obey Upgrade header'},

	[200]= {'OK', 'Request fulfilled, document follows'},
	[201]= {'Created', 'Document created, URL follows'},
	[202]= {'Accepted',
		  'Request accepted, processing continues off-line'},
	[203]= {'Non-Authoritative Information', 'Request fulfilled from cache'},
	[204]= {'No Content', 'Request fulfilled, nothing follows'},
	[205]= {'Reset Content', 'Clear input form for further input.'},
	[206]= {'Partial Content', 'Partial content follows.'},

	[300]= {'Multiple Choices',
		  'Object has several resources -- see URI list'},
	[301]= {'Moved Permanently', 'Object moved permanently -- see URI list'},
	[302]= {'Found', 'Object moved temporarily -- see URI list'},
	[303]= {'See Other', 'Object moved -- see Method and URL list'},
	[304]= {'Not Modified',
		  'Document has not changed since given time'},
	[305]= {'Use Proxy',
		  'You must use proxy specified in Location to access this resource.'},
	[307]= {'Temporary Redirect',
		  'Object moved temporarily -- see URI list'},

	[400]= {'Bad Request',
		  'Bad request syntax or unsupported method'},
	[401]= {'Unauthorized',
		  'No permission -- see authorization schemes'},
	[402]= {'Payment Required',
		  'No payment -- see charging schemes'},
	[403]= {'Forbidden',
		  'Request forbidden -- authorization will not help'},
	[404]= {'Not Found', 'Nothing matches the given URI'},
	[405]= {'Method Not Allowed',
		  'Specified method is invalid for this server.'},
	[406]= {'Not Acceptable', 'URI not available in preferred format.'},
	[407]= {'Proxy Authentication Required', 'You must authenticate with this proxy before proceeding.'},
	[408]= {'Request Timeout', 'Request timed out; try again later.'},
	[409]= {'Conflict', 'Request conflict.'},
	[410]= {'Gone',
		  'URI no longer exists and has been permanently removed.'},
	[411]= {'Length Required', 'Client must specify Content-Length.'},
	[412]= {'Precondition Failed', 'Precondition in headers is false.'},
	[413]= {'Request Entity Too Large', 'Entity is too large.'},
	[414]= {'Request-URI Too Long', 'URI is too long.'},
	[415]= {'Unsupported Media Type', 'Entity body in unsupported format.'},
	[416]= {'Requested Range Not Satisfiable',
		  'Cannot satisfy request range.'},
	[417]= {'Expectation Failed',
		  'Expect condition could not be satisfied.'},

	[500]= {'Internal Server Error', 'Server got itself in trouble'},
	[501]= {'Not Implemented',
		  'Server does not support this operation'},
	[502]= {'Bad Gateway', 'Invalid responses from another server/proxy.'},
	[503]= {'Service Unavailable',
		  'The server cannot process the request due to a high load'},
	[504]= {'Gateway Timeout',
		  'The gateway server did not receive a timely response'},
	[505]= {'HTTP Version Not Supported', 'Cannot fulfill request.'},
}

local version_string = ""
local protocol_version = "HTTP/1.0 200 OK"
local function _quote_html(html)
	local str = html
	str = string.gsub(str, "&", "&amp;")
	str = string.gsub(str, "<", "&lt;")
	str = string.gsub(str, ">", "&gt;")
end

local tbl_hstoi = {}
tbl_hstoi["0"] = 0
tbl_hstoi["1"] = 1
tbl_hstoi["2"] = 2
tbl_hstoi["3"] = 3
tbl_hstoi["4"] = 4
tbl_hstoi["5"] = 5
tbl_hstoi["6"] = 6
tbl_hstoi["7"] = 7
tbl_hstoi["8"] = 8
tbl_hstoi["9"] = 9
tbl_hstoi["a"] = 10
tbl_hstoi["b"] = 11
tbl_hstoi["c"] = 12
tbl_hstoi["d"] = 13
tbl_hstoi["e"] = 14
tbl_hstoi["f"] = 15
tbl_hstoi["A"] = 10
tbl_hstoi["B"] = 11
tbl_hstoi["C"] = 12
tbl_hstoi["D"] = 13
tbl_hstoi["E"] = 14
tbl_hstoi["F"] = 15

local function _hstoi(var)
	local len = string.len(var)
	local num = 0;
	for i = 1, 2 do
		num = num * 16 + tbl_hstoi[string.sub(var,i,i)]
	end
	return num
end

local function _unquoto_html(html)
	local str = html
	if (string.sub(html, 1,1 ) == '%') then
		str = string.sub(html, 2)
	end

	local var = {}
	SplitString(str, "%", var, true)
	if (string.sub(html, 1,1 ) ~= '%' and #var == 1) then
		return html
	end
	
	local flag = false
	if (string.sub(html, 1,1 ) == '%') then
		flag = true
	end


	local acr = CreateCBufAcr(CreateCBuf(#var + 1))
	acr:Seek("begin")
	for i, v in ipairs(var) do
		if flag == true then
			WriteUnByteNum(acr, _hstoi(v))
			local len = string.len(v) 
			if len > 2 then
				_write_size_str_to_buf(acr, string.sub(v, 3), len - 2)
			end
		else
			_write_size_str_to_buf(acr, v, string.len(v))
			flag = true
		end
		
	end
	WriteUnByteNum(acr, 0)
	acr:Seek("begin")
	local str = acr:Read("s")
	return str
end

local MAX_BLOCK_SIZE = 512
function CWebServerProcessorHandler:_send_html(html)
	local header = string.format("%s\r\n%s\r\n%s\r\n%s\r\nContent-length:%d\r\n", protocol_version,
		"Server: YBTX Gas 0.1", "Content-Type: text/html", "Connection: close",string.len(html))
	
	local whole_msg = header .. "\r\n" .. html

	local size = string.len(whole_msg)
	--print(whole_msg)
	--pipe:PopRecvData(size)
	local acr = CreateCBufAcr(CreateCBuf(size+1))
	_write_size_str_to_buf(acr, whole_msg, size)
	self.m_Pipe:Send(acr, size)
	self.m_uSize = size
	--print("send size", size)
end

function CWebServerProcessorHandler:_send_error(msg)
	self:_send_html(msg)
end

function CWebServerProcessorHandler:Ctor()
	self.m_bListening =  false						
end

function CWebServerProcessorHandler:OnConnected()
end

function CWebServerProcessorHandler:SetPipe(pPipe)
	self.m_Pipe = pPipe
end

function CWebServerProcessorHandler:DeletePipe()
	g_WebServerDeletedPipe = g_WebServerDeletedPipe + 1
	--print("--", g_WebServerDeletedPipe)
	self.m_Pipe:ShutDown()
	self.m_Pipe:Release()
	UnRegisterTick(self.m_DeletePipeTick)
end

function CWebServerProcessorHandler:OnAccepted(pPipe)
end

function CWebServerProcessorHandler:parse_request()
end

function CWebServerProcessorHandler:send_response(code, message)
end

function CWebServerProcessorHandler:send_header(keyword, value)
end

function CWebServerProcessorHandler:end_headers()
end

function CWebServerProcessorHandler:log_request(message, code, size)
end

function CWebServerProcessorHandler:log_error(msg)
end

function CWebServerProcessorHandler:log_message(format_str, message)
--[[
	log(string.format("%s - - [%s] %s " .. format_str .. " \n",
		address, time, unpack(message)))
]]
end

function CWebServerProcessorHandler:OnListenFaild(reason)
end

function CWebServerProcessorHandler:OnDisconnected(eReason, bGracefully)
	self:DeletePipe()
end

function CWebServerProcessorHandler:OnConnectFailed(eReason)
end

function CWebServerProcessorHandler:_Get_Version(v)
	local info = {}
	SplitString(v,'/',info)
	self.Protocal = info[1]
	self.Version = info[2]
end

function CWebServerProcessorHandler:_Get_Method(m)
	self.Method = string.upper(m)
end

function CWebServerProcessorHandler:_Get_Action(ac)
	self.Action = string.sub(ac, 2, string.len(ac))
end

function CWebServerProcessorHandler:_ParseQueryString(qs)
	self.Variable = {}
	local s = qs
	decode_url(self.Variable, s)
end

function CWebServerProcessorHandler:_Auth()
	local addr = CAddress:new()
	self.m_Pipe:GetRemoteAddress(addr)
	self.m_RemoteIp = addr:GetAddress()
	if not g_tblGmsValidIP then
		return false
	end
	return g_tblGmsValidIP[self.m_RemoteIp]
end

function CWebServerProcessorHandler:_Handle()
	local func = g_WebFuncDef[self.Action]
	if not func then
		self:_send_error("-101")
		return
	end
	local ret, msg = func(self.Variable)
	if not ret then
		self:_send_error(msg or "-100")
		return
	end
	self:_send_html(msg)
end

function CWebServerProcessorHandler:ProcRequest(requeststr)
	local tbl_split_res = {}
	SplitString(requeststr, '\n', tbl_split_res)
	
	local tbl_split_base = {}
	SplitString(tbl_split_res[1], "\r", tbl_split_base)
	
	local tbl_split_header = {}
	SplitString(tbl_split_base[1], " ", tbl_split_header)
	self:_Get_Method(tbl_split_header[1])
	if tbl_split_header[2] == nil then
		self:_send_error("-101")
		return
	end
	self:_Get_Action(tbl_split_header[2])
	self:_Get_Version(tbl_split_header[3])
	
	if self.Method ~= "POST" then
		self:_send_error("Method Must be Post")
		return
	end
	local index = 0
	for i=2,#tbl_split_res do
		local str = tbl_split_res[i]
		local start_pos, end_pos = string.find(string.lower(str), "content-length:", 1, true)
		if end_pos then
			self.ContentLength=tonumber(string.sub(str, end_pos+1))
		end
		
		if string.len(tbl_split_res[i]) == 0 
			or (string.len(tbl_split_res[i]) == 1 and tbl_split_res[i] == "\r")then
			index = i
			break
		end
	end
	--if not self.ContentLength or self.ContentLength == 0 then
	--	self:_send_error("form can't be empty")
	--	return
	--end
	local query_string = tbl_split_res[index+1]
	local msg = string.format("\tAction: %s,\tQueryString: %s", self.Action, query_string)
	g_GMSLog:Write(msg)
	self:_ParseQueryString(string.sub(query_string, 1, self.ContentLength))
	if not self:_Auth() then
		self:_send_error("-102")
		return
	end
	self:_Handle()
end

--[[
function CWebServerProcessorHandler:OnDataReceived()
	local function call()
		local size = self.m_Pipe:GetRecvDataSize()
		
		if size == 0 then
			self.m_Pipe:ShutDown()
			return
		end
		local obj = self.m_Pipe:GetRecvData()
		local requeststr = obj:Read("s")
		if not requeststr then
			self.m_Pipe:ShutDown()
			return
		end
		local co = function()
			self:ProcRequest(requeststr)
		end
		coroutine.resume(coroutine.create(co))
		self.m_Pipe:PopRecvData(string.len(requeststr))
	end
	local ret, msg = apcall(call)	
	if not ret then		
		ErrLogAndTellAll(msg)	
	end
	
	--self.m_Pipe:ShutDown()
end
--]]

function CWebServerProcessorHandler:OnDataReceived()
	local size = self.m_Pipe:GetRecvDataSize()
	if size == 0 then
		return
	end
	
	local obj = self.m_Pipe:GetRecvData()
	local http_resp = obj:Read("s", size)
	self.m_Pipe:PopRecvData(size)
	if self.msg then
		self.msg = self.msg .. http_resp
	else
		self.msg = http_resp
	end
	
	local headBegin, headEnd = string.find(self.msg, "\r\n\r\n", 1, true)
	if not headEnd then
		return
	end
	local headStr = string.lower( string.sub(self.msg, 1, headEnd))
	local dataStr = string.sub(self.msg, headEnd + 1, -1)
	local b, e = string.find(headStr, "content-length:", 1, true)
	if e then
		local b1,e1 = string.find(headStr, "\r\n", e, true)
		self.content_length = tonumber(string.sub(headStr, e+1, b1-1))
		if string.len(dataStr) >= self.content_length then
			local co = function()
				self:ProcRequest(self.msg)
			end
			coroutine.resume(coroutine.create(co))
			return
		end
	else
		local co = function()
			self:ProcRequest(self.msg)
		end
		coroutine.resume(coroutine.create(co))
	end
end

function CWebServerProcessorHandler:OnDataSent(uSentSize)
--	self.m_Pipe:ShutDown()
--	self.m_Pipe:Release()
--	self.m_Pipe:

	self.m_uSize = self.m_uSize - uSentSize
	--print("sent size", uSentSize, "left size", self.m_uSize)
	if(self.m_uSize > 0)then
		return
	end

	--print("send complete")
	self:DeletePipe()
end
