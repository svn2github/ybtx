CKickBehavior = class()

local function ResetCanDropObjState(data)
	local EntityID, ObjName = data[1],data[2]
	local CanDropObj = CEntityServerManager_GetEntityByID(EntityID)
	if CanDropObj == nil then
		return
	end
	
--	local ModelName = IntObj_Common(ObjName, "ModelName")
--	g_IntObjServerMgr:IntObjChangeModel(CanDropObj, ModelName)
	g_IntObjServerMgr:IntObjDoAction(CanDropObj,"respond02_2",1)
	CanDropObj.m_HaveBeKickedTime = nil
end

local function DoDropObj(CanDropObj, ObjName, DropName, Pos, PlayerId, QuestDrop)
	local Scene = CanDropObj.m_Scene
	local DropObj = nil
	local num = math.random()
	local DropTime = nil
	if not QuestDrop then
		DropTime = os.time()
	end
	if string.find(ObjName, "树") then
		local  BornPos = CFPos:new()
		BornPos.x = Pos.x + num
		BornPos.y = Pos.y + num
		DropObj = CreateIntObj(Scene, BornPos, DropName,true, PlayerId, nil, DropTime)
	else
		local GridFPos = CFPos:new()
		GridFPos.x = Pos.x + num
		GridFPos.y = Pos.y + num
		DropObj = CreateIntObj(Scene, GridFPos, DropName,true, PlayerId, nil, DropTime)
	end
	DropObj.m_OwnerId = PlayerId
	DropObj.m_CreateTime = os.time()
end

local DropPosTbl = 
{
	{0.5,0},{0,0.5},{-0.5,-0.5},{0.5,-0.5},
	{-0.5,0},{0,0.5},{0.5,0.5},{-0.5,0.5},
	{1,0},{0,-1},{-1,-1},{1,-1},{-1,0},
	{0,0},{0,1},{1,1},{-1,1},
}--共16个方向

local function NormalDrop(CanDropObj, ObjName, Player, reFrashTime, MultipleNum, IsSaveObj)
	if g_ObjActionArgTbl[ObjName] then
		if g_ObjActionArgTbl[ObjName][1] == "掉落" then
			local DropName = g_ObjActionArgTbl[ObjName][2]
			local disPos = nil
			for i =1 , MultipleNum do
				disPos = DropPosTbl[1]
				table.remove(DropPosTbl,1)
				table.insert(DropPosTbl,disPos)
				local Pos = CPos:new()
				Pos.x = ObjPos.x + disPos[1]
				Pos.y = ObjPos.y + disPos[2]
				--print(".............Pos1",i,Pos.x,Pos.y)
				DoDropObj(CanDropObj, ObjName, DropName, Pos, Player.m_uID)
			end
		elseif g_ObjActionArgTbl[ObjName][1] == "剧场" then
			local TheaterName = g_ObjActionArgTbl[ObjName][2]
			--添加触发剧场 
			Player.m_Scene.m_TheaterMgr:RunTriggerTheater(TheaterName, Player:GetEntityID())
		elseif g_ObjActionArgTbl[ObjName][1] == "触发隐藏任务" then
			local HideQuestName = g_ObjActionArgTbl[ObjName][2]  -- 隐藏任务名称
--				print("踢树触发隐藏任务")
			if g_HideQuestMgr[HideQuestName] ~= nil then
				TriggerHideQuest(Player, HideQuestName)
			end
		end
	end
	
	CanDropObj.m_HaveBeKickedTime["UseTime"] = os.time()
	CanDropObj.m_HaveBeKickedTime["CoolTime"] = reFrashTime/1000+1
	if IsSaveObj then--保留OBJ
		g_IntObjServerMgr:IntObjDoAction(CanDropObj,"respond02_1",2)
		local data = {CanDropObj:GetEntityID(), ObjName}
		RegisterOnceTick(CanDropObj.m_Scene,"ResetObjStateTick", ResetCanDropObjState, reFrashTime, data)
	else--干掉OBJ
		g_IntObjServerMgr:Destroy(CanDropObj,true)
	end
end
	
local function AttackObjByLeg(Player,GlobalID,MultipleNum,IsSaveObj)
	local CanDropObj = CEntityServerManager_GetEntityByID(GlobalID)
	if not IsServerObjValid(CanDropObj) then
		return false
	end
	
	local ObjName = CanDropObj.m_Properties:GetCharName()
	local PlayerId = Player.m_uID
	
	if CanDropObj.m_HaveBeKickedTime then
		return false
	end
	
	local reFrashTime = IntObj_Common(ObjName, "RefreshTime")
	if reFrashTime < 0.00001 then
		reFrashTime = 10000
	else
		reFrashTime = reFrashTime * 1000
	end
	
	local Scene = CanDropObj.m_Scene
	local ObjPos = CanDropObj.m_Pos
	ObjPos.x = math.floor(ObjPos.x)
	ObjPos.y = math.floor(ObjPos.y)
	--print(".............ObjPos",ObjPos.x,ObjPos.y)
	
	CanDropObj.m_HaveBeKickedTime = {["UseTime"]=os.time(),["CoolTime"]=(reFrashTime/1000+5),["LockState"]=0}
	
	if g_QuestDropItemMgr[ObjName] or ObjRandomDropItem_Server(ObjName) then
		local function CallBack(DropObjTbl)
			if not IsServerObjValid(CanDropObj) or not CanDropObj.m_HaveBeKickedTime then
				CanDropObj.m_HaveBeKickedTime = nil
				return
			end
			if not IsCppBound(Player) then
				CanDropObj.m_HaveBeKickedTime = nil
				return
			end
			
			for i = 1, table.getn(DropObjTbl) do
				local dropname = DropObjTbl[i]["DropName"]
				local dropnum = DropObjTbl[i]["DropNum"]
				local QuestDrop = DropObjTbl[i]["QuestDrop"]
				local disPos = nil
				for n = 1, dropnum do
					disPos = DropPosTbl[1]
					table.remove(DropPosTbl,1)
					table.insert(DropPosTbl,disPos)
					local Pos = CPos:new()
					Pos.x = ObjPos.x + disPos[1]
					Pos.y = ObjPos.y + disPos[2]
					--print(".............Pos2",n,Pos.x,Pos.y)
					DoDropObj(CanDropObj, ObjName,dropname, Pos, PlayerId, QuestDrop)
				end
			end
			
			NormalDrop(CanDropObj, ObjName, Player, reFrashTime, MultipleNum, IsSaveObj)
		end
		local parameter = {}
		parameter["nCharId"] = PlayerId
		parameter["ObjName"] = ObjName
		parameter["MultipleNum"] = MultipleNum
		CanDropObj.m_HaveBeKickedTime["LockState"] = 1
		local res = CallAccountManualTrans(Player.m_Conn.m_Account, "KickObjDropDB","KickObjDrop",CallBack,parameter)
		if not res or res ~= 0 then
			CanDropObj.m_HaveBeKickedTime = nil
		end
	else
		CanDropObj.m_HaveBeKickedTime["LockState"] = 2
		NormalDrop(CanDropObj, ObjName, Player, reFrashTime, MultipleNum, IsSaveObj)
	end
	return true
end


function CKickBehavior.KickSwapBehaviorActualize(Player, TargetEntityID, Strength, IsSaveObj)
	--根据力度可以得到能够踢下多少东西,踢坏一排的障碍,一次踢坏一个障碍墙
	if not TargetEntityID or TargetEntityID == 0 then
		return
	end
	
	local Target = CEntityServerManager_GetEntityByID(TargetEntityID)
	if not IsServerObjValid(Target) then
		return
	end
	
	local DropNum = Strength or 1
	
	local ObjName = Target.m_Properties:GetCharName()
	if Target.m_IsBarrier and Target.m_IsBarrier>=1 then
		--这是一个障碍,如果踢成功了,就会这这里面,把障碍移走
		local function SetBarrier(data)
			if IsCppBound(data[1]) then
				local ModelName = IntObj_Common(data[2], "ModelName")
				g_IntObjServerMgr:IntObjDoAction(Target,"respond02_2",1)
				data[1]:SetBottomSize(0.5)
				--data[1]:SetBarrierSize(0.5)
				data[1]:SetDynamicBarrier(EBarrierType.eBT_HighBarrier)
				data[1].m_OwnerId = nil
			end
		end
		
		if IsSaveObj then
			g_IntObjServerMgr:IntObjDoAction(Target,"respond02_1",2)
			Target:SetDynamicBarrier(EBarrierType.eBT_NoBarrier)
			local data = {Target,ObjName}
			RegisterOnceTick(Target.m_Scene,"KickSwapTick",SetBarrier,20000,data)
		else
			g_IntObjServerMgr:Destroy(Target,true)
		end
	else
		Target.m_OwnerId = nil
		if not AttackObjByLeg(Player,TargetEntityID,DropNum,IsSaveObj) then
			return
		end
	end
	
	AddVarNumForTeamQuest(Player, "对"..ObjName.."踢", 1)
end

--左键按下时,接收的包
function CKickBehavior.BeginKickBehavior(Conn)
	if not IsCppBound(Conn.m_Player) then
		return
	end
	--Conn.m_Player:SetAndSyncActionState(EActionState.eAS_Kick)	-- eAS_Kick_Stay 已删
	--记时间
	Conn.m_Player.m_KickTime = os.time()
end

--左键抬起时,接收的包
function CKickBehavior.EndKickBehavior(Conn,IntObjId,XLPos)
	local Player = Conn.m_Player
	if not IsCppBound(Player) or not Player.m_KickTime or not Player.m_KickBehavor then
		return
	end
	
	local strength = 1
	if XLPos <= 250 then
		strength = 1
	elseif XLPos <= 500 then
		strength = 2
	elseif XLPos <= 750 then
		strength = 3
	elseif XLPos <= 1000 then
		strength = 2
	end
	
	Player:SetAndSyncActionState(EActionState.eAS_Kick)
	
	local function DropObj()
		if IsCppBound(Player) then
			if Player.m_KickTime then--如果这个时候,他为nil了,说明已经取消了(原因可能是玩家已经死掉了)
				Player:CppSetCtrlState(EFighterCtrlState.eFCS_OnMission, false)
				Player:CppSetCtrlState(EFighterCtrlState.eFCS_ForbitNormalAttack, false)
				Player:LuaSetDoSkillCtrlState(EDoSkillCtrlState.eDSCS_ForbitUseSkill, false)
				Player.m_SetCtrlState = 26
				
				CKickBehavior.KickSwapBehaviorActualize(Player, IntObjId, strength, Player.m_KickSaveObj)
				
				Player.m_IsInToKickTick = nil
				Player.m_KickSaveObj = nil
				Player.m_KickTime = nil
			end
		end
	end
	RegisterOnceTick(Player.m_Scene,"KickDropObj",DropObj,500)
	
	Player.m_KickBehavor = nil
	Player:DelListeningWnd()
end

--取消时,接收的包
function CKickBehavior.CancelKickBehavior_C(Conn)
	if not IsCppBound(Conn.m_Player) then
		return
	end
	if not Conn.m_Player.m_KickBehavor then
		return
	end
	
	local Target = CEntityServerManager_GetEntityByID(Conn.m_Player.m_IsInToKickTick)
	if Target then
		Target.m_OwnerId = nil
	end
	
	Conn.m_Player:CppSetCtrlState(EFighterCtrlState.eFCS_OnMission, false)
	Conn.m_Player:CppSetCtrlState(EFighterCtrlState.eFCS_ForbitNormalAttack, false)
	Conn.m_Player:LuaSetDoSkillCtrlState(EDoSkillCtrlState.eDSCS_ForbitUseSkill, false)
	Conn.m_Player.m_KickTime = nil
	Conn.m_Player.m_SetCtrlState = 27
	Conn.m_Player.m_KickSaveObj = nil
	Conn.m_Player.m_IsInToKickTick = nil
	Conn.m_Player.m_KickBehavor = nil
	Conn.m_Player:DelListeningWnd()
end

function CKickBehavior.CancelKickBehavior_S(Player)
	if not Player.m_KickBehavor then
		return
	end
	
	local Target = CEntityServerManager_GetEntityByID(Player.m_IsInToKickTick)
	if Target then
		Target.m_OwnerId = nil
	end
	
	if Player:CppIsLive() then
		
		Player:CppSetCtrlState(EFighterCtrlState.eFCS_OnMission, false)
		Player:CppSetCtrlState(EFighterCtrlState.eFCS_ForbitNormalAttack, false)
		Player:LuaSetDoSkillCtrlState(EDoSkillCtrlState.eDSCS_ForbitUseSkill, false)
		Player.m_SetCtrlState = 28
	end
	Player.m_KickTime = nil
	Gas2Gac:CloseXLProgressWnd(Player.m_Conn)
	
	Player.m_KickSaveObj = nil
	Player.m_IsInToKickTick = nil
	Player.m_KickBehavor = nil
	Player:DelListeningWnd()
end
