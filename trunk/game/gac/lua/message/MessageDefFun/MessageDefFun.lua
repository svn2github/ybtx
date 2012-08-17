gac_gas_require "framework/message/MessageFun"

local function AutoDefGas2GacMessageFun(funName1,funName2)
	local str = 
	[[
		function Gas2Gac:%s(Conn, MessageId,...)
			%s(MessageId,...)
		end 
	]]
	str = string.format(str, funName1[1],funName2)
	loadstring(str)()
end

local MessageFunTbl = GetMessageFun("Message","Lan_Message_Common")
for i,funName in pairs(MessageFunTbl) do
	AutoDefGas2GacMessageFun(funName,"MsgClient")
end

local FriendMsgToClientFunTbl = GetMessageFun("SystemFriendMsgToClient","Lan_SystemFriendMessage_Common")
for i,funName in pairs(FriendMsgToClientFunTbl) do
	AutoDefGas2GacMessageFun(funName,"ReceiveSystemFriendMsg")
end