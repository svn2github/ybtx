gac_require "smallgames/GetHorseMedicine/CGetHorseMedicineWndInc"

local Using = false
local Tick = nil
local Step = 4
local NpcGlobalID = nil

function CreateGetMedicineWnd(parent)
	local Wnd = CGetHorseMedicineWnd:new()
	Wnd:CreateFromRes("SmallGameNoneTimeWnd", parent)
	Wnd.m_ProgressTime = Wnd:GetDlgChild("Progress")
	Wnd.m_GamePic = Wnd:GetDlgChild("GamePic")
	Wnd.m_StaticPic = Wnd:GetDlgChild("StaticPic")
	
	Wnd.m_Scale = SQRDialog:new()
	Wnd.m_Scale:CreateFromRes("SmallGame_Scale", Wnd.m_ProgressTime)
	Wnd.m_Scale:ShowWnd( true )
	--Wnd.m_Scale = Wnd:GetDlgChild("Scale")
	Wnd.m_ProgressTime:SetProgressMode(3)
	Wnd.m_ProgressTime:SetRange(1000)
	Wnd:SetStyle(0x60000000)--设置成子窗口
	Wnd.m_Scale:SetStyle(0x60000000)--设置成子窗口
	Wnd.m_ProgressTime:SetStyle(0x60000000)--设置成子窗口
	
	Wnd.m_Text = Wnd:GetDlgChild("Text")
	Wnd.m_Text:SetWndText(GetStaticTextClient(4900))
	
	----------------------
	local Rect=CFRect:new()
	Wnd.m_Scale:GetLogicRect(Rect)
	Wnd.m_ScaleRect = Rect
	----------------------
	return Wnd
end

function CGetHorseMedicineWnd:RunSmallGame(uMsgID)
	if Using and uMsgID == 1 then
		--Step = 1
		Using = false
		local times = 1
		if Tick then
			UnRegisterTick(Tick)
			Tick = nil
		end
		local num = self.m_ProgressTime:GetPos()
		
		function ProgressRise()
			--num = num + Step
			if num >= 1000 then
				self:FailSmallGame()
				return
			end
			--self.m_ProgressTime:SetPos( num )
			
			times = times + 1
			--if times >= 7 then
			--	Step = 0;
			--end
			if times >= 10 then
				if num >= (self.ScalePos*10-self.ScaleRange) and num <= (self.ScalePos*10+self.ScaleRange) then
					self:SuccSmallGame()
				else
					self:FailSmallGame()--失败
				end
			end
		end
		Tick = RegisterTick("ProgressRise", ProgressRise, 200)
	end
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
function CGetHorseMedicineWnd:BeginSmallGame(GameName,ObjID)
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
			AddImage(self.m_GamePic, strTexture)
		end
		
		local strTexture = GameCommon("WndTex")
		if strTexture and strTexture ~= "" then
			AddImage(self.m_StaticPic, strTexture)
		end
		
		self.m_SuccRenderTbl = g_SmallGameStrTbl[GameName].SuccEfxFile--GameCommon("SuccEfxFile")
		local ScriptArgTabl = g_SmallGameStrTbl[GameName].ScriptArg
		local GameTime = GameCommon("GameTime")
		GameTime = GameTime*1000
		self.ScalePos = ScriptArgTabl["刻度位置"]
		self.ScaleRange = ScriptArgTabl["容错值"]
		
		--------------------------------------
		local Rect = CFRect:new()
		Rect.left 	= self.m_ScaleRect.left
		Rect.top  	= self.m_ScaleRect.top
		Rect.right	= self.m_ScaleRect.right
		Rect.bottom	= self.m_ScaleRect.bottom

		local uHeight = Rect.bottom - Rect.top
		local ParentRect=CFRect:new()
		self.m_ProgressTime:GetLogicRect(ParentRect)
		Rect.left 	= ParentRect.left
		Rect.top  	= (ParentRect.bottom - (ParentRect.bottom - ParentRect.top)*(self.ScalePos/100)) - ((ParentRect.bottom - ParentRect.top)*(self.ScaleRange/1000))
		Rect.right	= ParentRect.right
		Rect.bottom	= Rect.top + 2*((ParentRect.bottom - ParentRect.top)*(self.ScaleRange/1000))
		
		self.m_Scale:SetLogicRect(Rect)
		self.m_Scale:SetFocus()
		----------------------------------------
		
		self.m_ProgressTime:SetPos(0)
		g_GameMain.m_SmallGame = GameName

		local num = 0
	
		function ProgressRise()
			num = num + Step			
			if num >= 1000 then
				self:FailSmallGame()
				return
			end
			self.m_ProgressTime:SetPos( num )
		end
		self:ShowWnd(true)
		Tick = RegisterTick("ProgressRise", ProgressRise, GameTime/500)

		NpcGlobalID = ObjID
	end
end

function CGetHorseMedicineWnd:SuccSmallGame()
	if Tick then
		UnRegisterTick(Tick)
		Tick = nil
	end
	if g_GameMain.m_SmallGame then
		Gac2Gas:SmallGameSucceed(g_Conn, g_GameMain.m_SmallGame,NpcGlobalID)
		g_GameMain.m_SGMgr:SmallGameAddRender(self.m_SuccRenderTbl)
		g_GameMain.m_SGMgr:CloseSmallGameEffectWnd()
		self.m_SuccRenderTbl = nil
		g_GameMain.m_SmallGame = nil
	end
	self:ShowWnd(false)
	NpcGlobalID = nil
	Using = false
end

function CGetHorseMedicineWnd:FailSmallGame()
	if Tick then
		UnRegisterTick(Tick)
		Tick = nil
	end
	if g_GameMain.m_SmallGame then
		Gac2Gas:SmallGameCancel(g_Conn,g_GameMain.m_SmallGame,NpcGlobalID)
		g_GameMain.m_SGMgr:CloseSmallGameEffectWnd()
		MsgClient(3250)
		g_GameMain.m_SmallGame = nil
	end
	self:ShowWnd(false)
	NpcGlobalID = nil
	Using = false
end

function CGetHorseMedicineWnd:CancelSmallGame(IsMove)
	if Tick then
		UnRegisterTick(Tick)
		Tick = nil
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
	NpcGlobalID = nil
	Using = false
end
