
local function DoTransport(data)
	local Conn, CallerName, bAccept = unpack(data)
	local PlayerId = Conn.m_Player.m_uID
	local Caller = g_GetPlayerInfoByName(CallerName)
	if not Caller then
		return
	end
	local Player = g_GetPlayerInfo(PlayerId)
	if Player:CppGetCtrlState(EFighterCtrlState.eFCS_InBattle) then
		return
	end
	if not Caller.m_WaitingTransportTbl then
		return
	end
	if not Caller.m_WaitingTransportTbl[PlayerId] then
		return
	end
	if not bAccept then
		Caller.m_WaitingTransportTbl[PlayerId] = nil
		return
	end
	local pos = CPos:new()
	Caller:GetGridPos( pos )
	local pointx = math.random(pos.x, pos.x+4)
	local pointy = math.random(pos.y, pos.y+4)
	Caller.m_WaitingTransportTbl[PlayerId] = nil
	local CallerSceneId = Caller.m_Scene.m_SceneId
	local PlayerSceneId = Player.m_Scene.m_SceneId
	if CallerSceneId == PlayerSceneId then	
		local newpos = CPos:new()
		newpos.x = pointx
		newpos.y = pointy
		Player:SetGridPosByTransport( newpos )		
	else
		gas_require "world/scene_mgr/SceneMgr"
		--__ChangeScene( Conn, CallerSceneId, pointx,pointy )
		ChangeSceneById(Conn, CallerSceneId, pointx,pointy)
	end
end

function Gac2Gas:ResponseCallRequest(Conn, CallerName, bAccept)
	local data = {Conn, CallerName, bAccept}
	DoTransport(data)
end

--print("world/trigger_action/obj/IntObjCallPlayerGac2Gas 的DoTransport函数还没有写完")
--[[
function Gac2Gas:ResponseCallRequest(Conn, CallerName, bAccept)
	local function CallBack(result)
		local data = {Conn, result, bAccept}
		DoTransport(data)
	end

	local ex = "Exchanger"
	local paramater = {}
	paramater["char_name"] = CallerName
	CallAccountManualTrans(Conn.m_Account, ex, "getPlayerIdByName", CallBack, paramater)	
end
--]]

function Gac2Gas:OnLeftClickTeammateIcon(Conn, TeammateId)
	local Player = g_GetPlayerInfo(Conn.m_Player.m_uID)
	local Teammate = g_GetPlayerInfo(TeammateId)
	if not Teammate then
--		print("队友不在线")
		return
	end
	local PlayerTeamId = Player.m_Properties:GetTeamID()
	local TeammateTeamId = Teammate.m_Properties:GetTeamID()
	if PlayerTeamId == 0 or PlayerTeamId ~= TeammateTeamId then
--		print("你们不是同一个队伍的")
		return
	end
	Player.m_CallTeammateId = TeammateId
end
