local function AddMoneyAward(Arg, Trigger, Player)
	if not IsCppBound(Player) then
		return
	end
	if Player.m_Properties:GetType() == ECharacterType.Player then
		local AddMoneyFun = g_TriggerMgr:GetExpressions(Arg[1])
		local nMoney = AddMoneyFun(Player:CppGetLevel())
		local type = 2
		if Arg[2] == "非绑定" then
			type = 1
		end
		local function callback(result,uMsgID)
			if IsNumber(uMsgID) then
				MsgToConn(Player.m_Conn,uMsgID)
			end
			if IsCppBound(Player) then
				if result then
					MsgToConn(Player.m_Conn, 3602, nMoney)
				end
			end
		end
		local data = {}
		data["char_id"] = Player.m_uID
		data["money_count"] = nMoney
		data["addType"]	= event_type_tbl["玩家踩trap加钱"]
		data["money_type"] = type
		CallAccountAutoTrans(Player.m_Conn.m_Account, "MoneyMgrDB", "AddMoneyForRpc", callback, data)
	end
end


local function AddExpAward(Arg, Trigger, Player)
	if not IsCppBound(Player) then
		return
	end
	if Player.m_Properties:GetType() == ECharacterType.Player then
		local AddExpFun = g_TriggerMgr:GetExpressions(Arg[1])
		local addExp = AddExpFun(Player:CppGetLevel())
		local function callback(CurLevel,LevelExp)
			if IsCppBound(Player) and CurLevel then
				if CurLevel then
					local AddExpTbl = {}
					AddExpTbl["Level"] = CurLevel
					AddExpTbl["Exp"] = LevelExp
					AddExpTbl["AddExp"] = addExp
					AddExpTbl["uInspirationExp"] = 0
					CRoleQuest.AddPlayerExp_DB_Ret(Player, AddExpTbl)
				end
			end
		end
		local data = {}
		data["char_id"] = Player.m_uID
		data["addExp"] = addExp
		data["addExpType"] = event_type_tbl["玩家踩Trap给经验"]
		
		OnSavePlayerExpFunc({Player})
		CallAccountManualTrans(Player.m_Conn.m_Account, "RoleQuestDB", "AddExp", callback, data)
	end
end


local function AddItem(Arg, Trigger, Player)
	local PlayerID = Player.m_uID;	--玩家ID]
	local TaskName = Arg[1]--任务名称	
	local ItemType = Arg[2]--物品类型
	local ItemName = Arg[3]--物品名称
	local ItemNum = Arg[4]--物品数量
	local questneed				--玩家任务

	local parameter = {}
	parameter["sQuestName"] = TaskName
	parameter["char_id"]		= PlayerID
	parameter["nType"]		= ItemType
	parameter["sName"] 		= ItemName
	parameter["nCount"]		= ItemNum
	parameter["sceneName"]		= Player.m_Scene.m_SceneName
	local Conn = Player.m_Conn
	local function CallBack(result)
		if IsNumber(result) then
			if result == 3 then
				if IsCppBound(Conn) then
					MsgToConn(Conn,160000)
				end
			end
			return
		end
		CRoleQuest.add_item_DB_Ret_By_Id(PlayerID,ItemType,ItemName,ItemNum,result)
	end
	if IsCppBound(Conn) and Conn.m_Account then
		CallAccountAutoTrans(Conn.m_Account, "RoleQuestDB","IntoTrapAddItem", CallBack, parameter)
	end
end


local function AddItemAward(Arg, Trigger, Player)
	if Player.m_Properties:GetType() == ECharacterType.IntObj then
		return 
	end 
	if Player.m_Properties:GetType() == ECharacterType.Npc then
		local PlayerID = Trigger.m_OwnerId   --摆放
		if PlayerID == nil then
			PlayerID = Player.m_OwnerId   --天然
			if PlayerID == nil then
				return
			end
		end
		Player = g_GetPlayerInfo(PlayerID)
		if not Player then
			return
		end
	end
	AddItem(Arg, Trigger, Player)
end


local funcTbl = {

	["物品"]		= AddItemAward,
	["钱"]		= AddMoneyAward,
	["经验"]		= AddExpAward,
}

local function Script(Arg, Trigger, Player)
	local type = Arg[1]
	if IsFunction (funcTbl[type]) then
		funcTbl[type](Arg[2], Trigger, Player)
	end
end

return Script