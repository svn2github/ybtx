gas_require "framework/web_server/WebServerInl"
gas_require "framework/web_server/WebProc"

g_WebServerCreatedPipe = 0
g_WebServerDeletedPipe = 0

function CWebServerHandler:Ctor()
	self.m_bListening =  false						
	self.m_Pipe = g_App:GetIPipeWithBufferSize(self, 10*1024*1024, 1024*1024,true )
end

function CWebServerHandler:_Listen()
	if g_CurServerId ~= 1 then
		return
	end
	local ip = GasConfig.GameWebServerIp or '127.0.0.1'
	local port = GasConfig.GameWebServerPort or 10077
	local addr = CAddress:new()
	addr:SetAddress(ip)
	addr:SetPort(port)
	self.m_Pipe:Listen(addr)
end

function CWebServerHandler:Listen()
	print("Webserver listen begin")
	self:_Listen()
end

function CWebServerHandler:Shutdown()	
	if self.m_Pipe and IsCppBound(self.m_Pipe) then
		self.m_Pipe:SetHandler(nil)
		self.m_Pipe=nil
	end
end

function CWebServerHandler:OnConnected()
end

function CWebServerHandler:OnAccepted(pPipe)
	g_WebServerCreatedPipe = g_WebServerCreatedPipe + 1
	--print("++", g_WebServerCreatedPipe)
	local pHandler = CWebServerProcessorHandler:new()
	local function DeleteElapsedPipe(Tick, WebServerProcHandler)
		--print("web server Ontick", WebServerProcHandler)
		WebServerProcHandler:DeletePipe()
	end
	pHandler.m_DeletePipeTick = RegisterTick("DeleteElapsedPipe", DeleteElapsedPipe, 5 * 60 * 1000, pHandler)
	pPipe:SetHandler(pHandler)
	pHandler:SetPipe(pPipe)
end

function CWebServerHandler:OnListenFaild(reason)
	print("Listen Failed", "Reason is " .. reason)
	self:_Listen()
end

function CWebServerHandler:OnDisconnected(eReason, bGracefully)
end

function CWebServerHandler:OnConnectFailed(eReason)
end

function CWebServerHandler:OnDataReceived()
	--print("CWebServerHandler:OnDataReceived()")
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

function CWebServerHandler:OnDataSent(uSentSize)
end

function Gas2GasDef:LogCreatedPipe(ServerConn)
	local name = "Pipe"..GetCurrentPID()
	local strpath = CBaseScriptApp_Inst():GetRootFilePath(nil) .. "/" .. "var/gas/" .. name .. ".log"
	local fo = CLuaIO_Open(strpath, "a")
	local strInfo = "CreatedPipe\t"..g_WebServerCreatedPipe.."\n".."DeletedPipe\t"..g_WebServerDeletedPipe
	fo:WriteString(strInfo.."\n")
	fo:Close()
end

g_WebServer = g_WebServer or CWebServerHandler:new()
