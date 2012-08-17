CRobResDeadMsgWnd = class(SQRDialog)

function CRobResDeadMsgWnd:Ctor()
	self:CreateFromRes("WndDeadMsg", g_GameMain)
	self:Init()
end

function CRobResDeadMsgWnd:Init()
	self.m_WndInfo			 		= self:GetDlgChild("MsgInfo")
	self.m_BtnOk						= self:GetDlgChild("BtnOK")
	self.m_BtnCancel 				= self:GetDlgChild("BtnCancel")	
	self.m_NoviceTime				= 0
	self.m_CountDownNum			= 0
	self.m_RebornCountTime	=	0
end

function CRobResDeadMsgWnd:SetOkBtnText(Num)  --Num 为时间
	self.m_BtnOk:SetWndText(GetStaticTextClient(11015, Num))
end

function CRobResDeadMsgWnd:SetNoviceTime()
	self.m_NoviceTime = 5
end

function CRobResDeadMsgWnd:GetNoviceTime()
	local time = self.m_NoviceTime - (os.time() - g_GameMain.m_DeadTime)
	if time < 0 then
		return 0
	end
	return time
end

function CRobResDeadMsgWnd:SetOriginRebornCountTime(time)
	self.m_RebornCountTime = time
end

function CRobResDeadMsgWnd:GetOriginRebornCountTime()
	if self.m_RebornCountTime then
		local time = self.m_RebornCountTime - (os.time() - g_GameMain.m_DeadTime)
		if time < 0 then
			return 0
		end
		return time
	end
end

function CRobResDeadMsgWnd:SetBackRebornCountTime(time)
	self.m_CountDownNum = time
end



function CRobResDeadMsgWnd:GetBackRebornCountTime()
	local time = self.m_CountDownNum - (math.floor(GetProcessTime()/1000) - g_GameMain.m_DeadTime)
	if time < 0 then
		return 0
	end
	return time
end

function CRobResDeadMsgWnd:ShowDeadWnd(time,RebornCountTime)
	if(self:IsToLogin(time)) then
		return
	end
	local LogTime = self:GetBackRebornCountTime()
	self:SetBackRebornCountTime(time) --初始化时间
	
	local sStr = nil
	local nItemCount = 0
	

	self:SetOriginRebornCountTime(RebornCountTime)
	nItemCount = g_GameMain.m_BagSpaceMgr:GetItemCountBySpace(g_StoreRoomIndex.PlayerBag,1, "复活石" )

	if nItemCount <= 0 then
		sStr = GetStaticTextClient(11002, nItemCount)
		self.m_BtnCancel:EnableWnd(false)
	else
		sStr = GetStaticTextClient(11002, nItemCount)
		self.m_BtnCancel:EnableWnd(true)
	end

	self:SetOkBtnText(self:GetBackRebornCountTime())
	self.m_BtnCancel:SetWndText(sStr)
	self.m_BtnOk:EnableWnd(false)
	g_GameMain.m_CreateChatWnd.m_ChatSendArea:SetParent(self)
	self:ShowWnd(true)
	
	if self.m_CountDownTick ~= nil then
		LogErr("客户端复活tick没有注销!", "tick剩余时间："..LogTime)
		return
	end
	self.m_CountDownTick = RegClassTick( "RobResDeadTick", self.CountLeftTimeTick, 1000, self)	
end

function CRobResDeadMsgWnd:CountLeftTimeTick()
	if( self:IsToLogin(self.m_CountDownNum) ) then
		return
	end
	local leftTime = self:GetBackRebornCountTime()
	
	local sStr = nil
	local nItemCount = g_GameMain.m_BagSpaceMgr:GetItemCountBySpace(g_StoreRoomIndex.PlayerBag,1, "复活石" )
	
	if not IsCppBound(g_MainPlayer) then
		UnRegisterTick(self.m_CountDownTick)
		self.m_CountDownTick = nil
		return
	end


	if nItemCount <= 0 then
		self.m_BtnCancel:EnableWnd(false)
	else
		sStr = GetStaticTextClient(11002, nItemCount)
		self.m_BtnCancel:EnableWnd(true)
	end
		
	self.m_BtnCancel:SetWndText(sStr)
		
	if leftTime > 0 then	
		self.m_BtnOk:EnableWnd(false)
		self:SetOkBtnText(leftTime)
	else
		self.m_BtnOk:SetWndText(GetStaticTextClient(11016))
		self.m_BtnOk:EnableWnd(true)
	end
	if leftTime <= 0 and self:GetOriginRebornCountTime() <= 0 then
		UnRegisterTick(self.m_CountDownTick)
		self.m_CountDownTick = nil
	end
end

function CRobResDeadMsgWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if( uMsgID == BUTTON_LCLICK ) then
		if self.m_BtnCancel == Child then
			Gac2Gas:GoRobResRebormPlaceStay(g_Conn, false)
			if self then
				g_GameMain.m_CreateChatWnd.m_ChatSendArea:SetParent(g_GameMain)
				self:ShowWnd(false)
			end
		elseif self.m_BtnOk == Child then
		--	g_MainPlayer.m_RebornTimeLog = {self:GetBackRebornCountTime(),g_GameMain.m_DeadTimeLog,os.time()}
			Gac2Gas:GoRobResRebormPlaceStay(g_Conn, true)
			if self then
				g_GameMain.m_CreateChatWnd.m_ChatSendArea:SetParent(g_GameMain)
				self:ShowWnd(false)
			end
		end
	end
end

function CRobResDeadMsgWnd:HideWnd()
	if self.m_CountDownTick then
		UnRegisterTick(self.m_CountDownTick)
		self.m_CountDownTick = nil
	end
	g_GameMain.m_CreateChatWnd.m_ChatSendArea:SetParent(g_GameMain)
	self:ShowWnd(false)
end

function CRobResDeadMsgWnd:IsToLogin(RebornCountTime)
	if ( g_GameMain.m_ToLoginMsgBox and g_GameMain.m_ToLoginMsgBox.m_CountDownTick ) then
		local CallBackToCharDeadWnd = function()
			self:ShowDeadWnd(RebornCountTime)
		end
		g_GameMain.m_ToLoginMsgBox.CancelAndRunCallBack = CallBackToCharDeadWnd
		return true
	end
	return false
end

