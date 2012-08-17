gas_require "world/trigger_action/obj/AllObjScript"

gas_require "world/trigger_action/obj/CallPlayerObj/IntObjCallPlayerGac2Gas"
gas_require "world/trigger_action/obj/CollObj/IntObjCollObjGac2Gas"

function Gac2Gas:OnLClickIntObj(Conn, EntityID)
	local Player = Conn.m_Player
	
	if not IsCppBound(Player) then
		return
	end
	
	if not Player:CppIsLive() then
		return
	end
	
	local ServerIntObj = CIntObjServer_GetIntObjServerByID(EntityID)
	if not IsServerObjValid(ServerIntObj) then
		return
	end
	
--	if ServerIntObj.m_Properties:GetCanUseState() == 0 then
--		return
--	end
	
	local ObjName = ServerIntObj.m_Properties:GetCharName()
	ServerIntObj.m_DlgOnClicked(Conn, ServerIntObj, ObjName, EntityID)
	g_TriggerMgr:Trigger("µã»÷", ServerIntObj, Player)
end

function Gac2Gas:OnSelectIntObj(Conn, EntityID)
	local Player = Conn.m_Player
	if not IsCppBound(Player) then
		return
	end
	local ServerIntObj = CIntObjServer_GetIntObjServerByID(EntityID)
	if ServerIntObj == nil then
		return
	end
	
--	if ServerIntObj.m_Properties:GetCanUseState() == 0 then
--		return
--	end
	if Player.m_LockingObj then
		Player:SetTarget( nil )
		Player.m_LockingObj = nil
	end
	Player.m_LockingIntObj = ServerIntObj
end

function Gac2Gas:UnLockIntObj( Conn )
	if( Conn.m_Player and Conn.m_Player.m_LockingIntObj ~= nil ) then
		Conn.m_Player.m_LockingIntObj  = nil
	end		
end