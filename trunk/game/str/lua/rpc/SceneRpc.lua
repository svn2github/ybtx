local ChangeSceneSucMsg = MsgTbl.ChangeSceneSucMsg
local ChangeSceneFailedMsg = MsgTbl.ChangeSceneFailedMsg
local RetEnterOtherServerMsg = MsgTbl.RetEnterOtherServerMsg
local ReadyGoFbMsg = MsgTbl.ReadyGoFbMsg
local ActionAllReadyMsg = MsgTbl.ActionAllReadyMsg
local SetLevelMsg = MsgTbl.SetLevelMsg
local ClearActionTimesMsg = MsgTbl.ClearActionTimesMsg
local ExitMatchGameMsg = MsgTbl.ExitMatchGameMsg

function Gas2Gac:ChangeSceneErrMsg(Conn, str)
	print(str)
	PostConnMsg(Conn, ChangeSceneFailedMsg)
end

function Gas2Gac:RetEnterOtherServer(Conn, sIp, Port, LoginType, sKey, LastMsgStr)
	Conn.m_IP = sIp
	Conn.m_Port = Port
	Conn.m_LoginType = LoginType
	Conn.m_sKey = sKey
	Conn.m_LastMsgStr = LastMsgStr
	
	PostConnMsg(Conn, RetEnterOtherServerMsg)
end

function Gas2Gac:ChangedInSameSceneSuc(Conn)
	PostConnMsg(Conn, ChangeSceneSucMsg)
end

function Gas2Gac:RetIsJoinFbActionAffirm(Conn, GameName, WaitTime, NpcLevel)
	--print("Open Msg Wnd")
	PostConnMsg(Conn, ReadyGoFbMsg)
end

function Gas2Gac:ActionAllReady(Conn, serverId, roomId, actionName)
	Conn.m_ServerId = serverId
	Conn.m_RoomId = roomId
	Conn.m_ActionName = actionName
	--print("ActionAllReady")
	PostConnMsg(Conn, ActionAllReadyMsg)
end

function Gas2Gac:SetExp(Conn, uAddExp, uInspirationExp, TeamAwardExp, uCurExp, uLevelExp)
	--print("AddLevel Succ")
	PostConnMsg(Conn, SetLevelMsg)
end

function Gas2Gac:ClearActionTimesSucMsg(Conn)
	--print("ClearActionTimesMsg Succ")
	PostConnMsg(Conn, ClearActionTimesMsg)
end

function Gas2Gac:ExitMatchGameSuccMsg(Conn)
	--print("ExitMatchGameSuccMsg")
	PostConnMsg(Conn, ExitMatchGameMsg)
end
