CXLProgressWnd = class(SQRDialog)

local XLTick = nil
local PointAt = 1--1表示增长,-1表示下降
local LockIntObjID = nil

function CXLProgressWnd:Ctor()
	self:CreateFromRes("StorageForce_loading",g_GameMain)
	self.m_Progress = self:GetDlgChild( "Progress" )
	self:SetStyle(0x60000000)--设置主窗口不可移动
	self.m_Progress:SetStyle(0x60000000)--设置成子窗口
	self.m_Progress:SetProgressMode(3)
	self.m_Progress:SetRange(1000)
	self.m_Pos = 0
	
	---------------------
	self.m_Scale = {}
	for i=1,3 do
		self.m_Scale[i] = SQRDialog:new()
		self.m_Scale[i]:CreateFromRes("SmallGame_Scale", self.m_Progress)
		self.m_Scale[i]:ShowWnd( true )
		self.m_Scale[i]:SetStyle(0x60000000)--设置成子窗口
	end
	---------------------
end

local function SetWndPos(PartenWnd,ChildWnd,Pos)
	local Rect = CFRect:new()
	ChildWnd:GetWndRect(Rect)
	Rect.left 	= Rect.left
	Rect.top  	= Rect.top
	Rect.right	= Rect.right
	Rect.bottom	= Rect.bottom
	
	local uHeight = Rect.bottom - Rect.top
	local ParentRect = CFRect:new()
	PartenWnd:GetWndRect(ParentRect)
	Rect.left 	= ParentRect.left
	Rect.top  	= ParentRect.bottom - (ParentRect.bottom - ParentRect.top)*((25*Pos)/100) - 1
	Rect.right	= ParentRect.right
	Rect.bottom	= Rect.top + 2
	ChildWnd:SetWndRect(Rect)
end

function CXLProgressWnd:SetPos( nNum )
	if self.m_Progress.SetPos then
		self.m_Progress:SetPos(nNum)
	end
end

function CXLProgressWnd:GetPos()
	return self.m_Progress:GetPos()
end

function CXLProgressWnd:BeginXLToProgress()
	if XLTick then
		UnRegisterTick( XLTick )
	end
	--发送一个蓄力开始
	Gac2Gas:BeginKickBehavior(g_Conn)
	
	g_GameMain.m_XLProgressWnd.m_IsShowWnd = nil
	g_GameMain.m_XLProgressWnd.m_IsButtonDown = true
	self:SetPos(0)
	self.m_Pos=0
	PointAt = 1
	local Loading = function()
		self.m_Pos = self.m_Pos + PointAt*2--所乘的数是为了提高速度
		if self.m_Pos > 1000 then
			self.m_Pos = 1000
			PointAt = -1
			-------到头直接发送----------
			self:SetPos( 1 )
			self:SendXLNum()
			------------------
		elseif self.m_Pos < 0 then
			self.m_Pos = 0
			PointAt = 1
		else
			self:SetPos( self.m_Pos )
		end
	end
	local time = 1000--蓄力时间
	XLTick = RegisterTick( "BeginXL_Loading", Loading , time/1000)  --注册时钟
end

local function HideXLProgress(Wnd, IsDelay)
	if XLTick then
		UnRegisterTick( XLTick )
		XLTick = nil
	end
	LockIntObjID = nil
	Wnd.m_IsShowWnd = nil
	Wnd.m_IsButtonDown = nil
	
	local function callback()
		Wnd:ShowWnd(false)
		Wnd:SetPos( 0 )
	end
	if IsDelay then
		RegisterOnceTick(g_GameMain, "HideXLProgress", callback, 500)
	else
		callback()
	end
end

function CXLProgressWnd:StopXLFromProgress()
	--发送一个失败
	HideXLProgress(self)
	Gac2Gas:CancelKickBehavior(g_Conn)
end

function CXLProgressWnd:SendXLNum()
	if XLTick then
		UnRegisterTick( XLTick )
		XLTick = nil
	end
	local CurrPos = self:GetPos()
	--发送一个成功
	Gac2Gas:EndKickBehavior(g_Conn,LockIntObjID,CurrPos)
	HideXLProgress(self, true)
end

function CXLProgressWnd.OpenXLProgressWnd(Conn,IntObjID)
	g_GameMain.m_XLProgressWnd:SetPos(0)
	g_GameMain.m_XLProgressWnd.m_Pos=0
	g_GameMain.m_XLProgressWnd.m_IsShowWnd = true
	LockIntObjID = IntObjID
	
	------------------
	for i=1,3 do
		SetWndPos(g_GameMain.m_XLProgressWnd.m_Progress,g_GameMain.m_XLProgressWnd.m_Scale[i],i)
	end
	------------------
	
	g_GameMain.m_XLProgressWnd:ShowWnd(true)
end

function CXLProgressWnd.CloseXLProgressWnd(Conn)
	if g_GameMain.m_XLProgressWnd.m_IsShowWnd then
		g_GameMain.m_XLProgressWnd:ShowWnd(false)
		LockIntObjID = nil
		g_GameMain.m_XLProgressWnd.m_IsShowWnd = nil
		g_GameMain.m_XLProgressWnd.m_IsButtonDown = nil
	elseif g_GameMain.m_XLProgressWnd.m_IsButtonDown then
		HideXLProgress(g_GameMain.m_XLProgressWnd)
	end
end
