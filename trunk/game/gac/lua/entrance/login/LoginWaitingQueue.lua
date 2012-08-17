gac_require "entrance/login/LoginWaitingQueueInc"

function CLoginWaitingQueue:Ctor(Parent)
	self:CreateFromRes("LoginWaitWnd",Parent)
end

function CLoginWaitingQueue:OnChildCreated()
	self.m_WaitMsgWnd			= self:GetDlgChild("WaitMsgWnd")
	self.m_WaitSequenceWnd		= self:GetDlgChild("WaitSequenceWnd")
	self.m_CanceLoginBtn		= self:GetDlgChild("CanceLoginBtn")
	self.m_WaitingProgress		= self:GetDlgChild("WaitingProgress")
	self.m_WaitingProgress:SetRange(100)
	self.m_Pos = 0
	self.m_WaitMsgWnd:SetWndText(GetStaticTextClient(20005))
end

function CLoginWaitingQueue:OnCtrlmsg(Child,uMsgID)
	if (uMsgID == BUTTON_LCLICK ) then
		if ( Child == self.m_CanceLoginBtn ) then
			self:ShowWnd(false)
			g_Login.m_LoginAccounts.m_editPassword:SetFocus()
			Gac2Gas:CancelLoginWaiting(g_Conn)
			UnRegisterTick( self.m_LoadTime )
		end
	end
end

function CLoginWaitingQueue:SetPos( nNum )
	if self.m_WaitingProgress.SetPos then
		self.m_WaitingProgress:SetPos(nNum)
	end
end

function CLoginWaitingQueue:GetPos()
	return self.m_WaitingProgress:GetPos()
end


function CLoginWaitingQueue:BeginWaitingProgress()
	local Loading = function()
		self.m_Pos = self.m_Pos + 1
		self:SetPos( self.m_Pos )
		if(self.m_Pos >= 100) then
			self.m_Pos = 0
		end
	end
	self.m_LoadTime = RegisterTick( "Loading", Loading , 100)  --×¢²áÊ±ÖÓ
end


function Gas2Gac:ReturnLoginWaitingQueue(Conn,SequenceId)
	g_Login.m_WndCancelMsgbox:DestroyWnd()
	g_Login.m_LoginWaitingQueue:ShowWnd(true)
	g_Login.m_LoginWaitingQueue.m_WaitSequenceWnd:SetWndText(GetStaticTextClient(20006,SequenceId))
	g_Login.m_LoginWaitingQueue:SetFocus()
	g_Login.m_LoginWaitingQueue:BeginWaitingProgress()
	CYYCtrl.YYLoginSucceed()
end

function Gas2Gac:ReturnDelWaitingUser(Conn)
	if g_Login then
		if(g_Login.m_nConnStatus == 1) then
			g_Login.m_Socket:ShutDown()
			g_Login.m_nConnStatus = 0
		elseif(g_Login.m_nConnStatus == 2) then
			g_Conn:ShutDown()
			g_Login.m_nConnStatus = 0
	    else
	        if g_Conn and g_Conn.ShutDown then
	           g_Conn:ShutDown()
	        end
		end
	end
    coroutine.resume( g_App.MainRoutine, EGameState.eToLogin )
end

function Gas2Gac:ReturnUpdateWaitSequence(Conn,SequenceId)
	if g_Login.m_LoginWaitingQueue then
		g_Login.m_LoginWaitingQueue.m_WaitSequenceWnd:SetWndText(GetStaticTextClient(20006,SequenceId))
	end
end
