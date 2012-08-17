gas_require "world/int_obj/IntObjServer"
cfg_load "int_obj/IntObj_Common"
gas_require "log_mgr/CLogMgr"

local function ClearItemWndCloseTick(CollObj)
	if CollObj and CollObj.m_ItemWndCloseTick then
		UnRegisterTick(CollObj.m_ItemWndCloseTick)
		CollObj.m_ItemWndCloseTick = nil
	end
end

function BreakPlayerCollItem(Player)
	local function Init()
		local GlobalID = Player.m_CollIntObjID
		local CollObj = CIntObjServer_GetIntObjServerByID( GlobalID )	
		Player.m_CollIntObjID = nil
		Player:DelListeningWnd()
		Gas2Gac:NotifyCloseCollWnd(Player.m_Conn)
		if CollObj then
			ClearItemWndCloseTick(CollObj)
			g_IntObjServerMgr:IntObjDoAction(CollObj,"respond02_2",1)
			CollObj.m_Openner = nil
			CollObj.m_PerfectType = nil
			CollObj.m_OpenTime = os.time()
		end
	end
	apcall(Init)
end

local function ClearState(CollObj)
	ClearItemWndCloseTick(CollObj)
	CollObj.m_DropItemsTbl = nil
	CollObj.m_OpenTime = nil
	CollObj.m_Openner = nil
	CollObj.m_OpenTime = nil
	--CollObj.m_HaveOpen = nil
end

local function FbDestroyObj(CollObj)
	local num = 0
	for _,_ in pairs(CollObj.m_CollPlayer) do
		num = num + 1
	end
	if CollObj.m_CollPlayerNum <= num then
		g_IntObjServerMgr:Destroy(CollObj,false)
	end
end

function DestroyCollObj(CollObj)
	if IsServerObjValid(CollObj) then
		local OwnerId = CollObj.m_OwnerId or CollObj.m_Properties:GetDropOwnerId()
		ClearState(CollObj)
		local Scene,Pos = CollObj.m_Scene,CollObj.m_Pos
		local ObjName = CollObj.m_Properties:GetCharName()
		local TriggerAction = IntObj_Common(ObjName, "TriggerAction")
		if string.find(TriggerAction,"副本采集物品") then
			FbDestroyObj(CollObj)
		elseif string.find(TriggerAction,"Obj保留") then
			g_IntObjServerMgr:IntObjDoAction(CollObj,"respond02_2",1)
			CollObj.m_OpenTime = os.time()
		else
			if OwnerId ~= 0 then
				g_IntObjServerMgr:Destroy(CollObj,false)
			else
				g_IntObjServerMgr:Destroy(CollObj,true)
			end
		end
	end
end 

function Gac2Gas:SelectSingleGridItem(Conn,GetIndex,GlobalID)
	local Player = Conn.m_Player
	if not IsCppBound(Player) then
		return
	end
	local PlayerId = Player.m_uID
	
	local Target = CIntObjServer_GetIntObjServerByID(GlobalID)
	if Target == nil then
		Player.m_CollIntObjID = nil
		BreakPlayerCollItem(Player)
		MsgToConn(Conn,3157)
		return
	end
	
	if g_GetDistance(Player,Target)>6 then --距离判断
		return
	end
	
	if Target.m_Openner and Target.m_Openner ~= PlayerId then
		Player.m_CollIntObjID = nil
		BreakPlayerCollItem(Player)
		--print("已经被其它人搞定了....")
		MsgToConn(Conn,3157)
		return
	end
	
	local ItemsTbl=Target.m_DropItemsTbl
	if ItemsTbl==nil then  --物品表为空
		return
	end
	if ItemsTbl[GetIndex]==nil then --此格子上没有物品
		return
	end
	
	local ItemType,ItemName,ItemNum = ItemsTbl[GetIndex][1],ItemsTbl[GetIndex][2],ItemsTbl[GetIndex][3]
	
	local function CallBack(result)
--		local Player = g_GetPlayerInfo(PlayerId)
--		local Target = CIntObjServer_GetIntObjServerByID(GlobalID)
		if IsCppBound(Player) then
			if not result[1] then
				if GetQuestAddItemErrorMsgID(result[2]) then
					MsgToConn(Player.m_Conn, GetQuestAddItemErrorMsgID(result[2]))
				end
				Gas2Gac:RetCollGetGridItem(Player.m_Conn,GetIndex,false)
			else
				CRoleQuest.add_item_DB_Ret(Player, ItemType,ItemName,ItemNum,result[2])
				g_MatchGameMgr:AddMatchGameCount(Player, 4, ItemName, ItemNum)
				if IsCppBound(Target) and Target.m_DropItemsTbl then
					table.remove(Target.m_DropItemsTbl,GetIndex)
					if Target.m_PlayerToItemTbl and Target.m_PlayerToItemTbl[PlayerId] then
						table.remove(Target.m_PlayerToItemTbl[PlayerId],GetIndex)
					end
				end
				Gas2Gac:RetCollGetGridItem(Player.m_Conn,GetIndex,true)
				
				if IsCppBound(Target) and Target.m_DropItemsTbl and table.getn(Target.m_DropItemsTbl)~=0 then --判断是否所有格子的物品都已经提取完
					return
				end
				Player:DelListeningWnd()
				Gas2Gac:NotifyCloseCollWnd(Player.m_Conn)
				Player.m_CollIntObjID = nil
				
				if Target.m_CollPlayer then
					Target.m_CollPlayer[PlayerId] = true
				end
				DestroyCollObj(Target)
			end
		end
	end

	local parameter = {}
	parameter["char_id"]		= PlayerId
	parameter["nType"]		= ItemType
	parameter["sName"] 		= ItemName
	parameter["nCount"]		= ItemNum
	parameter["sceneName"]		= Player.m_Scene.m_SceneName
	parameter["createType"]		= event_type_tbl["采集单个格子的物品"]
	CallAccountManualTrans(Conn.m_Account, "CollObjDB", "CollSingleGridItem", CallBack, parameter)
end

function Gac2Gas:SelectAllGridItems(Conn,GlobalID)
	local Player = Conn.m_Player
	if not IsCppBound(Player) then
		return
	end
	
	local PlayerId = Player.m_uID
	
	local ServerCollObj = CIntObjServer_GetIntObjServerByID(GlobalID)
	if ServerCollObj == nil then
		Player.m_CollIntObjID = nil
		BreakPlayerCollItem(Player)
		MsgToConn(Conn,3157)
		return
	end	
	if g_GetDistance(Player,ServerCollObj)>6 then --距离判断
		return
	end
	if ServerCollObj.m_Openner and ServerCollObj.m_Openner ~= PlayerId then
		Player.m_CollIntObjID = nil
		BreakPlayerCollItem(Player)
		--print("已经被其它人搞定了....")
		MsgToConn(Conn,3157)
		return
	end
	local ItemsTbl=ServerCollObj.m_DropItemsTbl
	if ItemsTbl==nil then  --物品表为空
		return
	end
	
	local function CallBack(result,removeindex)
--		local Player = g_GetPlayerInfo(PlayerId)
		local Target = CIntObjServer_GetIntObjServerByID(GlobalID)
		if IsCppBound(Player) then
			local allsucc = true
			for i,itemtbl in pairs(result) do
				if IsNumber(itemtbl["RetItemRes"]) then
					if itemtbl["RetItemRes"] == 3 then
						MsgToConn(Player.m_Conn,160000)
					elseif itemtbl["RetItemRes"] == 0 then
--						MsgToConn(Player.m_Conn,3000,'CreateFightItem error')
					end
					allsucc = false
				else
					CRoleQuest.add_item_DB_Ret(Player, itemtbl["nType"],itemtbl["sName"],itemtbl["nCount"],itemtbl["RetItemRes"])
					g_MatchGameMgr:AddMatchGameCount(Player, 4, itemtbl["sName"], itemtbl["nCount"])
					Gas2Gac:RetCollAllItemGrid(Player.m_Conn,itemtbl["Index"])
				end
			end
				
			local num = table.getn(removeindex)
			if Target and Target.m_DropItemsTbl then
				for i = 1, num do
					table.remove(Target.m_DropItemsTbl, removeindex[i]-i+1)
					if Target.m_PlayerToItemTbl and Target.m_PlayerToItemTbl[PlayerId] then
						table.remove(Target.m_PlayerToItemTbl[PlayerId],removeindex[i]-i+1)
					end
				end
			end
	
			if allsucc then
				Player:DelListeningWnd()
				Gas2Gac:RetSelectAllGridItems(Player.m_Conn,true)
				Player.m_CollIntObjID = nil
				
				if ServerCollObj.m_CollPlayer then
					ServerCollObj.m_CollPlayer[PlayerId] = true
				end
				DestroyCollObj(Target)
			else
				Gas2Gac:RetSelectAllGridItems(Player.m_Conn,false)
			end
		end
	end

	local parameter = {}
	parameter["PlayerId"] = PlayerId
	parameter["ItemsTbl"] = ItemsTbl
	parameter["sceneName"] = Player.m_Scene.m_SceneName
	parameter["createType"] = event_type_tbl["采集所有格子的物品"]
	
	CallAccountManualTrans(Conn.m_Account, "CollObjDB","CollAllGridItem", CallBack, parameter)
end

local function CancelCollItems(data)
	local Conn,GlobalID=unpack(data)
	local Player = Conn.m_Player
	if Player then
		if IsCppBound(Player) then
			Player:DelListeningWnd()
		end
		Player.m_CollIntObjID = nil
	end
	local ServerCollObj = CIntObjServer_GetIntObjServerByID(GlobalID)
	if ServerCollObj then
		ClearItemWndCloseTick(ServerCollObj)
		g_IntObjServerMgr:IntObjDoAction(ServerCollObj,"respond02_2",1)
		ServerCollObj.m_Openner=nil
		ServerCollObj.m_PerfectType = nil
		ServerCollObj.m_OpenTime = os.time()
		--ServerCollObj.m_HaveOpen=nil
		if Player then
			Player.m_ProcessingObjID = nil
		end
		--ServerCollObj.m_OpenTime=os.time()
	end
end

function Gac2Gas:CancelCollItems(Conn,GlobalID)
	local data = {Conn,GlobalID}
	CancelCollItems(data)
end

local m_CoefNum = {0.6772,0.6454,0.6772}
function Gac2Gas:SendSuccPerfetColl(Conn,PerfectType)
	if not IsCppBound(Conn) then
		return
	end
	if not IsCppBound(Conn.m_Player) then
		return
	end
	local ObjID = Conn.m_Player.m_ProcessingObjID
	if ObjID == nil then
		return
	end
	if not IsNumber(PerfectType) then
		return
	end
	local CollObj = CIntObjServer_GetIntObjServerByID(ObjID)
	if CollObj == nil then
		return
	end
	local ObjName = CollObj.m_Properties:GetCharName()
	local PerfetCollTime = g_ObjActionArgTbl[ObjName][1]
	if PerfectType > 3 or PerfectType < 1 then
		PerfectType = 1
	end
	local iCoefNum = m_CoefNum[PerfectType]
	
	if (GetProcessTime() - CollObj.m_BeginCollTime) < (PerfetCollTime*1000*iCoefNum) then
		return
	end
	
	if CollObj.m_IsPerfectColl then
		CollObj.m_PerfectType = PerfectType
		if Conn.m_Player.m_ActionLoadingTick then
			PerfectCollSuccStopLoading(Conn.m_Player)
		else
			CollObj.m_IsPerfectColl = nil
		end
	end
end
