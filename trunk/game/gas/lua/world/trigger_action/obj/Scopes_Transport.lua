--这里的传送是指 点击一个obj后传到另一个地方(要读条的)
gas_require "world/trigger_action/TriggerCommonFun"

local Transport_Server = Transport_Server
local ServerBeginProgress = ServerBeginProgress
local CheckDistance = CheckDistance
local CheckPlayerLev = CheckPlayerLev
local MsgToConn = MsgToConn
local IsCppBound = IsCppBound
local IntObj_Common = IntObj_Common
local CallAccountManualTrans = CallAccountManualTrans
local MultiServerChangeScene = MultiServerChangeScene

local g_GetPlayerInfo = g_GetPlayerInfo
local Entry = CreateSandBox(...)


function Entry.TransportTickFunSucc(Player, data)
	local TransPortCommon = data[1]
	local SceneId, ServerId = data[2],data[3]
	local PosX, PosY = TransPortCommon("PosX"), TransPortCommon("PosY")
	local Conn = Player.m_Conn
	if IsCppBound(Conn) and IsCppBound(Conn.m_Player) then
		if PosX and PosY then
			MultiServerChangeScene(Conn, SceneId, ServerId, PosX, PosY)
		end
	end
	Player.m_IsUsingObj = nil
end

function Entry.TransportTickFunFail(Player, data)
	if IsCppBound(Player) then
		Gas2Gac:NotifyStopProgressBar(Player.m_Conn)
		Player.m_IsUsingObj = nil
	end
end

function Entry.Exec(Conn, ServerIntObj, ObjName, GlobalID)
	local Player = Conn.m_Player
	
	local PlayerLev = Player:CppGetLevel()
	
	if Player.m_IsUsingObj then
		return
	end
	
	if not ServerIntObj.m_IsCanTransSpecilAreaFb then
		--print("副本还没有创建成功,等等再进")
		return
	end

	local Camp = Player:CppGetBirthCamp()
	local TargetName = ServerIntObj.m_Properties:GetCharName()
	local SceneId = ServerIntObj.m_SceneId
	local ServerId = ServerIntObj.m_ServerId
	local SceneName = ServerIntObj.m_SceneName
	local TargetCamp = ServerIntObj.m_CreateSceneCamp
	
	local TransPortCommon = nil
	if Transport_Server(TargetName) then
		for _, i in pairs(Transport_Server:GetKeys(TargetName, "交互对象")) do
			local TransInfo = Transport_Server(TargetName, "交互对象", i.."")
			if CheckDistance(Player,ServerIntObj, TransInfo("LeastDis"))
				and CheckPlayerLev(PlayerLev, TransInfo("LeastDis"), TransInfo("MaxLev")) then
					
				if SceneName == TransInfo("EndScene")  then
					TransPortCommon = TransInfo
					break
				end
			end
		end
	end
	
	local function CallBack(result)
		if IsCppBound(Player) then
			if result then
				Player.m_IsUsingObj = true
				
				local time = TransPortCommon("LeadTime")
				local data = {TransPortCommon,SceneId,ServerId}
				data["ActionName"] = IntObj_Common(TargetName, "ActionName")
				if time == "" then
					Entry.TransportTickFunSucc(Player, data)
				else
					ServerBeginProgress(time*1000, false, Conn, Entry.TransportTickFunSucc, Entry.TransportTickFunFail, data)
				end
			else
				MsgToConn(Player.m_Conn,3510)
			end
		end
	end
	
	if TransPortCommon then
		local param = {}
		param["uCharId"] = Player.m_uID
		param["PlayerCamp"] = Camp
		param["TrapCamp"] = TargetCamp
		param["SceneId"] = SceneId
		--CallAccountAutoTrans
		CallAccountManualTrans(Conn.m_Account, "SceneMgrDB", "IsEnterScopesFb", CallBack, param)
	end
end
 
return Entry
 
