cfg_load "tong/TongStartTime_Common"
cfg_load "tong/TongFirewoodExp_Common"

CTongNeedFireActivity = class()
local TongNeedFireActivityDB = "TongNeedFireActivityDB"

local NeedFireActivityStartTime = GetCfgTransformValue(false, "TongStartTime_Common", "佣兵小队篝火开始时间", "OpenTime") --篝火活动时间
local NeedFireActivityTimeLength = TongStartTime_Common("篝火活动时长", "OpenTime")  --篝火活动时长
local NeedFireActivityNpcTbl = {}
NeedFireAddExpTickTbl = {}
-----------------------------------------------------------------------
--@brief 判断是否在规定时间内
local function IsInOpenTime(timeTbl, length)
	local curDate = os.date("*t")
	local week = curDate.wday - 1
	for _, i in pairs(timeTbl.wday) do
		if week == i then
			local hour = curDate.hour
			local minute = curDate.min
			local second = curDate.sec
			local todayCurSecond = (hour * 60 + minute) * 60 + second 
			local offSet = GetOffsetSecond(timeTbl.time[1])
			local endOffSet = offSet + length * 60
			if todayCurSecond >= offSet and todayCurSecond <= endOffSet then
				return true, endOffSet - todayCurSecond
			end
		end
	end
	return false
end

--@brief 开启篝火活动
function CTongNeedFireActivity.OpenTongNeedFireActivity(Conn)
  local Player = Conn.m_Player
	local uTongID = Player.m_Properties:GetTongID()
	if uTongID == 0 then
		MsgToConn(Conn, 351000)
		return
	end
	if not IsInOpenTime(NeedFireActivityStartTime, NeedFireActivityTimeLength) then
		MsgToConn(Conn, 351001)
		return
	end
	
	local function CallBack(succ,msgId)
		if succ then
			local NpcId = NeedFireActivityNpcTbl[uTongID] 
			if not CheckAllNpcFunc(Player, NpcId, "篝火" ) then
				return
			end
			local npc = CCharacterDictator_GetCharacterByID(NpcId)
			npc:ServerDoNoRuleSkill("篝火特效",npc)
			npc:ServerDoNoRuleSkill("篝火光圈特效状态",npc)
			local memberSet = msgId
			if memberSet:GetRowNum() > 0 then
				for i = 1,memberSet:GetRowNum() do
					Gas2GacById:RetTongNeedFireActivityMsg(memberSet(i,1))
				end
			end
		end
		Gas2Gac:RetOpenTongNeedFireActivity(Conn,succ,msgId)
	end
	
  local data = {["uCharId"]	= Player.m_uID,["uServerId"] = g_CurServerId}
	CallAccountManualTrans(Conn.m_Account, TongNeedFireActivityDB, "OpenTongNeedFireActivityDB", CallBack, data, uTongID)
end

function CTongNeedFireActivity.CheckNeedFireIsOpen(Conn,NpcID)
	local Player = Conn.m_Player
	local uTongID = Player.m_Properties:GetTongID()
	if uTongID == 0 then
		MsgToConn(Conn, 351000)
		return
	end
	if not CheckAllNpcFunc(Player, NpcID, "篝火" ) then
		return
	end
	
	NeedFireActivityNpcTbl[uTongID] = NpcID
	
	if not IsInOpenTime(NeedFireActivityStartTime, NeedFireActivityTimeLength) then
		Gas2Gac:RetCheckNeedFireIsOpen(Conn,false,0)
		return
	end
	
	local function CallBack(succ,uLeftTime)
		Gas2Gac:RetCheckNeedFireIsOpen(Conn,succ,uLeftTime)
	end
	
  local data = {["uCharId"]	= Player.m_uID}
	CallAccountManualTrans(Conn.m_Account, TongNeedFireActivityDB, "CheckNeedFireIsOpenDB", CallBack, data, uTongID)
end
-------------------------------------------------------------------------
local function AddExpFun(tick,uCharId,uTongID,uFirewoodExp)
	if uTongID == 0 then
		UnRegisterTick(NeedFireAddExpTickTbl[uCharId][1])
		NeedFireAddExpTickTbl[uCharId][1] = nil
		NeedFireAddExpTickTbl[uCharId] = nil
		return
	end
	NeedFireAddExpTickTbl[uCharId][2] = NeedFireAddExpTickTbl[uCharId][2] - 1
	local NpcId =  NeedFireActivityNpcTbl[uTongID]
	local Npc = CCharacterDictator_GetCharacterByID(NpcId)
	if Npc == nil then
		UnRegisterTick(NeedFireAddExpTickTbl[uCharId][1])
		NeedFireAddExpTickTbl[uCharId][1] = nil
		NeedFireAddExpTickTbl[uCharId] = nil
		return
	end
	
	local memberAddExpTbl = {}
	local tblMembers = g_GasTongMgr:GetMembers(uTongID)
	for i = 1, #(tblMembers) do
		local Member = g_GetPlayerInfo(tblMembers[i])
		if Member then
			if g_GetDistance(Member,Npc) <= 12 then 
				table.insert(memberAddExpTbl,tblMembers[i])
			end
		end
	end
	
	if next(memberAddExpTbl) then
		local parameter = {["memberAddExpTbl"] = memberAddExpTbl,["addExp"] = uFirewoodExp}
		local function CallBack(memberLevelTbl)
			for memberId,memberInfo in pairs(memberLevelTbl) do
				local Member = g_GetPlayerInfo(memberId)
				CRoleQuest.AddPlayerExp_DB_Ret(Member,memberInfo)
			end
		end
		
		CallDbTrans(TongNeedFireActivityDB, "TongMemberAddExp", CallBack, parameter,unpack(memberAddExpTbl))
	end

	if NeedFireAddExpTickTbl[uCharId][2] == 0 then
		UnRegisterTick(NeedFireAddExpTickTbl[uCharId][1])
		NeedFireAddExpTickTbl[uCharId][1] = nil
		NeedFireAddExpTickTbl[uCharId] = nil
	end
end


--@brief 添加木柴
function CTongNeedFireActivity.AddNeedFireItem(Conn,uRoomIndex, uPos,sItemName,ColorStr)
	local Player = Conn.m_Player
	local uTongID = Player.m_Properties:GetTongID()
	if uTongID == 0 then
		MsgToConn(Conn, 351000)
		return
	end
	
	if not IsInOpenTime(NeedFireActivityStartTime, NeedFireActivityTimeLength) then
		MsgToConn(Conn, 351001)
		return
	end
	
	if not NeedFireActivityNpcTbl[uTongID] then
		return 
	end
	
	local firewoodInfo = TongFirewoodExp_Common(sItemName)
	if not firewoodInfo then
		MsgToConn(Conn,351003)
		return
	end
	local uFirewoodExp = TongFirewoodExp_Common(sItemName,"AddExp")
	local function CallBack(succ,memberSet)
		if succ then
			MsgToConn(Conn,351004)
			if memberSet:GetRowNum() > 0 then
				for i = 1,memberSet:GetRowNum() do
					Gas2GacById:RetAddFirewoodMsg(memberSet(i,1),Player.m_Properties:GetCharName(),sItemName,ColorStr)
				end
			end
			NeedFireAddExpTickTbl[Player.m_uID] = {}
			NeedFireAddExpTickTbl[Player.m_uID][1] = RegisterTick( "FirewoodCoolDownTick", AddExpFun, 60000,Player.m_uID,uTongID,uFirewoodExp)
			NeedFireAddExpTickTbl[Player.m_uID][2] = 3
		else
			if memberSet then
				MsgToConn(Conn,memberSet)
			else
				MsgToConn(Conn,351005)
			end
		end
		Gas2Gac:RetAddNeedFireItem(Conn,succ)
	end
	
  local data = {["uCharId"]	= Player.m_uID,["uRoomIndex"] = uRoomIndex,["uPos"] = uPos,["sItemName"] = sItemName,
  							["uFirewoodExp"] = uFirewoodExp}
	CallAccountManualTrans(Conn.m_Account, TongNeedFireActivityDB, "AddNeedFireItemDB", CallBack, data, uTongID)
end
--------------------------------------------------------------------
function Db2Gas:CleanTongNeedFireTick(data)
	for _,npcId in pairs(NeedFireActivityNpcTbl) do
		local npc = CCharacterDictator_GetCharacterByID(npcId)
		if npc then
			npc:ClearState("篝火特效状态")
			npc:ClearState("篝火光圈特效状态")
		end
	end
	NeedFireActivityNpcTbl = {}
	for k, v in pairs(NeedFireAddExpTickTbl) do
		if v[1] then
			UnRegisterTick(v[1])
			NeedFireAddExpTickTbl[k] = nil
		end
	end
end


function CTongNeedFireActivity.CleanTongNeedFireTick()
	local function CallBack()
		if not g_App:CallDbHalted() then
			local function CallBackFun(res)
				if res:GetRowNum() > 0 then
					for i = 1,res:GetRowNum() do
						Gas2GacById:RetNeedFireActivityEnd(res(i,1),351007)
					end
				end
			end
			CallDbTrans(TongNeedFireActivityDB,"CleanTongNeedFire", CallBackFun,{})
		end
	end
	g_AlarmClock:AddTask("CleanTongNeedFireTick", {wday = {1,2,3,4,5,6,7},time = {"22:00"}}, CallBack)
end


