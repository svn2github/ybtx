gas_require "log_mgr/CLogMgr"

function SendPlayerFinishDirect(Player, DirectTbl)
	if not IsCppBound(Player) then
		return
	end
	if not DirectTbl or not next(DirectTbl) then
		return
	end
--	Gas2Gac:ShowNewDirectBegin(Player.m_Conn)
	for i, v in pairs(DirectTbl) do
		Gas2Gac:ShowNewDirect(Player.m_Conn, i, v)
		--print("    DirectTbl    ",i,v)
	end
--	Gas2Gac:ShowNewDirectEnd(Player.m_Conn)
end

function SendPlayerFinishDirectById(CharId, DirectTbl)
	if not DirectTbl or not next(DirectTbl) then
		return
	end
	for i, v in pairs(DirectTbl) do
		Gas2GacById:ShowNewDirect(CharId, i, v)
		--print("    DirectTbl    ",CharId,i,v)
	end
end

function DbCallBack:PlayerFinishDirect(CharId, DirectTbl)
	SendPlayerFinishDirectById(CharId, DirectTbl)
end

function DbCallBack:MemberFinishDirect(DirectsTbl)
	for i, v in pairs(DirectsTbl) do
		SendPlayerFinishDirectById(i, v)
	end
end

function FinishPlayerDirect(Conn, DirectName)
	--print("FinishPlayerDirect", DirectName)
	local Player = Conn.m_Player
	if not IsCppBound(Player) then
		return
	end
	local PlayerId = Player.m_uID
	local function CallBack(result, DirectTbl)
		if IsNumber(result) then
			SendPlayerFinishDirectById(PlayerId, DirectTbl)
			return
		end
		if result then
			Gas2GacById:ShowNewDirect(PlayerId, DirectName, -1)
		end
	end

	local data = {}
	data["CharID"] = PlayerId
	data["DirectName"] = DirectName
	CallAccountManualTrans(Conn.m_Account, "DirectDB", "FinishPlayerDirect", CallBack, data)
--	CallAccountManualTrans(Conn.m_Account, DirectDB, "FinishPlayerDirect", CallBack, data)
end

--完成指引
function Gac2Gas:FinishPlayerDirect(Conn, DirectName)
	FinishPlayerDirect(Conn, DirectName)
end

function Gac2Gas:GetDirectAwardItem(Conn, BigType, ActionType)
	local Player = Conn.m_Player
	if not IsCppBound(Player) then
		return
	end
	local PlayerId = Player.m_uID
	local function CallBack(result1, result2)

		if IsNumber(result1) then
			if result1 == 3 then
				MsgToConnById(PlayerId, 330001)
				Gas2GacById:UpdateDirectAwardInfo(PlayerId, ActionType)
				return
			end
			SendPlayerFinishDirectById(PlayerId, result2)
			return
		end
		if not result1 then
			if result2 == 3 then
				MsgToConnById(PlayerId, 160000)
				return
			end
		end
		
		if result2 then
			for i,itemtbl in pairs(result2) do
				CRoleQuest.add_item_DB_Ret_By_Id(PlayerId, itemtbl["nType"],itemtbl["sName"],itemtbl["nCount"],itemtbl["AddItemRes"])
			end
			Gas2GacById:UpdateDirectAwardInfo(PlayerId, ActionType)
		end
	end
	local data = {}
	data["CharID"] = PlayerId
	data["BigType"] = BigType
	data["ActionType"] = ActionType
	data["sceneName"]	= Player.m_Scene.m_SceneName
	data["createType"]	= event_type_tbl["指引奖励"]
	CallAccountManualTrans(Conn.m_Account, "DirectDB", "GetDirectAwardItem", CallBack, data)
end

function Gac2Gas:AddPlayerDirectAction(Conn, Action, Type, Name)
	local Player = Conn.m_Player
	if not IsCppBound(Player) then
		return
	end
	local PlayerId = Player.m_uID
	local function CallBack(DirectTbl)
		SendPlayerFinishDirectById(PlayerId, DirectTbl)
	end
	local data = {}
	data["CharID"] = PlayerId
	data["Action"] = Action
	data["Type"] = Type
	data["Arg"] = Name
	CallAccountAutoTrans(Conn.m_Account, "DirectDB", "AddPlayerDirectAction", CallBack, data)
end

function Gac2Gas:UpdatePlayerDirect(Conn)
	local Player = Conn.m_Player
	if not IsCppBound(Player) then
		return
	end
	local function CallBack(DirectTbl)
		if not IsCppBound(Player) then
			return
		end
		if not DirectTbl or not next(DirectTbl) then
			return
		end
		for i, v in pairs(DirectTbl) do
			Gas2Gac:ShowNewDirect(Player.m_Conn, i, v)
		end
	end
	local data = {}
	data["CharID"] = Player.m_uID
	CallAccountAutoTrans(Conn.m_Account, "DirectDB", "UpdatePlayerDirect", CallBack, data)
end
