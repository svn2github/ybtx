cfg_load "item/OreAreaMap_Common"
gas_require "world/trigger_action/item/item_trigger/CreateAreaScene"
gas_require "world/trigger_action/item/item_trigger/UseItemStateCheck"

local function GetMapInfo(Conn, ItemName, res)
	local player = Conn.m_Player
	if #res == 5 then
		local ItemId,map,x,y = unpack(res, 1, 4)
		player.OreMapID = ItemId
		local pos = CFPos:new()
		local playerPos = CFPos:new()
		player:GetPixelPos(playerPos)
		pos.x = x * EUnits.eGridSpanForObj
		pos.y = y * EUnits.eGridSpanForObj
		if map == player.m_Scene:GetLogicSceneName() and CheckDist(playerPos,pos) < 3 then
			player.m_OreMapName = ItemName
			local SceneName = RandomSceneMap()
			Gas2Gac:CreateAreaSceneMsgBox(Conn, SceneName)
		else
			Gas2Gac:OreMapInfoShow(Conn, map, x, y)
		end
	else
		player.m_OreMapName = ItemName
		local SceneName = RandomSceneMap()
		Gas2Gac:CreateAreaSceneMsgBox(Conn, SceneName)
		player.OreMapID = res[1]
	end
end

local function UseOreAreaMap(Conn, ItemName, RoomIndex, Pos)
	if Conn.m_Player:CppGetCtrlState(EFighterCtrlState.eFCS_InBattle) then
		MsgToConn(Conn, 844, ItemName)
		return
	end
	if Conn.m_Player:IsMoving() then
		MsgToConn(Conn, 845, ItemName)
		return
	end
	local baseLevel = OreAreaMap_Common(ItemName, "BaseLevel")
	if not IsCppBound(Conn) or not IsCppBound(Conn.m_Player) then
		return
	end
	local level = Conn.m_Player:CppGetLevel()
	if level < baseLevel then
		local DisplayName = g_ItemInfoMgr:GetItemLanInfo(30, ItemName,"DisplayName")
		MsgToConn(Conn, 390001, DisplayName)
		return
	end
	local function CallBack(res)
		if not IsCppBound(Conn) or not IsCppBound(Conn.m_Player) then
			return
		end
		if res then
			local SkillLevel = OreAreaMap_Common(ItemName,"SkillLevel")
			local playerSkillLevel = 1
			if #res == 2 then
				playerSkillLevel = res[2]
			else
				playerSkillLevel = res[5]
			end
			if playerSkillLevel >= SkillLevel then
				GetMapInfo(Conn, ItemName, res)
			else
				MsgToConn(Conn, 9620, SkillLevel)
			end
		end
	end
	local data = {}
	data["nCharID"] = Conn.m_Player.m_uID    
	data["nRoom"]		= RoomIndex              
	data["nPos"]		= Pos
	data["ItemName"] = ItemName
	CallAccountManualTrans(Conn.m_Account, "OreMapItemDB", "QueryMapInfo", CallBack, data)                
end

return UseOreAreaMap
