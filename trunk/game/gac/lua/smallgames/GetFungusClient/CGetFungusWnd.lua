gac_require "smallgames/GetFungusClient/CGetFungusWndInc"

local Using = false
local Tick = nil

local SpeedTick = nil
local SpeedNum = 1

local RightTimes = 0
local ClickPos = nil
local ClickTimes = nil
local NpcGlobalID = nil

function CreateGetFungusWnd(parent)
	local Wnd = CGetFungusWnd:new()
	Wnd:CreateFromRes("SmallGameNoneTimeWnd", parent)
	Wnd.m_ProgressTime = Wnd:GetDlgChild("Progress")
	Wnd.m_GamePic = Wnd:GetDlgChild("GamePic")
	Wnd.m_StaticPic = Wnd:GetDlgChild("StaticPic")
	
	Wnd.m_Scale1 = SQRDialog:new()
	Wnd.m_Scale1:CreateFromRes("SmallGame_Scale", Wnd.m_ProgressTime)
	Wnd.m_Scale1:ShowWnd( true )
	Wnd.m_Scale2 = SQRDialog:new()
	Wnd.m_Scale2:CreateFromRes("SmallGame_Scale", Wnd.m_ProgressTime)
	Wnd.m_Scale2:ShowWnd( true )
	Wnd.m_Scale3 = SQRDialog:new()
	Wnd.m_Scale3:CreateFromRes("SmallGame_Scale", Wnd.m_ProgressTime)
	Wnd.m_Scale3:ShowWnd( true )
	
	Wnd.m_Text = Wnd:GetDlgChild("Text")
	Wnd.m_Text:SetWndText(GetStaticTextClient(4900))
	
	--Wnd.m_Scale1 = Wnd:GetDlgChild("Scale1")
	--Wnd.m_Scale2 = Wnd:GetDlgChild("Scale2")
	--Wnd.m_Scale3 = Wnd:GetDlgChild("Scale3")
	Wnd.m_ProgressTime:SetProgressMode(3)
	Wnd.m_ProgressTime:SetRange(1000)
	Wnd:SetStyle(0x60000000)--设置成子窗口
	Wnd.m_Scale1:SetStyle(0x60000000)--设置成子窗口
	Wnd.m_Scale2:SetStyle(0x60000000)--设置成子窗口
	Wnd.m_Scale3:SetStyle(0x60000000)--设置成子窗口
	Wnd.m_ProgressTime:SetStyle(0x60000000)--设置成子窗口
	
	----------------------
	local Rect=CFRect:new()
	Wnd.m_Scale1:GetLogicRect(Rect)
	Wnd.m_ScaleRect1 = Rect
	Wnd.m_Scale2:GetLogicRect(Rect)
	Wnd.m_ScaleRect2 = Rect
	Wnd.m_Scale3:GetLogicRect(Rect)
	Wnd.m_ScaleRect3 = Rect
	-----------------------

	return Wnd
end

function CGetFungusWnd:RunSmallGame(uMsgID)
	if uMsgID == 1 then
		if Using and ClickTimes > 0 then
			ClickTimes = ClickTimes - 1--减少一次单击的机会
			
			local num = self.m_ProgressTime:GetPos()
			if (num >= (self.ScalePos1*10-self.ScaleRange1) and num <= (self.ScalePos1*10+self.ScaleRange1) and ClickPos ~= 1) then
				ClickPos = 1
				RightTimes = RightTimes + 1
				MsgClient(3251,RightTimes)
			elseif (num >= (self.ScalePos2*10-self.ScaleRange2) and num <= (self.ScalePos2*10+self.ScaleRange2) and ClickPos ~= 2) then
				ClickPos = 2
				RightTimes = RightTimes + 1
				MsgClient(3251,RightTimes)
			elseif (num >= (self.ScalePos3*10-self.ScaleRange3) and num <= (self.ScalePos3*10+self.ScaleRange3) and ClickPos ~= 3) then
				ClickPos = 3
				RightTimes = RightTimes + 1
				MsgClient(3251,RightTimes)
			end
	
		end
	end
end

local function SetWndPos(PartenWnd,ChildWnd,ChildRect,Pos,ChildRange)
	local Rect = CFRect:new()
	Rect.left 	= ChildRect.left
	Rect.top  	= ChildRect.top
	Rect.right	= ChildRect.right
	Rect.bottom	= ChildRect.bottom
	
	local uHeight = Rect.bottom - Rect.top
	local ParentRect = CFRect:new()
	PartenWnd:GetWndRect(ParentRect)
	Rect.left 	= ParentRect.left
	Rect.top  	= (ParentRect.bottom - (ParentRect.bottom - ParentRect.top)*(Pos/100)) - ((ParentRect.bottom - ParentRect.top)*(ChildRange/1000))
	Rect.right	= ParentRect.right
	Rect.bottom	= Rect.top + 2*((ParentRect.bottom - ParentRect.top)*(ChildRange/1000))
	ChildWnd:SetWndRect(Rect)
	ChildWnd:SetFocus()
	return 1--math.floor(uHeight/((ParentRect.bottom - ParentRect.top)/100)/2 + 0.5)
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
function CGetFungusWnd:BeginSmallGame(GameName,ObjID)
	--进入小游戏
	if not Using then
		Using = true
		
		local GameCommon = SmallGame_Common(GameName)
		if not GameCommon then
			Using = false
			return 
		end
		
		local strTexture = GameCommon("GameWndInfoTex")
		if strTexture and strTexture ~= "" then
			AddImage(self.m_GamePic , strTexture)
		end
		
		local strTexture = GameCommon("WndTex")
		if strTexture and strTexture ~= "" then
			AddImage(self.m_StaticPic , strTexture)
		end
		
		self.m_SuccRenderTbl =  g_SmallGameStrTbl[GameName].SuccEfxFile
		local ScriptArgTabl = g_SmallGameStrTbl[GameName].ScriptArg
		local GameTime = GameCommon("GameTime")
		GameTime = GameTime*1000
		--GameTime = math.floor((GameTime + 500) / 1000) * 1000
		
		self.ScalePos1 = ScriptArgTabl["三刻度位置"][1]
		self.ScaleRange1 = ScriptArgTabl["容错值"]--SetWndPos(self,self.m_Scale1,self.ScalePos1)
		self.ScalePos2 = ScriptArgTabl["三刻度位置"][2]
		self.ScaleRange2 = ScriptArgTabl["容错值"]--SetWndPos(self,self.m_Scale2,self.ScalePos2)
		self.ScalePos3 = ScriptArgTabl["三刻度位置"][3]
		self.ScaleRange3 = ScriptArgTabl["容错值"]--SetWndPos(self,self.m_Scale3,self.ScalePos3)
		ClickTimes = ScriptArgTabl["可单击的次数"]
		self.m_SuccTimes = ScriptArgTabl["完成次数"]
		self.m_TimeSpeedTbl = ScriptArgTabl["时间调节速度"]
		ClickPos = nil
		SpeedNum = 1
		
		self.m_ProgressTime:SetPos(0)
		g_GameMain.m_SmallGame = GameName
		
		SetWndPos(self.m_ProgressTime,self.m_Scale1,self.m_ScaleRect1,self.ScalePos1,self.ScaleRange1)
		SetWndPos(self.m_ProgressTime,self.m_Scale2,self.m_ScaleRect2,self.ScalePos2,self.ScaleRange2)
		SetWndPos(self.m_ProgressTime,self.m_Scale3,self.m_ScaleRect3,self.ScalePos3,self.ScaleRange3)
		
		local num = 0
		function ProgressRise()
			num = num + SpeedNum
			local PlayerState = g_MainPlayer:CppGetCtrlState(EFighterCtrlState.eFCS_InBattle) 
			if num > 1000 or PlayerState then
				if RightTimes == self.m_SuccTimes then
					self:SuccSmallGame()
				else
					self:FailSmallGame()
				end
				return
			end
			self.m_ProgressTime:SetPos( num )
		end
		self:ShowWnd(true)
		Tick = RegisterTick("ProgressRise", ProgressRise, GameTime/1000)
		
		self:SpeedTick(100)
		
		NpcGlobalID = ObjID
	end
end

function CGetFungusWnd:SpeedTick(time)
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
	SpeedTick = RegisterTick( "TimeDownProgress", SetSpeed , time)  --注册时钟
end

function CGetFungusWnd:SuccSmallGame()
	if Tick then
		UnRegisterTick(Tick)
		Tick = nil
	end
	if SpeedTick then
		UnRegisterTick(SpeedTick)
		SpeedTick = nil
	end
	if g_GameMain.m_SmallGame then
		Gac2Gas:SmallGameSucceed(g_Conn, g_GameMain.m_SmallGame,NpcGlobalID)
		g_GameMain.m_SGMgr:SmallGameAddRender(self.m_SuccRenderTbl)
		g_GameMain.m_SGMgr:CloseSmallGameEffectWnd()
		self.m_SuccRenderTbl = nil
		g_GameMain.m_SmallGame = nil
	end
	self:ShowWnd(false)
	RightTimes = 0
	NpcGlobalID = nil
	ClickTimes = nil
	Using = false
end

function CGetFungusWnd:FailSmallGame()
	if Tick then
		UnRegisterTick(Tick)
		Tick = nil
	end
	if SpeedTick then
		UnRegisterTick(SpeedTick)
		SpeedTick = nil
	end
	if g_GameMain.m_SmallGame then
		Gac2Gas:SmallGameCancel(g_Conn, g_GameMain.m_SmallGame,NpcGlobalID)
		g_GameMain.m_SGMgr:CloseSmallGameEffectWnd()
		MsgClient(3250)
		g_GameMain.m_SmallGame = nil
	end
	self:ShowWnd(false)
	RightTimes = 0
	NpcGlobalID = nil
	ClickTimes = nil
	Using = false
end

function CGetFungusWnd:CancelSmallGame(IsMove)
	if Tick then
		UnRegisterTick(Tick)
		Tick = nil
	end
	if SpeedTick then
		UnRegisterTick(SpeedTick)
		SpeedTick = nil
	end
	if g_GameMain.m_SmallGame then
		if IsMove then
			Gac2Gas:ExitSmallGame(g_Conn,g_GameMain.m_SmallGame,NpcGlobalID)
		else
			Gac2Gas:SmallGameCancel(g_Conn,g_GameMain.m_SmallGame,NpcGlobalID)
		end
		g_GameMain.m_SGMgr:CloseSmallGameEffectWnd()
		g_GameMain.m_SmallGame = nil
	end
	self:ShowWnd(false)
	RightTimes = 0
	ClickTimes = nil
	NpcGlobalID = nil
	Using = false
end
