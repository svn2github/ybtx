CWarDeadMsgWnd = class(SQRDialog)

function CWarDeadMsgWnd:Ctor()
	self:CreateFromRes("WarDeadMsgWnd", g_GameMain)
	self:Init()
end

function CWarDeadMsgWnd:Init()
	self.m_WndInfo		= self:GetDlgChild("Label1")
	self.m_BtnRePoint	= self:GetDlgChild("Btn1")
	self.m_BtnStation	= self:GetDlgChild("Btn2")	
	self.m_BtnDeadPos = self:GetDlgChild("Btn3")
end

function CWarDeadMsgWnd:SetBtnRePointText(Num)
	self.m_BtnRePoint:SetWndText(GetStaticTextClient(11007, Num))
end

function CWarDeadMsgWnd:SetBtnStationText(Num)
	self.m_BtnStation:SetWndText(GetStaticTextClient(11009, Num))
end

function CWarDeadMsgWnd:ShowDeadWnd(time)
	if(self:IsToLogin(time)) then
		return
	end
	if self.m_CountDownTick ~= nil then
		return
	end
	self.m_CountDownNum = time--初始化时间
	
	local sStr = nil
	local nItemCount = 0
	
	if g_MainPlayer:CppGetLevel() < 15 then
		self.m_BtnDeadPos:EnableWnd(false)
		self.m_NoviceTime = 5
		sStr = GetStaticTextClient(11011, self.m_NoviceTime)
	elseif g_MainPlayer:CppGetLevel() >= 15 then
		nItemCount = g_GameMain.m_BagSpaceMgr:GetItemCountBySpace(g_StoreRoomIndex.PlayerBag,1, "复活石" )
		sStr = GetStaticTextClient(11002,nItemCount)
		if nItemCount <= 0  then
			self.m_BtnDeadPos:EnableWnd(false)
		else
			self.m_BtnDeadPos:EnableWnd(true)
		end
	end

	self.m_BtnDeadPos:SetWndText(sStr)
	self:SetBtnRePointText(self.m_CountDownNum)
	self:SetBtnStationText(self.m_CountDownNum)
	self.m_BtnRePoint:EnableWnd(false)
	self.m_BtnStation:EnableWnd(false)
	g_GameMain.m_CreateChatWnd.m_ChatSendArea:SetParent(self)
	self:ShowWnd(true)
	self.m_CountDownTick = RegClassTick( "WarDeadTick", self.CountLeftTimeTick, 1000, self)	
end

function CWarDeadMsgWnd:IsToLogin(RebornCountTime)
	if ( g_GameMain.m_ToLoginMsgBox and g_GameMain.m_ToLoginMsgBox.m_CountDownTick ) then
		local CallBackToCharDeadWnd = function()
			self:ShowDeadWnd(RebornCountTime)
		end
		g_GameMain.m_ToLoginMsgBox.CancelAndRunCallBack = CallBackToCharDeadWnd
		return true
	end
	return false
end

function CWarDeadMsgWnd:CountLeftTimeTick()
	if( self:IsToLogin(self.m_CountDownNum) ) then
		return
	end
	local leftTime = self.m_CountDownNum - 1
	self.m_CountDownNum = leftTime
	
	if leftTime > 0 then	
		local sStr = nil
		local nItemCount = 0
		
		if not IsCppBound(g_MainPlayer) then
			UnRegisterTick(self.m_CountDownTick)
			self.m_CountDownTick = nil
			return
		end

		if g_MainPlayer:CppGetLevel() < 15 then
			self.m_NoviceTime = self.m_NoviceTime - 1
			if self.m_NoviceTime > 0 then
				sStr = GetStaticTextClient(11011, self.m_NoviceTime)
			else
				self.m_BtnDeadPos:EnableWnd(true)
				sStr = GetStaticTextClient(11001)
			end
		elseif g_MainPlayer:CppGetLevel() >= 15 then
			nItemCount = g_GameMain.m_BagSpaceMgr:GetItemCountBySpace(g_StoreRoomIndex.PlayerBag,1, "复活石" )
			sStr = GetStaticTextClient(11002, nItemCount)
			if nItemCount <= 0  then
				self.m_BtnDeadPos:EnableWnd(false)
			else
				self.m_BtnDeadPos:EnableWnd(true)
			end
		end
		
		self.m_BtnDeadPos:SetWndText(sStr)
		self.m_BtnRePoint:EnableWnd(false)
		self.m_BtnStation:EnableWnd(false)
		self:SetBtnRePointText(leftTime)
		self:SetBtnStationText(leftTime)
	else
		UnRegisterTick(self.m_CountDownTick)
		self.m_CountDownTick = nil
		self.m_BtnRePoint:SetWndText(GetStaticTextClient(11008))
		self.m_BtnStation:SetWndText(GetStaticTextClient(11010))
		self.m_BtnRePoint:EnableWnd(true)
		local TongID = g_MainPlayer.m_Properties:GetTongID()
		if not TongID or TongID == 0 then
			self.m_BtnStation:EnableWnd(false)
		else
			self.m_BtnStation:EnableWnd(true)
		end
	end
end

function CWarDeadMsgWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if( uMsgID == BUTTON_LCLICK ) then
		if self.m_BtnDeadPos == Child then
			self:HideWnd()
			Gac2Gas:GoBackOrStay(g_Conn, false)
		elseif self.m_BtnStation == Child then
			self:HideWnd()
			Gac2Gas:StationReborn(g_Conn)
		elseif self.m_BtnRePoint == Child then
			self:HideWnd()
			Gac2Gas:GoBackOrStay(g_Conn, true)
		end
	end
end

function CWarDeadMsgWnd:HideWnd()
	if self.m_CountDownTick then
		UnRegisterTick(self.m_CountDownTick)
		self.m_CountDownTick = nil
	end
	g_GameMain.m_CreateChatWnd.m_ChatSendArea:SetParent(g_GameMain)
	self:ShowWnd(false)
end
