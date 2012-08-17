CWndDeadMsg = class(SQRDialog)

function CWndDeadMsg:Ctor()
	self:CreateFromRes("WndDeadMsg", g_GameMain)
	self:Init()
end

function CWndDeadMsg:Init()
	self.m_WndInfo			 	= self:GetDlgChild("MsgInfo")
	self.m_BtnOk					= self:GetDlgChild("BtnOK")
	self.m_BtnCancel 			= self:GetDlgChild("BtnCancel")	
--	self.m_WndInfo:SetWndText(GetStaticTextClient(11003))
	self.m_CountDownNum 	= 30
end

function CWndDeadMsg:IsToLogin(RebornCountTime)
	if ( g_GameMain.m_ToLoginMsgBox and g_GameMain.m_ToLoginMsgBox.m_CountDownTick ) then
		local CallBackToCharDeadWnd = function()
			self:ToCharDeadWnd(RebornCountTime)
		end
		g_GameMain.m_ToLoginMsgBox.CancelAndRunCallBack = CallBackToCharDeadWnd
		return true
	end
	return false
end

function CWndDeadMsg:ToCharDeadWnd(RebornCountTime)
	--1秒后打开死亡窗口
	if(self:IsToLogin(RebornCountTime)) then
		return
	end
	
	self.m_RebornCountTime = RebornCountTime
	self.m_BeforeCountDownTick = RegClassTick( "BeforeCharDeadTick", self.CharDeadTick, 1000, self, RebornCountTime)
end

function CWndDeadMsg:CharDeadTick(tick, RebornCountTime)
	UnRegisterTick(tick)
	self.m_BeforeCountDownTick = nil
	
	if( self:IsToLogin(RebornCountTime) ) then
		return
	end
	
	if g_GameMain.m_ToDeadMsgBox == nil then
		return
	end
	if self.m_CountDownTick ~= nil then
		return
	end
	--UnRegisterTick(self.m_BeforeCountDownTick)
	--self.m_BeforeCountDownTick = nil
	
	--1秒后打开死亡窗口
	self.m_BtnOk:SetWndText(GetStaticTextClient(11006, 30))
	local nItemCount = g_GameMain.m_BagSpaceMgr:GetItemCountBySpace(g_StoreRoomIndex.PlayerBag,1, "复活石" )
	local sStr = GetStaticTextClient(11001)
	
	if not IsCppBound(g_MainPlayer) then
		return
	end
	
	if g_MainPlayer:CppGetLevel() < 15 then
		self.m_BtnCancel:EnableWnd(true)
		--self.m_NoviceTime = 5
		--sStr = GetStaticTextClient(11011, self.m_NoviceTime)
	elseif g_MainPlayer:CppGetLevel() >= 15 then
		if self.m_RebornCountTime ~= 0 then
			sStr = GetStaticTextClient(11002, self.m_RebornCountTime)
		else
			sStr = GetStaticTextClient(11002, nItemCount)
		end
		if nItemCount <= 0 or self.m_RebornCountTime ~= 0 then
			self.m_BtnCancel:EnableWnd(false)
		else
			self.m_BtnCancel:EnableWnd(true)
		end
	end

--	if nItemCount <= 0 then
--		self.m_BtnCancel:EnableWnd(false)
--	else
--		self.m_BtnCancel:EnableWnd(true)
--	end
	
	self.m_BtnCancel:SetWndText(sStr)
	
	g_GameMain.m_CreateChatWnd.m_ChatSendArea:SetParent(self)
	self:ShowWnd(true)
	self.m_CountDownNum 	= 30	--重新设置时间
	self.m_CountDownTick = RegClassTick( "CharDeadTick", self.CountLeftTimeTick, 1000, self)
end

function CWndDeadMsg:ReShow()
	g_GameMain.m_CreateChatWnd.m_ChatSendArea:SetParent(self)
	self:ShowWnd(true)
	self.m_CountDownTick = RegClassTick( "CharDeadTick", self.CountLeftTimeTick, 1000, self)
end

function CWndDeadMsg:HideWnd()
	if self.m_CountDownTick then
		UnRegisterTick(self.m_CountDownTick)
		self.m_CountDownTick = nil
	end
	g_GameMain.m_CreateChatWnd.m_ChatSendArea:SetParent(g_GameMain)
	self:ShowWnd(false)
end

function CWndDeadMsg:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if( uMsgID == BUTTON_LCLICK ) then
		if self.m_BtnCancel == Child then
			--self:HideWnd()
			Gac2Gas:GoBackOrStay(g_Conn, false)
		elseif self.m_BtnOk == Child then
			self:HideWnd()
			Gac2Gas:GoBackOrStay(g_Conn, true)
		end
	end
end

function CWndDeadMsg:CountLeftTimeTick()
	local leftTime = self.m_CountDownNum - 1
	self.m_CountDownNum = leftTime
	local wndInfo = GetStaticTextClient(11006, leftTime)
	local nItemCount = g_GameMain.m_BagSpaceMgr:GetItemCountBySpace(g_StoreRoomIndex.PlayerBag,1, "复活石" )
	local sStr = GetStaticTextClient(11001)
	
	if not IsCppBound(g_MainPlayer) then
		UnRegisterTick(self.m_CountDownTick)
		self.m_CountDownTick = nil
		return
	end
	
	if g_MainPlayer:CppGetLevel() < 15 then
--		self.m_NoviceTime = self.m_NoviceTime - 1
--		if self.m_NoviceTime > 0 then
--			sStr = GetStaticTextClient(11011, self.m_NoviceTime)
--		else
--			self.m_BtnCancel:EnableWnd(true)
--			sStr = GetStaticTextClient(11001)
--		end
	elseif g_MainPlayer:CppGetLevel() >= 15 then
		if self.m_RebornCountTime ~= 0 then
			self.m_RebornCountTime = self.m_RebornCountTime - 1
		end
		if self.m_RebornCountTime ~= 0 then
			sStr = GetStaticTextClient(11002, self.m_RebornCountTime)
		else
			sStr = GetStaticTextClient(11002, nItemCount)
		end
		if nItemCount <= 0 or self.m_RebornCountTime ~= 0 then
			self.m_BtnCancel:EnableWnd(false)
		else
			self.m_BtnCancel:EnableWnd(true)
		end
	end
	
--	if nItemCount <= 0 then
--		self.m_BtnCancel:EnableWnd(false)
--	else
--		self.m_BtnCancel:EnableWnd(true)
--	end	
	
	self.m_BtnCancel:SetWndText(sStr)
	if leftTime > 0 then
		self.m_BtnOk:SetWndText(wndInfo)
	else
		UnRegisterTick(self.m_CountDownTick)
		self.m_CountDownTick = nil
		g_GameMain.m_CreateChatWnd.m_ChatSendArea:SetParent(g_GameMain)
		self:ShowWnd(false)
		if g_GameMain.m_PermitRebornMsgBox then
			g_GameMain.m_PermitRebornMsgBox:DestroyWnd()
			g_GameMain.m_PermitRebornMsgBox = nil
		end
		Gac2Gas:GoBackOrStay(g_Conn, true)
	end
end

function CWndDeadMsg.CloseRebornMsgWnd(Conn)
	g_GameMain.m_AreaFbDeadMsgBox:HideWnd()
	g_GameMain.m_RobResDeadMsgBox:HideWnd()
	g_GameMain.m_ToDeadMsgBox:HideWnd()
	g_GameMain.m_FBDeadMsgBox:HideWnd()
	g_GameMain.m_TeamPVPFBDeadMsgBox:HideWnd()
	g_GameMain.m_WarDeadMsgBox:HideWnd()
	local wnd = COldWarDeadWnd:GetWnd()
	wnd:HideWnd()
end
