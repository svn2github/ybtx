--帮会类资源采集方式 
gas_require "framework/main_frame/SandBoxDef"
local MsgToConn = MsgToConn
local IsCppBound = IsCppBound
local g_IntObjServerMgr = g_IntObjServerMgr
local ProTime = 10000

local function LoadResource(Player, Depositary, flag)

	local Conn = Player.m_Conn
	local DepositaryRes = Depositary.m_Properties:GetResourceCount()
	local TruckNpc = Player:GetServantByType(ENpcType.ENpcType_Truck)
	local Truck = TruckNpc.m_TruckInfo
	
--	if flag then
--		Truck.m_MaxLoad = DepositaryRes
--	end
	
	local TruckResource = Truck.m_Resource
	local TruckMaxLoad = Truck.m_MaxLoad
	local LoadResource = TruckMaxLoad - TruckResource
	
	if TruckMaxLoad <= TruckResource then
		MsgToConn(Conn, 9202)
		return
	end
	
	if LoadResource >= DepositaryRes then
		g_IntObjServerMgr:Destroy(Depositary,false)
--		Truck.m_Resource = Truck.m_Resource + DepositaryRes
		Truck:LoadResource(DepositaryRes)
	else
		Depositary.m_Properties:SetResourceCount(DepositaryRes - LoadResource)
--		Truck.m_Resource = Truck.m_Resource + LoadResource
		Truck:LoadResource(LoadResource)
	end
end

local function CollectResToTruck(Conn, IntObj, ObjName, GlobalID)

	local Player = Conn.m_Player
	local Truck = Player:GetServantByType(ENpcType.ENpcType_Truck)
	
	if Player.m_CollectResTick then
		MsgToConn(Conn, 9207)
		return
	end
	if Player:CppGetDoSkillCtrlState( EDoSkillCtrlState.eDSCS_InDoingSkill) then
		MsgToConn(Conn, 9208)
		return
	end
	if Player.m_CollIntObjID or Player.m_PickUpNpcID then
		MsgToConn(Conn, 3060)
		return
	end
	
--	--没有运输车
--	if not IsServerObjValid(Truck) then
--		MsgToConn(Conn, 9201)
--		return
--	end
	
	if Truck then
		local TruckResource = Truck.m_TruckInfo:GetResource()
		local TruckMaxLoad = Truck.m_TruckInfo:GetMaxResource()
		
		if TruckMaxLoad <= TruckResource then
			MsgToConn(Conn, 9202)
			return
		end
	end
	
--	if IntObj.m_Openner then
--		MsgToConn(Conn, 9209)
--		return
--	end
	if not IntObj.m_Openner then
		IntObj.m_Openner = {}
	end
	
	IntObj.m_Openner[Player.m_uID] = true
	Player.m_ResourceObjID = GlobalID
	local data = {GlobalID}
	
	local function LoadResSucc(data)
		IntObj.m_Openner[Player.m_uID] = nil
		if not IsCppBound(Player) then
			return
		end
		Player.m_ResourceObjID = nil
--		local uTongID = Player.m_Properties:GetTongID()
--		if uTongID == 0 then
--			MsgToConn(Conn, 9401)
--			return
--		end
		if not IsCppBound(IntObj) then
			MsgToConn(Conn, 9206)
			return
		end
		local flag = false
		if not IsServerObjValid(Truck) then
--			MsgToConn(Conn, 9203)
			local res = Player:PlayerDoPassiveSkill("召唤中型运输车",1)
			if res ~= EDoSkillResult.eDSR_Success then
				return
			end
			flag = true
		end
		LoadResource(Player, IntObj, flag)
	end
	
	local function LoadResFail(data)
		local GlobalID = data[1]
		local IntObj = CIntObjServer_GetIntObjServerByID(GlobalID)
		IntObj.m_Openner[Player.m_uID] = nil
		if Player then
			Player.m_ResourceObjID = nil
		end
	end
	
	if not TongLoadProgress(Player, ProTime, LoadResSucc, LoadResFail, data) then
		MsgToConn(Conn, 9208)
	end
end

--散落资源的收集方式
local g_GetPlayerInfo = g_GetPlayerInfo
local Entry = CreateSandBox(...)
function Entry.Exec(Conn, IntObj, ObjName, GlobalID)
	if not IsCppBound(Conn) or not IsCppBound(Conn.m_Player) then 
		return 
	end
--	local uTongID = Conn.m_Player.m_Properties:GetTongID()
--	if uTongID == 0 then
--		MsgToConn(Conn, 9401)
--		return
--	end
	CollectResToTruck(Conn, IntObj, ObjName, GlobalID)
end 

return Entry
