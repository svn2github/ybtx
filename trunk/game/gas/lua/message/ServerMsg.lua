gac_gas_require "framework/message/message"

function SafeMsgToConn(Conn,MessageId,...)
	local arg = {...}
	MsgToConn(Conn,MesageId,arg)
end

function MsgToConn(Conn,MessageId,...)
	local arg = {...}
	if g_MsgIDToFunTbl[MessageId] == nil then
		LogErr("Message_Common出错","MessageId:" .. MessageId .. "不在Message_Common表中")
	end
	Gas2Gac[g_MsgIDToFunTbl[MessageId]](Gas2Gac,Conn,MessageId,...)
end

function MsgToScene(Scene,MessageId,...)
	local arg = {...}
	Gas2Gac[g_MsgIDToFunTbl[MessageId]](Gas2Gac,Scene.m_CoreScene,MessageId,...)
end

function MsgToServer(MessageId,...)
	local arg = {...}
	Gas2Gac[g_MsgIDToFunTbl[MessageId]](Gas2Gac,g_AllPlayerSender,MessageId,...)
end

function SystemFriendMsgToClient(Conn, MessageId, ... )
	local messageCommon = g_MsgIDIDToFriendFunTbl[MessageId]
	if(not messageCommon) then return end
	local arg = {...}
	Gas2Gac[g_MsgIDIDToFriendFunTbl[MessageId]](Gas2Gac,Conn, MessageId,...)
end

function SystemFriendMsgToClientCrossServer(CharID, MessageId, ... )
    local arg = { ... }
    Gas2GacById[g_MsgIDIDToFriendFunTbl[MessageId]](Gas2GacById, CharID, MessageId,...)
end

--系统活动公告
function SysMovementNotifyToClient(ConnMgr,MessageId, ...)
	local arg = {...}
	local str = GacSystemMovementNotifyAssembleArgs(MessageId, arg)
	Gas2Gac:SysMovementNotifyToClient(ConnMgr, str)
end

function GetStaticTextServer(uTextId, ...)
	local arg = {...}
	return GacStaticTextAssembleArgs(uTextId, arg)
end

function MsgToConnById(CharId, MessageId,...)
	assert(IsNumber(CharId), "MsgToConnById 必须通过Id发消息")
	Gas2GacById[g_MsgIDToFunTbl[MessageId]](Gas2GacById,CharId,MessageId,...)
end