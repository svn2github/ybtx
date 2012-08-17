cfg_load "item/FlowersItem_Common"

local function UseCultivateFlowerItem(Conn, ItemName, RoomIndex, Pos)
	local Player = Conn.m_Player
	local function CallBack(result)
		if IsCppBound(Conn) and IsCppBound(Conn.m_Player) then
			local Player = Conn.m_Player
			if result[1] then
				CRoleQuest.delete_item_DB_Ret(Player, nil, nil, nil, result[2])
				CFlower.CultivateFlower(Conn, FlowersItem_Common(ItemName,"Arg"))
				if result[3] and result[3] ~= 0 then
					MsgToConn(Conn, 9618, result[3])
					if result[4] then
						CLiveskillBase.RetLiveskillInfo(Conn,result[4])
					end
				end
			else
				if result[2] ~= nil then
					if result[2][2] then
						MsgToConn(Conn, result[2][1], GetFlowerDispalyName(result[2][2]))
					else
						MsgToConn(Conn, result[2][1])
					end
				end
			end
		end
	end
	local data = {}
	data["PlayerID"]   = Player.m_uID
	data["flowerName"] = FlowersItem_Common(ItemName,"Arg")
	data["needLevel"] = FlowersItem_Common(ItemName,"SkillLevel")
	data["RoomIndex"] = RoomIndex
	data["Pos"] = Pos
	data["ItemNum"] = 1
	data["ItemName"] = ItemName
	data["sceneName"] = Player.m_Scene.m_SceneName
	CallAccountManualTrans(Conn.m_Account, "GatherLiveSkillDB", "CultivateFlower", CallBack, data)
end

return UseCultivateFlowerItem