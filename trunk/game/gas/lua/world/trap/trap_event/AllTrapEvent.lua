cfg_load "npc/Npc_Common"  --yy 09.05.31
cfg_load "int_obj/IntObj_Common"
cfg_load "fb_game/AreaFb_Common"
gas_require "log_mgr/CLogMgr"

CCommonTrap = class()
CNonCleanTrap = class()
CAddTemporarySkill = class()
CDoSkillOnPos = class()
CChangeBoxStateTrap = class()
CTransportTrap = class()
CDaTaoShaTransportTrap = class()
CDaDuoBaoTransportTrap = class()
CYbEducateActionTrap = class()
CYbEducateActionAddNum = class()
CInTrapCancelBuf = class()
CPlayerInTrapAddVar = class()
CNpcInExistTrapAddVar = class()
CNpcInPutTrapAddVar = class()
CPlayerInTrapAddRes = class()  
CNpcInExistTrapAddRes = class()
CNpcInPutTrapAddRes = class()
CDestroyMySelf = class()
CTriggerTheater = class()
CNpcInTrapDeleteNpc = class()
CPlayerInTrapAddNpcOrObj = class()
CInPutTrapAddVar = class()
CInPutTrapAddItem = class()
CSmallEctypeAddCount = class()
CCreateNpcOnPos = class()
CReplaceNpc = class()
CAddHp = class()
CNamedObjInPutTrapDelete = class()
CPlayerAddMoney = class()
CPlayerAddExper = class()  --		踩Trap给经验
CPlayerTransport = class()  --		玩家踩Trap传送到指定位置
CBugRunOut = class()
CNpcOutTrapDeleteNpc = class()
CInTrapSendMsg = class()
CMatchGameAddCount = class()
CAddDifferentCount = class()
CAddDifferentStateCount = class()
CNpcIntoTrapDeleted = class()
CRapRebornPoint = class()
CAddSameNpcCount = class()
CYbActionNpcAddNum = class()
CAreaFbTransportTrap = class()
CSpecilAreaTrap = class()
CChangeTongSceneByTrap = class()
CTakeBuffAddCount = class()

local function DoSkillToCharacter(pTrapHandler, pCharacter, TrapArg)
	if pCharacter == nil then
		return
	end
	--不能对OBJ施法
	if pCharacter.m_Properties:GetType() == ECharacterType.IntObj then
		return 
	end 

	local NpcCamp = pCharacter:CppGetCamp()
	if NpcCamp == ECamp.eCamp_Passerby then
		return
	end
	
	local SkLevel = TrapArg[2] 
	if SkLevel == nil or IsString(SkLevel) or SkLevel == 0 then
		SkLevel = 1
	end 
--	if pTrapHandler.m_Owner.m_OwnerId ~= nil then
--		local TrapOwner = g_GetPlayerInfo(pTrapHandler.m_Owner.m_OwnerId)
--		if TrapOwner then
--			if NpcCamp == TrapOwner:CppGetCamp() then
--				return
--			end
--			if IsCppBound( TrapOwner) then
--				TrapOwner:ServerDoNoRuleSkill(TrapArg[1], pCharacter)
--			end
--			return
--		end
--	end
	if IsCppBound(pTrapHandler.m_Owner) then
		pTrapHandler.m_Owner:DoIntObjSkill(TrapArg[1],SkLevel,pCharacter)
	end	
end

function CCommonTrap.OnStepInTrap(pTrapHandler, pCharacter, TrapArg)
	DoSkillToCharacter(pTrapHandler, pCharacter, TrapArg)
end
function CCommonTrap.OnStepOutTrap(pTrapHandler, pCharacter, TrapArg)
	if pCharacter.m_Properties:GetType() == ECharacterType.IntObj then
		return 
	end 
--	local Name = pCharacter.m_Properties:GetCharName()
--	print("清除",Name,"状态",TrapArg[1])
	if IsString(TrapArg[2]) then
		DoSkillToCharacter(pTrapHandler, pCharacter, {TrapArg[2]})
--	else
--		pCharacter:ClearState(TrapArg[1])
	end
end

function CAddHp.OnStepInTrap(pTrapHandler, pCharacter, TrapArg)
	if pCharacter.m_Properties:GetCharName() == TrapArg[2] then
		local Trap = pTrapHandler.m_Owner
		local FireNpc = Trap.m_FireNpc
		if not FireNpc then
			return
		end
		local fullHp = FireNpc:CppGetMaxHP()
		local curHp = FireNpc:CppGetHP()
		FireNpc:CppSetHP(fullHp * 0.05 + curHp)
	end
end

function CAddHp.OnStepOutTrap(pTrapHandler, pCharacter, TrapArg)
	
end

function CNonCleanTrap.OnStepInTrap(pTrapHandler, pCharacter, TrapArg)
--	print"CNonCleanTrap.OnStepInTrap"
	DoSkillToCharacter(pTrapHandler, pCharacter, TrapArg)
end

function CNonCleanTrap.OnStepOutTrap(pTrapHandler, pCharacter, TrapArg)
end

--加临时技能-------------
function CAddTemporarySkill.OnStepInTrap(pTrapHandler, pCharacter, TrapArg)
	if pCharacter == nil then
		return
	end
	--只能对玩家使用临时技能
	if pCharacter.m_Properties:GetType() == ECharacterType.Player then
		local BuffSkillName = TrapArg[1]
		if BuffSkillName and BuffSkillName ~= "" then
			pCharacter:AddTempSkill(BuffSkillName,1)
		end
	end
end

function CAddTemporarySkill.OnStepOutTrap(pTrapHandler, pCharacter, TrapArg)
end
-------------------------

--加地点技能-------------
function CDoSkillOnPos.OnStepInTrap(pTrapHandler, pCharacter, TrapArg)
	if pCharacter == nil then
		return
	end
	--只能对玩家使用临时技能
	if pCharacter.m_Properties:GetType() == ECharacterType.Player then
		local SkillName = TrapArg[1]
		if SkillName and SkillName ~= "" then
			local CenterPos = CFPos:new()
			pTrapHandler.m_Owner:GetPixelPos(CenterPos)
			pTrapHandler.m_Owner:DoPosIntObjSkill(SkillName, 1, CenterPos)
		end
	end
end

function CDoSkillOnPos.OnStepOutTrap(pTrapHandler, pCharacter, TrapArg)
end
---------------------------

--改变箱子状态
function CChangeBoxStateTrap.OnStepInTrap(pTrapHandler, pCharacter, TrapArg)

	if  TrapArg[1] == nil or TrapArg[1] == "" then
		return
	end
	local SceneId = pCharacter.m_Scene.m_SceneId
--	if pCharacter.m_Properties:GetType() == ECharacterType.IntObj then
--		local name = pCharacter.m_Properties:GetCharName()
--		db()
--	end
	local IntObj = GetOnlyIntObj(SceneId, TrapArg[1])
	if IntObj == nil then
		return
	end
--	print("进入重力开关(控制"..TrapArg[1]..")")
	if next(pTrapHandler.m_Owner.m_OnTrapCharIDTbl) then
		g_IntObjServerMgr:IntObjChangeState(IntObj, 1)
	else 
		g_IntObjServerMgr:IntObjChangeState(IntObj, 0)
	end
end
function CChangeBoxStateTrap.OnStepOutTrap(pTrapHandler, pCharacter, TrapArg)
	local Trap = pTrapHandler.m_Owner
	
	if table.maxn(Trap.m_OnTrapCharIDTbl) > 0 then
		return
	end
	
	if TrapArg[1] == nil or TrapArg[1] == "" then
		return
	end
	local SceneId = pCharacter.m_Scene.m_SceneId
	local IntObj = GetOnlyIntObj(SceneId, TrapArg[1])
	if IntObj == nil then
		return
	end
--	print("离开重力开关(控制"..TrapArg[1]..")")
	if next(Trap.m_OnTrapCharIDTbl) then
		g_IntObjServerMgr:IntObjChangeState(IntObj, 0)
	else 
		g_IntObjServerMgr:IntObjChangeState(IntObj, 1)
	end
end

local function Transport(pTrapHandler, pCharacter, TrapArg)
	local Type = pCharacter.m_Properties:GetType()
	if Type ~= ECharacterType.Player then
		return
	end
--	if pTrapHandler.m_Owner.m_Properties:GetCanUseState() == 0 then
----		print("传送点不可用")
--		return
--	end
	local TrapName = pTrapHandler.m_Owner.m_Properties:GetCharName()
	local SceneName = pCharacter.m_Scene.m_LogicSceneName 
	local PlayerId = pCharacter.m_uID
	if Transport_Server(TrapName) then
		for _, i in pairs(Transport_Server:GetKeys(TrapName, "陷阱")) do
			local TransInfo = Transport_Server(TrapName, "陷阱", i.."")
	 		if (TransInfo("BeginScene") == "" or SceneName == TransInfo("BeginScene"))
	 			and CheckPlayerLev(pCharacter:CppGetLevel(), TransInfo("LeastLev"), TransInfo("MaxLev")) 
				and (TransInfo("Camp") == 0 or TransInfo("Camp") == pCharacter:CppGetCamp()) then
				if Scene_Common[TransInfo("EndScene")] == nil then
					return
				end
				local Pos = CPos:new()
				Pos.x, Pos.y = TransInfo("PosX"), TransInfo("PosY")
	--			if p.QuestNeed == "" then
				if TransInfo("EndScene") == SceneName then
					pCharacter:SetGridPosByTransport(Pos)
					---下面这个函数主要是针对本地图内跨trap寻路
					Gas2Gac:PlayerTransportSetGrid(pCharacter.m_Conn)
				else
					local SaveX,SaveY = nil,nil
					if TransInfo("SavePosX") ~= "" and TransInfo("SavePosY") ~= "" then
						SaveX,SaveY = TransInfo("SavePosX"), TransInfo("SavePosY")
					end
					NewChangeScene(pCharacter.m_Conn, TransInfo("EndScene"), Pos.x , Pos.y, SaveX,SaveY)
				end
				break
			end	
		end
	end
end

--传送点
function CTransportTrap.OnStepInTrap(pTrapHandler, pCharacter, TrapArg)
	Transport(pTrapHandler, pCharacter, TrapArg)
end
function CTransportTrap.OnStepOutTrap(pTrapHandler, pCharacter, TrapArg)
end

-------------------------------------------------------
--大逃杀玩法 传送Trap
function CDaTaoShaTransportTrap.OnStepInTrap(pTrapHandler, pCharacter, TrapArg)
--	print("CDaTaoShaTransportTrap.OnStepInTrap")
	local Type = pCharacter.m_Properties:GetType()
	if Type ~= ECharacterType.Player then
		return
	end
	DaTaoShaTransport(pTrapHandler.m_Owner, pCharacter)
end

function CDaTaoShaTransportTrap.OnStepOutTrap(pTrapHandler, pCharacter, TrapArg)
end
------------------------------------------------------

--大夺宝玩法 传送Trap=======================
function CDaDuoBaoTransportTrap.OnStepInTrap(pTrapHandler, pCharacter, TrapArg)
	local Type = pCharacter.m_Properties:GetType()
	if Type ~= ECharacterType.Player then
		return
	end
	
	local MaxRoomNum = table.getn(g_DaTaoShaPointMgr)
	local SelRoomNum = math.random(1,MaxRoomNum)
	--print("SelRoomNum",SelRoomNum)
	if g_DaTaoShaPointMgr[SelRoomNum] then
		local Pos = CPos:new()
		Pos.x = g_DaTaoShaPointMgr[SelRoomNum]["IntoRoomPoint"][1]
		Pos.y = g_DaTaoShaPointMgr[SelRoomNum]["IntoRoomPoint"][2]
		pCharacter:SetGridPosByTransport( Pos )
	end
end

function CDaDuoBaoTransportTrap.OnStepOutTrap(pTrapHandler, pCharacter, TrapArg)
end
--===========================================

--佣兵训练传送========================
function CYbEducateActionTrap.OnStepInTrap(pTrapHandler, pCharacter, TrapArg)
	--print("                   ")
	--print("进入修行塔传送Trap")
	local Type = pCharacter.m_Properties:GetType()
	if Type ~= ECharacterType.Player then
		return
	end
	
	local Scene = pCharacter.m_Scene
	-- 检查下一层是不是奖励层
		Scene.m_MercenaryActionInfo.m_IsRewardFloor = CMercenaryEducateAct.IsNextRewardFloor(Scene)
	-- 计算下一层的房间号
	local SelRoomNum = CMercenaryEducateAct.GetNextRoomNum(pCharacter)	
	--print("选出房间 SelRoomNum = "..SelRoomNum)

	if g_MercenaryEduActMgr.m_RoomPoints[SelRoomNum] then
		--传送,调用弹出窗口并进行其它的加载处理(这个接口中有传送的坐标设置)
		CMercenaryRoomCreate.RandomLoadYbActionInfo(pCharacter, SelRoomNum)
		DestroyServerTrap(pTrapHandler.m_Owner, false)
	end
	CMercenaryRoomCreate.SetGameState(Scene, CMercenaryRoomCreate.GameState.BeforeGame)
	local GameID = Scene.m_MercenaryActionInfo.m_GameID
	CMercenaryRoomCreate.YbEduActAddOrResetSkill(pCharacter, GameID, true)
	--print("修行塔传送完毕")
end

function CYbEducateActionTrap.OnStepOutTrap(pTrapHandler, pCharacter, TrapArg)
end
--=====================================

--佣兵活动加计数
function CYbEducateActionAddNum.OnStepInTrap(pTrapHandler, pCharacter, TrapArg)
	local Player = nil 
	if pCharacter.m_OwnerId ~= nil then
		Player = g_GetPlayerInfo(pCharacter.m_OwnerId)
	else
		Player = pCharacter
	end
	if not IsCppBound(Player) then
		return
	end
	local TargetName = TrapArg[1]
	if not TargetName or TargetName == "" then
		return
	end
	
	CMercenaryRoomCreate.KillTargetAddNum(Player.m_Scene, TargetName)
end
function CYbEducateActionAddNum.OnStepOutTrap(pTrapHandler, pCharacter, TrapArg)
end

--进Trap取消buffer
function CInTrapCancelBuf.OnStepInTrap(pTrapHandler, pCharacter, TrapArg)
	if TrapArg[1] == "" or TrapArg[1] == nil or TrapArg[2] == "" or TrapArg[2] == nil then
		return
	end
	--不能对OBJ施法
	if pCharacter.m_Properties:GetType() == ECharacterType.IntObj then
		return 
	end 
	
	if Quest_Common(TrapArg[1], "触发技能") then
		local Buffname = Quest_Common(TrapArg[1], "触发技能", "Arg")
	end
	if Buffname == nil or Buffname == "" then
		return
	end
	if pCharacter:ExistState(Buffname) then
		pCharacter:ClearState(Buffname)
		if Quest_Common(TrapArg[1], "曾经达到状态需求") then
			local Keys = Quest_Common:GetKeys(TrapArg[1], "曾经达到状态需求")
			for k = 1, table.getn(Keys) do
				local tbl = GetCfgTransformValue(true, "Quest_Common", TrapArg[1], "曾经达到状态需求", Keys[k], "Function")
				pCharacter:ClearState(tbl[1])
			end
		end
		
		local RoleQuestDB = "RoleQuestDB"
		local parm = 
		{
			["iNum"] = TrapArg[3],
			["sQuestName"] = TrapArg[1],
			["sVarName"] = TrapArg[2],
			["char_id"] = pCharacter.m_uID
		}
		
		local function CallBack(isSuccess)
			if isSuccess and IsCppBound(pCharacter) then
				Gas2Gac:RetAddQuestVar(pCharacter.m_Conn, parm["sQuestName"], parm["sVarName"],parm["iNum"])
			end
		end
		local SceneMgrDB = "SceneMgrDB"
		CallAccountManualTrans(pCharacter.m_Conn.m_Account,RoleQuestDB,"AddQuestVarNum", CallBack, parm)	
	end
end
function CInTrapCancelBuf.OnStepOutTrap(pTrapHandler, pCharacter, TrapArg)
end

--++++++++++++++++++++++++++++begin+++++++++++++++++++++++++++++++++++++++++
local function InTrapAddQuestNum(pTrapHandler, pCharacter, TrapArg)
	if pCharacter.m_Properties:GetType() == ECharacterType.IntObj then
		return 
	end 
	local QuestName = TrapArg[1]
	local VarName = TrapArg[2]
	local AddNum = TrapArg[3]
--	if g_QuestNeedMgr[QuestName] == nil or g_QuestNeedMgr[QuestName][VarName] == nil then
--		return
--	end
	g_AddQuestVarNumForTeam(pCharacter, QuestName,VarName,AddNum)
end

--踩Trap，给玩家加任务记数
function CInPutTrapAddVar.OnStepInTrap(pTrapHandler, pCharacter, TrapArg)
	--Npc踩摆放Trap 玩家加任务计数
--	print("........................in")
	local PlayerId = pTrapHandler.m_Owner.m_OwnerId
	if PlayerId then
		local Player = g_GetPlayerInfo(PlayerId)
		if Player then
			InTrapAddQuestNum(pTrapHandler, Player, TrapArg)
		end
		return
	end
	-----------------------------------
	
	--Npc踩天然Trap 玩家加任务计数
	if pCharacter.m_OwnerId then
		local Player = g_GetPlayerInfo(pCharacter.m_OwnerId)
		if Player then
			InTrapAddQuestNum(pTrapHandler, Player, TrapArg)
		end
		return
	end
	------------------------------------
	
	--玩家踩Trap  玩家加任务计数
	InTrapAddQuestNum(pTrapHandler, Player, TrapArg)
	-------------------------------------
end

function CInPutTrapAddVar.OnStepOutTrap(pTrapHandler, pCharacter, TrapArg)
end
--+++++++++++++++++++++++++++++end+++++++++++++++++++++++++++++++++++++++++++

--Npc踩摆放Trap加任务计数 玩家加任务计数
function CNpcInPutTrapAddVar.OnStepInTrap(pTrapHandler, pCharacter, TrapArg)
	local PlayerId = pTrapHandler.m_Owner.m_OwnerId
	if PlayerId == nil then
		return
	end
	local Player = g_GetPlayerInfo(PlayerId)
	if not Player then
		return
	end
	CPlayerInTrapAddVar.OnStepInTrap(pTrapHandler, Player, TrapArg)
end

function CNpcInPutTrapAddVar.OnStepOutTrap(pTrapHandler, pCharacter, TrapArg)
end
--Npc踩天然Trap加任务计数 玩家加任务计数
function CNpcInExistTrapAddVar.OnStepInTrap(pTrapHandler, pCharacter, TrapArg)
	--local Player = pCharacter
	--pCharacter 为NPC
	if pCharacter.m_OwnerId == nil then
		return
	end
	local Player = g_GetPlayerInfo(pCharacter.m_OwnerId)
	if not Player then
		return
	end
	CPlayerInTrapAddVar.OnStepInTrap(pTrapHandler, Player, TrapArg)
end

function CNpcInExistTrapAddVar.OnStepOutTrap(pTrapHandler, pCharacter, TrapArg)

end

--玩家踩Trap加任务计数yy
-- pTrapHandler Trap  句柄
function CPlayerInTrapAddVar.OnStepInTrap(pTrapHandler, pCharacter, TrapArg)
	--Obj进入陷阱
	if pCharacter.m_Properties:GetType() == ECharacterType.IntObj then
		return 
	end 
	local QuestName = TrapArg[1]
	local VarName = TrapArg[2]
	local AddNum = TrapArg[3]
--	if g_QuestNeedMgr[QuestName] == nil or g_QuestNeedMgr[QuestName][VarName] == nil then
--		return
--	end
	g_AddQuestVarNumForTeam(pCharacter, QuestName,VarName,AddNum)
end

function CPlayerInTrapAddVar.OnStepOutTrap(pTrapHandler, pCharacter, TrapArg)
end


--++++++++++++++++++++++++++++begin+++++++++++++++++++++++++++++++++++++++++
local function InTrapAddItem(pTrapHandler, pCharacter, TrapArg)
	if pCharacter.m_Properties:GetType() == ECharacterType.IntObj then
		return 
	end 
	local PlayerID = pCharacter.m_uID;	--玩家ID]
	local TaskName = 			TrapArg[1]--任务名称	
	local ItemType = 			TrapArg[2]--物品类型
	local ItemName = 		  TrapArg[3]--物品名称
	local ItemNum = 			TrapArg[4]--物品数量
	local questneed				--玩家任务
	
	local parameter = {}
	parameter["sQuestName"] = TaskName
	parameter["char_id"]		= PlayerID
	parameter["nType"]		= ItemType
	parameter["sName"] 		= ItemName
	parameter["nCount"]		= ItemNum
	parameter["sceneName"]		= pCharacter.m_Scene.SceneName
	local Conn = pCharacter.m_Conn
	local function CallBack(result)
		if IsNumber(result) then
			if result == 3 then
				if IsCppBound(Conn) then
					MsgToConn(Conn,160000)
				end
			end
			return
		end
		CRoleQuest.add_item_DB_Ret_By_Id(PlayerID,ItemType,ItemName,ItemNum,result)
	end
	CallAccountManualTrans(Conn.m_Account, "RoleQuestDB","IntoTrapAddItem", CallBack, parameter)
end

--踩Trap，给玩家加物品
function CInPutTrapAddItem.OnStepInTrap(pTrapHandler, pCharacter, TrapArg)
	--Npc踩摆放Trap 玩家加物品
	local PlayerId = pTrapHandler.m_Owner.m_OwnerId
	if PlayerId then
		local Player = g_GetPlayerInfo(PlayerId)
		if Player then
			InTrapAddItem(pTrapHandler, Player, TrapArg)
		end
		return
	end
	-----------------------------------
	
	--Npc踩天然Trap 玩家加物品
	if pCharacter.m_OwnerId then
		local Player = g_GetPlayerInfo(pCharacter.m_OwnerId)
		if Player then
			InTrapAddItem(pTrapHandler, Player, TrapArg)
		end
		return
	end
	------------------------------------
	
	--玩家踩Trap  玩家加物品
	InTrapAddItem(pTrapHandler, Player, TrapArg)
	-------------------------------------
end

function CInPutTrapAddItem.OnStepOutTrap(pTrapHandler, pCharacter, TrapArg)
end
--+++++++++++++++++++++++++++++end+++++++++++++++++++++++++++++++++++++++++++

--Npc踩天然Trap 玩家加物品
function CNpcInExistTrapAddRes.OnStepInTrap(pTrapHandler, pCharacter, TrapArg)
	local PlayerId = pCharacter.m_OwnerId
	if PlayerId == nil then
		return
	end
	local Player = g_GetPlayerInfo(PlayerId)
	if not Player then
		return
	end
	CPlayerInTrapAddRes.OnStepInTrap(pTrapHandler, Player, TrapArg)
end

function CNpcInExistTrapAddRes.OnStepOutTrap(pTrapHandler, pCharacter, TrapArg)
end

--Npc踩摆放Trap 玩家加物品
function CNpcInPutTrapAddRes.OnStepInTrap(pTrapHandler, pCharacter, TrapArg)
	--local PlayerId = pTrapHandler.m_Owner.m_OwnerId or pCharacter.m_OwnerId
	local PlayerId = pTrapHandler.m_Owner.m_OwnerId
	if PlayerId == nil then
		return
	end
	local Player = g_GetPlayerInfo(PlayerId)
	if not Player then
		return
	end
	CPlayerInTrapAddRes.OnStepInTrap(pTrapHandler, Player, TrapArg)
end
function CNpcInPutTrapAddRes.OnStepOutTrap(pTrapHandler, pCharacter, TrapArg)
end



--玩家踩Trap 玩家背包中添加物品 yy
-- pTrapHandler Trap ID
-- pCharacter 玩家信息
function CPlayerInTrapAddRes.OnStepInTrap(pTrapHandler, pCharacter, TrapArg)
	if pCharacter.m_Properties:GetType() == ECharacterType.IntObj then
		return 
	end 
	local PlayerID = pCharacter.m_uID;	--玩家ID]
	local TaskName = 			TrapArg[1]--任务名称	
	local ItemType = 			TrapArg[2]--物品类型
	local ItemName = 		  TrapArg[3]--物品名称
	local ItemNum = 			TrapArg[4]--物品数量
	local questneed				--玩家任务
	
--	print "TaskName";
--	print (TaskName)
	
	local parameter = {}
	parameter["sQuestName"] = TaskName
	parameter["char_id"]		= PlayerID
	parameter["nType"]		= ItemType
	parameter["sName"] 		= ItemName
	parameter["nCount"]		= ItemNum
	parameter["sceneName"]		= pCharacter.m_Scene.m_SceneName
	local Conn = pCharacter.m_Conn
	local function CallBack(result)
		if IsNumber(result) then
			if result == 3 then
				if IsCppBound(Conn) then
					MsgToConn(Conn,160000)
				end
			end
			return
		end
		CRoleQuest.add_item_DB_Ret_By_Id(PlayerID,ItemType,ItemName,ItemNum,result)
	end
	if IsCppBound(Conn) and Conn.m_Account then
		CallAccountAutoTrans(Conn.m_Account, "RoleQuestDB","IntoTrapAddItem", CallBack, parameter)
	end
end

function CPlayerInTrapAddRes.OnStepOutTrap(pTrapHandler, pCharacter, TrapArg)

end
--触发剧场
function CTriggerTheater.OnStepInTrap(pTrapHandler, pCharacter, TrapArg)
	local ObjectType = pCharacter.m_Properties:GetType()
	local SceneId = pCharacter.m_Scene.m_SceneId
	local TheaterMgr = pCharacter.m_Scene.m_TheaterMgr
	local EntityID = pCharacter:GetEntityID()
	if TrapArg[1] == "" then
		TheaterMgr:RunTriggerTheater(TrapArg[2], nil)
	elseif ObjectType == ECharacterType.Npc then
		TheaterMgr:RunTriggerTheater(TrapArg[2], nil)
	elseif ObjectType == ECharacterType.Player then
		if CRoleQuest.DoingQuest_Check(pCharacter, TrapArg[1]) then
			TheaterMgr:RunTriggerTheater(TrapArg[2], EntityID)
		end
	end
end

function CTriggerTheater.OnStepOutTrap(pTrapHandler, pCharacter, TrapArg)

end
--陷阱自删除
function CDestroyMySelf.OnStepInTrap(pTrapHandler, pCharacter, TrapArg)
	
	DestroyServerTrap(pTrapHandler.m_Owner, true)
end 

function CDestroyMySelf.OnStepOutTrap(pTrapHandler, pCharacter, TrapArg)

end

function CNpcInTrapDeleteNpc.OnStepInTrap(pTrapHandler, pCharacter, TrapArg)
	if pCharacter.m_Properties:GetType() == ECharacterType.IntObj then
		return 
	end
	pCharacter:SetOnDisappear(true) 
end 

function CNpcInTrapDeleteNpc.OnStepOutTrap(pTrapHandler, pCharacter, TrapArg)

end

function CNpcIntoTrapDeleted.OnStepInTrap(pTrapHandler, pCharacter, TrapArg)
	if pCharacter.m_Properties:GetType() == ECharacterType.IntObj then
		return 
	end
	if pCharacter.m_TgtSentryIndex then
		if pTrapHandler.m_Owner.m_SentryIndex == pCharacter.m_TgtSentryIndex and
			 pTrapHandler.m_Owner.m_InBattleNum == 0 then
			pCharacter:SetOnDisappear(false)
		end
	end
end

function CNpcIntoTrapDeleted.OnStepOutTrap(pTrapHandler, pCharacter, TrapArg)

end

function CRapRebornPoint.OnStepInTrap(pTrapHandler, pCharacter, TrapArg)
	local timeLength = 60 * 1000
	
	local function ProgressSucceed()
		pTrapHandler.m_Occupyer = pCharacter.m_Properties:GetTongID()
	end
	
	local function ProgressFail()
		
	end
	TongLoadProgress(pCharacter, timeLength, ProgressSucceed, ProgressFail,{}) 
	
end

function CRapRebornPoint.OnStepOutTrap(pTrapHandler, pCharacter, TrapArg)

end


function CNamedObjInPutTrapDelete.OnStepInTrap(pTrapHandler, pCharacter, TrapArg)
	if pCharacter.m_Properties:GetCharName() == TrapArg[2] then
		pCharacter:SetOnDisappear(true)
	end
end 

function CNamedObjInPutTrapDelete.OnStepOutTrap(pTrapHandler, pCharacter, TrapArg)

end

-- yy 09.6.1 得到范围内的点.
local function GetRandomPos( CenterPos, max, min)
	if max == min == 0 then
		return CenterPos
	end
	local pos = CPos:new()
	assert(min <= max, "获取随机位置, 参数 min 必须小于等于 max")
	local radius = min + math.random()*(max - min)
	local angle = 2 * math.pi * math.random()
	pos.x = CenterPos.x + radius * math.sin(angle)
	pos.y = CenterPos.y + radius * math.cos(angle)
	return pos
end

-- yy  Player 进入Trap ADD Npc Or Obj
-- Npc, name , number , 范围  中心为trap中心
function CPlayerInTrapAddNpcOrObj.OnStepInTrap(pTrapHandler, pCharacter, TrapArg)

	--print("CPlayerInTrapAddNpcOrObj")
	if pCharacter.m_Properties:GetType() == ECharacterType.IntObj then
		return 
	end
	local scene = pTrapHandler.m_Owner.m_Scene
	local CenterPos = CPos:new()
	pTrapHandler.m_Owner:GetGridPos(CenterPos)
	local min = 0
	local max = TrapArg[4]
	if max == nil then
--		local error_str = "max 为 nil"
		--error( error_str)
--		print( error_str)
		max = 0
	end
	local Num = TrapArg[3]
	local Name = TrapArg[2]
	if TrapArg[1] == "Npc" then
		local NpcInfoTb = Npc_Common(Name)
		if NpcInfoTb == nil then
--			print ("Npc_Common 表中没有名为"..Name.."Npc")
			return
		end
		local NpcLevel = NpcInfoTb("MaxLevel")
		for i = 1, Num do 
			local pos = GetRandomPos( CenterPos, max, min)
			local fPos = CFPos:new( pos.x * EUnits.eGridSpan, pos.y * EUnits.eGridSpan )
			local otherData = {["m_OwnerId"]=pCharacter.m_uID}
			local CreNpc = g_NpcServerMgr:CreateServerNpc( Name, NpcLevel, scene, fPos, otherData)
		end
	elseif TrapArg[1] == "Obj" then
		local ObjInfoTb = IntObj_Common(Name)
		if ObjInfoTb == nil then
--			print ("IntObj_Common 表中没有名为"..Name.."Obj")
			return
		end
		for i = 1, Num do 
			local pos = GetRandomPos( CenterPos, max, min)
			CreateIntObj(scene, pos, Name, true, nil, nil)
		end
	end
end

function CPlayerInTrapAddNpcOrObj.OnStepOutTrap(pTrapHandler, pCharacter, TrapArg)

end 

--小副本加计数
function CSmallEctypeAddCount.OnStepInTrap( pTrapHandler, pCharacter, TrapArg)

	local Player = nil 
	if pCharacter.m_OwnerId ~= nil then
		Player = g_GetPlayerInfo(pCharacter.m_OwnerId)
	else
		Player = pCharacter
	end
	if not Player or not IsCppBound(Player) then
		return
	end
	
	if not Player.m_MatchGameCount then
		Player.m_MatchGameCount = 0
	end
	
	g_MatchGameMgr:AddMatchGameCount(Player, 2, pTrapHandler.m_Owner.m_Properties:GetCharName())
end

function CSmallEctypeAddCount.OnStepOutTrap( pTrapHandler, pCharacter, TrapArg)
end

--小副本加计数
function CMatchGameAddCount.OnStepInTrap( pTrapHandler, pCharacter, TrapArg)
	local scene = pCharacter.m_Scene
	g_MatchGameMgr:AllTeamAddCount(scene, 2, pTrapHandler.m_Owner.m_Properties:GetCharName())
end

function CMatchGameAddCount.OnStepOutTrap( pTrapHandler, pCharacter, TrapArg)
	
end

function CInTrapSendMsg.OnStepInTrap( pTrapHandler, pCharacter, TrapArg)
	local scene = pTrapHandler.m_Owner.m_Scene
	local Player = nil 
	if pCharacter.m_OwnerId ~= nil then
		Player = g_GetPlayerInfo(pCharacter.m_OwnerId)
	else
		Player = pCharacter
	end
	if not Player or not IsCppBound(Player) then
		return
	end
	
	local playername = Player.m_Properties:GetCharName()
	MsgToConn(scene.m_CoreScene, TrapArg[1], playername)
end

function CInTrapSendMsg.OnStepOutTrap( pTrapHandler, pCharacter, TrapArg)
end

--定点刷NPC
function CCreateNpcOnPos.OnStepInTrap( pTrapHandler, pCharacter, TrapArg)
--	local CreatType  = TrapArg[1]
--	local CreatName  = TrapArg[2]
--	local CreatScene = pCharacter.m_Scene
--	local pos = GetCreatePos(pTrapHandler.m_Owner)
--	if TrapArg[5] == "相对坐标" then
--		pos.x = pos.x + TrapArg[3]
--		pos.y = pos.y + TrapArg[4]
--	else
--		local ID = TrapArg[3]
--		pos.x, pos.y, scene = GetScenePosition(ID, CreatScene)
--		if scene ~= CreatScene.m_SceneName then
--			CfgLogErr("Trap配置表脚本填写错误！",pTrapHandler.m_Owner.m_Properties:GetCharName().."(指定刷新)脚本中填写的坐标ID("..ID..")在坐标表中填写的地图(".. scene ..")与实际地图(".. CreatScene.m_SceneName ..")不符！")
--			return
--		end
--	end
--	CreateOnPos(CreatType, CreatName, CreatScene, pos, pCharacter:GetEntityID())
	local ArgTbl = TrapArg
	ArgTbl["Scene"] = pCharacter.m_Scene
	ArgTbl["Pos"] = GetCreatePos(pTrapHandler.m_Owner)
	ArgTbl["CreatorEntityID"] = pCharacter:GetEntityID()
	g_TriggerScript:RunScript("创建", {ArgTbl}, pTrapHandler.m_Owner, pCharacter)
end

function CCreateNpcOnPos.OnStepOutTrap( pTrapHandler, pCharacter, TrapArg)
end

--替换
function CReplaceNpc.OnStepInTrap(pTrapHandler, pCharacter, TrapArg)
	if pCharacter.m_Properties:GetType() == ECharacterType.Npc then
		local CreatName  = TrapArg[1]
		local Level = g_NpcBornMgr:GetNpcBornLevel(CreatName)
		local fPos = CFPos:new() 
		pCharacter:GetPixelPos(fPos)
		local dir = pCharacter:GetActionDir()
		local Npc = g_NpcServerMgr:CreateServerNpc(CreatName,Level,pCharacter.m_Scene,fPos)
		if not IsServerObjValid(Npc) then
			return
		end
		if TrapArg[2] and TrapArg[2] == "消失不渐隐" then
			pCharacter:SetNoDissolve()
		end
		pCharacter:SetOnDisappear(true)
		Npc:SetAndSyncActionDir(dir)
	end 	
end 

function CReplaceNpc.OnStepOutTrap(pTrapHandler, pCharacter, TrapArg)

end

function CPlayerAddExper.OnStepInTrap(pTrapHandler, pCharacter, TrapArg)
	if not IsCppBound(pCharacter) then
		return
	end
	if pCharacter.m_Properties:GetType() == ECharacterType.Player then
		local AddExpFun = g_TriggerMgr:GetExpressions(TrapArg[1])
		local addExp = AddExpFun(pCharacter:CppGetLevel())
		local function callback(CurLevel,LevelExp)
			if IsCppBound(pCharacter) and CurLevel then
				if CurLevel then
					local AddExpTbl = {}
					AddExpTbl["Level"] = CurLevel
					AddExpTbl["Exp"] = LevelExp
					AddExpTbl["AddExp"] = addExp
					AddExpTbl["uInspirationExp"] = 0
					CRoleQuest.AddPlayerExp_DB_Ret(pCharacter, AddExpTbl)
				end
			end
		end
		local data = {}
		data["char_id"] = pCharacter.m_uID
		data["addExp"] = addExp
		data["addExpType"] = event_type_tbl["玩家踩Trap给经验"]
		
		OnSavePlayerExpFunc({pCharacter})
		CallAccountManualTrans(pCharacter.m_Conn.m_Account, "RoleQuestDB", "AddExp", callback, data)
	end
end

function CPlayerAddExper.OnStepOutTrap(pTrapHandler, pCharacter, TrapArg)
end

function CPlayerTransport.OnStepInTrap(pTrapHandler, pCharacter, TrapArg)
	if not IsCppBound(pCharacter) then
		return
	end
	if pCharacter.m_Properties:GetType() == ECharacterType.Player then
		local npc = pCharacter.m_ReplaceModel
		local NpcPos = CPos:new()
		npc:GetGridPos(NpcPos)
		pCharacter:SetGridPosByTransport(NpcPos)
		if IsCppBound(npc) then
			g_NpcServerMgr:DestroyServerNpcNow(npc, false)
			npc = nil
		end
	end
end

function CPlayerTransport.OnStepOutTrap(pTrapHandler, pCharacter, TrapArg)
end

function CPlayerAddMoney.OnStepInTrap(pTrapHandler, pCharacter, TrapArg)
	if not IsCppBound(pCharacter) then
		return
	end
	if pCharacter.m_Properties:GetType() == ECharacterType.Player then
		local AddMoneyFun = g_TriggerMgr:GetExpressions(TrapArg[1])
		local nMoney = AddMoneyFun(pCharacter:CppGetLevel())
		local type = 2
		if TrapArg[2] == "非绑定" then
			type = 1
		end
		local function callback(result,uMsgID)
			if IsNumber(uMsgID) then
				MsgToConn(pCharacter.m_Conn,uMsgID)
			end
			if IsCppBound(pCharacter) then
				if result then
					MsgToConn(pCharacter.m_Conn, 3602, nMoney)
				end
			end
		end
		local data = {}
		data["char_id"] = pCharacter.m_uID
		data["money_count"] = nMoney
		data["addType"]	= event_type_tbl["玩家踩trap加钱"]
		data["money_type"] = type
		CallAccountAutoTrans(pCharacter.m_Conn.m_Account, "MoneyMgrDB", "AddMoneyForRpc", callback, data)
	end
end

function CPlayerAddMoney.OnStepOutTrap(pTrapHandler, pCharacter, TrapArg)

end

-- 虫子跑出去
function CBugRunOut.OnStepOutTrap(pTrapHandler, pCharacter, TrapArg)
	if not IsCppBound(pCharacter) then
		return
	end
	local scene = pCharacter.m_Scene
	g_MatchGameMgr:AllTeamAddCount(scene, 2, pTrapHandler.m_Owner.m_Properties:GetCharName())
end

function CBugRunOut.OnStepInTrap(pTrapHandler, pCharacter, TrapArg)

end

function CNpcOutTrapDeleteNpc.OnStepInTrap(pTrapHandler, pCharacter, TrapArg)

end 

function CNpcOutTrapDeleteNpc.OnStepOutTrap(pTrapHandler, pCharacter, TrapArg)
	if pCharacter.m_Properties:GetType() == ECharacterType.IntObj then
		return 
	end
	pCharacter:SetOnDisappear(true) 
end
-- 不同Npc加不同分
function CAddDifferentCount.OnStepInTrap(pTrapHandler, pCharacter, TrapArg)
	if not IsCppBound(pCharacter) then
		return
	end
	local Player = nil 
	local Score = nil
	if pCharacter.m_OwnerId ~= nil then
		Player = g_GetPlayerInfo(pCharacter.m_OwnerId)
	else
		Player = pCharacter
	end
	if not IsCppBound(Player) then
		return
	end
	
	for i=1, #(TrapArg) do
		NpcName = TrapArg[i][1]
		if NpcName == pCharacter.m_Name then
			Score = TrapArg[i][2]
			CMercenaryRoomCreate.KillTargetAddNum(Player.m_Scene, NpcName, Score)
			break
		end
	end
end

function CAddDifferentCount.OnStepOutTrap(pTrapHandler, pCharacter, TrapArg)

end

-- 不同 状态的Npc加不同分
function CAddDifferentStateCount.OnStepInTrap(pTrapHandler, pCharacter, TrapArg)
	if not IsCppBound(pCharacter) then
		return
	end
	local Player = nil 
	local Score = nil
	if pCharacter.m_OwnerId ~= nil then
		Player = g_GetPlayerInfo(pCharacter.m_OwnerId)
	else
		Player = pCharacter
	end
	if not IsCppBound(Player) then
		return
	end
	
	for i=1, #(TrapArg) do
		NpcState = TrapArg[i][1]
		if pCharacter:ExistState(NpcState)  then
			Score = TrapArg[i][2]
			CMercenaryRoomCreate.KillTargetAddNum(Player.m_Scene, pCharacter.m_Name, Score)
			break
		end
	end
end

function CAddDifferentStateCount.OnStepOutTrap(pTrapHandler, pCharacter, TrapArg)
	
end

-- 2个相同NPC放进TRAP就加分，并让NPC消失  
function CAddSameNpcCount.OnStepInTrap(pTrapHandler, pCharacter, TrapArg)
	if not IsCppBound(pCharacter) then
		return
	end
	
	-- 计数
	if not IsCppBound(pTrapHandler.FirstNpc) then
		if pCharacter.m_CreateAnotherPigTick then
			UnRegisterTick(pCharacter.m_CreateAnotherPigTick)
			pCharacter.m_CreateAnotherPigTick = nil
		end
		pTrapHandler.NpcCount = 1
		pTrapHandler.FirstNpc	= pCharacter
		pTrapHandler.ExistNpcName = pCharacter.m_Name -- 记下NPC名字
	else
		-- 放正确 不重生
		if pCharacter.m_Name == pTrapHandler.ExistNpcName
				and pTrapHandler.NpcCount == 1 then				-- 成功后pTrapHandler.NpcCount设为负数
					
			pTrapHandler.NpcCount = pTrapHandler.NpcCount + 1
				-- 取消Tick不在原处创建NPC
			if pCharacter.m_CreateAnotherPigTick then
				UnRegisterTick(pCharacter.m_CreateAnotherPigTick)
				pCharacter.m_CreateAnotherPigTick = nil
			end
			
			if pTrapHandler.FirstNpc.m_CreateAnotherPigTick then
				UnRegisterTick(pTrapHandler.FirstNpc.m_CreateAnotherPigTick)
				pTrapHandler.FirstNpc.m_CreateAnotherPigTick = nil
			end	
			
			pCharacter.m_IsNotEmbrace = true
			pTrapHandler.FirstNpc.m_IsNotEmbrace = true
			
			-- 特效
			local ModelString = "fx/setting/other/other/fuwen_green/explode.efx"
			local StateString = "fuwen_green/explode"
			local PlayerID = pCharacter.m_OwnerId
			local Player = g_GetPlayerInfo(PlayerID)
			local NpcID = pCharacter:GetEntityID()
			local FirstNpcID = pTrapHandler.FirstNpc:GetEntityID()
			if IsCppBound(Player) then
				Gas2Gac:UseItemTargetEffect(Player.m_Conn, ModelString,StateString,NpcID)
				Gas2Gac:UseItemTargetEffect(Player.m_Conn, ModelString,StateString,FirstNpcID)
			end
			--Gas2Gac:UseItemTargetEffect(Player:GetIS(0), ModelString, StateString, Player:GetEntityID())
			
			local function DeleteTwoNpc()
				-- 抱进来的猪消失
				pCharacter:SetNoDissolve()
				g_NpcServerMgr:DestroyServerNpcNow(pCharacter, false)
				-- 第一个猪消失
				if IsCppBound(pTrapHandler.FirstNpc) then
					pTrapHandler.FirstNpc:SetNoDissolve()
					g_NpcServerMgr:DestroyServerNpcNow(pTrapHandler.FirstNpc, false)
				end		
			end
			
			RegisterOnceTick(pCharacter, "DeleteTwoNpcTick", DeleteTwoNpc, 1000)
		end
	end
	
	-- 如果够了2个
	if pTrapHandler.NpcCount == 2 then
		-- 加分
		local TargetName = TrapArg[1]
		if not TargetName or TargetName == "" then
			return
		end
		local Player = g_GetPlayerInfo(pCharacter.m_OwnerId)
		if not IsCppBound(Player) then
			return		
		end
		CMercenaryRoomCreate.KillTargetAddNum(Player.m_Scene, TargetName)	
	end
end

function CAddSameNpcCount.OnStepOutTrap(pTrapHandler, pCharacter, TrapArg)

end

function CYbActionNpcAddNum.OnStepInTrap(pTrapHandler, pCharacter, TrapArg)
	if not IsCppBound(pCharacter) then
		return
	end
		-- 加分
	local TargetName = TrapArg[1]
	if not TargetName or TargetName == "" then
		return
	end
	CMercenaryRoomCreate.KillTargetAddNum(pCharacter.m_Scene, TargetName)
end

function CYbActionNpcAddNum.OnStepOutTrap(pTrapHandler, pCharacter, TrapArg)

end

function CAreaFbTransportTrap.OnStepInTrap(pTrapHandler, pCharacter, TrapArg)
	local TrapName = pTrapHandler.m_Owner.m_Properties:GetCharName()
	
	if not IsCppBound(pCharacter) then
		return
	end
	
	local SceneName = pCharacter.m_Scene.m_SceneName 
	local EndScene = nil
	local PosX,PosY = nil,nil
	if Transport_Server(TrapName) then
		for _, i in pairs(Transport_Server:GetKeys(TrapName, "陷阱")) do
			local TransInfo = Transport_Server(TrapName, "陷阱", i.."")
			if TransInfo("BeginScene") ~= "" 
				and TransInfo("BeginScene") == SceneName then
				EndScene = TransInfo("EndScene") 
				PosX = TransInfo("PosX") 
				PosY = TransInfo("PosY") 
				break
			end
		end
	end
	if EndScene == nil or EndScene == "" 
		or PosX == nil or PosX == "" 
		or PosY == nil or PosY == "" then
		return
	end
	if g_AreaFbLev[EndScene] == nil then
		return
	end
	
	CAreaFbServer.ChangeToAreaFb(pCharacter.m_Conn, EndScene)
end

function CAreaFbTransportTrap.OnStepOutTrap(pTrapHandler, pCharacter, TrapArg)

end

--领域传送
function CSpecilAreaTrap.OnStepInTrap(pTrapHandler, pCharacter, TrapArg)
--	if not IsCppBound(pCharacter) then
--		return
--	end
--	
--	if not pTrapHandler.m_Owner.m_IsCanTransSpecilAreaFb then
--		--print("副本还没有创建成功,等等再进")
--		return
--	end
--	
--	if pCharacter.m_Properties:GetType() == ECharacterType.Player then
--		local Camp = pCharacter:CppGetBirthCamp()
--		
--		local TrapName = pTrapHandler.m_Owner.m_Properties:GetCharName()
--		local SceneId = pTrapHandler.m_Owner.m_SceneId
--		local ServerId = pTrapHandler.m_Owner.m_ServerId
--		local SceneName = pTrapHandler.m_Owner.m_SceneName
--		local TrapCamp = pTrapHandler.m_Owner.m_CreateSceneCamp
--		
--		local function CallBack(result)
--			if IsCppBound(pCharacter) then
--				if result then
--					local PosX,PosY = nil,nil
--					for _,v in pairs(Transport_Server) do
--						if v.ObjectName == TrapName and v.Type == "陷阱" then
--							if SceneName == v.EndScene then
--								PosX = v.PosX
--								PosY = v.PosY
--								break
--							end
--						end
--					end
--					
--					if PosX and PosY then
--						MultiServerChangeScene(pCharacter.m_Conn, SceneId, ServerId, PosX, PosY)
--					end
--				else
--					--MsgToConn(pCharacter.m_Conn,)
--				end
--			end
--		end
--		
--		local param = {}
--		param["uCharId"] = pCharacter.m_uID
--		param["PlayerCamp"] = Camp
--		param["TrapCamp"] = TrapCamp
--		param["SceneId"] = SceneId
--		--CallAccountAutoTrans
--		CallAccountManualTrans(pCharacter.m_Conn.m_Account, "SceneMgrDB", "IsEnterScopesFb", CallBack, param)
--	end
end

function CChangeTongSceneByTrap.OnStepInTrap(pTrapHandler, pCharacter, TrapArg)
	if not IsCppBound(pCharacter) then
		return
	end
	ChangeTongSceneByTrap(pCharacter.m_Conn)
end

function CChangeTongSceneByTrap.OnStepOutTrap(pTrapHandler, pCharacter, TrapArg)

end


function CTakeBuffAddCount.OnStepInTrap(pTrapHandler, pCharacter, TrapArg)
	if not IsCppBound(pCharacter) then
		return
	end
	
	local stateCascade = pCharacter:GetStateCascade(TrapArg[1])
	if stateCascade > 0 then
		g_MatchGameMgr:AddMatchGameCount(pCharacter, 11, TrapArg[1], stateCascade)
	end
end

function CTakeBuffAddCount.OnStepOutTrap(pTrapHandler, pCharacter, TrapArg)

end
