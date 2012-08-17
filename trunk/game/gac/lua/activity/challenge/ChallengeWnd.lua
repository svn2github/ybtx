CShowMessageWnd = class(SQRDialog)

function CShowMessageWnd.CreateShowMessageWnd(Parent,EntityID,TargetName,PlayerLevel,minLevel,maxLevel)
--	local Target = CCharacterFollower_GetCharacterByID(EntityID)
--	if not IsCppBound(Target) then
--		MsgClient(193506)
--		return
--	end
--	local TargetName = Target.m_Properties:GetCharName()
	local str = GetStaticTextClient(12501, TargetName)
	if PlayerLevel and minLevel ~= 0 then
		if g_MainPlayer:CppGetLevel() > maxLevel then
			str = GetStaticTextClient(12504, TargetName,PlayerLevel,minLevel,maxLevel)
		elseif g_MainPlayer:CppGetLevel() < minLevel then
			str = GetStaticTextClient(12505, TargetName,PlayerLevel,minLevel,maxLevel)
		end
	end
	
	local Wnd = CShowMessageWnd:new()
	Wnd:Init(Parent)
	Wnd.m_Text:SetWndText(str)
	Wnd.m_ChallengeID = EntityID
	Wnd:ShowWnd(true)
	return Wnd
end

function CShowMessageWnd:Init(Parent)
	self:CreateFromRes("ShowMessage", Parent)
	self:ShowWnd( true )
end

function CShowMessageWnd:OnChildCreated()
	self.m_AcceptBtn = self:GetDlgChild("Accept")
	self.m_RefuseBtn = self:GetDlgChild("Refuse")
	self.m_ShowBtn = self:GetDlgChild("Show")
	self.m_Text = self:GetDlgChild("Text")
end

function CShowMessageWnd:Destroy()
	self:ShowWnd(false)
	self.m_ChallengeID = nil
	self:DestroyWnd()
	g_GameMain.m_SureMessage = nil
	if g_GameMain.m_JoinChallengeMsgBox then
		g_GameMain.m_JoinChallengeMsgBox:DestroyWnd()
		g_GameMain.m_JoinChallengeMsgBox = nil
	end
	g_MainPlayer:UnLockObj()
end

function CShowMessageWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	if uMsgID == BUTTON_LCLICK then
		if Child == self.m_AcceptBtn then
			Gac2Gas:StartChallenge(g_Conn, true)
			self:Destroy()
		elseif Child == self.m_RefuseBtn then
			Gac2Gas:StartChallenge(g_Conn, false)
			self:Destroy()
		elseif Child == self.m_ShowBtn then
			local Target = CCharacterFollower_GetCharacterByID(self.m_ChallengeID)
			if not IsCppBound(Target) then
				MsgClient(193507)
				return
			end
			local Pos = CPos:new()
			Target:GetGridPos(Pos)
			if g_GetDistanceByPos(g_MainPlayer, Pos) <= 5 then
				g_MainPlayer:LockObj(Target)
				g_GameMain.m_TargetInfo:OnSee()
			else
				MsgClient(193507)
			end
		end
	end
end

function CShowMessageWnd.CancelChallengeWnd(Conn, bValue, flag)
	if g_GameMain.m_SureMessage then
		g_GameMain.m_SureMessage:Destroy()
	end
	if g_GameMain.m_WaitAcceptMsg and flag == 2 then
		g_GameMain.m_WaitAcceptMsg:Destroy()
	end
	if g_GameMain.m_JoinChallengeMsgBox then
		g_GameMain.m_JoinChallengeMsgBox:DestroyWnd()
		g_GameMain.m_JoinChallengeMsgBox = nil
	end
	if bValue == false then
		g_GameMain.m_ChallengeCompass:CancelCompassState()
	end
end

function CShowMessageWnd.ShowChallengeMessage(Conn,EntityID,name,Level,MinLevel,MaxLevel)
	if g_GameMain.m_JoinChallengeMsgBox then
		g_GameMain.m_JoinChallengeMsgBox:DestroyWnd()
		g_GameMain.m_JoinChallengeMsgBox = nil
	end
	if g_GameMain.m_JoinChallengeTick then
		UnRegisterTick(g_GameMain.m_JoinChallengeTick)
		g_GameMain.m_JoinChallengeTick = nil
	end
	local function IsJoinChallenge(g_GameMain,uButton)
		if uButton == MB_BtnOK then
			Gac2Gas:SendChallengeMessage(g_Conn, EntityID)--弹出另一个窗口
			g_GameMain.m_WaitAcceptMsg = CWaitAcceptChallengeWnd.CreateWaitAcceptChallWnd(g_GameMain,name)
		end
		if g_GameMain.m_JoinChallengeTick then
			UnRegisterTick(g_GameMain.m_JoinChallengeTick)
			g_GameMain.m_JoinChallengeTick = nil
		end
		g_GameMain.m_JoinChallengeMsgBox = nil
		return true
	end
	if g_GameMain.m_JoinChallengeMsgBox then
		return
	end
	
	local str = MsgBoxMsg(17020,name)
	if Level and MinLevel ~= 0 and MaxLevel ~= 0 then
		str = MsgBoxMsg(17021,name,Level,MinLevel,MaxLevel)
	end
	g_GameMain.m_JoinChallengeMsgBox = MessageBox( g_GameMain, str, BitOr( MB_BtnOK, MB_BtnCancel),IsJoinChallenge,g_GameMain)

	local function tickfun(Tick)
		local Obj = CIntObjClient_GetIntObjByID(EntityID)
		if not IsCppBound(Obj) then
			UnRegisterTick(Tick)
			if g_GameMain.m_JoinChallengeMsgBox then
				g_GameMain.m_JoinChallengeTick = nil
				g_GameMain.m_JoinChallengeMsgBox:DestroyWnd()
			end
		end
	end
	g_GameMain.m_JoinChallengeTick = RegisterTick("ChallengeTick", tickfun, 1000)
end