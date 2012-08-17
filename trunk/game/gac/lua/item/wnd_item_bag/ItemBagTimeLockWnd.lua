
CItemBagTimeLockWnd	= class( SQRDialog )

function CItemBagTimeLockWnd:Ctor()
	self:CreateFromRes("ItemBagTimeLockWnd", g_GameMain)
	g_ExcludeWndMgr:InitExcludeWnd(self, 1)
end

function CItemBagTimeLockWnd:OnChildCreated()
	self.m_CloseBtn				= self:GetDlgChild("CloseBtn")
	self.m_MsgShowStatic 	= self:GetDlgChild("MsgShowStatic")
	self.m_DayTime 				= self:GetDlgChild("Time1")
	self.m_HourTime 			= self:GetDlgChild("Time2")
	self.m_MinuteTime			= self:GetDlgChild("Time3")
	self.m_MsgShowStatic:SetWndText(GetStaticTextClient(350001))
	self.m_DayTime:EnableWnd(false) 		
	self.m_HourTime:EnableWnd(false) 	
	self.m_MinuteTime:EnableWnd(false)
end

function CItemBagTimeLockWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if( uMsgID == BUTTON_LCLICK ) then
		if (Child == self.m_CloseBtn) then
			self:ShowWnd(false)
		end
	end
end

function CItemBagTimeLockWnd:ItemBagTimeLockShow()
	local left_time = g_GameMain.m_ItemBagTimeLockWnd.m_LockTimeLeft
	if left_time == 0 then
		Gac2Gas:ItemBagLockTimeOut(g_Conn)
		if g_GameMain.m_ItemBagTimeLockWnd.m_ShowTimeTick then
			UnRegisterTick(g_GameMain.m_ItemBagTimeLockWnd.m_ShowTimeTick)
			g_GameMain.m_ItemBagTimeLockWnd.m_ShowTimeTick = nil
		end
		return
	end
	
	local dayTime = math.floor(left_time/1440) 
	local hourTime = math.floor((left_time - dayTime*1440)/60) 
	local minuteTime = left_time - dayTime*1440 - hourTime*60
	
	self.m_DayTime:SetWndText(dayTime)
	self.m_HourTime:SetWndText(hourTime)
	self.m_MinuteTime:SetWndText(minuteTime)
end

local function ShowTimeTickFun()
	g_GameMain.m_ItemBagTimeLockWnd.m_LockTimeLeft = g_GameMain.m_ItemBagTimeLockWnd.m_LockTimeLeft - 1
	g_GameMain.m_ItemBagTimeLockWnd:ItemBagTimeLockShow()
end

local function LockItemBagByTimeCommon(lock_time)
	g_MainPlayer.m_ItemBagLock = true
	g_GameMain.m_ItemBagTimeLockWnd.m_LockTimeLeft = lock_time
	g_GameMain.m_ItemBagTimeLockWnd:ItemBagTimeLockShow()
	if not g_GameMain.m_ItemBagTimeLockWnd.m_ShowTimeTick then
		g_GameMain.m_ItemBagTimeLockWnd.m_ShowTimeTick = RegisterTick( "ItemBagTimeLockShow", ShowTimeTickFun, 60000)
	end
end

function CItemBagTimeLockWnd.ReturnLockItemBagByTime(Conn,lock_time)
	LockItemBagByTimeCommon(lock_time)
	g_GameMain.m_ItemBagTimeLockWnd:ShowWnd(true)
end

function CItemBagTimeLockWnd.ReturnInitLockItemBagByTime(Conn,lock_time)
	lock_time = math.floor(lock_time/60)
	LockItemBagByTimeCommon(lock_time)
end

