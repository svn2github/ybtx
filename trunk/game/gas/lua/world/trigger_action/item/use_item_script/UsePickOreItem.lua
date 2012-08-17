cfg_load "item/PickOreItem_Common"

local function UsePickOreItem(Conn, ItemName, RoomIndex, Pos)
	local Player = Conn.m_Player
	local Interval = PickOreItem_Common(ItemName,"Interval")
	local UseLimit = PickOreItem_Common(ItemName,"UseLimit")
	if Player.m_PickOre then
		return
	end
	if Player:CppGetDoSkillCtrlState(EDoSkillCtrlState.eDSCS_InDoingSkill) then
		MsgToConn(Conn, 838)
		return
	end
	if Player.m_CollIntObjID or Player.m_PickUpNpcID then
		MsgToConn(Conn, 3060)
		return
	end
	
	local function CallBack(result)
		if IsCppBound(Conn) and IsCppBound(Conn.m_Player) then
			local Player = Conn.m_Player
			if result[1] then
				local f = Player.m_Properties.m_InTrapTblName[UseLimit]
				if f == nil or not next(f) then
					MsgToConn(Conn, 820, UseLimit, ItemName)
				 	return
				end
				--Gas2Gac:SetItemGridWndState(Conn, RoomIndex, Pos, false)
				Player.m_PickOre = CPickOre:new(Player, result[2], ItemName, RoomIndex, Pos)
			else
				if result[2] then
					if result[2][2] then
						MsgToConn(Conn, result[2][1], result[2][2])
					else
						MsgToConn(Conn, result[2][1])
					end
				end
			end
		end
	end
	local data = {}
	data["PlayerID"]	= Player.m_uID
	data["ItemName"]	= ItemName
	data["needLevel"] = PickOreItem_Common(ItemName,"SkillLevel")
	data["RoomIndex"] = RoomIndex
	data["Pos"]				= Pos
	data["sceneName"] = Player.m_Scene.m_SceneName
	CallAccountManualTrans(Conn.m_Account, "GatherLiveSkillDB", "PickOreStart", CallBack, data)
end

return UsePickOreItem