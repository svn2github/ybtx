
gac_gas_require "item/store_room_cfg/StoreRoomCfg"
gas_require "world/npc/ServerNpc"
engine_require "common/Math/MathHelper"
gas_require "entrance/login/LoginServer"

local NpcCount={Count=0}

function GMCommand.Executor.AddCreature(Connection, ID, x, y)
	local npc = CreateServerCreature(Connection.m_Player, 1, CPos:new(x,y), Connection.m_Player:CppGetLevel())
end

function GMCommand.Executor.TestNpc( Connection, sNpcName, Num )
	local Player = Connection.m_Player
	local Scene = Player.m_Scene
	if Npc_Common(sNpcName) then
		local PlayerPos = CFPos:new()
		Player:GetPixelPos(PlayerPos)
		local TargetPos = CFPos:new()
		local Dir = CDir:new()
		local PosDir = CVector2f:new()
		local uAngle = Player:GetActionDir()
		Dir:SetDir(uAngle)
		Dir:Get(PosDir)
		TargetPos.x = PlayerPos.x + 2*EUnits.eGridSpanForObj
		TargetPos.y = PlayerPos.y + 2*EUnits.eGridSpanForObj
		local AIType = Npc_Common(sNpcName, "AIType")
		if AIType =="虚拟Npc支持巡逻" or AIType =="Boss小弟" or AIType == "Boss主动支持巡逻" or AIType =="Boss被动支持巡逻" then
			local str = sNpcName .. " 不允许通过GM指令创建！"
			return str
		end
		local otherData =	{
								["m_CreatorEntityID"]=Player:GetEntityID(),
								["m_OwnerId"]=Player.m_uID
							}
		if IsServantType(Npc_Common(sNpcName, "Type")) then
			return "Does not support servant npc!"
		else
			local tblNpcs = {}
			local time = GetProcessTime()
			for i = 1, Num do
				local Npc = g_NpcServerMgr:CreateServerNpc(sNpcName, 0, Scene, TargetPos, otherData)
				table.insert(tblNpcs, Npc)
			end
			print("create npc", GetProcessTime() - time)
			
			time = GetProcessTime()
			for j = 1, Num do
				tblNpcs[j]:SetOnDisappear(false)
			end
			print("Delete npc", GetProcessTime() - time)
		end
	else
		return "The NPC named this dosn`t exist!"
	end
end

function GMCommand.Executor.AddNpc( Connection, sNpcName)
	local Player = Connection.m_Player
	local Scene = Player.m_Scene
	if Npc_Common(sNpcName) then
		local PlayerPos = CFPos:new()
		Player:GetPixelPos(PlayerPos)
		local TargetPos = CFPos:new()
		local Dir = CDir:new()
		local PosDir = CVector2f:new()
		local uAngle = Player:GetActionDir()
		Dir:SetDir(uAngle)
		Dir:Get(PosDir)
		TargetPos.x = PlayerPos.x + 2*EUnits.eGridSpanForObj
		TargetPos.y = PlayerPos.y + 2*EUnits.eGridSpanForObj
		local AIType = Npc_Common(sNpcName, "AIType")
		if AIType =="虚拟Npc支持巡逻" or AIType =="Boss小弟" or AIType == "Boss主动支持巡逻" or AIType =="Boss被动支持巡逻" then
			local str = sNpcName .. " 不允许通过GM指令创建！"
			return str
		end
		local otherData =	{
								["m_CreatorEntityID"]=Player:GetEntityID(),
								["m_OwnerId"]=Player.m_uID
							}
		if IsServantType(Npc_Common(sNpcName, "Type")) then
			local Npc = g_NpcServerMgr:CreateServerNpc(sNpcName, 0, Scene, TargetPos, otherData, Player:GetEntityID())
		else
			local Npc = g_NpcServerMgr:CreateServerNpc(sNpcName, 0, Scene, TargetPos, otherData)
		end
	else
		return "The NPC named this dosn`t exist!"
	end
end

function  GMCommand.Executor.DelNpc( Connection)
	local Player = Connection.m_Player
	local Scene = Player.m_Scene
	local TargetNpc = Player:GetTarget()
	if TargetNpc then
		TargetNpc:SetOnDisappear(false)
	end
end


function GMCommand.Executor.reload_skill()
	if( ReloadFightSkill() ) then
		return "[successful][reload skill config]."
	else
		return "[fail][reload skill config]"
	end
end

function  GMCommand.Executor.skill( Connection, skill_name )	
	GMCommand:print( Connection, "----------------------------------------" )
	local play = Connection.m_Player
	
	function play:OnSkillBegin()
		GMCommand:print( Connection, "OnSkillBegin" )
	end
	
	function play:OnSkillEnd()
		GMCommand:print( Connection, "OnSkillEnd" )
	end
	
--	function play:OnCoolDownBegin()
--		GMCommand:print( Connection, "OnCoolDownBegin" )
--	end

	function play:OnSingBegin()
		GMCommand:print( Connection, "OnSingBegin" )
	end
	
	function play:OnSpellEOSP()
		GMCommand:print( Connection, "OnSpellEOSP" )
	end

	function play:OnEOSPEnd()
		GMCommand:print( Connection, "OnEOSPEnd" )
	end
	
	function play:OnFreezeBegin()
		GMCommand:print( Connection, "OnFreezeBegin" )
	end
	
	function play:OnFreezeEnd()
		GMCommand:print( Connection, "OnFreezeEnd" )
	end
	
	function play:OnInterrupt()
		GMCommand:print( Connection, "OnInterrupt" )
	end
	
	play:Attack( skill_name )

	return ""
end


------------------------------金钱功能--------------------
local function AddMoney(Connection,ret,nMoney)
	if ret then
	else
		Gas2Gac:ReturnAddMoneyError(Connection,nMoney)
	end
end

function  GMCommand.Executor.addmoney( Connection, nMoney )
	local data = {
					["playerId"] = Connection.m_Player.m_uID,
					["nMoney"] = nMoney
					}
	if nMoney > 20000000000000 then
		MsgToConn(Connection,33)
		return
	end
	local function callback(ret,uMsgID)
		if IsNumber(uMsgID) then
			MsgToConn(Connection,uMsgID)
		end
		AddMoney(Connection,ret,nMoney)
	end
	--local MoneyManagerDB =	(g_DBTransDef["MoneyMgrDB"])
	CallAccountAutoTrans(Connection.m_Account, "MoneyMgrDB", "PlayerAddMoney", callback, data)
	return ""
end

function  GMCommand.Executor.targetaddmoney( Connection,sTargetName, nMoney )
	local Player = g_GetPlayerInfoByName(sTargetName)
	if Player == nil then
		MsgToConn(Connection,19)
		return
	end
	if nMoney > 20000000000000 then
		MsgToConn(Connection,33)
		return
	end
	local data = {
					["playerId"] = Player.m_uID,
					["nMoney"] = nMoney
					}
	local function callback(ret,uMsgID)
		if IsNumber(uMsgID) then
			MsgToConn(Connection,uMsgID)
		end
		AddMoney(Player.m_Conn,ret,nMoney)
	end
	--local MoneyManagerDB =	(g_DBTransDef["MoneyMgrDB"])
	CallAccountAutoTrans(Connection.m_Account, "MoneyMgrDB", "PlayerAddMoney", callback, data)
	return ""
end
function  GMCommand.Executor.addticket( Connection, nTicket )
   	local function CallBack(result)
		if not result then
   			Gas2Gac:ReturnAddTicketError(Connection,nTicket)
			return
		end
		Gas2Gac:ReturnAddTicket(Connection,nTicket)
	end
	local parameter = {["user_id"] = Connection.m_Account:GetID(), ["ticket"] = nTicket}
	--local lGMDBExecutor = (g_DBTransDef["GMDB"])
	CallAccountAutoTrans(Connection.m_Account, "GMDB", "AddTicket", CallBack, parameter)
	return ""
end
function  GMCommand.Executor.targetaddticket( Connection,sTargetName, nTicket )
   	local Player = g_GetPlayerInfoByName(sTargetName)
		if Player == nil then
			MsgToConn(Connection,19)
			return
		end
   	
   	local function CallBack(result)
			if not result then
   			Gas2Gac:ReturnAddTicketError(Connection,nTicket)
				return
			end
			Gas2Gac:ReturnAddTicket(Player.m_Conn,nTicket)
	end
	local parameter = {["user_id"] = Connection.m_Account:GetID(), ["ticket"] = nTicket}
	--local lGMDBExecutor = (g_DBTransDef["GMDB"])
	CallAccountAutoTrans(Connection.m_Account, "GMDB", "AddTicket", CallBack, parameter)
	return ""
end
  	
function GMCommand.Executor.addbindingmoney(Connection,nBindingMoney)
   	local function CallBack(result)
		if not result then
   			Gas2Gac:ReturnAddBindingMoneyError(Connection,nBindingMoney)
			return
		end
	end
	local parameter = {["player_id"] = Connection.m_Player.m_uID, ["bindingmoney"] = nBindingMoney}
	--local lGMDBExecutor = (g_DBTransDef["GMDB"])
	CallAccountAutoTrans(Connection.m_Account, "GMDB", "AddBindingMoney", CallBack, parameter)
	return ""
end
function GMCommand.Executor.targetaddbindingmoney(Connection, sTargetName, nBindingMoney)
   	
  local Player = g_GetPlayerInfoByName(sTargetName)
	if Player == nil then
		MsgToConn(Connection,19)
		return
	end
   	
  local function CallBack(result)
		if not result then
   		Gas2Gac:ReturnAddBindingMoneyError(Connection,nBindingMoney)
			return
		end
	end
	local parameter = {["player_id"] = Player.m_uID, ["bindingmoney"] = nBindingMoney}
	CallAccountAutoTrans(Connection.m_Account, "GMDB", "AddBindingMoney", CallBack, parameter)
	return ""
end
function GMCommand.Executor.addbindingticket(Connection,nBindingTicket)
	local function CallBack(result)
		if not result then
   			Gas2Gac:ReturnAddBindingTicketError(Connection,nBindingTicket)
			return
		end
	end
	local parameter = {["player_id"] = Connection.m_Player.m_uID, ["ticket"] = nBindingTicket}
	--local lGMDBExecutor = (g_DBTransDef["GMDB"])
	CallAccountAutoTrans(Connection.m_Account, "GMDB", "AddBindingTicket", CallBack, parameter)
	return ""
end
function GMCommand.Executor.targetaddbindingticket(Connection, sTargetName,nBindingTicket)
	local Player = g_GetPlayerInfoByName(sTargetName)
	if Player == nil then
		MsgToConn(Connection,19)
		return
	end
	
	local function CallBack(result)
		if not result then
   		Gas2Gac:ReturnAddBindingTicketError(Connection,nBindingTicket)
			return
		end
	end
	local parameter = {["player_id"] = Player.m_uID, ["ticket"] = nBindingTicket}
	--local lGMDBExecutor = (g_DBTransDef["GMDB"])
	CallAccountAutoTrans(Connection.m_Account, "GMDB", "AddBindingTicket", CallBack, parameter)
	return ""
end
-------------------------------GM指令打开面板-------------------------
function GMCommand.Executor.OpenPanel( Connection, quickKey)
	Gas2Gac:ReturnOpenPanel(Connection, quickKey)
end
---------------------------- Player 技能相关 ---------------------

function GMCommand.Executor.AddSkillGM( Connection, name, level)
	local data = { ["name"] = name, ["level"] = level-1, ["kind"] = FightSkillKind.Skill, ["PlayerId"] = Connection.m_Player.m_uID ,
			["Class"] =  Connection.m_Player:CppGetClass(),["IsGMcmd"] = true, ["layer"] = 0,["PlayerLevel"] = Connection.m_Player:CppGetLevel(),["sceneName"] = Connection.m_Player.m_Scene.m_SceneName}
	local function CallBack(bool,Data)
		local Player = Connection.m_Player
		if bool then 
			Player:Lua_AddFightSkill(Data)
		else
			MsgToConn(Connection,Data)
			return
		end
	end	
	--local entry = (g_DBTransDef["FightSkillDB"])
	CallAccountAutoTrans(Connection.m_Account, "FightSkillDB", "Lua_AddFightSkill", CallBack, data)	
end

function GMCommand.Executor.TargetAddSkill( Connection,sTargetName, name, level)
	local Player = g_GetPlayerInfoByName(sTargetName)
	if Player == nil then
		--target不在线
		MsgToConn(Connection,19)
		return
	end
	local data = { ["name"] = name, ["level"] = level-1, ["kind"] = FightSkillKind.Skill, ["PlayerId"] = Player.m_uID ,
			["Class"] =  Player:CppGetClass(),["IsGMcmd"] = true, ["layer"] = 0,["PlayerLevel"] = Player:CppGetLevel(),["sceneName"] = Connection.m_Player.m_Scene.m_SceneName}
	local function CallBack(bool,Data)
		
		if bool then 
			if Player ~= nil then
				Player:Lua_AddFightSkill(Data)
			end
		else
			MsgToConn(Connection,Data)
			return
		end
	end	
	--local entry = (g_DBTransDef["FightSkillDB"])
	CallAccountAutoTrans(Connection.m_Account, "FightSkillDB", "Lua_AddFightSkill", CallBack, data)	
end

function GMCommand.Executor.ResetAllCoolDown( Connection )
	Connection.m_Player:ResetAllCoolDown()
end

function GMCommand.Executor.CalPhyPro()
	CTestHurtResult_CalPhysicsProbability()
end

function GMCommand.Executor.CalMagicPro()
	CTestHurtResult_CalMagicProbability()
end
---------------------------- Player fight skill effect 相关 ---------------------

function GMCommand.Executor.DisplayAllBuffer( Connection )
	GMCommand:print( Connection, "Gas all skill effect: " )

	local handler = CBuffInfo:new()
	function handler:Handle( attach_eff )
		GMCommand:print( Connection, "skill effect id: " ..  attach_eff:GetFSEffectID() )
	end
	
	Connection.m_Player:ForAllBuff( handler )
end


function GMCommand.Executor.GMGetMailList(Connection, PlayerID)
	local function CallBack(result)
		if not result then
			return
		end
		Gas2Gac:RetMailListBegin(Conn)   --返回的是普通邮件列表
		for i = 1, #result do
			Gas2Gac:RetMailList(Conn, unpack(result[i]))
		end
		Gas2Gac:RetMailListEnd(Conn)
	end
	
	local parameter = {}
	parameter["char_id"] = PlayerID
	--local lMailExecutor = (g_DBTransDef["MailDB"])
	CallAccountAutoTrans(Connection.m_Account, "MailDB", "GetMailList", CallBack, parameter)
end

--删除tbl_shortcut表中数据
function  GMCommand.Executor.deltblshortcut(Conn, player_id)
	local function CallBack(result)
	end
	--local lGMDBExecutor = (g_DBTransDef["GMDB"])
	local parameter = { ["player_id"] = Connection.m_Player.m_uID}
	CallAccountAutoTrans(Connection.m_Account, "GMDB", "DeleteShortCut", CallBack, parameter)
end

function  GMCommand.Executor.RunTheater(Conn, TheaterName)
	local Scene = Conn.m_Player.m_Scene
	Scene.m_TheaterMgr:RunTriggerTheater(TheaterName, Conn.m_Player:GetEntityID())
end

function GMCommand.Executor.DoFacial(Conn, FacialName)
--	local Player = Conn.m_Player
--	local PlayerId = Player:GetEntityID()
--	local Npc = Player:GetTarget()
--	local NpcId = Npc:GetEntityID()
--	local NpcName = Npc.m_Properties:GetCharName()
--	Gas2Gac:DoFacialAction(Conn, FacialName, NpcName, NpcId)
end

--function GMCommand.Executor.PrintTopMem(Conn, nTop)
--	PrintTopMem(nTop)
--end

function GMCommand.Executor.GacProfilerStart(Conn)
	Gas2Gac:OpenGacProfiler(Conn, true)
end

function GMCommand.Executor.GacProfilerStop(Conn)
	Gas2Gac:OpenGacProfiler(Conn, false)
end

function GMCommand.Executor.GasProfilerStart(Conn)
	profiler.Start()
end

function GMCommand.Executor.GasProfilerStop(Conn)
	profiler.Stop()
end

function GMCommand.Executor.StartCallBackProfiler()
	CScriptAppServer_StartCallBackProfiler()
end

function GMCommand.Executor.StopCallBackProfiler()
	CScriptAppServer_StopCallBackProfiler()
end

function GMCommand.Executor.GetGMCmdLogAll(Conn, days)
	if days > 7 or days < 0 then
		days = 7
	end

	local function Callback(result)
		local msg = ""
		for i = 1, result:GetRowNum() do
			msg = msg .. result(i,1) .. " " .. result(i,2) .. " " .. result(i,3) .. " " .. result(i,4) .. "\n"
		end
		GMCommand:print( Conn, msg )
	end
	
	local gmloger = "GMCmdLogger"
	local data = {
		['days'] = days,
	}
	CallDbTrans(gmloger, "SelectAll", Callback, data)
	return ""
end

function GMCommand.Executor.GetGMCmdLogByName(Conn, days, name)
	if days > 7 or days < 0 then
		days = 7
	end

	local function Callback(result)
		local msg = ""
		for i = 1, result:GetRowNum() do
			msg = msg .. result(i,1) .. " " .. result(i,2) .. " " .. result(i,3) .. " " .. result(i,4) .. "\n"
		end
		GMCommand:print( Conn, msg )
	end
	
	local gmloger = "GMCmdLogger"
	local data = {
		['days'] = days,
		['name'] = name,
	}
	CallDbTrans(gmloger, "SelectByName", Callback, data)
	return ""
end

function GMCommand.Executor.GetGMCmdLogByIP(Conn, days, ip)
	if days > 7 or days < 0 then
		days = 7
	end

	local function Callback(result)
		local msg = ""
		for i = 1, result:GetRowNum() do
			msg = msg .. result(i,1) .. " " .. result(i,2) .. " " .. result(i,3) .. " " .. result(i,4) .. "\n"
		end
		GMCommand:print( Conn, msg )
	end
	
	local gmloger = "GMCmdLogger"
	local data = {
		['days'] = days,
		['ip'] = ip,
	}
	CallDbTrans(gmloger, "SelectByIP", Callback, data)
	return ""
end



function  GMCommand.Executor.SetMaxOnLineUserNum( Connection, values )
   	local data = {
   								["newMaxOnLineUserNum"] = values ,
   								["user_name"] = Connection.m_UserName,
   								["game_id"] = Connection.m_nGameID}
 		
   	CallDbTrans("LoginQueueDB", "SetMaxOnLineUserNum", nil, data)
end

function  GMCommand.Executor.SetAllServerMaxUserNum( Connection, values )
   	local data = {
   								["newMaxOnLineUserNum"] = values ,
   								["user_name"] = Connection.m_UserName,
   								["game_id"] = Connection.m_nGameID}
 		
   	CallDbTrans("LoginQueueDB", "SetAllServerMaxUserNum", nil, data)
end

function  GMCommand.Executor.SetMoveSpeedCheckArg( Connection, uClientTimeStampErrorThreshold, fMaxStepCommandRate )
	--print("setmovespeedcheckarg", uClientTimeStampErrorThreshold, fMaxStepCommandRate)
	CCoreObjectMediator_SetMoveSpeedCheckArg(uClientTimeStampErrorThreshold, fMaxStepCommandRate)
end

function  GMCommand.Executor.HideMe( Connection )
	local data = {["user_name"] = Connection.m_UserName,["game_id"] = Connection.m_nGameID}
				
   	local function Callback(nGMLevel)
   		if nGMLevel then
   			Connection.m_Player:PlayerDoFightSkillWithoutLevel("GM隐身")
   		end
	end 
   	CallDbTrans("GMDB", "GetGmLevel", Callback, data)
end

function  GMCommand.Executor.ShowMe( Connection )
	local data = {["user_name"] = Connection.m_UserName,["game_id"] = Connection.m_nGameID}
				
   	local function Callback(nGMLevel)
   		if nGMLevel then
 	 			Connection.m_Player:PlayerDoFightSkillWithoutLevel("取消GM隐身")
   		end
	end 
   	CallDbTrans("GMDB", "GetGmLevel", Callback, data)
end

--Player 帮会相关
function GMCommand.Executor.AddTongProperty(Connection, uHonor, uExploit, uMoney, uResource)
	local Player = Connection.m_Player
	local uTongID = Player.m_Properties:GetTongID()
	
	local data = {}
	data["uHonor"] = uHonor
	data["uMoney"] = uMoney
	data["uResource"] = uResource
	data["uTongID"] = uTongID
	
	local function callback(ret)
		if ret then
		
		end
	end
	CallAccountAutoTrans(Connection.m_Account, "GasTongBasicDB", "AddTongProperty", CallBack, data, uTongID)
	return ""
end

function GMCommand.Executor.CleanLeaveTongTime(Connection)
	local Player = Connection.m_Player
	
	local data = {}
	data["nCharID"] = Player.m_uID
	local function callback()
		MsgToConn(Connection,9345)
	end
	CallAccountAutoTrans(Connection.m_Account, "GasTongBasicDB", "CleanLeaveTongTimeGM", callback, data)
	return ""
end

function GMCommand.Executor.AddTongExploit(Connection,uExploit)
	local Player = Connection.m_Player
	local data = {}
	data["nCharID"] = Player.m_uID
	data["uExploit"] = uExploit
	local function callback(succ, succType, tongMember,bNeedNotify)
		MsgToConn(Connection,9345)
		CTongBasic.TongTypeChanged(Connection,succ,succType,tongMember,bNeedNotify)
	end
	CallAccountAutoTrans(Connection.m_Account, "GasTongBasicDB", "UpdateTongExploit", callback, data)
	return ""
end

function GMCommand.Executor.SetAutoChangeCaptainsTime(Connection,uTime,k)
	local Player = Connection.m_Player
	local data = {}
	data["Time"] = uTime
	CTongBasic.AutoChangeTongCaptainTick(uTime,k)
	return ""
end

function GMCommand.Executor.ClearWarData(Connection)
	CallAccountAutoTrans(Connection.m_Account,"GasTongBasicDB","ClearDataInDataBaseDB",nil,{})
end

function GMCommand.Executor.addJfsPoint(Connection, Type, Point)
	local function CallBack(res)
		if res then
			GMCommand:print( Connection, Type.."  "..Point )
			GMCommand:print( Connection, "current integral:"..res )
		else
			GMCommand:print( Connection, "add faild! point small! or become negative!")
		end
	end
	if Type>=1 and Type <=3 then
		local data = {}
		data["Type"] = Type
		data["Point"] = Point
		data["charId"] = Connection.m_Player.m_uID
		CallAccountAutoTrans(Connection.m_Account, "GMDB", "AddJfsPoint", CallBack, data)
	else
		GMCommand:print( Connection, "type error!")
	end
end

g_JfsTeamNum = 3

function Gas2GasDef:SetJfsTeamNum(Conn, num)
	g_JfsTeamNum = num
end

function GMCommand.Executor.SetJfsTeamNum(Conn, num)
	for server_id in pairs(g_ServerList) do
		Gas2GasAutoCall:SetJfsTeamNum(GetServerAutoConn(server_id), num)
	end
end

function GMCommand.Executor.SetForageTime(Conn, str)
	g_WarZoneMgr.m_CurrentTransportState = str
	return "succeed"
end

function GMCommand.Executor.AddAfflatusValue(Conn, uAfflatusValue)
	if uAfflatusValue > 0 then
		Conn.m_Player:AddPlayerAfflatusValue(uAfflatusValue)
		return "succeed"
	else
		return "faild"
	end
end

function GMCommand.Executor.AddOffLineExpValue(Conn, uOffLineExpValue)
	if uOffLineExpValue > 0 then
		Conn.m_Player:AddOffLineExpValue(uOffLineExpValue)
		return "succeed"
	else
		return "faild"
	end
end

function GMCommand.Executor.SetWarTime(Conn, time, wday, length)
	return g_WarZoneMgr:SetAllServerWarTime(time, wday, length)
end

function GMCommand.Executor.StartWar(Conn, length)
	return g_WarZoneMgr:StartAllServerWar(length)
end

function GMCommand.Executor.StopWar(Conn)
	return g_WarZoneMgr:StopAllServerWar()
end

function GMCommand.Executor.SetFetchTimes(Conn, num)
	return CFetchRes.SetFetchTimes(Conn, num)
end

function GMCommand.Executor.StartRobSign(Conn)
	return CTongRobResMgr.StartRobSign(Conn)
end

function GMCommand.Executor.EndRobSign(Conn)
	return CTongRobResMgr.EndRobSign(Conn)
end


function GMCommand.Executor.StartRobRes(Conn)
	return CTongRobResMgr.StartRobRes(Conn)
end


function GMCommand.Executor.StopRobRes(Conn)
	return CTongRobResMgr.StopRobRes(Conn)
end

function GMCommand.Executor.AddTongDevelopDegree(Conn, uPoint)
	return CTongBasic.AddTongDevelopDegree(Conn, uPoint)
end

function GMCommand.Executor.CalculateTongDevelopDegree(Conn)
	return CTongBasic.ImmediatelyCalculateTongDevelopDegree()
end

function GMCommand.Executor.SendAwardToTongCaptain(Conn)
	return CTongBasic.ImmediatelySendMailToTongCaptain()
end

function GMCommand.Executor.SetRecruitMethod(Conn, method, camp1, camp2, camp3)
	Gas2GasAutoCall:SetRecruitMethod(GetServerAutoConn(1),method, camp1, camp2, camp3)
end


function GMCommand.Executor.SetRecruitInfo(Conn, minNum, maxNum, time)
	if minNum > maxNum then
		return
	end
	if time <= 2 then
		return
	end
	local data = {}
	data["minNum"] = minNum
	data["maxNum"] = maxNum
	data["time"] = time
	CallAccountAutoTrans(Conn.m_Account, "RecruitPlayerDB", "UpdateRecruitInfo", nil, data)
end


function GMCommand.Executor.UpdateSort(Conn)
	CSortMgr.CreateSortCharInfoFunc1()
	CSortMgr.CreateSortCharInfoFunc2()
	CSortMgr.CreateSortCharInfoFunc3()
	CSortMgr.CreateSortCharInfoFunc4()
	CSortMgr.CreateSortCharInfoFunc5(1)
	CSortMgr.CreateSortCharInfoFunc6(2)
	CSortMgr.CreateSortCharInfoFunc7(3)
	CSortMgr.CreateSortCharInfoFunc8(1)
	CSortMgr.CreateSortCharInfoFunc9(2)
	CSortMgr.CreateSortCharInfoFunc10(3)
	CSortMgr.CreateSortFunc5()
	CSortMgr.CreateSortFunc6(1)
	CSortMgr.CreateSortFunc7(2)
	CSortMgr.CreateSortFunc8(3)
	CGasFightingEvaluation.UpdateEquipEffectDB()
end


function  GMCommand.Executor.ClearMoneyExchange( Connection ,sTime)
	CallDbTrans("MoneyMgrDB","ClearWMXAndCessGM", nil,{})
end


function  GMCommand.Executor.SetTongTechLevel( Connection ,TechName,TechLevel)
	local function Callback(bFlag,MsgID)
		if MsgID then
			MsgToConn(Connection,MsgID)
		end
	end
	local Player = Connection.m_Player
	if not(Player and IsCppBound(Player)) then return end

	local uPlayerID = Player.m_uID
	local data = {}
	data["CharID"] = uPlayerID
	data["TechName"] = TechName
	data["TechLevel"] = TechLevel
	CallAccountAutoTrans(Connection.m_Account, "TongTeachDB", "SetTongTechLevelGM", Callback, data)
end

function  GMCommand.Executor.SetCharTechLevel( Connection ,TechName,TechLevel)
	local Player = Connection.m_Player
	if not(Player and IsCppBound(Player)) then return end
	local uPlayerID = Player.m_uID
	local data = {}
	data["CharID"] = uPlayerID
	data["TechName"] = TechName
	data["TechLevel"] = TechLevel
	local function Callback(bFlag,result)
		if bFlag == false  then
			MsgToConn(Connection,result)
	    else
            local curTechLevel = result["CurTechLevel"]
            local preTechLevel = result["PreTechLevel"] 
            local msgID = result["MsgID"]
            if preTechLevel > 0 then 
    		    local PreTalentName	= g_TongTechMgr:GetFightTechTalentName(TechName, preTechLevel)
			    local PreTanlentPoint	= g_TongTechMgr:GetFightTechTanlentPoint(TechName, preTechLevel)
			    Player:RemoveEquipSuiteEff(PreTalentName, PreTanlentPoint)
			end
			local TalentName	= g_TongTechMgr:GetFightTechTalentName(TechName, curTechLevel)
			local TanlentPoint	= g_TongTechMgr:GetFightTechTanlentPoint(TechName, curTechLevel)
			Player:AddEquipSuiteEff(TalentName, TanlentPoint)
			local totalSoul = result["totalSoul"]
			Gas2GacById:RetCharLearnedTech(uPlayerID, TechName,curTechLevel)
			MsgToConn(Conn, msgID) 
		end
	end

	CallAccountAutoTrans(Connection.m_Account, "TongTeachDB", "SetCharTechLevelGM", Callback, data)
end


function GMCommand.Executor.CloseNeedfireActivity(Conn)
	local function CallBackFun(res)
		if res:GetRowNum() > 0 then
			for i = 1,res:GetRowNum() do
				Gas2GacById:RetNeedFireActivityEnd(res(i,1),351007)
			end
		end
	end
	
	CallDbTrans("TongNeedFireActivityDB","CleanTongNeedFire", CallBackFun,{})
end

function GMCommand.Executor.AssignSoulAndExp(Conn)
	CGasCompenate.AssignUserSoulAndExpGM()
end

--@brief 给玩家发佣兵福利奖励gm指令
function GMCommand.Executor.SendVipMail(Conn)

	CallDbTrans("UseVIPItemDB","SendMailItemToVip", CallBackFun,{})
	local param1 = {["WelfareLevel"] = 1}
	CallDbTrans("UseVIPItemDB","SendWelfareMailItem", CallBackFun,param1)
	local param2 = {["WelfareLevel"] = 2}
	CallDbTrans("UseVIPItemDB","SendWelfareMailItem", CallBackFun,param2)
	local param3 = {["WelfareLevel"] = 3}
	CallDbTrans("UseVIPItemDB","SendWelfareMailItem", CallBackFun,param3)
	local param4 = {["WelfareLevel"] = 4}
	CallDbTrans("UseVIPItemDB","SendWelfareMailItem", CallBackFun,param4)
end
