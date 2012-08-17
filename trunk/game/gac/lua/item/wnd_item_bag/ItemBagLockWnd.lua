
CItemBagLockWnd	= class( SQRDialog )

function CItemBagLockWnd:Ctor()
	self:CreateFromRes("ItemBagLockWnd", g_GameMain)
	g_ExcludeWndMgr:InitExcludeWnd(self, 1)
end

function CItemBagLockWnd:OnChildCreated()
	self.m_CloseBtn				= self:GetDlgChild("CloseBtn")
	self.m_MsgShowStatic 	= self:GetDlgChild("MsgShowStatic")
	self.m_CheckBtn1 			= self:GetDlgChild("CheckBtn1")
	self.m_CheckBtn2 			= self:GetDlgChild("CheckBtn2")
	self.m_DayTime 				= self:GetDlgChild("Time1")
	self.m_HourTime 			= self:GetDlgChild("Time2")
	self.m_MinuteTime			= self:GetDlgChild("Time3")
	self.m_AffirmBtn1 		= self:GetDlgChild("AffirmBtn1")
	self.m_AffirmBtn2 		= self:GetDlgChild("AffirmBtn2")
	self.m_PasswordEdit1 	= self:GetDlgChild("PasswordEdit1")
	self.m_PasswordEdit2 	= self:GetDlgChild("PasswordEdit2")
	self.m_UsePassword		= self:GetDlgChild("UsePassword")
	self.m_Input1					= self:GetDlgChild("Input1")
	self.m_Input2					= self:GetDlgChild("Input2")
	self.m_CheckBtn1:SetCheck(false)
	self.m_CheckBtn2:SetCheck(false)
	self.m_AffirmBtn1:EnableWnd(false)
	self.m_AffirmBtn2:EnableWnd(false)
	self.m_DayTime:SetMaxTextLenght(1)
	self.m_HourTime:SetMaxTextLenght(2)
	self.m_MinuteTime:SetMaxTextLenght(2)
	self.m_DayTime:EnableWnd(false) 		
	self.m_HourTime:EnableWnd(false) 	
	self.m_MinuteTime:EnableWnd(false)
	
	self.m_CheckBtn2:ShowWnd(false)
	self.m_PasswordEdit1:ShowWnd(false)
	self.m_PasswordEdit2:ShowWnd(false)
	self.m_AffirmBtn2:ShowWnd(false)
	self.m_UsePassword:ShowWnd(false)
	self.m_Input1:ShowWnd(false)			
	self.m_Input2:ShowWnd(false)
	self.m_MsgShowStatic:SetWndText(GetStaticTextClient(350001))		
end

function CItemBagLockWnd:SetCheckBtn1()
	self.m_CheckBtn1:SetCheck(true)
	self.m_CheckBtn2:SetCheck(false)
	self.m_AffirmBtn1:EnableWnd(true)
	self.m_AffirmBtn2:EnableWnd(false)
	self.m_DayTime:EnableWnd(true)
	self.m_HourTime:EnableWnd(true)
	self.m_MinuteTime:EnableWnd(true)
	self.m_PasswordEdit1:EnableWnd(false)
	self.m_PasswordEdit2:EnableWnd(false)
	self.m_PasswordEdit1:SetWndText("")
	self.m_PasswordEdit2:SetWndText("")
end

function CItemBagLockWnd:SetCheckBtn2()
	self.m_CheckBtn1:SetCheck(false)
	self.m_CheckBtn2:SetCheck(true)
	self.m_AffirmBtn1:EnableWnd(false)
	self.m_AffirmBtn2:EnableWnd(true)
	self.m_PasswordEdit1:EnableWnd(false)
	self.m_PasswordEdit2:EnableWnd(true)
	self.m_DayTime:EnableWnd(false)
	self.m_HourTime:EnableWnd(false)
	self.m_MinuteTime:EnableWnd(false)
	self.m_PasswordEdit1:EnableWnd(true)
	self.m_PasswordEdit2:EnableWnd(true)
	self.m_DayTime:SetWndText("")
	self.m_HourTime:SetWndText("")
	self.m_MinuteTime:SetWndText("")
end

function CItemBagLockWnd:TimeLock()
	local day,hour,minute = 0,0,0
	
	if self.m_DayTime:GetWndText() ~= "" then
		day = tonumber(self.m_DayTime:GetWndText()) * 24 * 60
	end
	
	if self.m_HourTime:GetWndText() ~= "" then
		hour = tonumber(self.m_HourTime:GetWndText()) * 60
	end
	if self.m_MinuteTime:GetWndText() ~= "" then
		minute = tonumber(self.m_MinuteTime:GetWndText())
	end
	
	local lock_time = day + hour + minute
	local msgId = 0
	if lock_time == 0 then
		msgId = 13014
	elseif lock_time < 1 then --最短一分钟
		msgId = 13015
	elseif lock_time > 10080 then  --最长7天
		msgId = 13016
	else
		msgId = 13017
	end

	local function CallBack(g_GameMain,uButton)
		if uButton == MB_BtnOK then
			if msgId == 13017 then
				Gac2Gas:LockItemBagByTime(g_Conn,lock_time)
			else
				self.m_DayTime:SetFocus()
			end
		end
		g_GameMain.m_ItemBagLockMsg = nil
		return true
	end
		
	if g_GameMain.m_ItemBagLockMsg then
		return
	end
	g_GameMain.m_ItemBagLockMsg = MessageBox(g_GameMain, MsgBoxMsg(msgId),  BitOr(MB_BtnOK,MB_BtnCancel), CallBack, g_GameMain)
end

function CItemBagLockWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if( uMsgID == BUTTON_LCLICK ) then
		if (Child == self.m_CloseBtn) then
			self:ShowWnd(false)
		elseif (Child == self.m_CheckBtn1) then
			self:SetCheckBtn1()
		elseif (Child == self.m_CheckBtn2) then
			self:SetCheckBtn2()
		elseif (Child == self.m_AffirmBtn1) then
			self:TimeLock()
		elseif (Child == self.m_AffirmBtn2) then
			
		end
	end
end

function CItemBagLockWnd.RetItemBagLockTimeOut(Conn)
	g_MainPlayer.m_ItemBagLock = false
	if g_GameMain.m_ItemBagTimeLockWnd:IsShow() then
		g_GameMain.m_ItemBagTimeLockWnd:ShowWnd(false)
	end
	MsgClient(160021)
end


