
CConnection = class(IPipeHandler)

--[[
返回的第一个参数含义
0 表示 操作成功
1 表示 timeout
2	表示 远端断开
3 表示 本地断开
--]]

CConnectionTick = class(CTick)

function CConnectionTick:Ctor(co)
	self.m_Routine = co
end

function CConnectionTick:OnTick()
	g_App:UnRegisterTick(self)
	coroutine.resume(self.m_Routine, 1)
end

function CConnection:Connect(IP, Port, OutTime)
	local addr= CAddress:new()
	addr:SetAddress(IP)
	addr:SetPort(Port)
	
	----print("************connect***************")									
	self.m_Pipe = g_App:GetIPipe(self)
	self.m_Routine = coroutine.running()
	self.m_Pipe:Connect(addr)
	self.m_Tick = CConnectionTick:new(self.m_Routine)

	g_App:RegisterTick(self.m_Tick,OutTime)
	self.m_Tick:SetTickName("CConnection")

	return coroutine.yield()
end

function CConnection:SetPipeEndecode(flag)
	self.m_Pipe:SetEnableEndecode(flag)
end

function CConnection:Recv(OutTime)
	
	local size = self.m_Pipe:GetRecvDataSize()
	----print("Recv Size = ", size)
	if size > 0 then
		local obj = self.m_Pipe:GetRecvData()
		
		local clone_buf_acr = CreateCBufAcr( CreateCBuf(size) )
		clone_buf_acr:Copy( obj, size )
		clone_buf_acr:Seek("begin")
		
		self.m_Pipe:PopRecvData(size)
		return 0, clone_buf_acr, size 
	elseif size == 0 then
		g_App:RegisterTick(self.m_Tick, OutTime)
		self.m_Tick:SetTickName("CConnection")
		return coroutine.yield()
	end
end

function CConnection:Send(Data, Size, OutTime)
	----print("---------------------->Send :", Data, Size)
	self.m_Pipe:Send(Data, Size)
	----print("---------------------->SendDone :", Data, Size)
	g_App:RegisterTick(self.m_Tick, OutTime)
	self.m_Tick:SetTickName("CConnection")
	return coroutine.yield()
end

function CConnection:ShutDown()
	local co = coroutine.running()
	if co ~= self.m_Routine then
		g_App:UnRegisterTick(self.m_Tick)
		coroutine.resume(self.m_Routine, 3)
	else
		self.m_Pipe:ShutDown()
		self.m_Pipe:Release()
	end
end


function CConnection:OnConnected()
	----print("---------------------> OnConnected()")
	g_App:UnRegisterTick(self.m_Tick)
	coroutine.resume(self.m_Routine, 0)
end

function CConnection:OnConnectFailed(eReason)
	----print("----------------------> OnConnectFailed()")
	g_App:UnRegisterTick(self.m_Tick)
	coroutine.resume(self.m_Routine, 1, eReason)
end

function CConnection:OnDisconnected(eReason,bGracefully)
	----print("------------------------> OnDisconnected()")
	g_App:UnRegisterTick(self.m_Tick)
	coroutine.resume(self.m_Routine, 2)
end

function CConnection:OnAccepted(pPipe)
end

function CConnection:OnDataReceived()
	--print("---------------------------> OnDataReceived()")
	g_App:UnRegisterTick(self.m_Tick)
	local size = self.m_Pipe:GetRecvDataSize()
	local obj = self.m_Pipe:GetRecvData()
	self.m_Pipe:PopRecvData(size)
	coroutine.resume(self.m_Routine, 0, obj, size)
end

function CConnection:OnDataSent(uSentSize)
	----print("------------------------------> OnDataSent()")
	g_App:UnRegisterTick(self.m_Tick)
	coroutine.resume(self.m_Routine, 0)
end

function CConnection:Ctor()
	self.m_Pipe = {}
	self.m_Routine =		{}
end
