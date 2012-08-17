local LoginSucMsg = MsgTbl.LoginSucMsg
local LoginFailedMsg = MsgTbl.LoginFailedMsg
local CreateRoleSucMsg = MsgTbl.CreateRoleSucMsg 
local CreateRoleFailedMsg = MsgTbl.CreateRoleFailedMsg
local GetCharListMsg = MsgTbl.GetCharListMsg
local CreatePlayerMsg = MsgTbl.CreatePlayerMsg
local LoginWaitingMsg = MsgTbl.LoginWaitingMsg
local DelWaitingMsg = MsgTbl.DelWaitingMsg
local UpdateWaitingMsg = MsgTbl.UpdateWaitingMsg
local RepeatLoginMsg = MsgTbl.RepeatLoginMsg

function Gas2Gac:ReturnCheckUser( Conn,strRet,userName)
	if strRet == "200" or strRet == "201" then
		PostConnMsg(Conn, LoginSucMsg)
	else	
		PostConnMsg(Conn, LoginFailedMsg)
	end	
end

function Gas2Gac:ReturnCreateRole(Conn, strRes)
	if strRes == 0 then
		PostConnMsg(Conn, CreateRoleSucMsg)
	else
		PostConnMsg(Conn, CreateRoleFailedMsg)
	end
end

function Gas2Gac:ReturnCharList( Connection, uID, Name,uHair, uHairColor, uFace, fScale, uSex, uClass, uCamp, uLevel, uSceneID,  
								 uArmetResID, uBodyResID, uBackResID, uShoulderResID, uArmResID, uShoeResID, uWeaponID, uOffWeaponID,uArmetPhase,uBodyPhase,uArmPhase,uShoePhase,uShoulderPhase,uWeaponPhaseu,OffWeaponPhase )
	local role = {}
	role.m_id				= uID
	role.m_name				= Names

	table.insert( Connection.m_RoleList, role )
	
end

function Gas2Gac:ReturnCharListEnd( Conn )
	PostConnMsg(Conn, GetCharListMsg)
end

function Gas2Gac:CreatePlayerEnded(Conn)
	PostConnMsg(Conn, CreatePlayerMsg)
end

function Gas2Gac:ReturnLoginWaitingQueue(Conn, SequenceId)
	Conn.m_WaitingID = SequenceId
	PostConnMsg(Conn, LoginWaitingMsg)
end

function Gas2Gac:ReturnDelWaitingUser(Conn)
	PostConnMsg(Conn, DelWaitingMsg)
end

function Gas2Gac:ReturnUpdateWaitSequence(Conn, SequenceId)
	Conn.m_WaitingID = SequenceId
	print(RunningCaseTbl[Conn.m_Co].m_UserName, SequenceId)
	PostConnMsg(Conn, UpdateWaitingMsg)
end 

function Gas2Gac:RetReSetLogin(Conn)
	PostConnMsg(Conn, RepeatLoginMsg)
end
