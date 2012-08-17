TempItemFailFun = {}
TempItemArg = {}

function StopItemProgressTick(Player, OnBreak)
	if not IsCppBound(Player) then
		return
	end
	local PlayerId = Player.m_uID
	local data = Player.m_UseItemLoadingTick.m_TickArg[1]
	local FailFun = data[3]
	local Arg = data[4]
	local attBreak	= data[6]
	if OnBreak and (not attBreak) then
		return
	end
	Gas2Gac:NotifyUseItemProgressStop(Player.m_Conn)
	MsgToConn(Player.m_Conn, 833, Arg.ItemName)
	UnRegisterTick(Player.m_UseItemLoadingTick)
	Player.m_UseItemLoadingTick = nil
	Player.m_IsBattle = nil
	Player:DelListeningWnd()
	Player:CppSetCtrlState(EFighterCtrlState.eFCS_ForbitAutoTrack, false)
	Player:SetInGatherProcess(false)
	Player:CppSetCtrlState(EFighterCtrlState.eFCS_ForbitMove, false)
	Player:LuaSetDoSkillCtrlState( EDoSkillCtrlState.eDSCS_InDoingSkill, false)
	Player.m_SetCtrlState = 5
	Player:SetAndSyncActionState(EActionState.eAS_Idle_BackWpn)
	if Player.UseItemParam[Arg.ItemName] then 
 		if Player.UseItemParam[Arg.ItemName].IsDoAction then
			if Player.UseItemParam[Arg.ItemName].IsDoAction == 2 then
				Player:PlayerDoItemSkill("高爆炸弹吟唱撤销特效",1)
			end
		end
	end
	FailFun(Player, Arg)
	TempItemFailFun[PlayerId] = nil
	TempItemArg[PlayerId] = nil
end

local function DoProgressTick(Tick, data)
	local Conn, SuccFun, FailFun, Arg ,CreatPos,attBreak,Target
	Conn 			= data[1]
	SuccFun		= data[2]
	FailFun		= data[3]
	Arg 			= data[4]
	CreatPos 	= data[5]
	attBreak	= data[6]
	Target 		= data[7]
	local Player = Conn.m_Player
	if not IsCppBound(Player) then
		return
	end
	UnRegisterTick(Player.m_UseItemLoadingTick)
	Player.m_UseItemLoadingTick = nil
	Player.m_IsBattle = nil
	Player:DelListeningWnd()
	Player:CppSetCtrlState(EFighterCtrlState.eFCS_ForbitAutoTrack, false)
	Player:SetInGatherProcess(false)
	Player:CppSetCtrlState(EFighterCtrlState.eFCS_ForbitMove, false)
	Player:LuaSetDoSkillCtrlState( EDoSkillCtrlState.eDSCS_InDoingSkill, false)
	Player:SetAndSyncActionState(EActionState.eAS_Idle_BackWpn)
	Player.m_SetCtrlState = 6
	if Arg and Arg.ItemName then
		if Conn.m_Player.UseItemParam[Arg.ItemName] then 
	 		if Conn.m_Player.UseItemParam[Arg.ItemName].IsDoAction then
	 			if Conn.m_Player.UseItemParam[Arg.ItemName].IsDoAction == 1 then
					Player:SetAndSyncActionState(EActionState.eAS_Item_Cast)
				elseif Conn.m_Player.UseItemParam[Arg.ItemName].IsDoAction == 2 then
					Player:PlayerDoItemSkill("高爆炸弹吟唱撤销特效",1)
				end
			end
		else
			UseItemEnd(Conn,Arg)
			return
		end
	end
	local ConsumeItemTbl = Conn.m_Player.UseItemParam[Arg.ItemName].ConsumeItemTbl
	if ConsumeItemTbl then
		if not LockItem(Conn,Arg) then
			UseItemEnd(Conn,Arg,802)
			return
		end
		local function CallBack(result,msg)
			if IsCppBound(Conn) and IsCppBound(Player) then
				if result then
					SuccFun(Player, Arg, CreatPos,Target)
					TempItemFailFun[Player.m_uID] = nil
					TempItemArg[Player.m_uID] = nil
				else
					UseItemEnd(Conn,Arg,802)
				end
			end
		end
		if not(Player and IsCppBound(Player)) then return end
		if not IsCppBound(Conn) then return end
		local data = {}
		data["PlayerId"] = Player.m_uID
		data["ConsumeTbl"] = ConsumeItemTbl
		CallAccountManualTrans(Conn.m_Account, "UseSpecialItemDB", "CheckItemType", CallBack, data)
	else
		if IsCppBound(Conn) and IsCppBound(Player) then
			SuccFun(Player, Arg, CreatPos,Target)
			TempItemFailFun[Player.m_uID] = nil
			TempItemArg[Player.m_uID] = nil
		end
	end
end

function BeginUseItemProgress(ProTime, Conn, SuccFun, FailFun, Arg,CreatPos,attBreak,Target)
	local Player = Conn.m_Player
	if Player.m_UseItemLoadingTick then
		UnRegisterTick(Player.m_UseItemLoadingTick)
		Player.m_UseItemLoadingTick = nil
		Player.m_IsBattle = nil
		Player:CppSetCtrlState(EFighterCtrlState.eFCS_ForbitAutoTrack, false)
		Player:SetInGatherProcess(false)
		Player:CppSetCtrlState(EFighterCtrlState.eFCS_ForbitMove, false)
		Player:LuaSetDoSkillCtrlState( EDoSkillCtrlState.eDSCS_InDoingSkill, false)
		Player.m_SetCtrlState = 7
		EndUseItemProgress(Player)
	end
	if Player:CppGetDoSkillCtrlState( EDoSkillCtrlState.eDSCS_InDoingSkill) then
		MsgToConn(Conn, 838)
		FailFun(Player, Arg)
		return
	end
	if Player.m_CollIntObjID or Player.m_PickUpNpcID then
		MsgToConn(Conn, 3060)
		FailFun(Player, Arg)
		return
	end
--	if CheckPlayerFightStateStopProgress(Player) then
--		MsgToConn(Conn, 3000, "你现在的状态下不能使用该物品")
--		return
--	end
	if Arg and Arg.ItemName and Player.UseItemParam[Arg.ItemName].IsDoAction then
		if Player.UseItemParam[Arg.ItemName].IsDoAction == 1 then
			Player:SetAndSyncActionState(EActionState.eAS_Item_Sing)
		elseif Player.UseItemParam[Arg.ItemName].IsDoAction == 2 then
			Player:PlayerDoItemSkill("高爆炸弹吟唱特效",1)
			Player:SetAndSyncActionState(EActionState.eAS_Item_Channel)
		end
	end
	Player:CancelAutoTracking()
	Player:CppSetCtrlState(EFighterCtrlState.eFCS_ForbitAutoTrack, true)
	Player:SetInGatherProcess(true)
	Player:CppSetCtrlState(EFighterCtrlState.eFCS_ForbitMove, true)
	Player:LuaSetDoSkillCtrlState( EDoSkillCtrlState.eDSCS_InDoingSkill, true)
	Player:CancelNormalAttack()
	Player:AddListeningWnd()
	Gas2Gac:NotifyUseItemProgressLoad(Conn, ProTime, Arg.ActionName or "")
	local data = {Conn, SuccFun, FailFun, Arg,CreatPos,attBreak,Target}
	TempItemFailFun[Player.m_uID] = FailFun
	TempItemArg[Player.m_uID] = Arg
	Player.m_UseItemLoadingTick = RegisterTick("ItemProgressBar", DoProgressTick, ProTime, data)
end

function EndUseItemProgress(Player)
	local PlayerId = Player.m_uID
	if TempItemFailFun[PlayerId] then
		TempItemFailFun[PlayerId](Player, TempItemArg[PlayerId])
		TempItemFailFun[PlayerId] = nil
		TempItemArg[PlayerId] = nil
	end
end
