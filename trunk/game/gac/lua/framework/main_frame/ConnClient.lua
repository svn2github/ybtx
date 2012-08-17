--给connection提供设置account和user name的函数
local _SetConnValue = CConnClient.SetValue

local ConnUserName 
local ConnRoleName 

function CConnClient:SetUserName(userName)
	ConnUserName = userName
	ErrLogMsg("UserName", userName)
	_SetConnValue(self, "UserName", userName)
end

function CConnClient:GetUserName()
	return ConnUserName
end

function CConnClient:SetRoleName(roleName)
	ConnRoleName = roleName
	ErrLogMsg("RoleName", roleName)
	_SetConnValue(self, "RoleName", roleName)
end

function CConnClient:GetRoleId()
	return ConnRoleName
end

function SwitchRecvTrafficStat(bStart)
	if(not g_Conn)then
		return
	end
		
	if(bStart ~= 0)then
		--print("StartRecvTrafficStat")
		g_Conn:StartRecvTrafficStat()
	else
		--print("CloseRecvTrafficStat")
		g_Conn:CloseRecvTrafficStat()
	end
end
