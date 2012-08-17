cfg_load "int_obj/IntObj_Common"
lan_load "int_obj/Lan_IntObj_Common"

local IntObjDoActionTick = {}

local function ClickSignPost(IntObjClient)
--	local IntObjClient = CIntObjClient_GetIntObjByID(uEntityID)
	local ObjName = IntObjClient.m_Properties:GetCharName()
	if g_ObjActionArgTbl[ObjName] 
		and g_ObjActionArgTbl[ObjName][2]
		and g_ObjActionArgTbl[ObjName][3] then
		local posx = g_ObjActionArgTbl[ObjName][2]
		local posy = g_ObjActionArgTbl[ObjName][3]
		local MoveToPos = CFPos:new()
		MoveToPos.x = posx*EUnits.eGridSpanForObj
		MoveToPos.y = posy*EUnits.eGridSpanForObj
		g_MainPlayer:BeforeMoveToSelectPos(MoveToPos)
	end
end

local function AddCanClickFx(IntObjFollower)
	if IntObjFollower.m_CanClickFxId then
		return
	end
	local ObjName = IntObjFollower.m_Properties:GetCharName()
	local ActionName = IntObj_Common(ObjName, "TriggerAction")
	if string.find(ActionName,"拾取") then
		IntObjFollower.m_CanClickFxId = IntObjFollower:GetRenderObject():AddEffect( "fx/setting/other/other/diaoluo/create.efx", "diaoluo/create", -1, nil)
	else
		if string.find(ActionName,"采集") 
			or string.find(ActionName,"踢")
			or string.find(ActionName,"小游戏") then
			IntObjFollower.m_CanClickFxId = IntObjFollower:GetRenderObject():AddEffect( "fx/setting/other/other/shiqu/create.efx", "shiqu/create", -1, nil)
		end
	end
	return
end

--点击目标
function OnLClickIntObj(Target)
	local ObjName = Target.m_Properties:GetCharName()
	local ActionName = IntObj_Common(ObjName, "TriggerAction")
	if ObjName == "矿洞机关" then
		if g_GameMain.m_ChangeOutMsgBox and g_GameMain.m_ChangeOutMsgBox:IsShow() then
			MsgClient(10007)
			return			
		end
	end
	if ActionName ~= "路标" then
		Gac2Gas:OnLClickIntObj(g_Conn,Target:GetEntityID())
	else
		ClickSignPost(Target)
	end

--	print("Hello World!")
end

local function CheckCollObj(ObjName)
	if g_ObjActionArgTbl[ObjName] then
		local ArgTbl = nil
		if IsTable(g_ObjActionArgTbl[ObjName][1]) then
			ArgTbl = g_ObjActionArgTbl[ObjName][1]
		elseif IsTable(g_ObjActionArgTbl[ObjName][2]) then
			ArgTbl = g_ObjActionArgTbl[ObjName][2]
		end
		if ArgTbl then
			local needitemtype = ArgTbl[1]
			local needitemname = ArgTbl[2]
			local needitemnum = ArgTbl[3]
			local havenum = g_GameMain.m_BagSpaceMgr:GetItemCountByType( tonumber(needitemtype), needitemname ) 
			if havenum < needitemnum then
				return false
			end
		end
	end
	if g_QuestDropItemMgr[ObjName] then
		for questname ,vartbl in pairs(g_QuestDropItemMgr[ObjName]) do
			local queststatetbl = g_GameMain.m_QuestRecordWnd.m_QuestStateTbl[questname]
			local questmgr = g_GameMain.m_QuestRecordWnd.m_DoingQuestInfo[questname]
			if queststatetbl and queststatetbl.m_State == QuestState.init and questmgr then
				for i =1 , table.getn(vartbl) do
					local dropname = vartbl[i].Object 
					if not questmgr[dropname] or questmgr[dropname].DoNum < questmgr[dropname].NeedNum then
						return true
					end
				end
			end
		end
	end
	if ObjRandomDropItem_Server(ObjName) then
		return true
	end
	return false
end

local function CheckTickObj(ObjName)
	local QuestTbl = g_GameMain.m_QuestRecordWnd.m_DoingQuestInfo
	for questname,_ in pairs(QuestTbl) do
		for varname, State in pairs(QuestTbl[questname]) do
			if string.find(varname, "踢") and string.find(varname, ObjName) then
				if State.DoNum < State.NeedNum then
					return true
				else
					return false
				end
			end
		end
	end
	return false
end

local function CheckItemObj(ObjName)
	local ItemType = g_ObjActionArgTbl[ObjName][1]
	local Only = g_ItemInfoMgr:GetItemInfo( ItemType,ObjName,"Only" )
	local havenum = g_GameMain.m_BagSpaceMgr:GetItemCountByType( ItemType, ObjName ) 
	if Only ~= 1 then
		return true
	else
		if havenum >= 1 then
			return false
		elseif ItemType == 16 and g_WhereGiveQuestMgr["Goods"][ObjName] then
			local QuestName = g_WhereGiveQuestMgr["Goods"][ObjName][1]
			local queststatetbl = g_GameMain.m_QuestRecordWnd.m_QuestStateTbl[QuestName]
			if queststatetbl == nil 
				or (queststatetbl.m_State == QuestState.finish 
				and g_RepeatQuestMgr[QuestName]) then
				return true
			end
		end
	end
	return false
end

local function CheckAddQuestCountObj(ObjName)
	local VarName = "使用Obj"..ObjName
	if g_VarQuestMgr[VarName] then
		for i,QuestName in pairs(g_VarQuestMgr[VarName]) do
			local StateTbl = g_GameMain.m_QuestRecordWnd.m_QuestStateTbl[QuestName]
			local QuestInfo = g_GameMain.m_QuestRecordWnd.m_DoingQuestInfo[QuestName]
			if StateTbl and StateTbl.m_State == QuestState.init 
				and QuestInfo[VarName].DoNum < QuestInfo[VarName].NeedNum then
				return true
			end
		end
	end
	return false
end

local function CheckSmallGameObj(ObjName)
	local ActionArg = g_ObjActionArgTbl[ObjName]
	if not ActionArg then
		return false
	end
	if SmallGame_Common(ActionArg[1]) then
		local QuestName = SmallGame_Common(ActionArg[1], "QuestName")
		if QuestName == "" then
			return true
		end
		
		local QuestType = SmallGame_Common(ActionArg[1], "QuestType")
		local QuestResult = g_SmallGameStrTbl[ActionArg[1]].QuestResult --SmallGame_Common[ActionArg[1]].QuestResult
		local VarName = nil
		if QuestType == "剧情需求" then
			VarName = QuestResult[1]
		elseif QuestType == "物品需求" then
			VarName = QuestResult[2]
		end
		if VarName == nil then
			return false
		end
		
		local StateTbl = g_GameMain.m_QuestRecordWnd.m_QuestStateTbl[QuestName]
		local QuestInfo = g_GameMain.m_QuestRecordWnd.m_DoingQuestInfo[QuestName]
		if StateTbl and StateTbl.m_State == QuestState.init 
			and QuestInfo[VarName].DoNum < QuestInfo[VarName].NeedNum then
			return true
		end
	end
	return false
end

function CheckUseCondition(IntObjFollower)
	local ObjName = IntObjFollower.m_Properties:GetCharName()
	local ActionName = IntObj_Common(ObjName, "TriggerAction")
	if string.find(ActionName, "副本采集物品") then
		return true
	end
	local OwnerId = IntObjFollower.m_Properties:GetDropOwnerId()
	local DropTime = IntObjFollower.m_Properties:GetDropTime()
	local BelongToTeam = IntObjFollower.m_Properties:GetBelongToTeam()
	if BelongToTeam ~= 0 then
		local TeamID = g_MainPlayer.m_Properties:GetTeamID()
		if TeamID == BelongToTeam then
			return true
		end
		if DropTime == 0 then
			return false
		end
		if os.time() - DropTime < DropItemProtectTime then
			return false
		end
	elseif OwnerId ~= 0 then
		if OwnerId == g_MainPlayer.m_uID then
			return true
		end
		if DropTime == 0 then
			return false
		end
		if os.time() - DropTime < DropItemProtectTime then
			return false
		end
	end
	
	if string.find(ActionName, "采集物品" ) then
		return CheckCollObj(ObjName)
	elseif ActionName == "可拾取的Obj" then
		return CheckItemObj(ObjName)
	elseif ActionName == "加任务计数" or ActionName == "加任务计数清除Obj" then
		return CheckAddQuestCountObj(ObjName)
	elseif ActionName == "小游戏" then
		return CheckSmallGameObj(ObjName)
	elseif string.find(ActionName,"踢") then
		local res = CheckCollObj(ObjName)
		if not res then
			res = CheckTickObj(ObjName)
		end
		return res
	end
	return true
end

function UpdateObjCanUseState()
	if not g_MainPlayer then
		return
	end
--print("UpdateObjCanUseState()")
	if g_MainPlayer.m_IntObjInVisionTbl 
		and table.maxn(g_MainPlayer.m_IntObjInVisionTbl ) >0 then
		for EntityID, IntObj in pairs(g_MainPlayer.m_IntObjInVisionTbl) do
			if IntObj then
				local ModelName = IntObj_Common(IntObj.m_Properties:GetCharName(), "ModelName")
				local ActionNum = IntObj.m_Properties:GetActionNum()
				if ActionNum ~= 0 then
--					if IsAction then
--						ObjFollowerDoAction(IntObj, ModelName, "stand0"..ActionNum)
--					else
						ObjFollowerDoAction(IntObj, ModelName, "stand0"..ActionNum)
--					end
				end
			end
		end
	end
end

function Gas2Gac:IntObjChangeModel(Conn, EntityID, ModelName, OldModelName)
--print("IntObjChangeModel")
	local IntObjFollower = CIntObjClient_GetIntObjByID(EntityID)
	if IntObjFollower == nil then
		return
	end
	if IntObjFollower.m_EffectId then
		IntObjFollower:GetRenderObject():DelEffect( IntObjFollower.m_EffectId )
		IntObjFollower.m_EffectId = nil
	end
	local RenderObj = IntObjFollower:GetRenderObject()
	--print("-----------------------IntObjChangeModel")
	--local OldPieceName = IntObjRes_Client(OldModelName, "PieceName")
	RenderObj:RemovePiece(nil)
	local IntObjRes = IntObjRes_Client(ModelName)
	if IntObjRes == nil then
		assert(false, "在IntObj_Client表中查找不到ModelName:"..ModelName)
	end
	RenderObj:AddPiece( RES_TYPE.RFT_ARA, IntObjRes("AniFile"), "", 0 )
	RenderObj:AddPiece( RES_TYPE.RFT_ARP, IntObjRes("ModelFile"), "", IntObjRes("RenderStyle") )
	RenderObj:SetScale( IntObjRes("Scaling") )
	ObjFollowerDoAction( IntObjFollower, ModelName, "stand01" )
end

function Gas2Gac:IntObjChangeState(Conn, EntityID, State)
--	print("IntObjChangeState")
	local IntObjFollower = CIntObjClient_GetIntObjByID(EntityID)
	if IntObjFollower == nil then
		return
	end
	if State == 1 and CheckUseCondition(IntObjFollower) then
		local ActionNum = IntObjFollower.m_Properties:GetActionNum()	
		local ModelName = IntObj_Common(IntObjFollower.m_Properties:GetCharName(), "ModelName")
		local config = IntObjRes_Client(ModelName, "AniFile")
		local fileName = string.gsub(config, "object/ani/", "fx/setting/obj/")
		fileName = string.gsub(fileName, ".ske", ".efx")
		--print("ObjDoEffect",fileName, "stand0"..ActionNum.."/create")
		IntObjFollower.m_EffectId = IntObjFollower:GetRenderObject():AddEffect( fileName, "stand0"..ActionNum.."/create", -1, nil)
		AddCanClickFx(IntObjFollower)
		IntObjFollower:SetCanBeSelected(true)--设置鼠标移动到Obj时能显示高亮
	else
		IntObjFollower:SetCanBeSelected(false)--设置鼠标移动到Obj时不能显示高亮
		if g_MainPlayer and g_MainPlayer.m_LockCenter.m_LockingIntObj == IntObjFollower then
			g_MainPlayer:UnLockIntObj()
		end
		if IntObjFollower.m_EffectId then
	--		print("DelEffect")
			IntObjFollower:GetRenderObject():DelEffect( IntObjFollower.m_EffectId )
			IntObjFollower.m_EffectId = nil
		end
		if IntObjFollower.m_CanClickFxId then
			IntObjFollower:GetRenderObject():DelEffect( IntObjFollower.m_CanClickFxId )
			IntObjFollower.m_CanClickFxId = nil
		end
	end
end

local function CheckIntObjCanBeSelect(IntObjFollower)
--	local ObjName = IntObjFollower.m_Properties:GetCharName()
	return IntObjFollower.m_Properties:GetCanUseState() == 1
end

--播放一次性动作,例如respond...的动作,然后恢复到stand....动作,就一直播
function Gas2Gac:NotifyIntObjDoAction(Conn, EntityID, RespondAction, NextActionNum)
	NotifyIntObjDoAction(EntityID, RespondAction, NextActionNum)
end

function NotifyIntObjDoAction(EntityID, RespondAction, NextActionNum)
	local IntObjFollower = CIntObjClient_GetIntObjByID(EntityID)
	if not IsCppBound(IntObjFollower) then
		return
	end
	
	local renderObj = IntObjFollower:GetRenderObject()
	local ModelName = IntObj_Common(IntObjFollower.m_Properties:GetCharName(), "ModelName")
	
	if CheckIntObjCanBeSelect(IntObjFollower) and CheckUseCondition(IntObjFollower) then
		local config = IntObjRes_Client(ModelName, "AniFile")
		local fileName = string.gsub(config, "object/ani/", "fx/setting/obj/")
		fileName = string.gsub(fileName, ".ske", ".efx")
		
		if IntObjFollower.m_EffectId then
			renderObj:DelEffect( IntObjFollower.m_EffectId )
			IntObjFollower.m_EffectId = nil
		end
		IntObjFollower.m_EffectId = renderObj:AddEffect( fileName, RespondAction.."/create", 0, nil)
		AddCanClickFx(IntObjFollower)
		IntObjFollower:SetCanBeSelected(true)
		
	else
		if IntObjFollower.m_EffectId then
			renderObj:DelEffect( IntObjFollower.m_EffectId )
			IntObjFollower.m_EffectId = nil
		end
		if IntObjFollower.m_CanClickFxId then
			renderObj:DelEffect( IntObjFollower.m_CanClickFxId )
			IntObjFollower.m_CanClickFxId = nil
		end
		IntObjFollower:SetCanBeSelected(false)--设置鼠标移动到Obj时不能显示高亮
		if g_MainPlayer and g_MainPlayer.m_LockCenter.m_LockingIntObj == IntObjFollower then
			g_MainPlayer:UnLockIntObj()
		end
	end
	
	renderObj:DoAni(RespondAction, false, -1.0 )
	IntObjFollower.NextActionNum = NextActionNum
	IntObjFollower.IsNeedCallback = true 
end

function Gas2Gac:NotifyIntObjEffect(Conn, EntityID, ObjEffectName)
	local IntObjFollower = CIntObjClient_GetIntObjByID(EntityID)
	if IntObjFollower == nil then
		return
	end
	local RenderObject = IntObjFollower:GetRenderObject()
	if IntObjFollower.m_EffectId then
		RenderObject:DelEffect( IntObjFollower.m_EffectId )
		IntObjFollower.m_EffectId = nil
	end
	local FxName = ObjEffect_Common(ObjEffectName, "EffectName")
	local FxFile = ObjEffect_Common(ObjEffectName, "FilePath")
	local PlayTime = 0
	if IntObjFollower.m_Properties:GetType() ~= ECharacterType.Trap then
		PlayTime = ObjEffect_Common(ObjEffectName, "PlayerTime")
	--else
		--RenderObject:CreateDefault()
	end
	IntObjFollower.m_EffectId = RenderObject:AddEffect( FxFile, FxName, PlayTime, nil )
end

--播放持续的动作,例如stand...的动作
function ObjFollowerDoAction(IntObjFollower, ModelName, ActionName)
	local ActionNum = IntObjFollower.m_Properties:GetActionNum()
	--if IntObjFollower.m_Properties:GetCanUseState() == 1 and CheckUseCondition(IntObjFollower) then
	if CheckIntObjCanBeSelect(IntObjFollower) and CheckUseCondition(IntObjFollower) then
		local config = IntObjRes_Client(ModelName, "AniFile")
		local fileName = string.gsub(config, "object/ani/", "fx/setting/obj/")
		fileName = string.gsub(fileName, ".ske", ".efx")
		
		--print("string.find(ActionName, stand")
		if IntObjFollower.m_EffectId then
			IntObjFollower:GetRenderObject():DelEffect( IntObjFollower.m_EffectId )
			IntObjFollower.m_EffectId = nil
		end
		IntObjFollower.m_EffectId = IntObjFollower:GetRenderObject():AddEffect( fileName, ActionName.."/create", -1, nil )
		
		AddCanClickFx(IntObjFollower)
		IntObjFollower:SetCanBeSelected(true)
	else
		if IntObjFollower.m_EffectId then
			IntObjFollower:GetRenderObject():DelEffect( IntObjFollower.m_EffectId )
			IntObjFollower.m_EffectId = nil
		end
		if IntObjFollower.m_CanClickFxId then
			IntObjFollower:GetRenderObject():DelEffect( IntObjFollower.m_CanClickFxId )
			IntObjFollower.m_CanClickFxId = nil
		end
		IntObjFollower:SetCanBeSelected(false)--设置鼠标移动到Obj时不能显示高亮
		if g_MainPlayer and g_MainPlayer.m_LockCenter.m_LockingIntObj == IntObjFollower then
			g_MainPlayer:UnLockIntObj()
		end
	end
	IntObjFollower:CIntObjDoActor(ActionName)
end

function GetIntObjDispalyName(name)
	assert(not IsNil(Lan_IntObj_Common(MemH64(name))),name .. "的IntObj没有显示名")
	return Lan_IntObj_Common(MemH64(name), "DisplayName")
end


