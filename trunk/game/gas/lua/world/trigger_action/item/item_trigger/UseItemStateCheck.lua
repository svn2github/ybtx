gas_require "world/trigger_action/item/item_trigger/UseItemOnSelf"
gas_require "world/trigger_action/item/item_trigger/UseItemOnTarget"
gas_require "world/trigger_action/item/item_trigger/UseItemOnPos"
gac_gas_require "activity/item/ItemUseInfoMgr"
lan_load "skill/Lan_BuffName"

--挂在Player上的东东太多了
--与物品使用相关都挂到Conn.m_Player.UseItemParam[ItemInfo.ItemName]上
--物品使用流程结束再把它nil掉
function UseItemEnd(Conn,ItemInfo,MsgID,...)
	local arg = {...}
	if Conn and Conn.m_Player then
		local tbl = Conn.m_Player.UseItemParam[ItemInfo.ItemName]
		if tbl then
			if tbl.LockItemTbl then
				for i,v in pairs(tbl.LockItemTbl) do
					Gas2Gac:SetItemGridWndState(Conn, v["nRoomIndex"], v["nPos"], true)
				end
			end
			Conn.m_Player.UseItemParam[ItemInfo.ItemName] = nil
			if IsNumber(MsgID) then
				MsgToConn(Conn, MsgID, ...)
			end
		end
	end
end

function Gac2Gas:UseItemEnd(Conn,ItemName)
	if IsCppBound(Conn) and IsCppBound(Conn.m_Player) then
		local Player = Conn.m_Player
		if ItemName == "" then
			if Player.m_UseItemLoadingTick then
				StopItemProgressTick(Player)
			end
		else
			local ItemInfo = {}
			ItemInfo.ItemName = ItemName
			UseItemEnd(Conn, ItemInfo)
		end
	end
end

function LockItem(Conn,ItemInfo)
	if Conn.m_Player:TestDBTransBlock() then
		return false
	end
	return true
end

--玩家与目标的距离
local function CheckUseRange(Player, Target)
	local PlayerPos = CFPos:new()
	local TargetPos = CFPos:new()
	Player:GetPixelPos(PlayerPos)
	Target:GetPixelPos(TargetPos)
	return CheckDist(PlayerPos,TargetPos)
end

--坐标A与坐标B的距离
function CheckDist(PosA,PosB)
	local a = PosA.x - GetPixelOfGrid(math.floor(PosB.x / EUnits.eGridSpanForObj), EUnits.eGridSpanForObj / 2)
	local b = PosA.y - GetPixelOfGrid(math.floor(PosB.y / EUnits.eGridSpanForObj), EUnits.eGridSpanForObj / 2)
	local dis = math.sqrt(a * a + b * b) / EUnits.eGridSpanForObj
	return dis
end

function GetCreatePos(Player)
	if Player == nil then
		return
	end
	local pos = CFPos:new() 
	Player:GetPixelPos(pos)
	pos.x = pos.x/EUnits.eGridSpanForObj
	pos.y = pos.y/EUnits.eGridSpanForObj
	return pos
end

local function TrunByDir(Player, dir)
--	print("TrunByDir(Player, dir)")
	local Dir = CDir:new()
	Dir:Set(dir)
	Player:SetAndSyncActionDir(Dir.uDir)
end

local function PlayerTrunToPos(TrunPos, Player)
--	print("PlayerTrunToPos(TrunPos, Player)")
	local vecDir = CVector2f:new() 
	local PlayerPos = CPos:new()
	Player:GetGridPos(PlayerPos)
	
	vecDir.x = TrunPos.x - PlayerPos.x 
	vecDir.y = TrunPos.y - PlayerPos.y 
	TrunByDir(Player, vecDir)
end

local function PlayerTrun(Conn, pos, ItemInfo)
	local UseParam = Conn.m_Player.UseItemParam[ItemInfo.ItemName]
	if not UseParam then
		return
	end
	if UseParam.IsOnTarget then
		if IsCppBound(UseParam.IsOnTarget) then
			local TargetPos = CPos:new()
			UseParam.IsOnTarget:GetGridPos(TargetPos)
			PlayerTrunToPos(TargetPos, Conn.m_Player)
		end
	elseif UseParam.IsOnPos or 
				 UseParam.IsOnRangeSelect then
		PlayerTrunToPos(pos, Conn.m_Player)
	end
end

local function PlayerTrunByNoLoadProgress(Conn, pos, ItemInfo)
	PlayerTrun(Conn, pos, ItemInfo)
end

local function GetBuffDisplayName(sBuffName)
	local displayNameIndex = MemH64(sBuffName)
	if Lan_BuffName(displayNameIndex) then
		local displayName = Lan_BuffName(displayNameIndex,"DisplayName")
		return displayName
	end
end

--判断血量下限 即目标NPC血量大于参数Requireparam
--@param Requireparam 血量下限值或总血量的百分比 如血量大于50%为0.5
--@return 目标NPC血量大于参数Requireparam则返回False,否则返回True
local function LifeLowerLimit(Conn,Requireparam,Target,ItemName)
	
	local TargetHP = Target:CppGetHP()--NPC 当前HP
	local TargetMaxHP = Target:CppGetMaxHP()--NPC 最大HP

	if Requireparam < 1 then		--判断需求参数是否是血量大于XX%
		if TargetHP > (TargetMaxHP*Requireparam) then 
			return false;
		else
			MsgToConn(Conn,808,(Requireparam*100),ItemName)
			return true
		end		
	else
		if TargetHP > Requireparam then
			return false;
		else
			MsgToConn(Conn,809,Requireparam,ItemName)
			return true
		end
	end
end

--判断血量上限
--类同上.血量下限LifeLowerLimit(Conn,Requireparam)
local function LifeUpperLimit(Conn,Requireparam,Target,ItemName)

	local IsAlive = Target:CppIsLive()     
	if IsAlive == false then               
		return true --"目标已死亡"                  
	end        
	                            
	local TargetHP = Target:CppGetHP()--NPC 当前HP
	local TargetMaxHP = Target:CppGetMaxHP()--NPC 最大HP
	
	if Requireparam <= 1 then		--判断需求参数是否是血量少于XX%
		
		if TargetHP < (TargetMaxHP*Requireparam) then
			return false;
		else
			MsgToConn(Conn,810,(Requireparam*100),ItemName)
			return true
		end		
	else	
		if TargetHP < Requireparam then
			return false;
		else
			MsgToConn(Conn,811,Requireparam,ItemName)
			return true
		end
	end
end

--判断魔量下限
--类同上.血量下限LifeLowerLimit(Conn,Requireparam)
local function ManaLowerLimit(Conn,Requireparam,Target,ItemName)

	local IsAlive = Target:CppIsLive()     
	if IsAlive == false then               
		return true --"目标已死亡"                  
	end        
	   
	local TargetMP = Target:CppGetMP()--NPC 当前MP
	local TargetMaxMP = Target:CppGetMaxMP()--NPC 最大MP

	if Requireparam < 1 then		--判断需求参数是否是魔量大于XX%
		
		if TargetMP > (TargetMaxMP*Requireparam) then --总魔量.?!
			return false;
		else
			MsgToConn(Conn,812,(Requireparam*100),ItemName)
			return true
		end		
	else
		if TargetMP > Requireparam then
			return false;
		else
			MsgToConn(Conn,813,Requireparam,ItemName)
			return true
		end
	end
end

--判断魔量上限
--类同上.血量上限LifeUpperLimit(Conn,Requireparam)
local function ManaUpperLimit(Conn,Requireparam,Target,ItemName)
	if Target == nil then
		return true --"没有选择正确的使用对象"
	end	
	local TargetMP = Target:CppGetMP()--NPC 当前MP
	local TargetMaxMP = Target:CppGetMaxMP()--NPC 最大MP
 
	if Requireparam <= 1 then		--判断需求参数是否是魔量少于XX%
		
		if TargetMP < (TargetMaxMP*Requireparam) then
			return false;
		else
			MsgToConn(Conn,814,(Requireparam*100),ItemName)
			return true
		end		
	else	
		if TargetMP < Requireparam then
			return false;
		else
			MsgToConn(Conn,815,Requireparam,ItemName)
			return true
		end
	end
end

--判断NPC状态
local function NpcState(Conn,Requireparam,Target,ItemName)
	if Target == nil then
		return true --"没有选择正确的使用对象"
	end		
	local IsAlive = Target:CppIsLive()--NPC 当前状态

	if Requireparam == "死亡" then
		if IsAlive == false then
			return IsAlive;
		else
			MsgToConn(Conn,816,ItemName)
			return true
		end
		
	elseif Requireparam == "未死亡" then
		if IsAlive == true then
			return false;
		else
			MsgToConn(Conn,817,ItemName)
			return true
		end
	elseif Requireparam == "战斗" then
		if Target:CppGetCtrlState(EFighterCtrlState.eFCS_InBattle) then
			return false
		else
			MsgToConn(Conn,818,ItemName)
			return true		
		end
	else
		local IsState = Target:ExistState(Requireparam)
		if not IsState then
			MsgToConn(Conn,140000,Target.m_Properties:GetCharName(),GetBuffDisplayName(Requireparam),ItemName)
			return true	
		else
			return false;
		end
	end
end

--判断NPC不在某状态
local function NpcNotState(Conn,Requireparam,Target,ItemName)
	if Target == nil then
		return true --"没有选择正确的使用对象"
	end		
	if Requireparam == "战斗" then
		if not Target:CppGetCtrlState(EFighterCtrlState.eFCS_InBattle) then
			return false
		else
			MsgToConn(Conn,836,ItemName)--目标在战斗状态，%s使用失败
			return true		
		end
	end
	local IsState = Target:ExistState(Requireparam)
	if IsState then
		MsgToConn(Conn,837,GetBuffDisplayName(Requireparam),ItemName)--目标还在%s，%s使用失败
		return true	
	else  
		return false
	end
end

--判断NPC阵营
local function TargetCamp(Conn,Requireparam,Target,ItemName)
	if Target == nil then
		return true --"没有选择正确的使用对象"
	end		
	local targetCamp = Target:CppGetCamp()
	if Requireparam == targetCamp then
		return false
	else
		MsgToConn(Conn,819,ItemName)
		return true	
	end
end

local CheckNpcStateFuncTbl =
{
	["血量小于"] = LifeUpperLimit,
	["血量大于"] = LifeLowerLimit,
	["魔量小于"] = ManaUpperLimit,
	["魔量大于"] = ManaLowerLimit,
	["目标状态"] = NpcState,
	["目标不在状态"] = NpcNotState,
	["目标阵营"] = TargetCamp,

}
--对目标状态的筛选
local function CheckNpcState(Conn,ItemInfo,Target)
	local ItemName = ItemInfo.ItemName
	local TargetStateRequire = ItemInfo.TargetStateRequire
	
	if TargetStateRequire == nil or TargetStateRequire == "" then
		return NpcState(Conn,"未死亡",Target,ItemName)
	end
	local RequireName = TargetStateRequire[1]
	local Requireparam = TargetStateRequire[2]

--	if RequireName ~= "目标状态" or Requireparam ~= "死亡" then
--		local IsAlive = Target:CppIsLive()     
--		if IsAlive == false then               
--			return "目标已死亡"                  
--		end  
--	end
	return CheckNpcStateFuncTbl[RequireName](Conn,Requireparam,Target,ItemName)
--	if RequireName == "血量大于" then
--		return LifeLowerLimit(Conn,Requireparam,Target,ItemName)
--	elseif RequireName == "血量小于" then
--		return LifeUpperLimit(Conn,Requireparam,Target,ItemName)
--	elseif RequireName == "魔量大于" then
--		return ManaLowerLimit(Conn,Requireparam,Target,ItemName)
--	elseif RequireName == "魔量小于" then
--		return ManaUpperLimit(Conn,Requireparam,Target,ItemName)
--	elseif RequireName == "目标状态" then
--		return NpcState(Conn,Requireparam,Target,ItemName)
--	elseif RequireName == "目标不在状态" then
--		return NpcNotState(Conn,Requireparam,Target,ItemName)
--	elseif RequireName == "目标阵营" then
--		return TargetCamp(Conn,Requireparam,Target,ItemName)
--	end
end

local function CheckIsTrap(Conn, AreaLimit)
	local f = Conn.m_Player.m_Properties.m_InTrapTblName[AreaLimit]
	if f ~= nil and next(f) then
	 	return false
	end
	return 820,AreaLimit
end

local function CheckIsScene(Conn, AreaLimit)
	local SceneName = Conn.m_Player.m_Scene.m_SceneName
	for i = 2, #AreaLimit do
		if AreaLimit[i] == SceneName then
			return false
		end
	end
	return 840,AreaLimit[2]
end

local function CheckIsSceneType(Conn, AreaLimit)
	local sceneType = Conn.m_Player.m_Scene.m_SceneAttr.SceneType
	for i = 2, #AreaLimit do
		if AreaLimit[i] == sceneType then
			return false
		end
	end
	return 847, AreaLimit[2], true
end

local function CheckIsArea(Conn, AreaLimit)
	local pos = CPos:new()
	Conn.m_Player:GetGridPos(pos)
	local AreaName = g_AreaMgr:GetSceneAreaName(Conn.m_Player.m_Scene.m_SceneName, pos)
	if AreaName ~= nil and AreaName == AreaLimit then
		return false
	end
	return 839,AreaLimit
end

--物品使用区域限制
local function AreaLimit(Conn,ItemInfo)
	if ItemInfo.AreaLimit == nil or ItemInfo.AreaLimit == "" then
		return false
	end
	local AreaLimit = ItemInfo.AreaLimit
--	for i,v in pairs(AreaLimit) do
--		local f = Conn.m_Player.m_Properties.m_InTrapTblName[v]
--		if f ~= nil and table.maxn(f) ~= 0 then
--			print("在"..v.."范围内,使用"..ItemInfo.ItemName)
--		 	return false;
--		end
--		AreaNameStr = AreaNameStr.."("..v..")"
--	end
	if AreaLimit[1] == "Trap" then
		return CheckIsTrap(Conn, AreaLimit[2])
	elseif AreaLimit[1] == "区域" then
		return CheckIsArea(Conn, AreaLimit[2])
	elseif AreaLimit[1] == "场景" then
		return CheckIsScene(Conn, AreaLimit)
	elseif AreaLimit[1] == "场景类型" then
		return CheckIsSceneType(Conn, AreaLimit)
	end
end

--检查对目标使用的状态需求
local function CheckEffectOnTarget(Conn, ItemInfo, TargetType, TargetName)

	local Player = Conn.m_Player
	local Target = Conn.m_Player.m_LockingObj or Conn.m_Player.m_LockingIntObj
	
	if Target == nil or not IsServerObjValid(Target) then
		MsgToConn(Conn,804,ItemInfo.ItemName)
		UseItemEnd(Conn,ItemInfo)
		return
	end
	
	if ItemInfo.UseRange ~= nil and ItemInfo.UseRange ~= "" then
		local dis = CheckUseRange(Player, Target)
		if dis > ItemInfo.UseRange then
			MsgToConn(Conn,806,ItemInfo.ItemName)
			UseItemEnd(Conn,ItemInfo)
			return
		end
	end
	
	if TargetType == "Npc" then
		if Target.m_Properties:GetType() ~= ECharacterType.Npc then
			MsgToConn(Conn,805,ItemInfo.ItemName)
			UseItemEnd(Conn,ItemInfo)
			return
		end
	elseif TargetType == "Object" then
		if Target.m_Properties:GetType() ~= ECharacterType.IntObj then
			MsgToConn(Conn,805,ItemInfo.ItemName)
			UseItemEnd(Conn,ItemInfo)
			return
		end
	end
	
	local name = Target.m_Properties:GetCharName()
	if TargetName ~= "任意" and name ~= TargetName and GetNpcTitleName(name) ~= TargetName then
		MsgToConn(Conn,805,ItemInfo.ItemName)
		UseItemEnd(Conn,ItemInfo)
		return
	end
	
	if TargetType == "Npc" then
		local result = CheckNpcState(Conn,ItemInfo,Target)
		if result then
			UseItemEnd(Conn,ItemInfo)
			return
		end
	end
	
	if Conn.m_Player.UseItemParam[ItemInfo.ItemName] == nil then
		return
	end
	Conn.m_Player.UseItemParam[ItemInfo.ItemName].IsOnTarget = Target
	UseItemEffectOnTarget(Conn, ItemInfo, Target)		--对目标使用物品
end

--范围内筛选对象
local function UseItemEffectOnRangeSelect(Conn, ItemInfo, pos)
	local CoreScene = Conn.m_Player.m_Scene.m_CoreScene
	local QueryMgr = CGameGridRadiusCallback:new()
	local QueryPos = CPos:new()
	QueryPos.x = pos.x
	QueryPos.y = pos.y
	QueryMgr:QueryObjs(CoreScene, QueryPos, 5)
	local NpcTbl = QueryMgr:QueryByECharacterType(ECharacterType.Npc)	
	for i,v in pairs(NpcTbl) do
		local TargetEntityID = v:GetEntityID()
		local EntityID = Conn.m_Player:GetEntityID()
--		print(i)
	end
end

--调用客户端 选择光圈
local function EffectOnPos(Conn,ItemInfo,nSize)
	local nRange = ItemInfo.UseRange
	Gas2Gac:CreatItemSelector(Conn,nSize,ItemInfo.ItemName,nRange)
end

--检查对地点使用的状态需求
local function CheckEffectOnPos(Conn, ItemInfo, pos)
	if ItemInfo.UseRange ~= nil and ItemInfo.UseRange ~= "" then
		local PlayerPos = CFPos:new()
		Conn.m_Player:GetPixelPos(PlayerPos)
		local dis = CheckDist(PlayerPos,pos)
		if dis > ItemInfo.UseRange then
			MsgToConn(Conn,824,ItemInfo.ItemName)
			UseItemEnd(Conn,ItemInfo)
			return
		end
	end
	pos.x = pos.x/EUnits.eGridSpanForObj
	pos.y = pos.y/EUnits.eGridSpanForObj
--	if Conn.m_Player.UseItemParam[ItemInfo.ItemName].IsOnPos then
		UseItemEffectOnPos(Conn, ItemInfo, pos)		--对地点使用物品
--	elseif Conn.m_Player.UseItemParam[ItemInfo.ItemName].IsOnRangeSelect then
--		UseItemEffectOnRangeSelect(Conn, ItemInfo, pos)
--	end
	
end

--选择地点 (*注意*传过来的pos是像素坐标)
function Gac2Gas:SelectArea(Conn,fPosX,fPosY,fPosZ,ItemName) 
	local UseParam = Conn.m_Player.UseItemParam[ItemName]
	if not UseParam then
		return
	end
	Conn.m_Player.UseItemParam[ItemName].OnPosEffectVector3f = {}
	Conn.m_Player.UseItemParam[ItemName].OnPosEffectVector3f.x = fPosX
	Conn.m_Player.UseItemParam[ItemName].OnPosEffectVector3f.y = fPosY
	Conn.m_Player.UseItemParam[ItemName].OnPosEffectVector3f.z = fPosZ
	local pos = CFPos:new()
	pos.x = fPosX
	pos.y = fPosZ
	local ItemInfo = g_ItemUseInfoMgr:GetItemUseInfo(ItemName)
	CheckEffectOnPos(Conn, ItemInfo, pos)
end

--使用类型选择('自身', 对目标'Npc'或'Object', '地点')
local function ChooseUseType(Conn, ItemInfo)
	local InfTarget = ItemInfo.InfluenceTarget
	local TargetType = InfTarget[1]
	local TargetName = InfTarget[2]
	if not Conn.m_Player or not Conn.m_Player.UseItemParam then
		return
	end
	if Conn.m_Player.UseItemParam[ItemInfo.ItemName] == nil then
		return
	end
	if TargetType == "自身" then
		UseItemEffectOnSelf(Conn, ItemInfo)						--对自身使用物品
	elseif TargetType == "Npc" or TargetType == "Object" then
--		Conn.m_Player.UseItemParam[ItemInfo.ItemName].IsOnTarget = true
		CheckEffectOnTarget(Conn, ItemInfo, TargetType, TargetName)
	elseif TargetType == "地点" then
		Conn.m_Player.UseItemParam[ItemInfo.ItemName].IsOnPos = true
		EffectOnPos(Conn,ItemInfo,TargetName)
--	elseif TargetType == "范围筛选" then
--		Conn.m_Player.UseItemParam[ItemInfo.ItemName].IsOnRangeSelect = true
--		EffectOnPos(Conn,ItemInfo,TargetName)
	end
end

local function IsCanUseItem(Conn, ItemInfo)
	if Conn.m_Player.m_Properties:GetTongID() == 0 then
		MsgToConn(Conn,450001)
		UseItemEnd(Conn,ItemInfo)
		return
	end
	--local NeedPurview = g_ItemInfoMgr:GetItemInfo(ItemInfo.BigID,ItemInfo.Arg[1][1],"NeedPurview")
	--if NeedPurview and NeedPurview > 0 then
	local data = {}
	data["uPlayerId"] = Conn.m_Player.m_uID
	data["PurviewName"] = "CanUseTongItem"
	local function CallBack(result)
		if not result then
			MsgToConn(Conn,450001)
			UseItemEnd(Conn,ItemInfo)
			return
		end
		local MsgID, str, flag = AreaLimit(Conn,ItemInfo)
		if MsgID then
			if flag then
				MsgToConn(Conn,MsgID)
				UseItemEnd(Conn,ItemInfo)
				return
			end
			MsgToConn(Conn,MsgID,str,ItemInfo.ItemName)
			UseItemEnd(Conn,ItemInfo)
			return
		end
		--以上,通用检查完毕
		ChooseUseType(Conn, ItemInfo)		--使用类型选择
	end
	CallAccountManualTrans(Conn.m_Account, "GasTongBasicDB", "IsPurviewEnough", CallBack, data)
	--end	
end

--物品使用条件检查
local function AllStateCheck(Conn, ItemInfo)
	
	--刷新出的NPC(刷怪器)在同一时间只能刷一只
	local CreatTbl = ItemInfo.Arg[1]
	if CreatTbl and CreatTbl[1] == "Npc" and RandomCreate_Server(CreatTbl[2]) ~= nil then
		if Conn.m_Player.UseItemParam.CreateTbl and Conn.m_Player.UseItemParam.CreateTbl[CreatTbl[2]] then
			MsgToConn(Conn,822,CreatTbl[2],ItemInfo.ItemName)
			UseItemEnd(Conn,ItemInfo)
			return 
		end
	end
	
	local NeedPurview = g_ItemInfoMgr:GetItemInfo(ItemInfo.BigID,ItemInfo.ItemName,"NeedPurview")
	if ItemInfo.BigID == 10 and NeedPurview == 1 then
		IsCanUseItem(Conn, ItemInfo)
		return
	end

	--区域限制 应当在TRAP范围内使用
	local MsgID,str = AreaLimit(Conn,ItemInfo)
	if MsgID then
		MsgToConn(Conn,MsgID,str,ItemInfo.ItemName)
		UseItemEnd(Conn,ItemInfo)
		return
	end
	--以上,通用检查完毕
	
	ChooseUseType(Conn, ItemInfo)		--使用类型选择
end

--从物品使用服务端脚本直接调来先检查物品消耗是否满足
function ItemNumCheck(Conn, ItemInfo, RoomIndex, Pos)
	local ItemName = ItemInfo.ItemName
	local Player = Conn.m_Player
	local castType = ItemInfo.CastType[1]
	local function CallBack(result1)
		if not IsCppBound(Conn) then
			return
		end
		local Player = Conn.m_Player
		if not IsCppBound(Player) then
			return
		end
		if result1 then
			if castType ~= "不读条" then
				for i,v in pairs(result1) do
					if v["nRoomIndex"] ~= RoomIndex or v["nPos"] ~= Pos then
						Gas2Gac:SetItemGridWndState(Conn, v["nRoomIndex"], v["nPos"], false)
						table.insert(Player.UseItemParam[ItemName].LockItemTbl, v)
					end
				end
			end
			Player.UseItemParam[ItemName].ConsumeItemTbl = result1
			AllStateCheck(Conn, ItemInfo)		--消耗品足够继续检查其余项
		else
			MsgToConn(Conn,827,ItemName)
			UseItemEnd(Conn,ItemInfo)
		end
	end

	--检查要使用的物品是否在冷却中
	if Player.m_SpecialItemCoolDown and Player.m_SpecialItemCoolDown[ItemName] then
		MsgToConn(Conn,821,ItemName)
		UseItemEnd(Conn,ItemInfo)
		return
	end
	if castType ~= "不读条" then
		Player.UseItemParam[ItemName].LockItemTbl = {}
		local tbl = {}
		tbl["nRoomIndex"] = RoomIndex
		tbl["nPos"] = Pos
		Gas2Gac:SetItemGridWndState(Conn, RoomIndex, Pos, false)
		table.insert(Player.UseItemParam[ItemName].LockItemTbl, tbl)
	end
	local ConsumeItemTbl = ItemInfo.ItemConsume
	if #ConsumeItemTbl == 0 then
		AllStateCheck(Conn, ItemInfo)		--消耗品足够继续检查其余项
	else
		local parameter = {}
		parameter["ItemType"] = ItemInfo.BigID
		parameter["ItemName"] = ItemName
		parameter["RoomIndex"] = RoomIndex
		parameter["Pos"] = Pos
		for i,v in pairs(ConsumeItemTbl) do
			local ItemTbl = {}
			ItemTbl["nCharId"] = Conn.m_Player.m_uID
			ItemTbl["ItemType"] = v[1]
			ItemTbl["ItemName"] = v[2]
			ItemTbl["ItemNum"] = v[3]
			parameter[i] = ItemTbl
		end
		if not LockItem(Conn,ItemInfo) then
			UseItemEnd(Conn,ItemInfo,802)
			return
		end
		CallAccountManualTrans(Conn.m_Account, "UseSpecialItemDB", "GetItemListNum", CallBack, parameter)
	end
end

local function EffectPlay(Conn,ItemInfo)
	local PlayerEffectTbl = ItemInfo.EffectName[1]
	local TargetEffectTbl = ItemInfo.EffectName[2]
	local UseParam = Conn.m_Player.UseItemParam[ItemInfo.ItemName]
	if PlayerEffectTbl and table.maxn(PlayerEffectTbl) ~= 0 then
		local ModelString = PlayerEffectTbl[1]
		local StateString = PlayerEffectTbl[2]
		Gas2Gac:UseItemPlayerEffect(Conn,ModelString,StateString)
		Gas2Gac:UseItemTargetEffect(Conn.m_Player:GetIS(0),ModelString,StateString,Conn.m_Player:GetEntityID())
	end
	if TargetEffectTbl and table.maxn(TargetEffectTbl) ~= 0 then
		local ModelString = TargetEffectTbl[1]
		local StateString = TargetEffectTbl[2]
		if UseParam.IsOnPos then
			local v3Pos = UseParam.OnPosEffectVector3f
--			Conn.m_Player.UseItemParam[ItemInfo.ItemName].OnPosEffectVector3f = nil
--			Conn.m_Player.UseItemParam[ItemInfo.ItemName].IsOnPos = nil
			Gas2Gac:UseItemOnPosEffect(Conn,ModelString,StateString,v3Pos.x,v3Pos.y,v3Pos.z)
			Gas2Gac:UseItemOnPosEffect(Conn.m_Player:GetIS(0),ModelString,StateString,v3Pos.x,v3Pos.y,v3Pos.z)
		elseif UseParam.IsOnTarget 
				and IsCppBound(UseParam.IsOnTarget) then
			local Npc = UseParam.IsOnTarget
			local NpcId = Npc:GetEntityID()
--			Conn.m_Player.UseItemParam[ItemInfo.ItemName].IsOnTarget = nil
			Gas2Gac:UseItemTargetEffect(Conn,ModelString,StateString,NpcId)		
			Gas2Gac:UseItemTargetEffect(Npc:GetIS(0),ModelString,StateString,NpcId)
		else
			local pos = CFPos:new()
			Conn.m_Player:GetPixelPos(pos)
			Gas2Gac:UseItemOnPosEffect(Conn,ModelString,StateString,nil,nil,nil)
			Gas2Gac:UseItemOnPosEffect(Conn.m_Player:GetIS(0),ModelString,StateString,pos.x,nil,pos.y)
		end
	end
end

--使用任务物品计数
local function UseQuestItem(Conn,ItemInfo)
	local varname = "使用Item"..ItemInfo.ItemName
	if ItemInfo.QuestRequire ~= nil and ItemInfo.QuestRequire ~= "" then
		local RequireQuestTbl = assert(ItemInfo.QuestRequire)				--物品使用表中任务需求描述
		for _, questname in pairs(RequireQuestTbl) do
			g_AddQuestVarNumForTeam(Conn.m_Player, questname, varname, 1)
		end
	end
end

--转冷却
local function BeginCoolDown(Conn,ItemInfo)
	local Player = Conn.m_Player
	local CoolDownTime = ItemInfo.CoolDownTime
	if CoolDownTime == nil or CoolDownTime == "" or CoolDownTime == 0 then
		return
	end
	local ItemName = ItemInfo.ItemName
	local CoolDownType = 0
	local ConsumeTbl = ItemInfo.ItemConsume
	for i,v in pairs(ConsumeTbl) do
		if v[2] == ItemName then
			 CoolDownType = v[3]							--如果ItemConsume有更改.这里也要改动
		end
	end
	if Player.UseItemParam[ItemName] == nil then
		return
	end
	--因为客户端冷却时间旋转需要判断是否是消耗物品所以这里加了一个CoolDownType
	Gas2Gac:SpecailItemCoolDown(Conn,ItemName,CoolDownTime,CoolDownType,Player.UseItemParam[ItemName].RoomIndex,Player.UseItemParam[ItemName].RoomPos)
	local function coolDownEnd(itemName)
		if Player.m_SpecialItemCoolDown then
			Player.m_SpecialItemCoolDown[itemName] = nil
		end
	end
	if Player.m_SpecialItemCoolDown == nil then
		Player.m_SpecialItemCoolDown = {}
	end
	Player.m_SpecialItemCoolDown[ItemName] = os.time()
	RegisterOnceTick(Player, "SpecialItemCoolDownTick",coolDownEnd,CoolDownTime, ItemName)
--	if CoolDownTime > 60000 then
--		local data = {}
--		data["CharId"] = Player.m_uID
--		data["ItemName"] = ItemName
--		CallAccountAutoTrans(Conn.m_Account, "UseSpecialItemDB", "UseItem", nil, data)
--	end
end

--消耗同时增加道具
--local function ConsumeAndGetItems(Conn,ItemInfo)
--
--	local Player = Conn.m_Player
--	
--	local function CallBack(result,result2,result3)
--	
--		
--		if not result then
--			if type(result2) == "string" then
--				MsgToConn(Conn, 8003, result2)
--				return
--			elseif type(result2) == "number" then
--				if result2 == 3 then
--					MsgToConn(Conn, 8003, '包裹空间不足')
--				elseif result2 == 0 then
--					MsgToConn(Conn, 8003, 'CreateFightItem error')
--				end
--				return
--			end
--		end
--		
--		if IsCppBound(Player) then
--			if result2 then
--				for i,itemtbl in pairs(result2) do
--					delete_item_DB_Ret(Player, result2[i][1], result2[i][2], result2[i][3], result2[i][4])
--				end
--			end
--			
--			if result3 then
--				for i,itemtbl in pairs(result3) do
--					add_item_DB_Ret(Player, itemtbl["nType"],itemtbl["sName"],itemtbl["nCount"],itemtbl["AddItemRes"])
--				end
--			end
--		end
--	end
--
--	local ConsumeTbl, GievnItemTbl
--	local parameter = {}
--	if ItemInfo.ItemConsume ~= nil and ItemInfo.ItemConsume ~= "" and Player.UseItemParam[ItemInfo.ItemName] ~= nil then
--		ConsumeTbl = ItemInfo.ItemConsume
--		for i,v in pairs(ConsumeTbl) do
--			if v[2] == ItemInfo.ItemName then
--				ConsumeTbl[i][4] = Player.UseItemParam[ItemInfo.ItemName].RoomIndex
--				ConsumeTbl[i][5] = Player.UseItemParam[ItemInfo.ItemName].RoomPos
--			end
--		end
--	end
--	
--	if ItemInfo.GetItem ~= nil and ItemInfo.GetItem ~= "" then
--		GievnItemTbl = ItemInfo.GetItem
--	end
--	
--	if table.maxn(ConsumeTbl) == 0 and table.maxn(GievnItemTbl) == 0 then
--		return
--	end
--	parameter["PlayerId"] = Player.m_uID
--	parameter["ConsumeTbl"] = ConsumeTbl
--	parameter["AddItemsTbl"] = GievnItemTbl
--	CallAccountManualTrans(Conn.m_Account, "RoleQuestDB", "DelAndAddItem", CallBack, parameter)
--end

--消耗同时增加道具
local function ConsumeAndGetItems(Conn,ItemInfo)
	if not LockItem(Conn,ItemInfo) then
		UseItemEnd(Conn,ItemInfo,802)
		return
	end
	local Player = Conn.m_Player
	local function CallBack(result,result2,result3)
		if not result then
			if type(result2) == "string" then
				MsgToConn(Conn, 8019)
				return
			elseif type(result2) == "number" then
				if result2 == 3 then
					MsgToConn(Conn, 160000)
--				elseif result2 == 0 then
--					MsgToConn(Conn, 8003, 'CreateFightItem error')
				end
				return
			end
		end
		if IsCppBound(Player) then
			if result2 then
--				for i,itemtbl in pairs(result2) do
					CRoleQuest.delete_item_DB_Ret(Player, nil, nil, nil, result2)
--				end
			end
			
			if result3 then
				for i,itemtbl in pairs(result3) do
					CRoleQuest.add_item_DB_Ret(Player, itemtbl["nType"],itemtbl["sName"],itemtbl["nCount"],itemtbl["AddItemRes"])
				end
			end
		end
	end
	local parameter = {}
	local GievnItemTbl
	if ItemInfo.GetItem ~= nil and ItemInfo.GetItem ~= "" then
		GievnItemTbl = ItemInfo.GetItem
	end
	parameter["PlayerId"] = Player.m_uID
	parameter["ConsumeTbl"] = Conn.m_Player.UseItemParam[ItemInfo.ItemName].ConsumeItemTbl
	parameter["AddItemsTbl"] = GievnItemTbl
	parameter["sceneName"] = Conn.m_Player.m_Scene.m_SceneName
	
	CallAccountManualTrans(Conn.m_Account, "CharacterMediatorDB", "DelAndAddItem", CallBack, parameter)
end

function BeginProgress(Conn,ItemInfo,succFunc,FailFunc,pos,Target)
	if Conn.m_Player.UseItemParam[ItemInfo.ItemName] == nil then
		return
	end
	local Progress = ItemInfo.CastType
	local ProgressType = Progress[1]
	local ProgressTime = Progress[2]	
	if ItemInfo.effect ~= "对自身释放技能" and ItemInfo.effect ~= "对目标释放技能" and ItemInfo.effect ~= "对地点释放技能" then
		Conn.m_Player.UseItemParam[ItemInfo.ItemName].IsDoAction = 1
	end
	if ItemInfo.ItemName == "高爆炸弹" then
		Conn.m_Player.UseItemParam[ItemInfo.ItemName].IsDoAction = 2
	end
	local state = ItemInfo.HorseState
	if state and state ~= "" and state == 1 and Conn.m_Player:CppGetCtrlState(EFighterCtrlState.eFCS_InNormalHorse) then
		--local ItemName = ItemInfo.ItemName
		--MsgToConn(Conn,823,ItemName)
		--UseItemEnd(Conn,ItemInfo)
		--return
		Conn.m_Player:PlayerDoFightSkillWithoutLevel("下马")
	end
	if ProgressType == "不读条" then
		if Conn.m_Player.UseItemParam[ItemInfo.ItemName].IsDoAction then
			Conn.m_Player:SetAndSyncActionState(EActionState.eAS_Item_Use)
		end
		PlayerTrunByNoLoadProgress(Conn, pos, ItemInfo)
		succFunc(Conn.m_Player,ItemInfo,pos,Target)
		return
	elseif ProgressType == "被移动打断的读条" then
		BeginUseItemProgress(ProgressTime, Conn, succFunc, FailFunc, ItemInfo,pos,false,Target)
		PlayerTrun(Conn, pos, ItemInfo)
	elseif ProgressType == "被攻击打断的读条" then
		BeginUseItemProgress(ProgressTime, Conn, succFunc, FailFunc, ItemInfo,pos,true,Target)
		PlayerTrun(Conn, pos, ItemInfo)
	end
end

function UseItemOtherList(Conn,ItemInfo)
	ConsumeAndGetItems(Conn,ItemInfo)  
	BeginCoolDown(Conn,ItemInfo) 
	UseQuestItem(Conn,ItemInfo)    
	if Conn.m_Player.UseItemParam[ItemInfo.ItemName] and
		Conn.m_Player.UseItemParam[ItemInfo.ItemName].IsDoAction then
		if Conn.m_Player.UseItemParam[ItemInfo.ItemName].IsDoAction == 1 then
			EffectPlay(Conn,ItemInfo)
		end
		Conn.m_Player.UseItemParam[ItemInfo.ItemName].IsDoAction = nil
	end
	UseItemEnd(Conn,ItemInfo)
end

function LayBuildUseItemOtherList(Conn,ItemInfo)
	BeginCoolDown(Conn,ItemInfo)
	UseQuestItem(Conn,ItemInfo)
	if Conn.m_Player.UseItemParam[ItemInfo.ItemName] and
		Conn.m_Player.UseItemParam[ItemInfo.ItemName].IsDoAction then
		if Conn.m_Player.UseItemParam[ItemInfo.ItemName].IsDoAction == 1 then
			EffectPlay(Conn,ItemInfo)
		end
		Conn.m_Player.UseItemParam[ItemInfo.ItemName].IsDoAction = nil
	end
	UseItemEnd(Conn,ItemInfo)
end

function CancelItemUseEffect(Player, BigID, ItemName)
	local ItemInfo = g_ItemUseInfoMgr:GetItemUseInfo(ItemName)
	if ItemInfo and ItemInfo["BigID"] == BigID and ItemInfo["effect"] == "对自身释放技能" then
		local ArgTbl = ItemInfo.Arg[2]
		if ArgTbl then
			local SkillName = ArgTbl[1]
			local SkillLevel = ArgTbl[2] or 1
			Player:PlayerDoItemSkill(SkillName,SkillLevel)
		end
	end
end
