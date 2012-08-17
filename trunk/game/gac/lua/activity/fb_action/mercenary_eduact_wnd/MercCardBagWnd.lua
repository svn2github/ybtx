lan_load "fb_game/Lan_MercCardTooltip_Common"
cfg_load "fb_game/MercCardTooltip_Common"

CMercCardBagWnd = class(SQRDialog)

local MercCardLine 			= 5			-- 5行
local MercCardNum 			= 10 		-- 每行10列
local MercCardPage 			= 2			-- 页数
local MercCardWeightInv = 15 		-- 间距
local MercCardHeightInv = 16
local NowPageWnd 				= nil		-- 当前页面窗口
local NowPageNum				= 1 		-- 当前页数

local function GetMercCardNum()
	local MercCardNumber = 0
	for _, CardID in pairs(MercCardTooltip_Common:GetKeys()) do
		if CardID < 1000 then
			MercCardNumber = MercCardNumber + 1
		end
	end
	return MercCardNumber
end

function CMercCardBagWnd:Ctor()
--	self.RecordMecCardTbl = {}
	NowPageNum = 1
	self:CreateFromRes("Fb_MercCardBagWnd",g_GameMain) 
	self.m_CloseBtn 				= self:GetDlgChild("CloseBtn")
	self.m_MercCardPageBtn = self:GetDlgChild("MercCardPageBtn")
	self.m_PrePageBtn 			= self:GetDlgChild("PrePageBtn")
	self.m_NextPageBtn 		= self:GetDlgChild("NextPageBtn")
	self.m_PageNumber			= self:GetDlgChild("PageNumber")
	
	self.m_MercCardPageOneWnd = CMercCardPageOneWnd:new(self.m_MercCardPageBtn)
	self.m_MercCardPageTwoWnd = CMercCardPageTwoWnd:new(self.m_MercCardPageBtn)
	NowPageWnd = self.m_MercCardPageOneWnd 		-- 初始是第一页
	
	self.m_MercCards = {}
	local MercCardNumber	= GetMercCardNum()
	for i = 1, MercCardNumber do
		Line = math.floor((i-1) / 10) + 1
		Num = (i-1) % 10 + 1
		local number = (Line-1)*MercCardNum + Num	
		local TempWnd = nil				
		if Line <= MercCardLine then															-- 第一页
--			self.m_MercCards[number] = CreateMercCardWnd(self.m_MercCardPageOneWnd, number)	
			self.m_MercCards[number] = CMercCardWnd:new(self.m_MercCardPageOneWnd, number)
			TempWnd = self.m_MercCardPageOneWnd
		elseif Line <= 2 * MercCardLine then											-- 第二页
--			self.m_MercCards[number] = CreateMercCardWnd(self.m_MercCardPageTwoWnd, number)
			self.m_MercCards[number] = CMercCardWnd:new(self.m_MercCardPageTwoWnd, number)
			TempWnd = self.m_MercCardPageTwoWnd
			Line = Line - 5
		end
		
		-- 位置
		local CardWnd = self.m_MercCards[number]
		local number = (Line-1)*MercCardNum + Num
		local MercCardWindth = CardWnd:GetWndOrgWidth()
		local MercCardHeight = CardWnd:GetWndOrgHeight()
		local rt = CFRect:new()
		TempWnd:GetLogicRect(rt)
		local rt_1 	= CFRect:new()
		rt_1.top 		= rt.top + (MercCardHeightInv + MercCardHeight) * (Line - 1)
		rt_1.left 	= rt.left + (MercCardWeightInv + MercCardWindth) * (Num - 1)
		rt_1.bottom = rt_1.top + MercCardHeight
		rt_1.right 	= rt_1.left + MercCardWindth
		CardWnd:SetLogicRect(rt_1)
	end
	
	g_ExcludeWndMgr:InitExcludeWnd(self, 1)
	self:ShowWnd(false)
	--Wnd:ShowWnd(false)
	return Wnd
end

function CMercCardBagWnd:OpenMercCardBag()
	if not self:IsShow() then
		self:ShowWnd(true)
		NowPageWnd:ShowWnd(false)
		NowPageWnd = self.m_MercCardPageOneWnd 		-- 初始是第一页
		self:ShowFirstPage()
		NowPageNum = 1
		self.m_PageNumber:SetWndText(NowPageNum)
		self.m_PrePageBtn:ShowWnd(false)							-- 不显示“前一页”
		self.m_NextPageBtn:ShowWnd(true)							-- 显示“下一页”
	else
		self:ShowWnd(false)
		NowPageWnd:ShowWnd(false)
	end
end

function CMercCardBagWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if uMsgID == BUTTON_LCLICK  then
		if Child == self.m_PrePageBtn then
			if NowPageNum > 1 then
				NowPageNum = NowPageNum - 1
				if NowPageNum == 1 then
					NowPageWnd:ShowWnd(false)											-- 不显示刚才那页
					--self.m_MercCardPageOneWnd:ShowWnd(true)				-- 显示第一页
					self:ShowFirstPage()
					NowPageWnd = self.m_MercCardPageOneWnd				-- 当前的WND
					self.m_PageNumber:SetWndText(NowPageNum)			-- 显示页数
					self.m_PrePageBtn:ShowWnd(false)							-- 不显示“前一页”
					self.m_NextPageBtn:ShowWnd(true)							-- 显示“下一页”
				end	
			end		
		elseif Child == self.m_NextPageBtn then
			if NowPageNum < MercCardPage then
				NowPageNum = NowPageNum + 1
				if NowPageNum == 2 then
					NowPageWnd:ShowWnd(false)
					--self.m_MercCardPageTwoWnd:ShowWnd(true)
					self:ShowSecondPage()
					NowPageWnd = self.m_MercCardPageTwoWnd
					self.m_PageNumber:SetWndText(NowPageNum)
					self.m_NextPageBtn:ShowWnd(false)							-- 不显示“下一页”
					self.m_PrePageBtn:ShowWnd(true)								-- 显示“前一页”
				end
			end
		elseif Child == self.m_CloseBtn then
			self:ShowWnd(false)
			NowPageWnd:ShowWnd(false)
		end
	end
end

function CMercCardBagWnd:ShowFirstPage()
	self.m_MercCardPageOneWnd:ShowWnd(true)
	for i=1, MercCardLine*MercCardNum do
		if self.RecordMecCardTbl[i] then
			self:ShowMercCard(i)
		end
	end
end

function CMercCardBagWnd:ShowSecondPage()
	self.m_MercCardPageTwoWnd:ShowWnd(true)	
	for i=MercCardLine*MercCardNum + 1, 2*MercCardLine*MercCardNum do
		if self.RecordMecCardTbl[i] then
			self:ShowMercCard(i)
		end	
	end
end

function CMercCardBagWnd:ShowMercCard(ID)
	self.m_MercCards[ID]:ShowWnd(true)
end

function CMercCardBagWnd:InitMercCard(Conn)
	self.RecordMecCardTbl = {}
end

function CMercCardBagWnd:RecordMercCard(Conn,i)
	self.RecordMecCardTbl[i] = true
end