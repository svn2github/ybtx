gas_require "multiserver/MultiRpcLoad"

CMultiServerConn = class(IPipeHandler)

CMultiServerConn.__new = CMultiServerConn.new

function CMultiServerConn:new(NeedEndecode)
	local Inst = CMultiServerConn:__new()
	return Inst 
end

function CMultiServerConn:Listen(IP, Port)
	local addr= CAddress:new()
	addr:SetAddress(IP)
	addr:SetPort(Port)
	self:ShutDown()
	
	self.m_Pipe = g_App:GetIPipeWithBufferSize(self, 1024*1024*10, 1024*1024,false)
	self.m_Pipe:Listen(addr)
end

function CMultiServerConn:Connect(IP, Port)
	local addr= CAddress:new()
	addr:SetAddress(IP)
	addr:SetPort(Port)	
	self:ShutDown()
	
	self.m_Pipe = g_App:GetIPipeWithBufferSize(self, 1024*1024*10, 1024*1024*10,false)
	self.m_Pipe:Connect(addr)
end

function CMultiServerConn:ShutDown()
	--print("Shut Down")
	if self.m_Pipe then
		self.m_Pipe:ShutDown()
		self.m_Pipe:Release()
		self.m_Pipe = nil
	end
end

function CMultiServerConn:OnConnected()
	--print("OnConnect111111")
	local TargetServerId = self.m_TargetServerId
	SetOtherServConnByServId(TargetServerId,self.m_Pipe)
	--g_OtherServerConnList[TargetServerId] = self.m_Pipe
	g_App:Log("OnConnected TargetServerId" .. TargetServerId)
	if g_ConnOtherServerTickTbl[self.m_TargetServerId] then
		UnRegisterTick(g_ConnOtherServerTickTbl[self.m_TargetServerId])
		g_ConnOtherServerTickTbl[self.m_TargetServerId] = nil
	end
	Gas2GasCall:SendSuccConn(self.m_Pipe,g_CurServerId)
end

function CMultiServerConn:OnConnectFailed(eReason)
	self:ShutDown()
	local tarservid = self.m_TargetServerId
	local ip = g_ServerList[tarservid].serverIp
	local port = g_ServerList[tarservid].serverport
	local StartConnTime = g_StartConnTime[tarservid]
		
	if not StartConnTime or (os.time()-StartConnTime) >= 100 then
		LogErr("OnConnectFailed eReason:"..eReason," tarservid:" .. tarservid.. " ip:"..ip.." port:"..port)
		g_StartConnTime[tarservid] = os.time()
	end
	if not g_ConnOtherServerTickTbl[tarservid] then
		
		local function ConnectTick(tick, Conn, IP, Port)
			UnRegisterTick(g_ConnOtherServerTickTbl[Conn.m_TargetServerId])
			g_ConnOtherServerTickTbl[Conn.m_TargetServerId] = nil
			Conn:Connect(IP, Port)
		end
		g_ConnOtherServerTickTbl[tarservid] = RegisterTick("ConnOtherServerTick", ConnectTick, g_MultiServReConnTickTime,self,ip,port)
	end
	
	g_App:Log("OnConnectFailed TargetServerId" .. tarservid)

end

function CMultiServerConn:OnDisconnected(eReason,bGracefully)
	self:ShutDown()
	--关服了
	if not g_App then
		return
	end
	local tarservid = self.m_TargetServerId
	local ip = g_ServerList[tarservid].serverIp
	local port = g_ServerList[tarservid].serverport
		
	if not IsServerShutDownState(tarservid) then --如果不是正常关服而导致连接断开则报错误信息
		if bGracefully then
			bGracefully = "true"
		else
			bGracefully = "false"
		end
		LogErr("多服之间Connection连接断开 eReason:"..eReason.." bGracefully:"..bGracefully,"g_CurServerId:"..g_CurServerId.." TargetServerId:"..tarservid.." ip:"..ip.." port:"..port)
	else
		ResetServerShutDownState(tarservid)
	end
	if not g_ConnOtherServerTickTbl[tarservid] then
		local function ConnectTick(tick, Conn, IP, Port)
			UnRegisterTick(g_ConnOtherServerTickTbl[Conn.m_TargetServerId])
			g_ConnOtherServerTickTbl[Conn.m_TargetServerId] = nil
			Conn:Connect(IP, Port)
		end
		g_StartConnTime[tarservid] = os.time()
		g_ConnOtherServerTickTbl[tarservid] = RegisterTick("ConnOtherServerTick", ConnectTick, g_MultiServReConnTickTime,self,ip,port)
	end
	
	g_App:Log("OnDisconnected TargetServerId" .. tarservid)
end

CMultiServerPipeHandler = class(IPipeHandler)
CMultiServerPipeHandler.__new = CMultiServerPipeHandler.new

function CMultiServerPipeHandler:new(pipe, szTableName)
	local Inst = CMultiServerPipeHandler:__new()
	Inst.m_szTableName = szTableName
	Inst.m_Pipe = pipe
	return Inst
end

local function _OnDataReceived(Conn, TableName)
	local RegistUnPack = CMultiUnPackerTable_Inst()
	RegistUnPack = RegistUnPack:GetUnPacker(TableName)
	
	while true do
		local uRecDataSize = Conn:GetRecvDataSize()
		
		if uRecDataSize >= 2 then
			local ReadLen = RegistUnPack:UnPack(Conn)
			
			--ReadLen只有大于0，才表明读取成功了数据
			if ReadLen <= 0 then
				break
			end
		else
			break
		end	
	end
end

function CMultiServerPipeHandler:OnDataReceived()
	--关服了
	if not g_App then
		return
	end
	--print("OnDataReceived")
	_OnDataReceived(self.m_Pipe, self.m_szTableName)
end

function CMultiServerPipeHandler:OnDisconnected(eReason,bGracefully)
	if self.m_Pipe then
		self.m_Pipe:ShutDown()
		self.m_Pipe:Release()
		self.m_Pipe = nil
	end
end

function CMultiServerPipeHandler:OnDataSent(uSentSize)
	--print("OnDataSent")
end


function CMultiServerConn:OnAccepted(pPipe)
	--print("OnAccept g_CurServerId"..g_CurServerId)
	--g_App:Log("OnAccept g_CurServerId"..g_CurServerId)
	local handler = CMultiServerPipeHandler:new(pPipe, self.m_szTableName)
	pPipe:SetHandler(handler)
end

function CMultiServerConn:OnDataReceived()
	--print("OnDataReceived")
	_OnDataReceived(self.m_Pipe, self.m_szTableName)
end

local CallRegist = false

function CMultiServerConn:RegistCall( AliasPath, szFileName,szTableName, szClassName )
	if CallRegist then
		return
	end
	
	CallRegist = true
	
	local ObjCall = CMultiCallRegist:new()
	ObjCall:Load(AliasPath, szFileName,szTableName, szClassName)
end

local DefRegist = false

function CMultiServerConn:RegistDef( AliasPath, szFileName,szTableName, szClassName )	
	self.m_szTableName= szTableName

	if DefRegist then
		return
	end
	
	DefRegist = true
	
	local ObjDef = CMultiDefRegist:new()
	ObjDef:Load(AliasPath, szFileName,szTableName,szClassName)
end
