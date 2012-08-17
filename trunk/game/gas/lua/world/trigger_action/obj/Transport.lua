--这里的传送是指 点击一个obj后传到另一个地方(要读条的)
gas_require "framework/main_frame/SandBoxDef"
gas_require "world/trigger_action/TriggerCommonFun"
gas_require "world/trigger_action/item/item_trigger/CreateAreaScene"
gas_require "world/tong_area/TongLoadProgress"

local Transport_Server = Transport_Server
local IntObj_Common = IntObj_Common
local loadstring = loadstring 
local LeaveAreaScene = LeaveAreaScene
local ServerBeginProgress = ServerBeginProgress
local CPos = CPos
local CheckDistance = CheckDistance
local CheckPlayerLev = CheckPlayerLev
local MsgToConn = MsgToConn
local IsCppBound = IsCppBound
local IsServerObjValid = IsServerObjValid
local CallAccountAutoTrans = CallAccountAutoTrans
local RandomEvent = RandomEvent
local EActionState = EActionState
local g_IntObjServerMgr = g_IntObjServerMgr
local CommLoadProgress = CommLoadProgress
local EProgressBreak = EProgressBreak
local NewChangeScene = NewChangeScene
local g_GetPlayerInfo = g_GetPlayerInfo
local TongLoadProgress = TongLoadProgress

local Entry = CreateSandBox(...)


function Entry.TransportTickFunSucc(Player, data)
	local TransPortCommon = data[2]
	local Pos = CPos:new()
--	local tbl = assert(loadstring("return " .. TransPortCommon.TargetPos))()
	Pos.x,Pos.y = TransPortCommon("PosX"), TransPortCommon("PosY")
	if IsCppBound(Player) then
		if TransPortCommon("EndScene") == Player.m_Scene.m_SceneName then		
			Player:SetAndSyncActionState(EActionState.eAS_Idle_BackWpn)
			Player:SetGridPosByTransport(Pos)
		else
			NewChangeScene(Player.m_Conn, TransPortCommon("EndScene"), Pos.x , Pos.y )
		end
		Player.m_IsUsingObj = nil
	end
end

function Entry.TransportTickFunFail(Player, data)
	if IsCppBound(Player) then
		Gas2Gac:NotifyStopProgressBar(Player.m_Conn)
		Player.m_IsUsingObj = nil
	end
end

function Entry.Exec(Conn, ServerIntObj, ObjName, GlobalID)
	local Player = Conn.m_Player
	
	if Player.m_Scene.m_SceneAttr.SceneType == 12 then
		if ObjName == "矿洞机关" then
			if ServerIntObj.IsClick then
				return
			end
			local function SuccFun()
				if RandomEvent(Conn, ServerIntObj) then
					ServerIntObj.IsClick = true
					if IsServerObjValid(ServerIntObj) then
						g_IntObjServerMgr:IntObjChangeState(ServerIntObj, 0)
					end
				end
			end
			TongLoadProgress(Player, 5000, SuccFun, nil,{}) 
		elseif ObjName == "离开矿洞" then
			local function SuccFun(data)
				if IsCppBound(Conn) then
					LeaveAreaScene(Conn)
				end
			end
			local data = {}
			data["BreakType"] = {EProgressBreak.ePB_Attack}
			data["ActionName"] = IntObj_Common(ObjName, "ActionName")
			CommLoadProgress(Player, 5000, SuccFun, nil, data)
		end
		return
	end
	
	local PlayerLev = Player:CppGetLevel()
	local SceneName = Player.m_Scene.m_SceneName
	
	if Player.m_IsUsingObj then
		return
	end
	
	------------------------
	local function Transport(Player,TransPortCommon)
		local time = TransPortCommon("LeadTime")
		local data = {GlobalID,TransPortCommon}
		data["ActionName"] = IntObj_Common(ObjName, "ActionName")
		if time == "" then
			Entry.TransportTickFunSucc(Player, data)
		else
			ServerBeginProgress(time*1000, false, Conn, Entry.TransportTickFunSucc, Entry.TransportTickFunFail, data)
		end
	end
	---------------------------
	if Transport_Server(ObjName) then
		for _, i in pairs(Transport_Server:GetKeys(ObjName, "交互对象")) do
			local TransInfo = Transport_Server(ObjName, "交互对象", i.."")
			if (TransInfo("BeginScene") == "" or SceneName == TransInfo("BeginScene") )
				and (TransInfo("Camp") == 0 or TransInfo("Camp") == Player:CppGetBirthCamp())
				and CheckDistance(Player,ServerIntObj, TransInfo("LeastDis"))
				and CheckPlayerLev(PlayerLev, TransInfo("LeastLev") ,TransInfo("MaxLev")) then
				
				Player.m_IsUsingObj = true
				
				if TransInfo("QuestNeed") == "" then
					Transport(Player,TransInfo)
				else
					local function CallBack(IsSatisfy)
						if IsCppBound(Player) then
							if not IsSatisfy then
								Player.m_IsUsingObj = nil
							else
								Transport(Player,TransInfo)
							end
						end
					end
					local param = {}
					param["sQuestName"] = TransInfo("QuestNeed") 
					param["uCharId"] = Player.m_uID
					CallAccountAutoTrans(Conn.m_Account, "RoleQuestDB","CheckQuestSatisfy", CallBack, param)
				end
				break 
			end	
		end
	end
end
 
return Entry
 
