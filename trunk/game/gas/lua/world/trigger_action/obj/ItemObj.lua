gas_require "world/int_obj/IntObjServer"
gas_require "activity/quest/RoleQuest"
gas_require "world/trigger_action/obj/AssignObj"
gas_require "world/trigger_action/obj/assign_mode/NeedAssign"
gas_require "world/trigger_action/obj/assign_mode/AcutionAssign"
gac_gas_require "item/item_info_mgr/ItemInfoMgr"



local g_GetDistance = g_GetDistance
local os = os
local MsgToConn = MsgToConn
local g_ObjActionArgTbl = g_ObjActionArgTbl
local DropItemProtectTime = DropItemProtectTime
local IsCppBound = IsCppBound
local EAssignMode = EAssignMode
local g_AverageAssignOrder = g_AverageAssignOrder
local NpcDropObjTbl = NpcDropObjTbl
local NeedAssignOneObj = NeedAssignOneObj
local GetQuestAddItemErrorMsgID = GetQuestAddItemErrorMsgID
local BreakPlayerActionLoading = BreakPlayerActionLoading
local g_AreaMgr = g_AreaMgr
local LogErr = LogErr
local AllotObj = AllotObj
local AuctionAssignOneObj = AuctionAssignOneObj

local g_ItemInfoMgr = CItemInfoMgr:new()

local g_GetPlayerInfo = g_GetPlayerInfo
local Entry  = CreateSandBox(...)

function Entry.GetPackData(Conn, ItemObj, ObjName, ObjID) 
	local PlayerId = Conn.m_Player.m_uID
	
	if g_GetDistance(Conn.m_Player,ItemObj)>6 then --判断距离
		return
	end

	local ItemType = g_ObjActionArgTbl[ObjName][1]
	local data = {}
	data["char_id"]		= PlayerId
	data["nType"]		= ItemType
	data["sName"] 		= ObjName
	
	local IsProTime = true
	if ItemObj.m_Scene.m_SceneAttr.SceneType == 5 then
		local DropNpcID = ItemObj.m_BelongToNpc
		local DropObjTbl = NpcDropObjTbl[DropNpcID]
		if DropNpcID and not DropObjTbl then
			return
		end
		local Sharertbl = DropObjTbl.m_CanSharePlayer
		local IsInShare = false
		if not IsTable(Sharertbl) then
			IsInShare = true
		else
			--5类地下城副本 只有在分配列表中的玩家可以拾取
			for i = 1, table.getn(Sharertbl) do
				if Sharertbl[i] == PlayerId then   
					IsInShare = true
					break
				end
			end
		end
		if not IsInShare then
			MsgToConn(Conn, 3034)
			return
		end
		IsProTime = false
	end
--	local WaitTime = 20
	if ItemObj.m_QuestObjOwnner then
		if ItemObj.m_QuestObjOwnner ~= PlayerId then
			MsgToConn(Conn, 3034)
			return
		else
			data["CheckQuestNeed"] = true
			return data
		end
	elseif ItemObj.m_FreePickup then
		data["AssignMode"] = EAssignMode.eAM_FreePickup
		return data
	elseif ItemObj.m_NeedMaster then
		return data
	else
		local DropNpcID = ItemObj.m_BelongToNpc
		local DropObjTbl = NpcDropObjTbl[DropNpcID]
		if DropNpcID and not DropObjTbl then
			return
		end
		
		local BelongTeam = ItemObj.m_Properties:GetBelongToTeam()
		local DropOwnerId = ItemObj.m_Properties:GetDropOwnerId()
		local DropTime = ItemObj.m_Properties:GetDropTime()
		if BelongTeam ~= 0 then  --该npc掉落的物品已经指明了属于哪个队伍
			local TeamID = Conn.m_Player.m_Properties:GetTeamID()
			local IsInSharertbl = false 
--			if TeamID == BelongTeam then
--				IsInSharertbl = true
--			else
				local Sharertbl = DropObjTbl.m_CanSharePlayer
				for i = 1, table.getn(Sharertbl) do
					--玩家的队伍已经解散，但在Npc上仍然记录着可以分享物品的玩家列表，而且玩家在次列表中
					if Sharertbl[i] == PlayerId then   
						IsInSharertbl = true
						break
					end
				end
--			end
			
			if IsInSharertbl then   --玩家在物品享有者的列表里
				local AssignMode = DropObjTbl.m_AssignMode				--物品 分配置模式
				local AuctionStandard = DropObjTbl.m_AuctionStandard   --   拍卖模式 品质设定	
				if AssignMode == EAssignMode.eAM_RespectivePickup then    --各自拾取
					if PlayerId == DropOwnerId  --队伍成员各自拾取自己打死的npc掉落的物品
						or (DropTime ~= 0 and IsProTime and (os.time()-DropTime) >= DropItemProtectTime) then 
						return data
					end
				elseif AssignMode == EAssignMode.eAM_FreePickup then  --队伍成员自由拾取队内所有人打落的物品  自由拾取
					return data
				elseif AssignMode == EAssignMode.eAM_AverageAssign then  --队伍成员平均分配队内所有人打落的物品  平均分配
					--打开平均分配的拾取面板
					data["AssignMode"] = EAssignMode.eAM_AverageAssign
					if g_AverageAssignOrder[BelongTeam] then
						data["AverageAssignOrder"] = g_AverageAssignOrder[BelongTeam]
					end
					data["CanSharePlayer"] = DropObjTbl.m_CanSharePlayer
					return data
				elseif AssignMode == EAssignMode.eAM_AuctionAssign then  --在队伍内对队内所有人打落的物品进行拍卖  拍卖模式
					--打开拍卖模式的面板(暂时用个人的模式,因为没有实现)
					data["AssignMode"] = EAssignMode.eAM_AuctionAssign
					local Quality = g_ItemInfoMgr:GetItemInfo( ItemType, ObjName,"Quality" )
--					print("物品+=+=+=+=品质", Quality)
					if AuctionStandard > Quality then  --物品的品质级
						data["AssignMode"] = EAssignMode.eAM_AverageAssign
						if g_AverageAssignOrder[BelongTeam] then
							data["AverageAssignOrder"] = g_AverageAssignOrder[BelongTeam]
						end
						data["CanSharePlayer"] = DropObjTbl.m_CanSharePlayer
						return data
					end
					ItemObj.m_AssignPlayer = DropObjTbl.m_CanSharePlayer
					data["AuctionStandard"] = AuctionStandard
					data["AuctionBasePrice"] = DropObjTbl.m_AuctionBasePrice
					AuctionAssignOneObj(Conn, data, ItemObj, ObjID)
				elseif AssignMode == EAssignMode.eAM_NeedAssign then --需求分配模式
					local Quality = g_ItemInfoMgr:GetItemInfo( ItemType, ObjName,"Quality" )
					if AuctionStandard > Quality then  --物品的品质级
						data["AssignMode"] = EAssignMode.eAM_AverageAssign
						if g_AverageAssignOrder[BelongTeam] then
							data["AverageAssignOrder"] = g_AverageAssignOrder[BelongTeam]
						end
						data["CanSharePlayer"] = DropObjTbl.m_CanSharePlayer
						return data
					end
					ItemObj.m_AssignPlayer = DropObjTbl.m_CanSharePlayer
					NeedAssignOneObj(Conn, data, ItemObj, ObjID)
				end
			elseif DropTime ~= 0 and IsProTime and (os.time()-DropTime)>=DropItemProtectTime then
				return data
			else
				MsgToConn(Conn, 3034)
			end
		--物品没有指明属于哪个队伍,掉落的物品属于该玩家
		else
			if DropOwnerId ~= 0 then
				if PlayerId == DropOwnerId
					or (DropTime ~= 0 and IsProTime and (os.time()-DropTime)>=DropItemProtectTime) then
					return data
				else
					MsgToConn(Conn, 3034)
				end
			else
				return data
			end
		end
	end
	return
end

function Entry.Exec(Conn, ItemObj, ObjName, ObjID)
	if ItemObj.m_IsClick then
		return
	end
	
	local data = Entry.GetPackData(Conn, ItemObj, ObjName, ObjID)
	if data == nil then
		return
	end
	
	if Conn.m_Player.m_ActionLoadingTick then
		BreakPlayerActionLoading(Conn.m_Player)
	end
	
--	Conn.m_Player:IsFirstTimeAndSendMsg("拾取", 2005, sMsg)
	
	data["sceneName"] = Conn.m_Player.m_Scene.m_SceneName
	AllotObj( Conn, ItemObj, ObjName, ObjID, data)
end

return Entry
