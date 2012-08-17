gac_require "smallgames/GetStoneClient/CGetStoneWndInc"

local Using = false
local TimeTick = nil
local UpTick = nil
local ScoreTick = nil
local SpeedTick = nil

local TimeNum = nil
local CountScore = 0
local SpeedNum = 1
local NpcGlobalID = nil

function CreateGetStoneWnd(parent)
	local Wnd = CGetStoneWnd:new()
	Wnd:CreateFromRes("SmallGameTimeWnd",parent)
	Wnd.m_DownTime = Wnd:GetDlgChild( "Time" )
	Wnd.m_ProgressUp = Wnd:GetDlgChild( "UpProgress" )
	Wnd.m_GamePic = Wnd:GetDlgChild("GamePic")
	Wnd.m_StaticPic = Wnd:GetDlgChild("StaticPic")
	
	Wnd.m_Scale1 = SQRDialog:new()
	Wnd.m_Scale1:CreateFromRes("SmallGame_Scale", Wnd.m_ProgressUp)
	Wnd.m_Scale1:ShowWnd( true )
	Wnd.m_Scale2 = SQRDialog:new()
	Wnd.m_Scale2:CreateFromRes("SmallGame_Scale", Wnd.m_ProgressUp)
	Wnd.m_Scale2:ShowWnd( true )
	
	Wnd.m_Text = Wnd:GetDlgChild("Text")
	Wnd.m_Text:SetWndText(GetStaticTextClient(4901))
	
	--Wnd.m_Scale1 = Wnd:GetDlgChild( "Scale1" )
	--Wnd.m_Scale2 = Wnd:GetDlgChild( "Scale2" )
	Wnd.m_ProgressUp:SetProgressMode(3)
	Wnd.m_ProgressUp:SetRange(1000)
	Wnd:SetStyle(0x60000000)--设置成子窗口
	Wnd.m_Scale1:SetStyle(0x60000000)--设置成子窗口
	Wnd.m_Scale2:SetStyle(0x60000000)--设置成子窗口
	Wnd.m_DownTime:SetStyle(0x65000000)--设置成子窗口
	Wnd.m_ProgressUp:SetStyle(0x60000000)--设置成子窗口
	-------------------------
	local Rect=CFRect:new()
	Wnd.m_Scale1:GetLogicRect(Rect)
	Wnd.m_ScaleRect1 = Rect
	Wnd.m_Scale2:GetLogicRect(Rect)
	Wnd.m_ScaleRect2 = Rect
	---------------------------
	return Wnd
end

local function ClearAllTick()
	if TimeTick then
		UnRegisterTick(TimeTick)
		TimeTick = nil
	end
	if UpTick then
		UnRegisterTick(UpTick)
		UpTick = nil
	end
	if SpeedTick then
		UnRegisterTick(SpeedTick)
		SpeedTick = nil
	end
	if ScoreTick then
		UnRegisterTick(ScoreTick)
		ScoreTick = nil
	end
end

local function SetWndPos(PartenWnd,ChildWnd,ChildRect,Pos)
	local Rect = CFRect:new()
	Rect.left 	= ChildRect.left
	Rect.top  	= ChildRect.top
	Rect.right	= ChildRect.right
	Rect.bottom	= ChildRect.bottom
	
	local uHeight = Rect.bottom - Rect.top
	local ParentRect = CFRect:new()
	PartenWnd:GetWndRect(ParentRect)
	Rect.left 	= ParentRect.left
	Rect.top  	= ParentRect.bottom - (ParentRect.bottom - ParentRect.top)*(Pos/100) - uHeight/2
	Rect.right	= ParentRect.right
	Rect.bottom	= Rect.top + uHeight
	
	ChildWnd:SetWndRect(Rect)
	ChildWnd:SetFocus()
	--return math.floor(uHeight/((ParentRect.bottom - ParentRect.top)/100)/2 + 0.5)
end

local function AddImage(Wnd, strTexture)
	local Flag = IMAGE_PARAM:new()
	Flag.CtrlMask = SM_BS_BK
	Flag.StateMask = IP_ENABLE	
	local DefaultImage = WND_IMAGE_LIST:new()
	DefaultImage:AddImage(Wnd:GetGraphic(), -1,  strTexture, nil, CFPos:new(), "0xffffffff", 15 ) 	
	Wnd:SetWndBkImage( Flag, DefaultImage )
end

-- 客户端开始小游戏
function CGetStoneWnd:BeginSmallGame(GameName,ObjID)
	--进入小游戏
	if not Using then
		Using = true
		if TimeTick then
			self:ShowWnd(false)
			UnRegisterTick(TimeTick)
			TimeTick = nil
		end
		
		local GameCommon = SmallGame_Common(GameName)
		if not GameCommon then
			Using = false
			return 
		end
		
		local strTexture = GameCommon("GameWndInfoTex")
		if strTexture and strTexture ~= "" then
			AddImage(self.m_GamePic, strTexture)
		end
		
		local strTexture = GameCommon("WndTex")
		if strTexture and strTexture ~= "" then
			AddImage(self.m_StaticPic, strTexture)
		end
		
		self.m_SuccRenderTbl = g_SmallGameStrTbl[GameName].SuccEfxFile--GameCommon("SuccEfxFile")
		local ScriptArgTabl = g_SmallGameStrTbl[GameName].ScriptArg
		
		local GameTime = GameCommon("GameTime")
		TimeNum = GameTime
		GameTime = GameTime*1000
		--GameTime = math.floor((GameTime + 500) / 1000) * 1000
		
		self.m_UpNum = ScriptArgTabl["单击一次提高的百分比"]*10
		self.m_KeepTime = ScriptArgTabl["保持时间"]
		self.m_TimeSpeedTbl = ScriptArgTabl["时间调节速度"]
		SpeedNum = 1
		
		local ScaleTbl = ScriptArgTabl["区间位置"]
		self.ScalePos1 = ScaleTbl[1]
		self.ScalePos2 = ScaleTbl[2]
		if self.ScalePos1 > self.ScalePos2 then
			local num = self.ScalePos1
			self.ScalePos1 = self.ScalePos2
			self.ScalePos2 = num
		end
		SetWndPos(self.m_ProgressUp,self.m_Scale1,self.m_ScaleRect1,self.ScalePos1)
		SetWndPos(self.m_ProgressUp,self.m_Scale2,self.m_ScaleRect2,self.ScalePos2)

		self.m_DownTime:SetWndText(TimeNum)
		self.m_ProgressUp:SetPos(1000)
		
		g_GameMain.m_SmallGame = GameName

		self:TimeDownProgress(50)
		self:ClickUpProgress(10)
		self:ScoreProgress(500)--半秒查一次
		self:SpeedTick(100)
		self:ShowWnd(true)

		NpcGlobalID = ObjID
	end
end

function CGetStoneWnd:SpeedTick(time)
	local Time = 0
	local SetSpeed = function()
		if g_MainPlayer then
			Time = Time + 1
			
			if next(self.m_TimeSpeedTbl) then
				if Time >= (self.m_TimeSpeedTbl[1][1]*10) then
					SpeedNum = self.m_TimeSpeedTbl[1][2]
					table.remove(self.m_TimeSpeedTbl,1)
				end
			else
				if SpeedTick then
					UnRegisterTick(SpeedTick)
					SpeedTick = nil
				end
				self.m_TimeSpeedTbl = nil
			end
			
		else
			ClearAllTick()
		end
	end
	SpeedTick = RegisterTick( "SpeedTick", SetSpeed , time)  --注册时钟
end

function CGetStoneWnd:DevTime(num)
	TimeNum = TimeNum - num
	self.m_DownTime:SetWndText(TimeNum)
end

function CGetStoneWnd:GetTimeNum()
	return TimeNum
end

function CGetStoneWnd:TimeDownProgress(time)
	local times = 1
	local Loading = function()
		if g_MainPlayer then
			local PlayerState = g_MainPlayer:CppGetCtrlState(EFighterCtrlState.eFCS_InBattle) 
			if PlayerState then
				if CountScore >= (self.m_KeepTime*2*10) then
					self:SuccSmallGame()
					return
				else
					self:FailSmallGame()
					return
				end
			end
			
			if times>= 20 then--1秒后执行
				self:DevTime(1)
				if self:GetTimeNum() <= 0 then
					if CountScore >= (self.m_KeepTime*2*10) then
						self:SuccSmallGame()
						return
					else
						self:FailSmallGame()
						return
					end
				end
				times = 1
			else
				times = times + 1
			end
		else
			ClearAllTick()
		end
	end
	TimeTick = RegisterTick( "TimeDownProgress", Loading , time)  --注册时钟
end

function CGetStoneWnd:ClickUpProgress(time)
	local UpLoading = function()
		local num = self.m_ProgressUp:GetPos()
		num = num - SpeedNum
		if num <= 0 then
			num = 0
		end
		self.m_ProgressUp:SetPos(num)
	end
	UpTick = RegisterTick( "ClickUpProgress", UpLoading , time)  --注册时钟
end

function CGetStoneWnd:ScoreProgress(time)
	local ScoreFun = function()
		local num = self.m_ProgressUp:GetPos()
		if num >= (self.ScalePos1)*10 and num <= (self.ScalePos2)*10 then
			CountScore = CountScore + 10
			
			if CountScore >= (self.m_KeepTime*2*10) then
				self:SuccSmallGame()
				return
			end
			
		end
	end
	ScoreTick = RegisterTick( "ScoreProgress", ScoreFun , time)  --注册时钟
end

function CGetStoneWnd:RunSmallGame(uMsgID)
	if Using and uMsgID == 1 then
		local num = self.m_ProgressUp:GetPos()
		self.m_ProgressUp:SetPos(num + self.m_UpNum)
	end
end

function CGetStoneWnd:SuccSmallGame()
	ClearAllTick()
	if g_GameMain.m_SmallGame then
		Gac2Gas:SmallGameSucceed(g_Conn,g_GameMain.m_SmallGame,NpcGlobalID)
		g_GameMain.m_SGMgr:SmallGameAddRender(self.m_SuccRenderTbl)
		g_GameMain.m_SGMgr:CloseSmallGameEffectWnd()
		self.m_SuccRenderTbl = nil
	end
	NpcGlobalID = nil
	CountScore = 0
	local function Close()
		if TimeTick then
			UnRegisterTick(TimeTick)
			TimeTick = nil
		end
		if g_MainPlayer then
			g_MainPlayer.m_IsWaitShowWnd = nil
			self:ShowWnd(false)
		end
	end
	g_MainPlayer.m_IsWaitShowWnd = true
	TimeTick = RegisterTick( "CloseWnd", Close , 2000)  --注册时钟
	g_GameMain.m_SmallGame = nil
	Using = false
end

function CGetStoneWnd:FailSmallGame()
	ClearAllTick()
	if g_GameMain.m_SmallGame then
		Gac2Gas:SmallGameCancel(g_Conn,g_GameMain.m_SmallGame,NpcGlobalID)
		g_GameMain.m_SGMgr:CloseSmallGameEffectWnd()
		MsgClient(3250)
		g_GameMain.m_SmallGame = nil
	end
	NpcGlobalID = nil
	CountScore = 0
	self:ShowWnd(false)
	Using = false
end

function CGetStoneWnd:CancelSmallGame(IsMove)
	ClearAllTick()
	if g_GameMain.m_SmallGame then
		if IsMove then
			Gac2Gas:ExitSmallGame(g_Conn,g_GameMain.m_SmallGame,NpcGlobalID)
		else
			Gac2Gas:SmallGameCancel(g_Conn,g_GameMain.m_SmallGame,NpcGlobalID)
		end
		g_GameMain.m_SGMgr:CloseSmallGameEffectWnd()
		g_GameMain.m_SmallGame = nil
	end
	NpcGlobalID = nil
	CountScore = 0
	self:ShowWnd(false)
	Using = false
end
