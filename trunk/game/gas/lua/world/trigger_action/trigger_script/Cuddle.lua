local function Script(Arg, Trigger, Player)
	if not IsCppBound(Player) then
		return
	end
	if g_GetDistance(Player,Trigger)>6 then --判断距离
		return
	end
	if not Player:CppIsLive() then
		return
	end
	if Player:CppGetDoSkillCtrlState( EDoSkillCtrlState.eDSCS_InDoingSkill) or Player:CppGetCtrlState(EFighterCtrlState.eFCS_OnMission) then
		--MsgToConn(Conn,3290)--"不能进行手部动作")
		return
	end
	if Player:CppGetCtrlState(EFighterCtrlState.eFCS_InNormalHorse) or Player:CppGetCtrlState(EFighterCtrlState.eFCS_InBattleHorse) then--骑乘
		MsgToConn(Player.m_Conn,3291)--"骑马状态，不可以拾取或者抱起")
		return
	end
	
	--Player:IsFirstTimeAndSendMsg("抱", 2001, sMsg)
	Player:CancelNormalAttack()
	CActiveBehavior.TargetDoBehavior(Player, Trigger:GetEntityID())
end 

return Script