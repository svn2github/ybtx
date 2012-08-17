gas_require "framework/main_frame/SandBoxDef"
gas_require "world/int_obj/IntObjServerInc"
gas_require "world/int_obj/IntObjCallbackHandlerServer"
gas_require "world/trigger_action/RandomCreate"
gas_require "world/trigger_action/obj/ActionLoadProgress"
cfg_load"int_obj/ObjAction_Server"
cfg_load "scene/Trap_Common"

g_AllIntObj = { }
RegMemCheckTbl("g_AllIntObj", g_AllIntObj)
g_AllIntObjNum = {}
local LogDataTemp  --- 为跟踪obj错位 bug

local function RequireOnClickedObjScript(Obj,strOnClicked)
	local scriptFileName = g_AllObjScript[strOnClicked]
--------------------------------------------------
	if scriptFileName ~= nil then
		local entry = RequireSandBox(scriptFileName)
		if entry ~= nil then
			if strOnClicked == "采集矿脉" then
				Obj.m_IsLode = true
			end
			Obj.m_DlgOnClicked:Add(entry.Exec)
		end
	end
end

g_IntObjServerMgr = CIntObjServerMgr:new()
g_IntObjServerMgr._m_IntObjCreateTbl={}
g_IntObjServerMgr._m_OnlyObj = {}
g_IntObjServerMgr.m_Barrier = {}

RegMemCheckTbl("g_IntObjServerMgr._m_OnlyObj", g_IntObjServerMgr._m_OnlyObj)
RegMemCheckTbl("g_IntObjServerMgr.m_Barrier", g_IntObjServerMgr.m_Barrier)
AddCheckLeakFilterObj(g_IntObjServerMgr._m_IntObjCreateTbl)

function CIntObjServerMgr:AddIntObjTbl(key,tbl)
	assert(nil == self._m_IntObjCreateTbl[key])
	self._m_IntObjCreateTbl[key] = tbl
end
function CIntObjServerMgr:Ctor()
end

--得到当前场景唯一的intobj
function GetOnlyIntObj(uSceneId, sObjName)
	if g_IntObjServerMgr._m_OnlyObj[uSceneId] then
		return g_IntObjServerMgr._m_OnlyObj[uSceneId][sObjName]
	end
	return
end

local function CreateSingleServerObj(IntObjCommon,Scene,Pos,ObjName,OwnerId,OwnerTeamID,DropTime)
	
	--检测场景Trap是否超出限制
	local bLimit = GasConfig.SceneObjLimit
	local uMaxLimit = Scene_Common[Scene.m_SceneName].ObjLimit
	local uSceneId = Scene.m_SceneId
	if bLimit and g_AllIntObjNum[uSceneId] and g_AllIntObjNum[uSceneId] > uMaxLimit then
		local errmsg = "场景【" .. Scene.m_SceneName .. "】同时存在的IntObj数量大于最大限制： 【" .. uMaxLimit .. "】，不允许继续创建！"
		LogErr(errmsg, ObjName)
		return nil
	end
	
	local IntObj = nil
	local PixelFPos = CFPos:new()
	PixelFPos.x = Pos.x * EUnits.eGridSpanForObj
	PixelFPos.y = Pos.y * EUnits.eGridSpanForObj
	
	IntObj = CEntityServerCreator_CreateServerIntObject(Scene.m_CoreScene, PixelFPos, "IntObj", ObjName )
	
	if LogDataTemp then
		local curPos = CFPos:new()
		IntObj:GetPixelPos(curPos)
		if math.abs(LogDataTemp.GridX * 64 - curPos.x) > 32 or math.abs(LogDataTemp.GridY * 64 - curPos.y) > 32 then
			local sourcePos = "原始坐标:[" .. LogDataTemp.GridX .. "," .. LogDataTemp.GridY .. "]"
			local argPos = "参数坐标:[" .. PixelFPos.x .. "," .. PixelFPos.y .. "]"
			local strCurPos = "创建完成后坐标:[" .. curPos.x .. "," .. curPos.y.."]"
			LogErr("Obj创建错位" , "[" .. ObjName .. "] " .. sourcePos .. argPos .. strCurPos)
		end
	end

	IntObj.m_Scene = Scene
	IntObj.m_Pos = Pos -- 保存格子坐标

	--IntObj看不见任何东西
	IntObj:SetIntObjSeeType(ETrapSeeType.ETrapSeeType_None)
	
	IntObj:SetStealth(0)
	IntObj:SetKeenness(0)
	IntObj.m_Properties = CServerIntObjProperties:new()
	IntObj.m_Properties:Init(IntObj)
	IntObj.m_Properties:InitIntObjStatic(ObjName, ECharacterType.IntObj,OwnerId,OwnerTeamID,DropTime)
	if g_NatureArg2ObjTbl[ObjName] then
		IntObj:SetSizeType(EAoiItemSizeType.eAIST_InterestingIntObj)
	end
	IntObj.m_DlgOnClicked = CDelegate:new()
	return IntObj
end

--[[
	Scene										所在场景
	x,y											坐标
]]
function CIntObjServerMgr:CreateSceneIntObj( Scene , File, isDynamicCreate)
	local srcTbl, indexTbl = g_AreaSceneDataMgr:GetObjTbl(Scene, File, isDynamicCreate)
	if srcTbl==nil then
		return
	end
	if indexTbl and not next(indexTbl) then
		return
	end
	local forTbl = indexTbl or srcTbl
	for i, v in pairs(forTbl) do
		local p = indexTbl and srcTbl[v] or v
		local pos=CFPos:new()
		pos.x, pos.y = p.GridX, p.GridY
		if Scene.m_DynamicCenterPos then --动态Obj创建时候 通过相对位置计算坐标
			pos.x = pos.x + Scene.m_DynamicCenterPos[1]
			pos.y = pos.y + Scene.m_DynamicCenterPos[2]
		end	
		if pos.x==nil or pos.y==nil then
			assert(false ,"坐标为空")
			return
		end
		if Scene.m_SceneName == "抱小猪选拔赛" then
			LogDataTemp = {}
			LogDataTemp.GridX = p.GridX
			LogDataTemp.GridY = p.GridY
		end
		CreateIntObj(Scene, pos, p.Name,false, nil, nil, nil, p.Direction)
		LogDataTemp = nil
	end
end

--参数
--（所在场景，
--	坐标，
--	ObjName，
--	OBJ是否销毁
--	Obj归属者（打怪掉落归属者），
--	队伍归属（组队打怪掉落），
--	掉落时间（当除了归属者和所属Team以外，别的人也可以共享时需要传DropTime），
--	方向）
function CreateIntObj(Scene, Pos, ObjName, IsDestroyObj, OwnerId, OwnerTeamID, DropTime,Direction)
	local IntObjCommon = IntObj_Common(ObjName)
	if IntObjCommon==nil then
		assert(false, ObjName.." 没有在IntObj_Common表中")
		return
	end
	
	--加一个判断,如果是在游戏中产生的OBJ,要先判断唯一
	if IsDestroyObj then
		if IntObjCommon("OnlyObj") == 1 then
			if g_IntObjServerMgr._m_OnlyObj[Scene.m_SceneId] then
				if g_IntObjServerMgr._m_OnlyObj[Scene.m_SceneId][ObjName] then
					local lObj = g_IntObjServerMgr._m_OnlyObj[Scene.m_SceneId][ObjName]
					g_IntObjServerMgr:Destroy(lObj,false)
				end
			end
		end
	end
	
	local IntObj = CreateSingleServerObj(IntObjCommon,Scene,Pos,ObjName,OwnerId,OwnerTeamID,DropTime)
	if not IntObj then
		LogErr("Obj创建失败", "场景名:" .. Scene.m_SceneName .. "  Obj名:" .. ObjName)
		return
	end	
	RegOnceTickLifecycleObj("IntObj", IntObj)
	IntObj:SetAndSyncActionDir(Direction)
	IntObj.m_Direction = Direction
	
	local ObjEntityID = IntObj:GetEntityID()
	local TriggerType = IntObjCommon("TriggerType")
	local TriggerAction = IntObjCommon("TriggerAction")
	
	local RandomCreateCommon = RandomCreate_Server(ObjName)
	if RandomCreateCommon ~= nil then
		local fistTime = RandomCreateCommon("FirstTime")
		local intervalTime = RandomCreateCommon("IntervalTime") * 1000
		local time = 0;
		if fistTime == nil or fistTime == "" then
			time = intervalTime
		else
			time = fistTime ~= 0 and fistTime * 1000 or 10
			IntObj.m_isNewRandomCreateTick = true
		end
		local function TickCallBack()
			g_RandomCreateTick(ObjEntityID)
		end
		IntObj.m_RandomCreateTick =  RegisterTick("ObjRandomCreateTick", TickCallBack, time)
	end

	local ObjAction = ObjAction_Server(ObjName)
	if ObjAction ~= nil then
		IntObj.m_Properties:SetBornAction(2)
		IntObj.m_IsBornAction = true
	end
	
	IntObj.m_CallbackHandler = CIntObjCallbackHandler:new()
	IntObj:SetCallbackHandler(IntObj.m_CallbackHandler)
	
	RequireOnClickedObjScript(IntObj,TriggerAction)
	
	local SceneId = Scene.m_SceneId
	if g_AllIntObj[SceneId] == nil then
		g_AllIntObj[SceneId] = {}
		g_AllIntObjNum[SceneId] = 0
	end
	g_AllIntObjNum[SceneId] = g_AllIntObjNum[SceneId] + 1
	
	g_AllIntObj[SceneId][ObjEntityID] = IntObj
	if IntObjCommon("OnlyObj") == 1 then   --场景内的唯一 Obj
		if g_IntObjServerMgr._m_OnlyObj[SceneId] == nil then
			g_IntObjServerMgr._m_OnlyObj[SceneId] = {} 
		end
		if g_IntObjServerMgr._m_OnlyObj[SceneId][ObjName] == nil then
			g_IntObjServerMgr._m_OnlyObj[SceneId][ObjName] = IntObj
		else 
			assert(false, "Obj "..ObjName.." 必须是场景内唯一的(见配置表IntObj_Common)")
		end
	end
	
	IntObj:SetSize(IntObjCommon("size"))
	IntObj:SetBottomSize(IntObjCommon("size"))
	--IntObj:SetBarrierSize(IntObjCommon("size"))
	
	local BarrierType =  IntObjCommon("IsBarrier")
	if BarrierType >= 1 then   --障碍 Obj
		local PixelFPos = CFPos:new()
		IntObj:GetPixelPos(PixelFPos)
		
		IntObj.m_IsBarrier = BarrierType
		if g_IntObjServerMgr.m_Barrier[SceneId] == nil then
			g_IntObjServerMgr.m_Barrier[SceneId] = {} 
		end
		if g_IntObjServerMgr.m_Barrier[SceneId][ObjName] == nil then
			g_IntObjServerMgr.m_Barrier[SceneId][ObjName] = BarrierType
		end
		if BarrierType == 1 then
			IntObj:SetDynamicBarrier(EBarrierType.eBT_LowBarrier)
		elseif BarrierType == 2 then
			IntObj:SetDynamicBarrier(EBarrierType.eBT_MidBarrier)
		elseif BarrierType == 3 then
			IntObj:SetDynamicBarrier(EBarrierType.eBT_HighBarrier)
		else
			assert(false, "Obj "..ObjName.." 格子的障碍类型设置有误(见配置表IntObj_Common)")
		end
		
		if LogDataTemp then
			local curPos = CFPos:new()
			IntObj:GetPixelPos(curPos)
			if math.abs(LogDataTemp.GridX * 64 - curPos.x) > 32 or math.abs(LogDataTemp.GridY * 64 - curPos.y) > 32 then
				local sourcePos = "原始坐标:[" .. LogDataTemp.GridX .. "," .. LogDataTemp.GridY .. "]"
				local argPos = "设置动态障碍前:[" .. PixelFPos.x .. "," .. PixelFPos.y .. "]"
				local strCurPos = "设置动态障碍后:[" .. curPos.x .. "," .. curPos.y.."]"
				LogErr("Obj设置动态障碍后错位", "[" .. ObjName .. "] " .. sourcePos .. argPos .. strCurPos)
			end
		end
		
	end
	
	local function TickDestroyObj(data)
		local Target,IsReborn = data[1],data[2]
		g_TriggerMgr:Trigger("生存期结束", Target)
		CIntObjServerMgr:Destroy(Target,IsReborn)
	end
	local ExistTime = IntObjCommon("ExistTime")
	if not IsDestroyObj then
		if ExistTime ~= "" and IsNumber(ExistTime) and ExistTime > 0 then
			local data = {IntObj,true}
			RegisterOnceTick(IntObj,"ObjExistTick", TickDestroyObj, ExistTime*1000, data)
		end
	else
		if ExistTime == "" or not IsNumber(ExistTime) or ExistTime <= 0 then
			ExistTime = 30
		end
		local data = {IntObj,false}
		RegisterOnceTick(IntObj,"ObjExistTick", TickDestroyObj, ExistTime*1000, data)
	end
	
	if g_NatureArg2NpcTbl[ObjName] then
		IntObj:SetSizeType(EAoiItemSizeType.eAIST_InterestingIntObj)
	end
	IntObj:SetObjName(ObjName)
	g_TriggerMgr:Trigger("出生", IntObj)
	return IntObj
end

local function ClearDropObjTbl(DropObjTblID, ObjID)
	if NpcDropObjTbl[DropObjTblID] and NpcDropObjTbl[DropObjTblID].m_CreateObjTbl then
		
		if IsTable(NpcDropObjTbl[DropObjTblID].m_CreateObjTbl) then
			if NpcDropObjTbl[DropObjTblID].m_CreateObjTbl[ObjID] then
				NpcDropObjTbl[DropObjTblID].m_CreateObjTbl[ObjID] = nil
			end
			if not next(NpcDropObjTbl[DropObjTblID].m_CreateObjTbl) then
				NpcDropObjTbl[DropObjTblID].m_CreateObjTbl = nil
				NpcDropObjTbl[DropObjTblID].m_Objs_List = nil
				NpcDropObjTbl[DropObjTblID] = nil
			end
		end
		
	end
end

----------------------------------
local function IsRebornObj(Obj,IsReborn)
	if not IsReborn then
		return
	end
	local ObjName = Obj.m_Properties:GetCharName()
	--print("开服务器,自动删掉的OBJ名字:",ObjName)
	local IntObjCommon = IntObj_Common(ObjName)
	if not IntObjCommon then
		return
	end
	local IsOrNot = IntObjCommon("IsReborn")
	if IsOrNot == 0 or IsOrNot == "" or not IsNumber(IsOrNot) then
		return
	end
	--print("ObjName create..........")
	local RebornTime = IntObjCommon("RefreshTime")
	if RebornTime == "" or not IsNumber(RebornTime) or RebornTime <= 0 then
		RefreshTime = 30
	end
	
	local function ObjReborn(data)
		CreateIntObj(data.m_ObjScene, data.m_ObjPos, data.m_ObjName,false, nil,nil,nil,data.m_Direction)
	end

	local data = {}
	local Pos = CPos:new()
	Obj:GetGridPos(Pos)
	data.m_ObjName = ObjName
	data.m_ObjScene = Obj.m_Scene
	data.m_ObjPos = Obj.m_Pos
	data.m_SceneId = Obj.m_Scene.m_SceneId
	data.m_Direction = Obj.m_Direction
	
	RegisterOnceTick(Obj.m_Scene, "ChildbirthTick", ObjReborn, RebornTime*1000, data)
end
-----------------------------------

-- 当Obj 删除的时候 清除挂在陷阱上面的ObjID
local function ClearObjTrapData(Obj)
	for TrapName, IDTbl in pairs( Obj.m_Properties.m_InTrapTblName) do
		for _, TrapID in pairs(IDTbl) do 
			local Trap = CIntObjServer_GetIntObjServerByID(TrapID)
			if Trap then
				if Trap.m_OnTrapCharIDTbl[Obj:GetEntityID()] then
					CIntObjCallbackHandler:OnObjLeaveTrapViewAoi(TrapID, Obj)
					Trap.m_OnTrapCharIDTbl[Obj:GetEntityID()] = nil
				end
--				local TrapCharIDTbl = Trap.m_TrapHandler.m_OnTrapCharIDTbl
--				for i=1, #(TrapCharIDTbl) do
--					if TrapCharIDTbl[i] == Obj:GetEntityID() then
--						CIntObjCallbackHandler:OnObjLeaveTrapViewAoi(TrapID, Obj)
--						--ObjLeaveTrap(Obj, Trap)
--						table.remove(Trap.m_TrapHandler.m_OnTrapCharIDTbl, i)
--					end
--				end
			end
		end
	end	
end

function CIntObjServerMgr:Destroy(IntObj,IsReborn,IsDestroyScene)
	if not IsServerObjValid(IntObj) then
		return
	end
	UnRegisterObjOnceTick(IntObj)
	
--	ClearObjTrapData(IntObj)
	
	if IntObj.m_RandomCreateTick ~= nil then
		UnRegisterTick(IntObj.m_RandomCreateTick)
		IntObj.m_RandomCreateTick = nil
	end
	if IntObj.m_QuanTick then
		UnRegisterTick(IntObj.m_QuanTick)
		IntObj.m_QuanTick = nil	
	end
	if IntObj.m_AssignTick then
		UnRegisterTick(IntObj.m_AssignTick)
		IntObj.m_AssignTick = nil	
	end
	
	if IntObj.m_ItemWndCloseTick then
		UnRegisterTick(IntObj.m_ItemWndCloseTick)
		IntObj.m_ItemWndCloseTick = nil	
	end
	
	if IntObj.m_SmallGameTick then
		UnRegisterTick(IntObj.m_SmallGameTick)
		IntObj.m_SmallGameTick = nil
	end
	--------------------------
	--OBJ是否重生
	IsRebornObj(IntObj,IsReborn)
	--------------------------
	if not IsDestroyScene then
		g_TriggerMgr:Trigger("销毁", IntObj)
	end
	
	local ObjEntityID = IntObj:GetEntityID()
	if IntObj.m_BelongToNpc then--如果是打怪掉落OBJ,每删除一次,就要判断清table
		ClearDropObjTbl(IntObj.m_BelongToNpc, ObjEntityID)
	end
	
	if IntObj.m_Openner then
		if IsTable(IntObj.m_Openner) then
			for ID, v in pairs(IntObj.m_Openner) do
				local Player = g_GetPlayerInfo(ID)
				if Player and Player.m_ResourceObjID == ObjEntityID then
					TongStopLoadProgress(Player)
				end
			end
		else
			local Player = g_GetPlayerInfo(IntObj.m_Openner)
			if Player then
				if Player.m_CollIntObjID == ObjEntityID then
					BreakPlayerCollItem(Player)
				elseif Player.m_ProcessingObjID == ObjEntityID then
					BreakPlayerActionLoading(Player)
				elseif Player.m_ResourceObjID == ObjEntityID then
					TongStopLoadProgress(Player)
				end
			end
		end
	end
	if IntObj.m_AssignPlayer then
		RemoveObjAllAssignTbl(IntObj)
	end
	if IntObj.m_AcutionParam then
		local function CallBack(PlayerID, Money)
			if PlayerID then
				MsgToConnById(PlayerID, 3992, Money)
			end
		end
		if IntObj.m_AcutionParam[2] > 0 then
			local data = {["CharID"] = IntObj.m_AcutionParam[1], ["ObjID"] = IntObj:GetEntityID(), ["EventId"] = IntObj.m_AcutionEventId}
			local Player = g_GetPlayerInfo(IntObj.m_AcutionParam[1])
			if IsCppBound(Player) and IsCppBound(Player.m_Conn) then
				CallAccountManualTrans(Player.m_Conn.m_Account, "AuctionAssignDB", "ReturnAuctionMoney", CallBack, data)
			else
				CallDbTrans("AuctionAssignDB", "ReturnAuctionMoney", CallBack, data, IntObj.m_AcutionParam[1])
			end
		end
	end
	--删除帮会挑战的Obj
	if IntObj.m_ObjIndex then
		ReMoveChallengeObj(IntObj)
	end
	local SceneId = IntObj.m_Scene.m_SceneId
	local SceneType = IntObj.m_Scene.m_SceneAttr.SceneType
	local ObjName = IntObj.m_Properties:GetCharName()
	if g_IntObjServerMgr._m_OnlyObj[SceneId] and g_IntObjServerMgr._m_OnlyObj[SceneId][ObjName] then
		g_IntObjServerMgr._m_OnlyObj[SceneId][ObjName] = nil
	end
	if g_IntObjServerMgr.m_Barrier[SceneId] and g_IntObjServerMgr.m_Barrier[SceneId][ObjName] then
		g_IntObjServerMgr.m_Barrier[SceneId][ObjName] = nil
	end
	if not g_AllIntObjNum[SceneId] then
		local Scene = g_SceneMgr:GetScene(SceneId)
		local flag = "true"
		if not Scene then
			flag = "false"
		end
		LogErr("销毁obj不正常","场景Id:"..SceneId..",场景是否存在"..flag..",场景类型:"..SceneType)
	end
	g_AllIntObjNum[SceneId] = g_AllIntObjNum[SceneId] - 1
	g_AllIntObj[SceneId][ObjEntityID] = nil
	CEntityServerCreator_DestroyEntity(IntObj)
	IntObj = nil
end

function DestroyOneSceneIntObj(uSceneID)
	if g_AllIntObj[uSceneID] then
		for _, IntObj in pairs(g_AllIntObj[uSceneID]) do
			g_IntObjServerMgr:Destroy(IntObj,false,true)
		end
		g_IntObjServerMgr.m_Barrier[uSceneID] = nil
		g_IntObjServerMgr._m_OnlyObj[uSceneID] = nil
		g_AllIntObj[uSceneID] = nil
		
		g_AllIntObjNum[uSceneID] = nil
	end
end

--由于OBJ抱起的时候删除了OBJ  所以在回调里无法进入 故在此解决了在trap内的OBJ被抱走  的问题.
--local function ObjLeaveTrap(Obj, Trap)
--	local TrapName = Trap.m_Properties:GetCharName()
--	if not Trap_Common(TrapName) then
--		return 
--	end
--	local TrapArg = GetCfgTransformValue(true, "Trap_Common", TrapName, "Arg")
--	if not (TrapArg and TrapArg[1]) then
--		return 
--	end
--	local SceneId = Obj.m_Scene.m_SceneId
--	local IntObj = GetOnlyIntObj(SceneId, TrapArg[1])
--	if IntObj == nil then
--		return
--	end
----	print("OBJ 离开陷阱 ObjLeaveTrap(Obj, Trap)")
--	if #(pTrapHandler.m_OnTrapCharIDTbl) == 0 then
--		g_IntObjServerMgr:IntObjChangeState(IntObj, 0)
--	else 
--		g_IntObjServerMgr:IntObjChangeState(IntObj, 1)
--	end	
--	CIntObjCallbackHandler:OnObjLeaveTrapViewAoi(ObjEntityID, IntObj)
--end

function CIntObjServerMgr:IntObjChangeModel(IntObj, ModelName)
--print("IntObjChangeModel")
	local OldModelName = IntObj.m_Properties:GetModelName()
	local EntityID = IntObj:GetEntityID()
	IntObj.m_Properties:SetModel(ModelName)
	IntObj.m_Properties:SetActionNum(1)
	Gas2Gac:IntObjChangeModel(IntObj:GetIS(0), EntityID, ModelName, OldModelName)
end

function CIntObjServerMgr:IntObjDoAction(IntObj, RespondAction, NextActionNum)
--print("IntObjDoAction")
	local EntityID = IntObj:GetEntityID()
--	local ObjName = IntObj.m_Properties:GetCharName()
--	if ObjChangeModel_Server[ObjName] ~= nil
--		and ObjChangeModel_Server[ObjName][RespondAction] ~= nil 
--		and ObjChangeModel_Server[ObjName][RespondAction] ~= "" then
--		local ModelName = ObjChangeModel_Server[ObjName][RespondAction]
--		Gas2Gac:NotifyIntObjDoAction(IntObj:GetIS(0), EntityID, RespondAction, "")
--		local function Func(Tick, IntObj)
--			g_IntObjServerMgr:IntObjChangeModel(IntObj, ModelName)
--			UnRegisterTick(IntObj.m_ChangeModelTick)
--			IntObj.m_ChangeModelTick = nil
--		end
--		IntObj.m_ChangeModelTick = RegisterTick("ChangeModelTick", Func,300,IntObj)
--	else
		Gas2Gac:NotifyIntObjDoAction(IntObj:GetIS(0), EntityID, RespondAction, NextActionNum)
		if NextActionNum then
			IntObj.m_Properties:SetActionNum(NextActionNum)
		end
--	end
end

function CIntObjServerMgr:IntObjChangeState(IntObj, State)
--print("IntObjChangeState")
	IntObj.m_Properties:SetCanUseState(State)
	local EntityID = IntObj:GetEntityID()
	Gas2Gac:IntObjChangeState(IntObj:GetIS(0), EntityID, State)
end



function CIntObjServerMgr:RefreshObjState( IntObj)
	local intObjname = IntObj.m_Properties:GetCharName()
	local refreshtime = IntObj_Common(intObjname, "RefreshTime")
	if refreshtime == "" or refreshtime == 0 or refreshtime == nil then
		refreshtime = 10	
	end
	local function RefreshObjStateTick()
		CIntObjServerMgr:IntObjChangeState(IntObj, 1)
	end
	RegisterOnceTick(IntObj,"ReflreshObjState", RefreshObjStateTick, refreshtime*1000)
end
