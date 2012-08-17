cfg_load "tong/TongBuildHonor_Common"

local function Script(Arg, Npc)
	local camp = Npc:CppGetCamp()
	local npcName = Npc.m_Name
	local tongId = Npc.m_TongID
	local scene = Npc.m_Scene
	local fPos = CFPos:new()
	Npc:GetPixelPos(fPos)
	
	if not tongId or tongId == 0 then
		return
	end
	
	local function DropCallBack(dropResources)
		if dropResources > 0 then
			local ResFoldLimit = g_ItemInfoMgr:GetItemInfo(g_ItemInfoMgr:GetTongItemBigID(), npcName,"ResFoldLimit")
			DropResource(scene, fPos.x, fPos.y, dropResources, ResFoldLimit)
		end
	end
	
	local parameter = {}
	parameter["uTongID"] = tongId
	parameter["BuildingName"] = npcName
	CallDbTrans("GasTongBasicDB", "TongBuildingDeadSubResource", DropCallBack, parameter, tongId)
	
	--º”»Ÿ”˛
	local uPlayerId = Npc.m_KillerID
	local Player = CEntityServerManager_GetEntityByID(uPlayerId)
	if Player.m_Properties:GetType() == ECharacterType.Player then
		local uTongID = Player.m_Properties:GetTongID()
		if uTongID ~= 0 then
			local function HonorCallBack(flag, msgId)
				if not flag and IsNumber(msgId) then
					MsgToConn(Player.m_Conn, msgId)
					return
				else
					MsgToConn( Player.m_Conn, 9439, TongBuildHonor_Common(npcName, "OriginalHonor") )
					return
				end
			end
			local parameters = {}
			parameters["uTongID"] = uTongID
			parameters["uHonor"] = TongBuildHonor_Common(npcName, "OriginalHonor")
			parameters["nEventType"] = 131
			CallDbTrans("GasTongBasicDB", "UpdateHonor", HonorCallBack, parameters, uTongID)
		end
	end
	if npcName ~= g_WarZoneMgr.m_StationBaseCampTbl[camp] then
		local function DestroyCallBack(RetTongID)
			CTongTech.SendTeachPropertyByServer(Player.m_Conn,RetTongID)
		end
		local parameters = {}
		parameters["build_id"]	= Npc.m_BuildingId
		parameters["uTongID"]	= tongId
		CallDbTrans("GasTongbuildingDB", "DestroyBuilding", DestroyCallBack, parameters, tongId)
	end
end

return Script