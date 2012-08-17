cfg_load "item/MercCardItem_Common"

local function UseMercCardItem(Conn, ItemName, RoomIndex, Pos)
	local Player = Conn.m_Player
	if not IsCppBound(Player) then
		return
	end
	
	local function CallBack(result, CardId)
		if not IsCppBound(Conn) then
			return
		end
		if IsCppBound(Player) then
			if IsNumber(result) then
				MsgToConn(Conn, 195000+result)
			elseif IsTable(result) then
				CRoleQuest.delete_item_DB_Ret(Player, nil, nil, nil, result)
				Gas2Gac:ShowMercCard(Conn, CardId)
				table.insert(Player["m_MercCardInfo"], CardId)
			end
		end
	end
	
	local parameter = {}
	parameter["PlayerId"] = Player.m_uID
	parameter["RoomIndex"] = RoomIndex
	parameter["Pos"] = Pos
	parameter["ItemNum"] = 1
	parameter["CardId"] = MercCardItem_Common(ItemName,"GameID")
	parameter["sceneName"] = Player.m_Scene.m_SceneName
	CallAccountManualTrans(Conn.m_Account, "CollectMercCardDB", "DelItem", CallBack, parameter)
end

return UseMercCardItem