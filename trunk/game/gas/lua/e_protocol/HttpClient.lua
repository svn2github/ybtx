gas_require "e_protocol/HttpClientInc"
gas_require "e_protocol/LuaInt2Byte"

function HttpClient:Ctor()
	
end

function HttpClient:_Connect(host, port)
	self.m_Pipe = g_App:GetIPipe(self)
	local ip = RandGetHostByName(host) or host
	self.host = host
	self.port = port
	
	local addr = CAddress:new()
	addr:SetAddress(ip)
	addr:SetPort(port)
	self.m_Pipe:Connect(addr)
end

function HttpClient:OnConnected()
	--here to send data
	local send_data = ""
	local tbl_data = self.params or {}
	local send_data = encode_url(tbl_data)
	if string.len(send_data) > 0 then
		send_data = "?" .. send_data
	end

	local http_req = string.format( [[GET %s%s HTTP/1.1
host: %s
Accept: */*

]], self.url, send_data, self.host)
	local send_size = string.len(http_req)
	local acr = CreateCBufAcr(CreateCBuf(send_size+1))
	_write_size_str_to_buf(acr, http_req, send_size)
	
	self.m_Pipe:Send(acr, send_size)
end

function HttpClient:OnDisconnected()
	if coroutine.status(self.m_co) == 'suspended' then		
		coroutine.resume(self.m_co, false)	
	end	
	self.m_Pipe:Release()
end

function HttpClient:OnConnectFailed()
	coroutine.resume(self.m_co, false)
	self.m_Pipe:Release()
end

function HttpClient:OnDataSent(uSentSize)
end



local function GetChunkSize(dataStr, begin)
	local b, e = string.find(dataStr, "\r\n", begin, true)
	if b then
		return tonumber( string.sub(dataStr, begin, b-1) ,16), e+1
	end
end

function HttpClient:OnDataReceived()
	self.m_ret_state = true
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
			self.m_ret_msg = string.sub(dataStr, 1, self.content_length)
			if coroutine.status(self.m_co) == 'suspended' then		
				coroutine.resume(self.m_co, true, self.m_ret_msg)
			end
			return
		end
	else
		b, e = string.find(headStr, ": chunked", 1, true)
		if e then
			local msg = ""
			local chunkBegin = 1
			local chunkEnd = nil
			local dataStrLen = string.len(dataStr)
			while chunkBegin and chunkBegin < dataStrLen do
				local chunkSize, dataBegin = GetChunkSize(dataStr, chunkBegin)
				if chunkSize == nil then --未完整的块, 继续等
					return
				end
				if chunkSize == 0 then --最后的块
					self.m_ret_msg = msg
					if coroutine.status(self.m_co) == 'suspended' then		
						coroutine.resume(self.m_co, true, self.m_ret_msg)
					end
					return
				end
				local b, e = string.find(dataStr, "\r\n", dataBegin + chunkSize)
				if e then
					chunkBegin = e + 1
					msg = msg .. string.sub(dataStr, dataBegin, dataBegin + chunkSize -1)
				else
					break
				end
			end
			
		else
			LogErr("未处理的其他类型 http 数据", http_resp)
			return
		end
	end
end

function HttpClient:SendData(host, port, url, params, callback)
	if self.Used then
		return false
	end
	self.Used = true
	self.url = url
	self.params = params
	if not GasConfig.CISCheck then
		if callback then
			callback(200)
			return true
		end
		return true
	end
	local function call()
		
		self:_Connect(host, port)
		local flag, msg = coroutine.yield()
		
		if callback then
			callback(self.m_ret_msg)
		end
	end
	self.m_co = coroutine.create(call)
	coroutine.resume(self.m_co)
	return true
end
