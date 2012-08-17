cfg_load "item/PickOreItem_Common"
CPickOre = class()

local function ProgressSetCtrlState(Player, state)
	Player:CppSetCtrlState(EFighterCtrlState.eFCS_ForbitAutoTrack, state)
	Player:SetInGatherProcess(state)
	Player:CppSetCtrlState(EFighterCtrlState.eFCS_ForbitMove, state)
	Player:CppSetCtrlState(EFighterCtrlState.eFCS_ForbitTurnAround, state)
	Player:LuaSetDoSkillCtrlState(EDoSkillCtrlState.eDSCS_InDoingSkill, state)
	if state then
		Player:AddListeningWnd()
	else
		Player:DelListeningWnd()
	end
	local actionState = state and EActionState.eAS_PickOre or EActionState.eAS_Idle_BackWpn
	Player:SetAndSyncActionState(actionState)
end

function CPickOre:Ctor(Player, ItemId, ItemName, RoomIndex, Pos)
	self.m_ItemId = ItemId
	self.m_ItemName = ItemName
	self.m_RoomIndex = RoomIndex
	self.m_Pos = Pos
	self.m_PlayerID = Player.m_uID
	self:PickOreStart(Player)
end

function CPickOre:PickOreStart(Player)
--print("开始使用:"..self.m_ItemName)
	ProgressSetCtrlState(Player, true)
	MsgToConnById(self.m_PlayerID, 9627)
	Gas2GacById:SetItemGridWndState(self.m_PlayerID, self.m_RoomIndex, self.m_Pos, false)
	Gas2Gac:PickOreStart(Player.m_Conn)
	local function CallBack(result)
		if IsTable(result[3]) then
			for i,itemtbl in ipairs(result[3]) do
				CRoleQuest.add_item_DB_Ret_By_Id(self.m_PlayerID, itemtbl["nType"],itemtbl["sName"],itemtbl["nCount"],itemtbl["AddItemRes"])
				MsgToConnById(self.m_PlayerID, 9631, itemtbl["nCount"], itemtbl["sName"])
			end
		elseif IsNumber(result[3]) then
			if IsCppBound(Player) then
				self:PickOreEnd(Player, result[3])
			end
			return
			--MsgToConnById(self.m_PlayerID, result[3])
		end
		
		if IsTable(result[2]) then
			local AddExp, skill_info = result[2][1],result[2][2]
			if AddExp and AddExp ~= 0 then
				MsgToConnById(self.m_PlayerID, 9624, AddExp)
				if skill_info then
					if IsCppBound(Player.m_Conn) then
						CLiveskillBase.RetLiveskillInfo(Player.m_Conn,skill_info)
					end
				end
			end
		end
		
		if IsTable(result[1]) then
			local MaxDura, CurDura = result[1][1],result[1][2]
			if CurDura <= 0 then
				if IsCppBound(Player) then
					self:PickOreEnd(Player, 9629)
				end
			end
			Gas2Gac:RetPickOreItemInfo(Player.m_Conn, self.m_ItemId, MaxDura, CurDura)
		end
		if result[4] then
			Gas2Gac:UpdatePlayerCurOffLineExp(Player.m_Conn, result[4])
		end
	end
	local UseLimit = PickOreItem_Common(self.m_ItemName,"UseLimit")
	local AddOffLineExp = PickOreItem_Common(self.m_ItemName,"AddOffLineExp")
	local uOffLineExpValue
	if AddOffLineExp ~= "" then
		local AddExpFun = g_TriggerMgr:GetExpressions(AddOffLineExp)
		uOffLineExpValue = AddExpFun(Player:CppGetLevel())
	end
	local function TickFun()
		if not IsCppBound(Player) or not IsCppBound(Player.m_Conn) then
			return
		end
		local f = Player.m_Properties.m_InTrapTblName[UseLimit]
		if f == nil or not next(f) then
			MsgToConn(Player.m_Conn, 820, UseLimit, self.m_ItemName)
			self:PickOreEnd(Player)
		 	return
		end
		local data = {}
		data["PlayerID"] = Player.m_uID
		data["ItemId"] = self.m_ItemId
		data["ItemName"] = self.m_ItemName
		if AddOffLineExp then
			data["uOffLineExpValue"] = uOffLineExpValue
		end
		CallAccountManualTrans(Player.m_Conn.m_Account, "GatherLiveSkillDB", "PickOre", CallBack, data)
	end
	local TickTime = PickOreItem_Common(self.m_ItemName,"Interval")
	self.m_Tick = RegisterTick("PickOreTick", TickFun, TickTime*1000, data)
end

function CPickOre:PickOreEnd(Player, MsgID)
--print("结束使用:"..self.m_ItemName)
	MsgToConnById(self.m_PlayerID, MsgID or 9628)
	if self.m_Tick then
		UnRegisterTick(self.m_Tick)
		self.m_Tick = nil
	end
	if Player.m_PickOre then
		ProgressSetCtrlState(Player, false)
		Player.m_SetCtrlState = 13
		Gas2GacById:SetItemGridWndState(self.m_PlayerID, self.m_RoomIndex, self.m_Pos, true)
		Gas2Gac:PickOreEnd(Player.m_Conn)
		Gas2Gac:PickOreActionEnd(Player:GetIS(0), Player:GetEntityID())
		Player.m_PickOre = nil
	end
	self = nil
end
