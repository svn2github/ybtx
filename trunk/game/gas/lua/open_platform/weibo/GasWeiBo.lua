CGasWeiBo = class(CWeiBoServer)

function CGasWeiBo:WeiBoRequestStart()
	self:RequestStart()
end

function CGasWeiBo:OpenWebBrowser(sToken)
	Gas2Gac:OpenWebBrowser(self.m_Conn, sToken)
end

function CGasWeiBo:WeiBoLoginCheck(Conn, sCode)
	self:GetAccessToken(sCode)
end

function CGasWeiBo:WeiBoLogin(sUId)
	local sIDCardNumber = ""
	ServerCheckUser(self.m_Conn, sUId, "", self.m_LoginKey, self.m_MacAddress, tonumber(ERatingConfig.AGIP_YYGameID), self.m_DistStr, sIDCardNumber)
end

function CGasWeiBo:LoginFaild()
	Gas2Gac:WeiBoLoginFaild(self.m_Conn)
end