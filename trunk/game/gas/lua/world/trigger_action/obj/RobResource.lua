--帮会类资源采集方式 
gas_require "framework/main_frame/SandBoxDef"

local MsgToConn = MsgToConn
local IsCppBound = IsCppBound
local g_ObjActionArgTbl = g_ObjActionArgTbl
local g_IntObjServerMgr = g_IntObjServerMgr
local StartSignTime = GetCfgTransformValue(false, "TongStartTime_Common", "抢夺资源点报名时间", "OpenTime") --loadstring("return " .. TongStartTime_Common["抢夺资源点报名时间"].OpenTime)()  --玩家可以开始报名的时间(分)
local WaitJointTime = TongStartTime_Common("抢夺资源点报名时长", "OpenTime")   		--可以继续报名的时间(分)
local WaitEnterSceneTime = TongStartTime_Common("抢夺资源点进场时长", "OpenTime")	--玩家可以进场的时间(秒)
local GameLength = TongStartTime_Common("抢夺资源点游戏时长", "OpenTime")
local _ActionName = "抢夺资源点"
local RobTime = 600
local objTbl = {
	[1] = "艾米抢夺资源点",
	[2] = "神圣抢夺资源点",
	[3] = "修斯抢夺资源点"
}

local playCampTbl = {
	[1] = GetStaticTextServer(2501),
	[2] = GetStaticTextServer(2502),
	[3] = GetStaticTextServer(2503)
}

local function IsInOpenTime(data, length)
	local curDate = os.date("*t")
	local hour = curDate.hour
	local minute = curDate.min
	local second = curDate.sec
	local todayCurSecond = (hour * 60 + minute) * 60 + second 
	local offSet = GetOffsetSecond(data.time[1])
	local endOffSet = offSet + length 
	if todayCurSecond >= offSet and todayCurSecond <= endOffSet then
		return true, endOffSet - todayCurSecond
	end
	return false
end

local function GetRobResource(Player, GlobalID)

	local Obj = CIntObjServer_GetIntObjServerByID(GlobalID)
	local name = Obj.m_Properties:GetCharName()
	local tongName = Player.m_Properties:GetTongName()
	local tongId = Player.m_Properties:GetTongID()
	local PlayerCamp = Player:CppGetCamp()
	local PlayerCampStr = playCampTbl[PlayerCamp]
	local scene = Player.m_Scene
	local PlayerTbl = scene.m_tbl_Player
	local PlayerIdTbl = {}
	for id, _ in pairs(PlayerTbl) do
		table.insert(PlayerIdTbl, id)
	end 
	local parameter = {}	
	parameter["tongId"] = tongId
	parameter["PlayerCamp"] = PlayerCamp
	parameter["PlayerTbl"] = PlayerIdTbl
	
	--读条成功,更换obj
	local pos = GetCreatePos(Player)
  local dir = Obj:GetActionDir()
	g_IntObjServerMgr:Destroy(Obj,true)
	local name = objTbl[PlayerCamp]
	--Obj = CreateIntObj(Player.m_Scene,pos,name,false)
  Obj = CreateIntObj(Player.m_Scene,pos,name,false,nil,nil,nil,dir)
	--将此资源点存数据库
	CallAccountManualTrans(Player.m_Conn.m_Account, "GasTongRobResourceDB", "AddTempRobResTong", nil, parameter)  --临时信息添加数据库
	Obj.m_Flag = tongId
	--Gas2Gac:ShowRobResOnMiddleMap(Player.m_Conn, tongName, name)
	for charId,_ in pairs(PlayerTbl) do
		MsgToConnById(charId, 9468, PlayerCampStr,tongName)
		local Player = g_GetPlayerInfo(charId)
		if IsCppBound(Player) and IsCppBound(Player.m_Conn) then
			Gas2Gac:ShowCountDown(Player.m_Conn, tongName, 10 * 60)
		end
	end
	scene.m_objName = tongName
	scene.m_Time = RobTime
	local function RobResourceTick(Tick, scene)
		scene.m_Time = scene.m_Time - 1
		if scene.m_Time < 0 then
			UnRegisterTick(scene.m_TimeTick)
			scene.m_TimeTick = nil
			return
		end
	end
	scene.m_TimeTick = RegisterTick("RobResourceTick", RobResourceTick, 1000, scene)
	
	local function GetResourceTick(Tick,scene)
		if g_RobResMgr.m_EndRobResTick then
			UnRegisterTick(g_RobResMgr.m_EndRobResTick)
			g_RobResMgr.m_EndRobResTick = nil
		end
		if g_RobResMgr.m_BeginRobResTick then
			UnRegisterTick(g_RobResMgr.m_BeginRobResTick)
			g_RobResMgr.m_BeginRobResTick = nil
		end
		
		g_RobResMgr.m_CurrentType = "Winning"
		parameter["state"] = g_RobResMgr.m_CurrentType 
		parameter["flag"] = true
		Player.m_Scene.m_RobResObj = false
		local function CallBack(resTongName)
			CTongRobResMgr.GetRobResWinInfo()
			if PlayerTbl then
				for _, tempplayer in pairs(PlayerTbl) do
					if not resTongName then
						LogErr("占领资源点后出现错误","资源点名:"..name.."小队id:"..tongId)
					end
					if resTongName then
						Gas2Gac:ShowRobResZeroExitWnd(tempplayer.m_Conn, resTongName)
					end
				end	
			end
		end
		CallDbTrans("GasTongRobResourceDB", "AddRobResTong", CallBack, parameter, "addRobRes")
		UnRegisterTick(scene.m_ResourceTick)
		scene.m_ResourceTick = nil
	end
	if scene.m_ResourceTick then
		UnRegisterTick(scene.m_ResourceTick)
		scene.m_ResourceTick = nil
	end
	scene.m_ResourceTick = RegisterTick("GetResourceTick", GetResourceTick, 10 * 60 * 1000, scene)
end

local function CollectRobRes(Conn, IntObj,ObjName, GlobalID, ProTime)
	local Player = Conn.m_Player
	local tongID = Player.m_Properties:GetTongID()
	
	if Player.m_Scene.m_RobResObj == false or not g_RobResMgr.m_CurrentType or g_RobResMgr.m_CurrentType == "Signing" or g_RobResMgr.m_CurrentType == "Waiting" then
		MsgToConn(Conn, 9466)
		return
	end
	
	if tongID == 0 then
		MsgToConn(Conn, 9450)
		return 
	end
	
	if Player.m_CollIntObjID or Player.m_PickUpNpcID then
		MsgToConn(Conn, 3060)
		return
	end

	if IntObj.m_Openner and IntObj.m_Openner ~= Player.m_uID then
		MsgToConn(Conn, 9459)
		return
	end
	
	if IntObj.m_Flag and IntObj.m_Flag == Player.m_Properties:GetTongID() then
		MsgToConn(Conn, 9469)
		return 
	end
	
	IntObj.m_Openner = Player.m_uID
	local tongID = Player.m_Properties:GetTongID()
	IntObj.m_OpennerTong = tongID
	local data = {GlobalID}
	
	
	local function LoadResSucc(data)
		IntObj.m_Openner = nil
		IntObj.m_OpennerTong = nil
		if not IsCppBound(Player) then
			return
		end
		if not IsCppBound(IntObj) then
			return
		end
		
		if IsCppBound(Player) then
			GetRobResource(Player, GlobalID)
		end
	end
	
	local function LoadResFail(data)
		IntObj.m_Openner = nil
		IntObj.m_OpennerTong = nil
	end
	
	if not TongLoadProgress(Player, ProTime, LoadResSucc, LoadResFail, data) then
		IntObj.m_Openner = nil
		IntObj.m_OpennerTong = nil
		MsgToConn(Conn, 9463)
	end
end


--散落资源的收集方式
local g_GetPlayerInfo = g_GetPlayerInfo
local Entry = CreateSandBox(...)
function Entry.Exec(Conn, IntObj, ObjName, GlobalID)
	local ProTime = 0
	if not IsCppBound(Conn) or not IsCppBound(Conn.m_Player) then 
		return 
	end
	
	local ActionArg = g_ObjActionArgTbl[ObjName][1]
	if ActionArg ~= "" and ActionArg > 0 and ActionArg ~= nil and IsNumber(ActionArg) then
		ProTime = ActionArg * 1000
	end
	CollectRobRes(Conn, IntObj, ObjName, GlobalID, ProTime)
end 

return Entry


