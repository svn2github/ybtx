cfg_load "tong/TongBuildHonor_Common"

local function Script(Arg, Npc)
	--IsObjValid
	local npcName = Npc.m_Name
	local scene = Npc.m_Scene
	local fPos = CFPos:new()
	local camp = Npc:CppGetCamp()
	Npc:GetPixelPos(fPos)
	local tongId = Npc.m_TongID
	if not tongId or tongId == 0 then
		local NeedRes, ResFoldLimit
		if npcName == g_WarZoneMgr.m_StationBaseCampTbl[camp] then
			NeedRes = g_ItemInfoMgr:GetItemInfo(g_ItemInfoMgr:GetTongItemBigID(), npcName,"NeedRes")
			ResFoldLimit = g_ItemInfoMgr:GetItemInfo(g_ItemInfoMgr:GetTongItemBigID(), npcName,"ResFoldLimit")
		else
			NeedRes = g_ItemInfoMgr:GetItemInfo(g_ItemInfoMgr:GetTongItemBigID(), g_WarZoneMgr.m_WarZoneDefenderTbl[camp],"NeedRes")
			ResFoldLimit = g_ItemInfoMgr:GetItemInfo(g_ItemInfoMgr:GetTongItemBigID(), g_WarZoneMgr.m_WarZoneDefenderTbl[camp],"ResFoldLimit")
		end
		
		local dropResources = NeedRes * 0.20
		DropResource(scene, fPos.x, fPos.y, dropResources, ResFoldLimit)
		
		--加荣誉
		local uPlayerId = Npc.m_KillerID
		local Player = CEntityServerManager_GetEntityByID(uPlayerId)
		if not Player or Player.m_Properties:GetType() ~= ECharacterType.Player then
			return
		end
		local uTongID = Player.m_Properties:GetTongID()
		local uAddHonor = TongBuildHonor_Common(npcName, "OriginalHonor")
		-- 摧毁后 显示灰色图标
		if string.find(Npc.m_Name, "大本营") then
			Gas2Gac:UpdateWarZoneState(scene.m_CoreScene, Npc.m_StationIndex, 0, "")
			scene.m_WarZone.m_StationOwnerCamp[Npc.m_StationIndex] = 0
		end
		if uTongID ~= 0 and uAddHonor then
			local function HonorCallBack(flag, msgId)
				if not flag and IsNumber(msgId) then
					MsgToConn(Player.m_Conn, msgId)
					return
				else
					MsgToConn(Player.m_Conn, 9439, uAddHonor)
				end
			end
			local parameters = {}
			parameters["uTongID"] = uTongID
			parameters["uHonor"] = uAddHonor
			parameters["nEventType"] = 132
			CallDbTrans("GasTongBasicDB", "UpdateHonor", HonorCallBack, parameters, uTongID)
		end
	end
end

return Script