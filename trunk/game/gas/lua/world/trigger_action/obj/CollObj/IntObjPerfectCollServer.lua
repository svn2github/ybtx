gas_require "framework/main_frame/SandBoxDef"
cfg_load "int_obj/IntObj_Common"

local GetStaticTextServer = GetStaticTextServer
local DropItemProtectTime = DropItemProtectTime
local CollProgressFail = CollProgressFail
local CollProgressSucc = CollProgressSucc
local DestroyCollObj = DestroyCollObj
local g_MatchGameMgr = g_MatchGameMgr
local g_ObjActionArgTbl = g_ObjActionArgTbl
local MsgToConn = MsgToConn
local ServerBeginProgress = ServerBeginProgress
local IntObj_Common = IntObj_Common
local os = os
local string = string
local CIntObjServer_GetIntObjServerByID = CIntObjServer_GetIntObjServerByID
local EActionState = EActionState
local GetProcessTime = GetProcessTime
--local loadstring = loadstring
local EFighterCtrlState = EFighterCtrlState
local CMercenaryRoomCreate = CMercenaryRoomCreate

local g_GetPlayerInfo = g_GetPlayerInfo
local Entry = CreateSandBox(...)

function Entry.CollProgressFail(Player, data)
	local GlobalID = data[1]
	local CollObj = CIntObjServer_GetIntObjServerByID(GlobalID)
	if Player then
--		Player:SetAndSyncActionState(EActionState.eAS_Idle_BackWpn)
		Gas2Gac:NotifyStopProgressBar(Player.m_Conn)
		Player.m_ProcessingObjID = nil
		if CollObj == nil then
			MsgToConn(Player.m_Conn, 3152)
		end
	end
	if CollObj then
		CollObj.m_Openner = nil
		CollObj.m_IsPerfectColl = nil
		if CollObj.m_OpenTime then
			CollObj.m_OpenTime = os.time()
		end
	end
end

local function PlayerPlusPerfectCollPoint(Player, point, ObjName)
	g_MatchGameMgr:AddMatchGameCount(Player, 4, ObjName, point)
--	Gas2Gac:PlayerPlusPointEffect(Player.m_Conn, "green", "+"..point, 1)
end

local function CountPoint(CollObjCombo, PerfectCombo, PrePlusPoint)
	local AllPoint = 3
	local CollObjComboPoint = CollObjCombo - 1
	local PerfectComboPoint = PerfectCombo - 1 + PrePlusPoint
	if CollObjComboPoint > 5 then
		CollObjComboPoint = 5
	end
	if PerfectCombo > 15 then
		PerfectComboPoint = PrePlusPoint
	end
	AllPoint = AllPoint + CollObjComboPoint + PerfectComboPoint
	return AllPoint, PerfectComboPoint
end

local function ComboEffect(Player, Str, combo)
--	if combo > 4 then
		Gas2Gac:PlayHeadEffect(Player.m_Conn, "yellow", Str..combo, Player:GetEntityID())
--	end
end

local function CollObjPerfectCombo(Player, PerfectType, ObjName)
	if PerfectType == 1 then
		Player.m_CollObjCombo = 0 
		Player.m_PerfectCombo = 0 
		Player.m_PrePlusPoint = 0
		PlayerPlusPerfectCollPoint(Player, 1, ObjName)
	elseif PerfectType == 2 then
		Player.m_CollObjCombo = Player.m_CollObjCombo + 1
		Player.m_PerfectCombo = 0
		Player.m_PrePlusPoint = 0
		PlayerPlusPerfectCollPoint(Player, 2, ObjName)
--		Gas2Gac:UseItemPlayerEffect(Player.m_Conn,"fx/setting/other/other/xiaoyouxi/create.efx","xiaoyouxi/create")
--		Gas2Gac:UseItemTargetEffect(Player:GetIS(0),"fx/setting/other/other/xiaoyouxi/create.efx","xiaoyouxi/create", Player:GetEntityID())
		ComboEffect(Player, GetStaticTextServer(8887), Player.m_CollObjCombo)
	elseif PerfectType == 3 then
		Player.m_CollObjCombo = Player.m_CollObjCombo + 1
		Player.m_PerfectCombo = Player.m_PerfectCombo + 1
		local point, PrePlusPoint = CountPoint(Player.m_CollObjCombo, Player.m_PerfectCombo, Player.m_PrePlusPoint)
		Player.m_PrePlusPoint = PrePlusPoint
		PlayerPlusPerfectCollPoint(Player, point, ObjName)
		Gas2Gac:UseItemPlayerEffect(Player.m_Conn,"fx/setting/other/other/xiaoyouxi/create.efx","xiaoyouxi/create")
		Gas2Gac:UseItemTargetEffect(Player:GetIS(0),"fx/setting/other/other/xiaoyouxi/create.efx","xiaoyouxi/create", Player:GetEntityID())
--		ComboEffect(Player, GetStaticTextServer(8887), Player.m_CollObjCombo)
		ComboEffect(Player, GetStaticTextServer(8888), Player.m_PerfectCombo)
	end
end

function Entry.CollProgressSucc(Player, data)
	Player:SetAndSyncActionState(EActionState.eAS_Idle_BackWpn)
	local GlobalID = data[1]
	local CollObj = CIntObjServer_GetIntObjServerByID(GlobalID)
	if CollObj == nil then
		return
	end
	local PerfectType = CollObj.m_PerfectType or 1
	Player.m_ProcessingObjID = nil
	CollObj.m_IsPerfectColl = nil
	local ObjName = CollObj.m_Properties:GetCharName()
	DestroyCollObj(CollObj)
	
	if Player.m_Scene.m_SceneAttr.SceneType == 14 then
		CMercenaryRoomCreate.CollAddNum(Player, ObjName, PerfectType)
	else
		CollObjPerfectCombo(Player, PerfectType, ObjName)
	end
end

function Entry.BeginProgress(data)
	local PlayerId, ObjName, GlobalID = unpack(data, 1, 3)
	
	local CollObj = CIntObjServer_GetIntObjServerByID(GlobalID)
	if CollObj == nil then
		return
	end
	
	local Player = g_GetPlayerInfo(PlayerId)
	if not Player then
		CollObj.m_Openner = nil
		return
	end
	
	if Player.m_ProcessingObjID then
		if Player.m_ProcessingObjID == GlobalID then
--			print("Player.m_ProcessingObjID == GlobalID")
			return
		end
		local IntObj = CIntObjServer_GetIntObjServerByID( Player.m_ProcessingObjID )
		if IntObj ~= nil then
			IntObj.m_Openner = nil
		end
	end
	
	--通知客户端读取进度条
	CollObj.m_Openner=PlayerId
	
	Player.m_ProcessingObjID = GlobalID
	local data = {GlobalID}
	data["ActionName"] = IntObj_Common(ObjName, "ActionName")
	local ProTime = g_ObjActionArgTbl[ObjName][1]
	local IsPerfectColl = nil  --表示此次采集是否是完美采集
	local RefreshTime = IntObj_Common(ObjName, "RefreshTime")
	if RefreshTime == 0 or RefreshTime == "" then
		RefreshTime = 10
	end
	CollObj.m_CheckOpenTime = {["UseTime"] = os.time(),["CoolTime"] = RefreshTime+2}
	
	--完美采集也走CD
	if not CollObj.m_OpenTime or (os.time()-CollObj.m_OpenTime) >= RefreshTime then
		IsPerfectColl = true
		CollObj.m_BeginCollTime = GetProcessTime()
	end
	
	CollObj.m_IsPerfectColl = IsPerfectColl
	ServerBeginProgress(ProTime*1000, IsPerfectColl, Player.m_Conn, Entry.CollProgressSucc, Entry.CollProgressFail, data)
end

function Entry.Exec(Conn, CollObj, ObjName, GlobalID)	
	local PlayerId=Conn.m_Player.m_uID
	local Player=Conn.m_Player
	if not Player.m_CollObjCombo then
		Player.m_CollObjCombo = 0											-- 完美采集
		Player.m_PerfectCombo = 0											-- 非常完美采集
		Player.m_PrePlusPoint = 0											-- 前一次额外加分
	end
--	if CollObj.m_Properties:GetCanUseState() == 0 then
--		return
--	end
	
	if Player:CppGetCtrlState(EFighterCtrlState.eFCS_OnMission) then
		return
	end
	
	if Player.m_CollIntObjID or Player.m_PickUpNpcID then
		return
	end
	
	if CollObj.m_OwnerId then
		if PlayerId ~= CollObj.m_OwnerId and (os.time()-CollObj.m_CreateTime) < DropItemProtectTime then
--			print("已指定归属...!")  
			MsgToConn(Conn, 3153)
			return
		end
	else
		local DropOwner = CollObj.m_Properties:GetDropOwnerId()
		local DropTime = CollObj.m_Properties:GetDropTime()
		if DropOwner ~= 0 and DropOwner ~= PlayerId then
			if DropTime == 0 or (os.time()-DropTime) < DropItemProtectTime then
				return
			end
		end
	end
	
	if CollObj.m_Openner ~= nil then
--		print("CollObj.m_Openner ~= nil")
		if  CollObj.m_Openner ~= PlayerId then   --m_Openner==true表示有人正在进行采集
			if CollObj.m_CheckOpenTime then
--			print("别人正在使用中...!")  
				if (os.time() - CollObj.m_CheckOpenTime["UseTime"]) < CollObj.m_CheckOpenTime["CoolTime"] then
					MsgToConn(Conn, 3154)
					return
				end
			end
		end
	end
	
	local data = {PlayerId, ObjName, GlobalID}
	Entry.BeginProgress(data)
end

return Entry
