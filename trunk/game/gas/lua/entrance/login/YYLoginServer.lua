CYYLoginServer = class()

function CYYLoginServer.YYRequest(Conn,sLoginKey,sMacAddress,DistStr)
	Conn.m_YYLoginKey = sLoginKey
	Conn.m_YYMacAddress = sMacAddress
	Conn.m_YYDistStr = DistStr
	local str = CYYServer_GetInst():RecvLoginRequest(Conn.m_YYLoginKey)
	local msg =  "sLoginKey:" .. Conn.m_YYLoginKey .. "  YYKey1:" .. str 
	g_YYLog:Write(msg)
	Gas2Gac:YYReturn(Conn,str)
end

function CYYLoginServer.YYlogin(Conn,key)
	local flag = CYYServer_GetInst():RecvLoginInfo(key,Conn.m_YYLoginKey)
	local RetFun = ""
	if tonumber(flag) == 1 then
		local UserID = CYYServer_GetInst():GetUserID(key)
		local CardNumber = CYYServer_GetInst():CardNumber(key)
		ServerCheckUser( Conn,UserID,"", Conn.m_YYLoginKey,Conn.m_YYMacAddress,tonumber(ERatingConfig.AGIP_YYGameID),Conn.m_YYDistStr,CardNumber)
		RetFun = "ServerCheckUser"
	else
		Gas2Gac:YYLoginFail(Conn)
		RetFun = "YYLoginFail"
	end
	local msg =  "sLoginKey:" .. Conn.m_YYLoginKey .. "  YYKey2:" .. key .. "  Result:" .. flag .. "  RetFun:" .. RetFun
	g_YYLog:Write(msg)
end