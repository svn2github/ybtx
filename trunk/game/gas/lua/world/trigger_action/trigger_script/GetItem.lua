gas_require "log_mgr/CLogMgr"

local function Script(Arg, Trigger, Player)
	
	if not IsCppBound(Player) then
		return
	end
	local PlayerId = Player.m_uID
	local function CallBack(result1,result2)
		if not result1 then
			if result2 == 3 then
				if IsCppBound(Player) and IsCppBound(Player.m_Conn) then
					MsgToConn(Player.m_Conn, 8003, '包裹空间不足')
				end
				return
			end
			return
		end
		
		if result2 then
			for i,itemtbl in pairs(result2) do
				CRoleQuest.add_item_DB_Ret_By_Id(PlayerId, itemtbl["nType"],itemtbl["sName"],itemtbl["nCount"],itemtbl["AddItemRes"])
			end
		end
	end
	local data = {}
	data["PlayerId"] = Player.m_uID
	data["AddItemsTbl"] = Arg
	data["sceneName"]	= Player.m_Scene.m_SceneName
	data["createType"]	= event_type_tbl["点npc给物品"]
	CallAccountManualTrans(Player.m_Conn.m_Account, "CharacterMediatorDB", "GetItems", CallBack, data)
end

return Script