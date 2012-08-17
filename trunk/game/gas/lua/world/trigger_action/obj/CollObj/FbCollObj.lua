gas_require "framework/main_frame/SandBoxDef"
gas_require "activity/quest/RoleQuest"
cfg_load "int_obj/IntObj_Common"
cfg_load "npc/Npc_Common"

local CFPos = CFPos
local EUnits = EUnits
local DropItemProtectTime = DropItemProtectTime
local ShowItemsToPlayer = ShowItemsToPlayer
local CollProgressFail = CollProgressFail
local CollProgressSucc = CollProgressSucc
local g_ItemInfoMgr = g_ItemInfoMgr
local delete_item_DB_Ret = CRoleQuest.delete_item_DB_Ret
local MsgToConn = MsgToConn
local ServerBeginProgress = ServerBeginProgress
local RButtonClickCollObj = RButtonClickCollObj
local IntObj_Common = IntObj_Common
local Npc_Common = Npc_Common
local os = os
local GetProcessTime = GetProcessTime
local StopItemProgressTick = StopItemProgressTick
local string = string
local g_ObjActionArgTbl = g_ObjActionArgTbl
local g_QuestDropItemMgr = g_QuestDropItemMgr
local DestroyCollObj = DestroyCollObj
local BreakPlayerCollItem = BreakPlayerCollItem
local CIntObjServer_GetIntObjServerByID = CIntObjServer_GetIntObjServerByID
local CallAccountAutoTrans = CallAccountAutoTrans
local g_NpcServerMgr = g_NpcServerMgr
local CreateIntObj = CreateIntObj
local EActionState = EActionState
local loadstring = loadstring
local EFighterCtrlState = EFighterCtrlState
local IsCppBound = IsCppBound
local IsServerObjValid = IsServerObjValid
local g_GetPlayerInfo = g_GetPlayerInfo
local NpcDropObjTbl = NpcDropObjTbl
local g_TeamMgr = g_TeamMgr
local LogErr = LogErr
local CLiveskillBase = CLiveskillBase

local Entry = CreateSandBox(...)

local CreateTargetPos = {{0,-1},{-1,-1},{1,-1},{-1,0},{1,0},{0,1},{1,1},{-1,1}}

--随机产生让玩家采集的物品
function Entry.ShowItemsToPlayer(data,PerfectType,ShowItemTbl)
	local Player, GlobalID = data[1], data[2]
	local ServerCollObj = CIntObjServer_GetIntObjServerByID( GlobalID )
	if not ServerCollObj then
		return
	end
	--打断物品使用读条
	if Player.m_UseItemLoadingTick then
		StopItemProgressTick(Player)
	end
	local Scene = ServerCollObj.m_Scene
	local Pos = CPos:new()
	ServerCollObj:GetGridPos(Pos)
	--g_IntObjServerMgr:IntObjDoAction(ServerCollObj,"respond02_1",2)
	ServerCollObj.m_DropItemsTbl={}   --初始化物品列表
	
	if not ServerCollObj.m_PlayerToItemTbl then
		ServerCollObj.m_PlayerToItemTbl = {}
	end
	ServerCollObj.m_PlayerToItemTbl[Player.m_uID] = {}
	
	for i=1,table.getn(ShowItemTbl) do
		local ItemType = ShowItemTbl[i]["ItemType"]
		local ItemName = ShowItemTbl[i]["ItemName"]
		local ItemNum = ShowItemTbl[i]["ItemNum"]
		if ItemType == "Npc" then
			for i = 1, ItemNum do
				local tPos = CPos:new()
				local index = math.random(1,#(CreateTargetPos))
				tPos.x = Pos.x + CreateTargetPos[index][1]
				tPos.y = Pos.y + CreateTargetPos[index][2]
				--print("CollNpc...........index",index)
				local NpcName = ItemName 
				local NpcInfo = Npc_Common(ItemName)
				local levelNum = 1
				if NpcInfo then
					levelNum = math.random(NpcInfo("MinLevel"),NpcInfo("MaxLevel"))
				else
--					ItemName = assert(loadstring("return {" .. ItemName.."}"))()
					NpcName = ItemName[1]
					if ItemName[2] then
						local lev = Player:CppGetLevel()
						levelNum = string.gsub(ItemName[2],"PlayerLevel",lev)
						levelNum = loadstring("return " .. levelNum)()
					else
						NpcInfo = Npc_Common(NpcName)
						levelNum = math.random(NpcInfo("MinLevel"),NpcInfo("MaxLevel"))
					end
				end
				local fPos = CFPos:new( tPos.x * EUnits.eGridSpan, tPos.y * EUnits.eGridSpan )
				local otherData = {["m_CreatorEntityID"]=Player:GetEntityID(),["m_OwnerId"]=Player.m_uID}
				local Npc = g_NpcServerMgr:CreateServerNpc(NpcName,levelNum,Scene,fPos,otherData)
			end
		elseif ItemType == "Obj" then
			for i = 1, ItemNum do
				local index = math.random(1,#(CreateTargetPos)-3)
				local tPos = CPos:new()
				tPos.x = Pos.x + CreateTargetPos[index][1]
				tPos.y = Pos.y + CreateTargetPos[index][2]
				--print("CollObj...........index",index)
				local IntObj = CreateIntObj( Scene ,tPos,ItemName,true, Player.m_uID, nil,os.time())
				IntObj.m_OwnerId = Player.m_uID
				IntObj.m_CreateTime = os.time()
			end
		else
			ItemType = tonumber(ItemType)
			local FoldLimit = g_ItemInfoMgr:GetItemInfo( ItemType,ItemName,"FoldLimit" ) or 1
			if FoldLimit == 1 then
				for j = 1, ItemNum do
					table.insert(ServerCollObj.m_DropItemsTbl, {ItemType,ItemName,1})
					table.insert(ServerCollObj.m_PlayerToItemTbl[Player.m_uID], {ItemType,ItemName,1})
				end
			elseif ItemNum > FoldLimit then
				local fornum = math.floor(ItemNum/FoldLimit)
				local residualnum = ItemNum%FoldLimit
				if residualnum ~= 0 then
					table.insert(ServerCollObj.m_DropItemsTbl, {ItemType,ItemName,residualnum})
					table.insert(ServerCollObj.m_PlayerToItemTbl[Player.m_uID], {ItemType,ItemName,residualnum})
				end
				if fornum ~= 0 then
					for j = 1, fornum do
						table.insert(ServerCollObj.m_DropItemsTbl, {ItemType,ItemName,FoldLimit})
						table.insert(ServerCollObj.m_PlayerToItemTbl[Player.m_uID], {ItemType,ItemName,FoldLimit})
					end
				end
			else
				table.insert(ServerCollObj.m_DropItemsTbl, {ItemType,ItemName,ItemNum})
				table.insert(ServerCollObj.m_PlayerToItemTbl[Player.m_uID], {ItemType,ItemName,ItemNum})
			end
		end
	end
	
	ServerCollObj.m_CheckOpenTime = {["UseTime"] = os.time(),["CoolTime"] = 12}
	local DropItemsTbl=ServerCollObj.m_DropItemsTbl
--	print("--------------------------")
	if table.getn(DropItemsTbl) ~= 0 then
		for i=1,table.getn(DropItemsTbl) do
			local ItemType,ItemName,ItemNum=DropItemsTbl[i][1],DropItemsTbl[i][2],DropItemsTbl[i][3]
			local itemtype=tonumber(ItemType)
--			print("ItemType,ItemName,ItemNum",ItemType,ItemName,ItemNum)
			Gas2Gac:RetCanCollItem(Player.m_Conn,itemtype,ItemName,ItemNum)
		end
		Gas2Gac:RetCanCollItemEnd(Player.m_Conn,GlobalID,PerfectType)
		--ServerCollObj.m_HaveOpen = true
		Player.m_ProcessingObjID = nil
		Player.m_CollIntObjID = GlobalID
	--注册Tick检测玩家是否已经下线，如果玩家下线就改变Obj的状态m_Openner（见CheckOpennerOnline）
		Player:AddListeningWnd()
		
		--------------------------------------------------------
		--加一个tick,如果玩家打开了采集,半天不采的话,就关掉
		local function CloseWndFun(Tick, param)
			local Target, TempObj = param[1],param[2]
			if TempObj and TempObj.m_ItemWndCloseTick then
				UnRegisterTick(TempObj.m_ItemWndCloseTick)
				TempObj.m_ItemWndCloseTick = nil
			end
			if IsCppBound(Target) then
				BreakPlayerCollItem(Target)
			end
		end
		if ServerCollObj.m_ItemWndCloseTick then
			UnRegisterTick(ServerCollObj.m_ItemWndCloseTick)
			ServerCollObj.m_ItemWndCloseTick = nil
		end
		local Tempdata = {Player, ServerCollObj}
		ServerCollObj.m_ItemWndCloseTick = RegisterTick("ItemWndCloseTick",CloseWndFun,10*1000,Tempdata)
		---------------------------------------------------------------
	else
		DestroyCollObj(ServerCollObj)
	end
end

function Entry.CollProgressFail(Player, data)
	local GlobalID = data[1]
	local CollObj = CIntObjServer_GetIntObjServerByID( GlobalID )
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
		if CollObj.m_OpenTime then
			CollObj.m_OpenTime = os.time()
		end
	end
end

function Entry.CollProgressSucc(Player, data)
	Player:SetAndSyncActionState(EActionState.eAS_Idle_BackWpn)
	local GlobalID = data[1]
	local CollObj = CIntObjServer_GetIntObjServerByID( GlobalID )
	
	if not IsServerObjValid(CollObj) then
		return
	end
	
	local PerfectType = 1
	
	Player.m_ProcessingObjID = nil
	local ObjName = CollObj.m_Properties:GetCharName()
	
	local Conn = Player.m_Conn
	if not IsCppBound(Conn) then
		if CollObj then
			CollObj.m_Openner = nil
		end
		return
	end
	
	local PlayerId = Player.m_uID
	local TriggerAction = IntObj_Common(ObjName, "TriggerAction")
	local needitemtype = nil
	local needitemname = nil
	local needitemnum = nil
	if string.find(TriggerAction,"不读条") then
		if g_ObjActionArgTbl[ObjName] and g_ObjActionArgTbl[ObjName][1] then
			needitemtype = g_ObjActionArgTbl[ObjName][1][1]
			needitemname = g_ObjActionArgTbl[ObjName][1][2]
			needitemnum = g_ObjActionArgTbl[ObjName][1][3]
		end
	elseif g_ObjActionArgTbl[ObjName][2] then
		needitemtype = g_ObjActionArgTbl[ObjName][2][1]
		needitemname = g_ObjActionArgTbl[ObjName][2][2]
		needitemnum = g_ObjActionArgTbl[ObjName][2][3]
	end
	
	local function CallBack(result)
		if result[1] and IsCppBound(Player) then
			if needitemname and result[3] then
				delete_item_DB_Ret(Player,needitemtype,needitemname,needitemnum,result[3])
			end
			if result[4] and result[4] ~= 0 then
				MsgToConn(Conn, 9624, result[4])
				local skill_info = result[5]
				if skill_info then
					CLiveskillBase.RetLiveskillInfo(Conn,skill_info)
				end
			end
			local data = {Player,GlobalID}
			Entry.ShowItemsToPlayer(data,PerfectType,result[2])
		else
			if CollObj then
				CollObj.m_Openner = nil
			end
		end
	end
	
	local parameter = {}
	parameter["PlayerId"] = PlayerId
	
	if CollObj.m_PlayerToItemTbl and CollObj.m_PlayerToItemTbl[PlayerId] then
		parameter["DropItemsTbl"] = CollObj.m_PlayerToItemTbl[PlayerId]
	else
		if needitemnum and needitemnum > 0 then
			parameter["ItemType"] = needitemtype
			parameter["ItemName"] = needitemname
			parameter["nCont"] = needitemnum
		end
	end
	parameter["ObjName"] = ObjName
	parameter["PerfectType"] = PerfectType
	parameter["sceneName"] = Player.m_Scene.m_SceneName
	if Conn.m_Account then
		CollObj.m_CheckOpenTime["UseTime"] = os.time()
		CallAccountAutoTrans(Conn.m_Account, "CollObjDB", "ShowItemsToPlayer", CallBack, parameter)
	else
		if CollObj then
			CollObj.m_Openner = nil
		end
	end
end

function Entry.RButtonClickCollObj(data,HaveItemNum)
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
	
	local TriggerAction = IntObj_Common(ObjName, "TriggerAction")
	if g_ObjActionArgTbl[ObjName] and HaveItemNum then
		local needitemname = nil
		local needitemnum = nil
		if string.find(TriggerAction,"不读条") and g_ObjActionArgTbl[ObjName][1] then
			needitemname = g_ObjActionArgTbl[ObjName][1][2]
			needitemnum = g_ObjActionArgTbl[ObjName][1][3]
		elseif g_ObjActionArgTbl[ObjName][2] then
			needitemname = g_ObjActionArgTbl[ObjName][2][2]
			needitemnum = g_ObjActionArgTbl[ObjName][2][3]
		end
		if needitemname and (HaveItemNum < needitemnum or HaveItemNum == 0) then
			if needitemnum == 0 then
				needitemnum = 1
			end
			MsgToConn(Player.m_Conn, 3151, needitemnum, needitemname)
			CollObj.m_Openner = nil
			return
		end
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
	local ProTime = nil
	
	if not string.find(TriggerAction,"不读条") then
		ProTime = g_ObjActionArgTbl[ObjName][1]
	else
		ProTime = 0
	end
	
	CollObj.m_CheckOpenTime = {["UseTime"] = os.time(),["CoolTime"] = ProTime + 5}
	ServerBeginProgress(ProTime*1000, nil, Player.m_Conn, Entry.CollProgressSucc, Entry.CollProgressFail, data)
end

local function WriteLog(Player, Sharertbl, TempTeamMemberID, BelongTeamID)
	local str = ""
	local TeamID = Player.m_Properties:GetTeamID()
	local TeamInfo = ""
	if TeamID ~= 0 then
		local tblMembers = g_TeamMgr:GetMembers(TeamID)
		for i = 1, #(tblMembers) do
			TeamInfo = TeamInfo..","..tblMembers[i]
		end
	end
	
	str = "玩家ID:"..Player.m_uID.."区域信息:"..Player.m_AreaName.."玩家队伍信息:"..TeamID..TeamInfo
	
	if TempTeamMemberID then
		TeamInfo = ""
		for i = 1, #(TempTeamMemberID) do
			TeamInfo = TeamInfo..","..TempTeamMemberID[i]
		end
	end
	str = str .. "没有修改的队伍信息:" .. TeamInfo
	
	if Sharertbl and IsTable(Sharertbl) then
		local TempTeamID = nil
		local TempAreaName = ""
		TeamInfo = ""
		for i = 1, #(Sharertbl) do
			TeamInfo = TeamInfo..","..Sharertbl[i]
			local Member = g_GetPlayerInfo(Sharertbl[i])
			if (not TempTeamID or TempTeamID == 0) and Player.m_uID ~= Sharertbl[i] and Member then
				TempTeamID = Member.m_Properties:GetTeamID()
				TempAreaName = Member.m_AreaName
			end
		end
		str = str .. "可拾取列表:" .. BelongTeamID .. TeamInfo
		
		if TempTeamID and TempTeamID ~= 0 then
			TeamInfo = ""
			local tblMembers = g_TeamMgr:GetMembers(TempTeamID)
			for i = 1, #(tblMembers) do
				TeamInfo = TeamInfo..","..tblMembers[i]
			end
			
			str = str .. "其它队伍区域信息:" .. TempAreaName .. "其它队伍信息:" .. TempTeamID..TeamInfo
		end
	end
	
	LogErr("帮助分析副本中打怪掉落的LOG",str)
end

function Entry.GetPackData(Conn, Obj, ObjName, ObjID) 
	local Player = Conn.m_Player
	local PlayerId = Conn.m_Player.m_uID
	
	if Player:CppGetCtrlState(EFighterCtrlState.eFCS_OnMission) then
		return
	end
	
	if Obj.m_CollPlayer[PlayerId] then
		--print("已经采集过了你，不能再采集。。。")
		MsgToConn(Conn, 3156)
		return
	end
	
	if Player.m_CollIntObjID or Player.m_PickUpNpcID then
		return
	end
	
	if Obj.m_Openner ~= nil then
		if Obj.m_CheckOpenTime then
			if (os.time() - Obj.m_CheckOpenTime["UseTime"]) < Obj.m_CheckOpenTime["CoolTime"] then
				if Obj.m_Openner ~= PlayerId then
					MsgToConn(Conn, 3154)
				end
				return
			end
		end
	end
	
	local DropNpcID = Obj.m_BelongToNpc
	local DropObjTbl = NpcDropObjTbl[DropNpcID]
	if DropNpcID and not DropObjTbl then
		return
	end
	
	local IsInShare = false
	Obj.m_CollPlayerNum = #(DropObjTbl.m_TempTeamMemberID)
	for i = 1, Obj.m_CollPlayerNum do
		if DropObjTbl.m_TempTeamMemberID[i] == PlayerId then   
			IsInShare = true
			break
		end
	end
	if not IsInShare then
		MsgToConn(Conn, 3034)
		return
	end
	
	local Sharertbl = DropObjTbl.m_CanSharePlayer
	IsInShare = false
	if Sharertbl and IsTable(Sharertbl) then
		for i = 1, #(Sharertbl) do
			if Sharertbl[i] == PlayerId then   
				IsInShare = true
				break
			end
		end
	elseif DropObjTbl.m_OwnerId == PlayerId then
		IsInShare = true
	end
	if not IsInShare then
		WriteLog(Player, Sharertbl, DropObjTbl.m_TempTeamMemberID, DropObjTbl.m_BelongToTeam or 0)
	end
	
	return true
end

function Entry.Exec(Conn, CollObj, ObjName, GlobalID)	
	local PlayerId=Conn.m_Player.m_uID
	if not CollObj.m_CollPlayer then
		CollObj.m_CollPlayer = {}
	end
	
	if not Entry.GetPackData(Conn, CollObj, ObjName, GlobalID) then
		return
	end
	
	local data = {PlayerId, ObjName, GlobalID}
	local function CallBack(result)
		if result[1] then
			Entry.RButtonClickCollObj(data,result[2])
		else
			if IsTable(result[2]) then
				MsgToConn(Conn, unpack(result[2]))
			end
			local CollObj = CIntObjServer_GetIntObjServerByID(GlobalID)
			if CollObj == nil then
				return
			end
			--如果数据库判断不成功就把m_Openner清掉
			CollObj.m_Openner = nil
		end
	end
	
	local TriggerAction = IntObj_Common(ObjName, "TriggerAction")
	local parameter = {}
	parameter["nCharId"] = PlayerId
	parameter["ObjName"] = ObjName
	if string.find(TriggerAction,"不读条") then
		if g_ObjActionArgTbl[ObjName] and g_ObjActionArgTbl[ObjName][1] then
			parameter["ItemType"] = g_ObjActionArgTbl[ObjName][1][1]
			parameter["ItemName"] = g_ObjActionArgTbl[ObjName][1][2]
		end
	elseif g_ObjActionArgTbl[ObjName][2] then
		parameter["ItemType"] = g_ObjActionArgTbl[ObjName][2][1]
		parameter["ItemName"] = g_ObjActionArgTbl[ObjName][2][2]
	end
	
	if Conn.m_Account then
		CollObj.m_Openner = PlayerId
		CollObj.m_CheckOpenTime = {["UseTime"] = os.time(),["CoolTime"] = 5}
		CallAccountAutoTrans(Conn.m_Account, "CollObjDB", "ClickCollItemObj",CallBack,parameter)
	end
end

return Entry
