gac_require "e_protocol/HttpDownloadInc"
gac_require "e_protocol/LuaInt2Byte"

local BLOCK_SIZE = 4096

function HttpDownload:_Connect(host, port)
	self.m_Pipe = g_App:GetIPipeWithBufferSize(self, 1024*1024, 1024*1024,false)

	local ip = RandGetHostByName(host) or host
	self.host = host
	self.port = port
	
	local addr = CAddress:new()
	addr:SetAddress(ip)
	addr:SetPort(port)
	self.m_Pipe:Connect(addr)
end

function HttpDownload:OnConnected()
	--here to send data
	local send_data = ""
	local tbl_data = self.params or {}
	local send_data = encode_url(tbl_data)
	if string.len(send_data) > 0 then
		send_data = "?" .. send_data
	end

	local http_req = string.format( [[GET %s%s HTTP/1.0
host: %s
Accept: */*

]], self.url, send_data, self.host)

	local send_size = string.len(http_req)
	local acr = CreateCBufAcr(CreateCBuf(send_size+1))
	_write_size_str_to_buf(acr, http_req, send_size)
	
	self.m_Pipe:Send(acr, send_size)
end

function HttpDownload:OnDisconnected()
	if coroutine.status(self.m_co) == 'suspended' then		
		coroutine.resume(self.m_co, false)	
	end	
	self.m_Pipe:Release()
end

function HttpDownload:OnConnectFailed()
	coroutine.resume(self.m_co, false)
	self.m_Pipe:Release()
end


function HttpDownload:OnDataReceived()
	
	local size = self.m_Pipe:GetRecvDataSize()
	local resp_size = size
	local obj = self.m_Pipe:GetRecvData()
	
	local str_read = obj:Read("s", size) -- 已经seek了
	
	local e = 0	
	if flag1 == 1 then    --flag=0表示已经找到响应头，所以以后的实体片段都不用再找了.每次重新下载一个文件时置1
		for i=1,size do
			if string.sub(str_read, i, i+1) == '\r\n' and string.sub(str_read, i+2, i+3) == '\r\n' then
				e = i + 3
				flag1 = 0
				break
			end 
		end
	end
	
	obj:Seek("begin")
	obj:Seek(e)
	local data_size = size - e

	local data_buf = CreateCBufAcr( CreateCBuf(data_size) )
	data_buf:Copy(obj, data_size)
	data_buf:Seek("begin")
	
	g_file:WriteBuffer(data_buf, data_size)
	
	self.m_Pipe:PopRecvData(resp_size)

	self.m_ret_state = true
	self.m_ret_msg = 200
	
end

function HttpDownload:SendData(host, port, url, params, callback, filename)
	flag1 = 1
	g_file = CLog_CreateLog(filename, "wb")

	if self.Used then
		return false
	end
	self.Used = true
	self.url = url
	self.params = params

	local function call()
		self:_Connect(host, port)
		local flag, msg = coroutine.yield()
		
		if callback then
			g_file:Release()
			callback(self.m_ret_msg)
		end		
	end
	
	self.m_co = coroutine.create(call)
	coroutine.resume(self.m_co)
	return true
end

