cfg_load "npc/DropTeam_Server"
cfg_load "npc/NpcDropItem_Common"

cfg_load "npc/DropItemObjTeam_Server"
cfg_load "npc/NpcDropItemObj_Server"

gas_require "world/npc/OpenPickUpDropWnd"

local DropTeamNumAOI = 38

NpcDropItemTbl = {}
NpcDropObjTbl = {}

function CheckIsDestroyDropTbl(GlobalID)
	if NpcDropItemTbl[GlobalID] then
		if NpcDropItemTbl[GlobalID].m_Openner then
			local Player = g_GetPlayerInfo(NpcDropItemTbl[GlobalID].m_Openner)
			if Player then
				BreakPlayerPickUpItem(Player)
			end
		end
	end
	NpcDropItemTbl[GlobalID] = nil
	return true
end

function AddNpcDropSharer(PlayerID, TeamID)   -- 记录新加入队伍的玩家可以分配到哪些的npc掉落的物品
	local sharemgr = g_AverageAssignOrder[TeamID]
	if not sharemgr or table.getn(sharemgr.m_OwnNpcIDtbl) == 0 then
		return
	end
	for i = 1, table.getn(sharemgr.m_OwnNpcIDtbl) do
		local GlobalID = sharemgr.m_OwnNpcIDtbl[i]
		local Npc = CCharacterDictator_GetCharacterByID(GlobalID)
		table.insert(NpcDropItemTbl[GlobalID].m_CanSharePlayer, PlayerID)
	end
end

function BreakPickUpItem(Player, data)
	local GlobalID = data[1]
	local CoreObj = CCharacterDictator_GetCharacterByID( GlobalID )
	if Player then
		if CoreObj == nil then
			MsgToConn(Player.m_Conn, 3029)
		else
			NpcDropItemTbl[GlobalID].m_Openner = nil
			NpcDropItemTbl[GlobalID].m_PickUpIndex = nil
		end
		Player:DelListeningWnd()
		Gas2Gac:NotifyClosePickUpWnd(Player.m_Conn)
		Player.m_PickUpNpcID = nil
	else
		if CoreObj then
			NpcDropItemTbl[GlobalID].m_Openner = nil
			NpcDropItemTbl[GlobalID].m_PickUpIndex = nil
		end
	end
end

function BreakPlayerPickUpItem(Player)
	local function Init()
	--	if Player.m_PickUpNpcID == nil then
	--		return
	--	end
		Player:DelListeningWnd()
		local GlobalID = Player.m_PickUpNpcID
		local Npc = CCharacterDictator_GetCharacterByID( GlobalID )
		if Npc == nil then
			MsgToConn(Player.m_Conn, 3029)
		else
			NpcDropItemTbl[GlobalID].m_Openner = nil
			NpcDropItemTbl[GlobalID].m_PickUpIndex = nil
		end
		Gas2Gac:NotifyClosePickUpWnd(Player.m_Conn)
		Player.m_PickUpNpcID = nil
	end
	apcall(Init)
end

--普通物品掉落
local function  SaveAllDropItems(PlayerId, EntityID, cServerNpc, DropItemTbl)
	if table.getn(DropItemTbl.m_Items_List) > 0 then
		NpcDropItemTbl[EntityID] = DropItemTbl
		cServerNpc.m_Properties:SetDropOwnerId(PlayerId)
		cServerNpc.m_Properties:SetDropTime(os.time())
		cServerNpc.m_Properties:SetBelongToTeam(DropItemTbl.m_BelongToTeam or 0)
		Gas2Gac:NotifyNpcDropItem(cServerNpc:GetIS(0),EntityID,true)
	else
		Gas2Gac:NotifyNpcDropItem(cServerNpc:GetIS(0),EntityID,false)
	end
end

local function GetUsebalePos(CoreScene, NowPos, DropPosTbl)
	local GridPos = CPos:new()
	local TblLen = #(DropPosTbl)
	if TblLen ~= 0 then

		for i=1,TblLen do
			local RandNum = math.random(1,#(DropPosTbl))
			GridPos.x = NowPos.x + DropPosTbl[RandNum][1]
			GridPos.y = NowPos.y + DropPosTbl[RandNum][2]
			table.remove(DropPosTbl,RandNum)
			if CoreScene:GetBarrier(GridPos) ==  EBarrierType.eBT_NoBarrier then
				return GridPos
			end
		end

	end
	return NowPos
end

local DropPosTbl = 
{
	{0.5,0},{0,0.5},{-0.5,-0.5},{0.5,-0.5},
	{-0.5,0},{0,0.5},{0.5,0.5},{-0.5,0.5},
	{1,0},{0,-1},{-1,-1},{1,-1},{-1,0},
	{0,0},{0,1},{1,1},{-1,1},
}--共16个方向

local function  SaveAllDropObjs(PlayerId,EntityID, DeadScene, DeadPos, DropObjTbl)
	NpcDropObjTbl[EntityID] = DropObjTbl
	NpcDropObjTbl[EntityID].m_CreateObjTbl = {}
	
	local x,y = DeadPos.x,DeadPos.y
	local Obj
	local dispos 
	local objtbl
	local ObjName
	local FPos = CFPos:new()
	local DropOwnerId
	local DropTime
	local BelongToTeam
	local QuestDropOwnner
	local CreateNum = table.getn(DropObjTbl.m_Objs_List)
	for i=1,CreateNum do
		dispos = DropPosTbl[1]
		objtbl = DropObjTbl.m_Objs_List[i]
		ObjName = objtbl[1]
		table.remove(DropPosTbl,1)
		table.insert(DropPosTbl,dispos)
		FPos.x = x + dispos[1]
		FPos.y = y + dispos[2]
		DropOwnerId = PlayerId
		DropTime = os.time()
		BelongToTeam = DropObjTbl.m_BelongToTeam
		local IntObjCommon = IntObj_Common(ObjName)
		if IntObjCommon then
			QuestDropOwnner = objtbl["QuestDropOwner"]
			if QuestDropOwnner then
				--如果是任务物品,那么只有本人可以拾取
				DropTime = nil  --不指定掉落时间
				DropOwnerId = nil
			end
			
			if BelongToTeam and BelongToTeam ~= 0 and IntObjCommon("TriggerAction") ~= "可拾取的Obj" then
				DropOwnerId = nil
			end
			Obj = CreateIntObj( DeadScene, FPos, ObjName,true, DropOwnerId, BelongToTeam,DropTime)
			Obj.m_QuestDropObjOwnner = QuestDropOwnner
			Obj.m_BelongToNpc = EntityID
			NpcDropObjTbl[EntityID].m_CreateObjTbl[Obj:GetEntityID()] = true
		end
	end
end
---------------------------rpc---------------------------

local function ClickOnDeadNpc(data)
	local Conn, GlobalID = unpack(data)
	if not IsCppBound(Conn) or not IsCppBound(Conn.m_Player) then
		return false
	end
	if Conn.m_Player.m_CollIntObjID or Conn.m_Player.m_PickUpNpcID then
		return false
	end
	
	local ItemHolder = CCharacterDictator_GetCharacterByID(GlobalID)
	if ItemHolder == nil then
		return false
	end
	
	local PlayerId = Conn.m_Player.m_uID
	local Player = Conn.m_Player
	
	if g_GetDistance(Player,ItemHolder)>6 then
		return false
	end
	
	if Player:IsMoving() then
		return false
	end
	
	if NpcDropItemTbl[GlobalID] == nil then
		return false
	end
	
	if NpcDropItemTbl[GlobalID].m_Items_List == nil and not IsTable(NpcDropItemTbl[GlobalID].m_Items_List) then
--		print("物品列表空！")
		return false
	end
	
	local ItemNum = table.getn(NpcDropItemTbl[GlobalID].m_Items_List)
	if ItemNum == 0 then
--		print("物品列表空！")
		return false
	end
	
	if NpcDropItemTbl[GlobalID].m_Openner then
--		print("有m_Openner")
		return false
	end
	
	local Sharertbl = NpcDropItemTbl[GlobalID].m_CanSharePlayer
	if Sharertbl then
		for i = 1, table.getn(Sharertbl) do
			if Sharertbl[i] == PlayerId then  --玩家在物品享有者的列表里
				return true
			end
		end
	end
	
	if NpcDropItemTbl[GlobalID].m_OwnerId == PlayerId then 
		--物品没有指明属于哪个队伍，而且npc掉落的物品属于该玩家
		return true
	elseif (os.time()-NpcDropItemTbl[GlobalID].m_CreateTime)>DropItemProtectTime then
		--掉落物品的保护时间已过，任何人都可以自由拾取
		return true
	end
	
	return false
end

function SaveNpcDropTbl(PlayerId,cServerNpc,EntityID,DeadSceneId,DeadPos,DropItemTbl,DropObjTbl)
	--杀死怪物掉落物品(包括任务物品和普通物品)
	local DeadScene = g_SceneMgr:GetScene(DeadSceneId)
	if DeadScene and #(DropObjTbl.m_Objs_List) > 0 then
		SaveAllDropObjs(PlayerId, EntityID,DeadScene,DeadPos,DropObjTbl)
		local player = g_GetPlayerInfo(PlayerId)
		if IsCppBound(player) then
			player:IsFirstTimeAndSendMsg("自动拾取", 2005)
		end
	end
	
	--local cServerNpc = CCharacterDictator_GetCharacterByID(EntityID)
	if not IsCppBound(cServerNpc) then
		return
	end
	
	SaveAllDropItems(PlayerId, EntityID,cServerNpc,DropItemTbl)
end

function Gac2Gas:ShowNpcDropItemWnd(Conn, GlobalID)
	local data = {Conn, GlobalID}
	if not ClickOnDeadNpc(data) then
		return
	end
	
	local Player = Conn.m_Player
	local PlayerId = Player.m_uID
	Player.m_PickUpNpcID = GlobalID
	
	if Player.m_ActionLoadingTick then
		BreakPlayerActionLoading(Player)
	end
	
	--打断物品使用读条
	if Player.m_UseItemLoadingTick then
		StopItemProgressTick(Player)
	end
	
	if Player.m_CollectResTick then
		TongStopLoadProgress(Player)
	end
	
	NpcDropItemTbl[GlobalID].m_Openner = PlayerId
	local function CallBack(PickUpIndexTbl)
		local Openner = g_GetPlayerInfo(PlayerId)
		if Openner then
			if NpcDropItemTbl[GlobalID]  == nil then
				return
			end
				
			if not OpenPickUpDropWnd(Openner,GlobalID,PickUpIndexTbl) then
				Openner.m_PickUpNpcID = nil
				if NpcDropItemTbl[GlobalID] then
					NpcDropItemTbl[GlobalID].m_Openner = nil
				end
			end
		else
			if NpcDropItemTbl[GlobalID] then
				NpcDropItemTbl[GlobalID].m_Openner = nil
			end
		end
	end
	
	local params = {}
	params["PlayerId"] = PlayerId
	params["TeamID"] = Player.m_Properties:GetTeamID()
	params["DropItemTbl"] = NpcDropItemTbl[GlobalID]
	
	CallAccountManualTrans(Conn.m_Account, "NpcDropItemDB", "ShowNpcDropItem",CallBack,params)
end

function Gac2Gas:SendAutoPickUpNpcID(Conn, GlobalID)
	if not Conn.m_AutoPickUpNpcTbl then
		Conn.m_AutoPickUpNpcTbl = {}
		table.insert(Conn.m_AutoPickUpNpcTbl, GlobalID)
	else
		table.insert(Conn.m_AutoPickUpNpcTbl, GlobalID)
	end
end
