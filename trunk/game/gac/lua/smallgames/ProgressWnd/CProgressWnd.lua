gac_require "smallgames/ProgressWnd/CProgressWndInc"

local TimeNum = nil
local TestCoef = 0.06--上升系数

function CreateProgressWnd(parent)
	local Wnd = CProgressWnd:new()
	Wnd:CreateFromRes("SmallGameTimeWnd",parent)
	Wnd.m_DownTime = Wnd:GetDlgChild( "Time" )
	Wnd.m_ProgressPer = Wnd:GetDlgChild( "UpProgress" )
	Wnd.m_GamePic = Wnd:GetDlgChild("GamePic")
	Wnd.m_StaticPic = Wnd:GetDlgChild("StaticPic")
	
	Wnd.m_ProgressPer:SetProgressMode(3)
	Wnd.m_ProgressPer:SetRange(100)
	
	Wnd.m_Text = Wnd:GetDlgChild("Text")
	
	Wnd:SetStyle(0x60000000)--设置成子窗口
	Wnd.m_ProgressPer:SetStyle(0x60400000)--设置成子窗口
	Wnd.m_DownTime:SetStyle(0x65000000)--设置成子窗口
	return Wnd
end

function CProgressWnd:HeadTick(time)
	if self.m_HeadTick then
		UnRegisterTick(self.m_HeadTick)
		self.m_HeadTick = nil
	end
	
	local Pos = 0
	local function HeadFun()
		local StrengeNum = g_App:GetTGInst():GetThinkgearData(TG_DATA_ATTENTION)
		if StrengeNum == -2 then
			UnRegisterTick(self.m_HeadTick)
			self.m_HeadTick = nil
			return
		end
		if StrengeNum > 20 and StrengeNum <= 100 then
			Pos = Pos + StrengeNum * TestCoef
			if Pos >= 100 then
				self:StopProgress()
				self.m_DoFun(true)
			end
			self.m_ProgressPer:SetPos(Pos)
		end
	end
	self.m_HeadTick = RegisterTick("HeadTick", HeadFun, time)
end

function CProgressWnd:LoadProgress(time) 
	if self.m_LoadTime then
		UnRegisterTick( self.m_LoadTime )
	end
	local times = 1
	local Loading = function()
		
		local PlayerState = g_MainPlayer:CppGetCtrlState(EFighterCtrlState.eFCS_InBattle) 
		if PlayerState then
			self:StopProgress()
			self.m_DoFun(false)
			return
		end
		
		if times>= 20 then--1秒后执行
			self:DevTime(1)
			if self:GetTimeNum() <= 0 then
				self:StopProgress()
				self.m_DoFun(false)
				return
			end
			times = 1
		else
			times = times + 1
		end
		
	end
	self.m_LoadTime = RegisterTick( "Loading", Loading , time)  --注册时钟
end

function CProgressWnd:GetTimeNum()
	return TimeNum
end

function CProgressWnd:DevTime(num)
	TimeNum = TimeNum - num
	self.m_DownTime:SetWndText(TimeNum)
end

function CProgressWnd:GetPerPos()
	return  self.m_ProgressPer:GetPos()
end

function CProgressWnd:AddPerPos(add)
	local Pos = self:GetPerPos() + add
	if Pos >= 100 then
		self:StopProgress()
		self.m_DoFun(true)
	end
	self.m_ProgressPer:SetPos(Pos)
end

local function AddImage(Wnd, strTexture)
	local Flag = IMAGE_PARAM:new()
	Flag.CtrlMask = SM_BS_BK
	Flag.StateMask = IP_ENABLE	
	local DefaultImage = WND_IMAGE_LIST:new()
	DefaultImage:AddImage(Wnd:GetGraphic(), -1,  strTexture, nil, CFPos:new(), "0xffffffff", 15 ) 	
	Wnd:SetWndBkImage( Flag, DefaultImage )
end

function CProgressWnd:BeginTimeProgress(time, GameCommon, showInfo, fun)
	TimeNum = time/1000
	self.m_DownTime:SetWndText(TimeNum)
	self.m_Text:SetWndText(showInfo)
	self.m_ProgressPer:SetPos(0)
	self.m_DoFun = fun
	
	local strTexture = GameCommon("GameWndInfoTex")
	if strTexture and strTexture ~= "" then
		AddImage(self.m_GamePic ,strTexture)
	end
	local strTexture = GameCommon("WndTex")
	if strTexture and strTexture ~= "" then
		AddImage(self.m_StaticPic ,strTexture)
	end
	
	self:ShowWnd(true)
	self:LoadProgress(50)
	self:HeadTick(1000)
end



function CProgressWnd:StopProgress()
	self:ShowWnd(false)
	if self.m_LoadTime then
		UnRegisterTick( self.m_LoadTime )
		self.m_LoadTime = nil
	end
	if self.m_HeadTick then
		UnRegisterTick(self.m_HeadTick)
		self.m_HeadTick = nil
	end
end