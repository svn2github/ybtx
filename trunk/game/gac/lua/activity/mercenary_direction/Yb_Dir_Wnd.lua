local CYBDirWnd = class(SQRDialog)
local CYBDirTempWnd = class(SQRDialog)
local CYBDirTempBtnWnd = class(SQRDialog)
CYBDirTempFxWnd = class(SQRDialog)

local TickTime = 500 --毫秒

--窗口中的按扭
local function CreateBtn(parent)
	local Wnd = CYBDirTempBtnWnd:new()
	Wnd:CreateFromRes("yb_rz_btn_wnd",parent) 
	Wnd.m_OKBtn = Wnd:GetDlgChild("Btn")
	
	Wnd:ShowWnd(true)
	return Wnd
end

--主要的显示文字的窗口
local function CreateMainYbDirWnd(parent)
	local Wnd = CYBDirWnd:new()
	Wnd:CreateFromRes("yb_rz_wnd",parent) 
	Wnd.m_Btn = Wnd:GetDlgChild("Ok_Btn")
	Wnd.m_StopBtn = Wnd:GetDlgChild("Stop_Btn")
	Wnd.m_Btn.m_BtnWnd = CreateBtn(Wnd.m_Btn)
	
	Wnd.m_GuiZeText = {}
	for i=1,11 do
		local strWnd = "guize"..i
		Wnd.m_GuiZeText[i] = Wnd:GetDlgChild(strWnd)
	end
	
	Wnd.m_DialogText = {}
	for i=1,9 do
		local strWnd = "dialog"..i
		Wnd.m_DialogText[i] = Wnd:GetDlgChild(strWnd)
	end
	
	--	Wnd.m_GuiZeHeightTbl = {89,70,90,130}
	--	Wnd.m_DialogWidthTbl = {200,230,200,230,300,230,150,230}
	return Wnd
end

--一个窗口的载体,为了处理让窗口闪的效果
function CYBDirTempFxWnd:Ctor()
	self:CreateFromRes("yb_rz_fx_wnd",g_GameMain)
	
	self.m_YbDirTempWnd = CYBDirTempWnd:new()
	self.m_YbDirTempWnd:CreateFromRes("yb_rz_tempwnd",self)
	self.m_YbDirTempWnd:ShowWnd(true)
	
	self.m_MainYbDirWnd = CreateMainYbDirWnd(self.m_YbDirTempWnd)
end

local ClassName = {
	[EClass.eCL_Warrior] = "signet_dajianshi",--大剑
	[EClass.eCL_NatureElf] = "signet_fashi",--法师
	[EClass.eCL_MagicWarrior] = "signet_mojianshi",--魔剑
	[EClass.eCL_Priest] = "signet_mushi",--牧师
	[EClass.eCL_OrcWarrior] = "signet_shourenzhanshi",--兽人
	[EClass.eCL_EvilElf] = "signet_xiemo",--邪魔
}

local function SetButtonFx(Wnd, IsCenter, CenterX,CenterY)
	local FildPath = g_ImageMgr:GetImagePath(10004)
	
	local PlayerClass = g_MainPlayer:CppGetClass()
	FildPath = FildPath..ClassName[PlayerClass]
	
	local Flag = IMAGE_PARAM:new()
	Flag.CtrlMask = SM_BS_BK
	
	--改变按扭的4个状态贴图
	--1.
	Flag.StateMask = IP_ENABLE
	local DefaultImage = Wnd:GetWndBkImage(Flag)
	if not DefaultImage then
		DefaultImage = WND_IMAGE_LIST:new()
	end
	DefaultImage:AddImage( Wnd:GetGraphic(), -1,  FildPath.."_2.tex", nil, CFPos:new(), "0xffffffff", IsCenter ) 	
	Wnd:SetWndBkImage( Flag, DefaultImage )
	
	--2.
	Flag.StateMask = IP_MOUSEOVER
	local DefaultImage = Wnd:GetWndBkImage(Flag)
	if not DefaultImage then
		DefaultImage = WND_IMAGE_LIST:new()
	end
	DefaultImage:AddImage( Wnd:GetGraphic(), -1,  FildPath.."_3.tex", nil, CFPos:new(), "0xffffffff", IsCenter ) 	
	Wnd:SetWndBkImage( Flag, DefaultImage )
	
	--3.
	Flag.StateMask = IP_CLICKDOWN
	local DefaultImage = Wnd:GetWndBkImage(Flag)
	if not DefaultImage then
		DefaultImage = WND_IMAGE_LIST:new()
	end
	DefaultImage:AddImage( Wnd:GetGraphic(), -1,  FildPath.."_1.tex", nil, CFPos:new(), "0xffffffff", IsCenter ) 	
	Wnd:SetWndBkImage( Flag, DefaultImage )
	
	--4.
	Flag.StateMask = IP_DISABLE
	local DefaultImage = Wnd:GetWndBkImage(Flag)
	if not DefaultImage then
		DefaultImage = WND_IMAGE_LIST:new()
	end
	DefaultImage:AddImage( Wnd:GetGraphic(), -1,  FildPath.."_1.tex", nil, CFPos:new(CenterX, CenterY), "0xffffffff", IsCenter ) 	
	Wnd:SetWndBkImage( Flag, DefaultImage )
end

local function ShowWndInfo(Wnd)
	if g_GameMain.m_ShowMercDirTick then
		UnRegisterTick(g_GameMain.m_ShowMercDirTick)
		g_GameMain.m_ShowMercDirTick = nil
	end
	
	local rt = CFRect:new()
	for i=1,11 do
		Wnd.m_GuiZeText[i]:SetTransparent(1)
		Wnd.m_GuiZeText[i]:ShowWnd(true)
--		Wnd.m_GuiZeText[i]:GetLogicRect(rt)
--		rt.bottom = rt.top + Wnd.m_GuiZeHeightTbl[i]
--		Wnd.m_GuiZeText[i]:SetLogicRect(rt)
	end
	
	for i=1,9 do
		Wnd.m_DialogText[i]:SetTransparent(1)
		Wnd.m_DialogText[i]:ShowWnd(true)
--		Wnd.m_DialogText[i]:GetLogicRect(rt)
--		rt.right = rt.left + Wnd.m_DialogWidthTbl[i]
--		Wnd.m_DialogText[i]:SetLogicRect(rt)
	end
	
	Wnd.m_Btn:ShowWnd(true)
end

function CYBDirWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	if uMsgID == BUTTON_LCLICK then
		if Child == self.m_StopBtn and self.m_ShowState ~= 3 then
			ShowWndInfo(self)
		end
	end
end

--点击完之后要处理的事务
local function ClickBtnAffter(ClickWnd)
	local Wnd = g_GameMain.m_MercenaryDirWnd
	SetButtonFx(Wnd.m_MainYbDirWnd.m_Btn, 0, 128,128)
	Wnd.m_MainYbDirWnd.m_Btn:AddFlashInfoByName("ybbook_btn")
	
	Wnd.m_MainYbDirWnd.m_Btn:EnableWnd(false)
	ClickWnd:EnableWnd(false)
	
	--小变大
	local rt = CFRect:new()
	local rt_1 = CFRect:new()
	ClickWnd:GetWndRect(rt)
	
	rt_1.left = rt.left - 30
	rt_1.right = rt.right + 30
	rt_1.top = rt.top - 30
	rt_1.bottom = rt.bottom + 30
	
	ClickWnd:SetShapeEffect(1000, rt, rt_1)
	ClickWnd:SetTransparent(0.6)
	ClickWnd:SetTransparentObj(1500, true)
	
	g_GameMain.m_MercenaryDirWnd.m_YbDirTempWnd:DelFlashAttention()
	
	local function CallBack_Tick()
		--RetShowHintMsgWnd()
		g_GameMain.m_GuideBtn:BeAlertQuestEmptyAlert()
		g_GameMain.m_MercenaryDirWnd:DelWndImeFx("heiping/w_b")
		g_GameMain.m_MercenaryDirWnd:PlayWndImeFx("heiping/b_w",0)
		g_GameMain.m_MercenaryDirWnd:ShowWnd(false)
		g_GameMain.m_GuideWnd:OpenPanel(true)
		
		g_GameMain.m_MercenaryDirWnd.m_ClickBtn = true
		Gac2Gas:SetCheckInMercenaryInfo(g_Conn)
		g_GameMain.m_MercenaryLevelTrace.m_IsCheckIn = nil
		
		g_GameMain.m_NpcHeadSignMgr:UpdateNpcAndObjInView()
		--Gac2Gas:CreateMercenaryLevel(g_Conn)
	end
	RegisterOnceTick(g_GameMain, "CloseRegisterWnd", CallBack_Tick, 2000)
end

function CYBDirTempBtnWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	if uMsgID == BUTTON_LCLICK then
		if Child == self.m_OKBtn then
			ClickBtnAffter(Child)
		end
	end
end

--改变进度条的颜色
function CYBDirTempBtnWnd:SetButtonWnd()
	SetButtonFx(self.m_OKBtn, 16)
	self.m_OKBtn:EnableWnd(true)
end

function CYBDirWnd:ResetWnd()
	self.m_ShowState = 0--当前文字显示的状态
	self.m_ShowNum = 1--显示到第几行了
	self.m_Frequency = 3--频率
	self.m_Btn.m_BtnWnd:SetButtonWnd()
	
	local rt = CFRect:new()
	for i=1,11 do
		self.m_GuiZeText[i]:ShowWnd(false)
--		self.m_GuiZeText[i]:GetLogicRect(rt)
--		rt.bottom = rt.top
--		self.m_GuiZeText[i]:SetLogicRect(rt)
	end
	
	for i=1,9 do
		self.m_DialogText[i]:ShowWnd(false)
--		self.m_DialogText[i]:GetLogicRect(rt)
--		rt.right = rt.left
--		self.m_DialogText[i]:SetLogicRect(rt)
	end
	
	self.m_StopBtn:ShowWnd(false)
	self.m_Btn:ShowWnd(false)
end

local function TickRun(Tick, TempWnd, Wnd)
	--变大后,让主窗口开始一直发亮的闪
	--显示左侧的文字(一行行)
	--显示右侧的文字(一个字一个字的)
	--显示按扭(其它都显示完后)
	local rt = CFRect:new()
	
	if Wnd.m_ShowState == 0 then
		Wnd.m_ShowNum = Wnd.m_ShowNum + 1
		if Wnd.m_ShowNum >= 4 then
			Wnd.m_ShowState = 1
			Wnd.m_ShowNum = 1
			TempWnd:AddFlashInfoByName("ybbook_gb")
			--Wnd.m_StopBtn:ShowWnd(true)
		end
	elseif Wnd.m_ShowState == 1 then
--			Wnd.m_GuiZeText[ShowNum]:GetLogicRect(rt)
--			local tempHeight = 1.5
--			if (rt.bottom-rt.top+tempHeight) >= Wnd.m_GuiZeHeightTbl[ShowNum] then
--				tempHeight = Wnd.m_GuiZeHeightTbl[ShowNum] - (rt.bottom-rt.top)
--				Wnd.m_ShowNum = Wnd.m_ShowNum + 1
--				if Wnd.m_ShowNum > 4 then
--					Wnd.m_ShowState = 1
--					Wnd.m_ShowNum = 1
--				end
--			end
--			rt.bottom = rt.bottom + tempHeight
--			Wnd.m_GuiZeText[ShowNum]:SetLogicRect(rt)
		Wnd.m_Frequency = Wnd.m_Frequency - 1
		if Wnd.m_Frequency == 2 then
			Wnd.m_GuiZeText[Wnd.m_ShowNum]:SetTransparent(0)
			Wnd.m_GuiZeText[Wnd.m_ShowNum]:ShowWnd(true)
			Wnd.m_GuiZeText[Wnd.m_ShowNum]:AddFlashInfoByName("ybbook_l"..Wnd.m_ShowNum)
		elseif Wnd.m_Frequency <= 1 then
			Wnd.m_GuiZeText[Wnd.m_ShowNum]:SetTransparent(1)
			Wnd.m_Frequency = 3
			Wnd.m_ShowNum = Wnd.m_ShowNum + 1
			if Wnd.m_ShowNum > 11 then
				Wnd.m_ShowState = 2
				Wnd.m_ShowNum = 1
			end
		end
	elseif Wnd.m_ShowState == 2 then
--			Wnd.m_DialogText[ShowNum]:GetLogicRect(rt)
--			local tempWidth = 3
--			if (rt.right-rt.left+tempWidth) >= Wnd.m_DialogWidthTbl[ShowNum] then
--				tempWidth = Wnd.m_DialogWidthTbl[ShowNum] - (rt.right-rt.left)
--				Wnd.m_ShowNum = Wnd.m_ShowNum + 1
--				if Wnd.m_ShowNum > 9 then
--					Wnd.m_ShowState = 2
--					Wnd.m_ShowNum = 1
--				end
--			end
--			rt.right = rt.right + tempWidth
--			Wnd.m_DialogText[ShowNum]:SetLogicRect(rt)
		Wnd.m_Frequency = Wnd.m_Frequency - 1
		if Wnd.m_Frequency == 2 then
			Wnd.m_DialogText[Wnd.m_ShowNum]:SetTransparent(0)
			Wnd.m_DialogText[Wnd.m_ShowNum]:ShowWnd(true)
			Wnd.m_DialogText[Wnd.m_ShowNum]:AddFlashInfoByName("ybbook_r"..Wnd.m_ShowNum)
		elseif Wnd.m_Frequency <= 1 then
			Wnd.m_DialogText[Wnd.m_ShowNum]:SetTransparent(1)
			Wnd.m_Frequency = 3
			Wnd.m_ShowNum = Wnd.m_ShowNum + 1
			if Wnd.m_ShowNum > 9 then
				Wnd.m_ShowState = 3
				Wnd.m_ShowNum = 1
			end
		end
	elseif Wnd.m_ShowState == 3 then
		UnRegisterTick(g_GameMain.m_ShowMercDirTick)
		g_GameMain.m_ShowMercDirTick = nil
		Wnd.m_Btn:SetTransparent(0)
		Wnd.m_Btn:SetTransparentObj(1000, false)
		--Wnd.m_Btn:ShowWnd(true)
	end
	
end

function CYBDirTempFxWnd:InitWnd()
	if self:IsShow() then
		return
	end
	
	Gas2Gac:ClearCompassHeadDir(g_Conn)
	
	self.m_MainYbDirWnd:ResetWnd()
	self:ShowWnd(true)
	self.m_MainYbDirWnd:ShowWnd(true)
	
	--g_GameMain:PlayScreenFx("fx\setting\mapfx_ui.efx",2)
	self:PlayWndImeFx("heiping/w_b",2)
	
	
	--小变大
	local rt = CFRect:new()
	local rt_1 = CFRect:new()
	self.m_YbDirTempWnd:GetWndRect(rt)
	--print(rt.left, rt.right, rt.top, rt.bottom)
	rt_1.left = rt.left + 70
	rt_1.right = rt.right - 70
	rt_1.top = rt.top + 50
	rt_1.bottom = rt.bottom - 50
	--print(rt_1.left, rt_1.right, rt_1.top, rt_1.bottom)
	
	self.m_YbDirTempWnd:SetShapeEffect(1000, rt_1, rt);
	--DelShapeEffect( SQRWnd* Wnd )
	self.m_YbDirTempWnd:SetTransparent(0.5)
	self.m_YbDirTempWnd:SetTransparentObj(1500, false)
	g_GameMain.m_ShowMercDirTick = RegisterTick("ShowMercenaryDirTick", TickRun, TickTime, self.m_YbDirTempWnd, self.m_MainYbDirWnd)
end
