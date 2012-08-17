cfg_load "fb_game/MercCardTooltip_Common"
cfg_load "message/Description_Client"

CYbEducateActionInfoWnd = class(SQRDialog)

local CYbEducateActionInfoPage = class(SQRDialog)

function CYbEducateActionInfoPage:Ctor(Parent, type)
	if type == 1 then
		self:CreateFromRes("Fb_MercActionInfoPage", Parent)
		self.m_DescPic = self:GetDlgChild("DescPic") -- 图片
		self.m_Desc = self:GetDlgChild("Desc") -- 文字
	elseif type == 2 then
		self:CreateFromRes("Fb_MercActionInfoLeftSide", Parent)
	elseif type == 3 then
		self:CreateFromRes("Fb_MercActionInfoRightSide", Parent)		
	end
	self:ShowWnd(false)
end

function CYbEducateActionInfoWnd:Ctor()
--	print("CYbEducateActionInfoWnd~~~")
	self:CreateFromRes("Fb_MercActionInfoWnd",g_GameMain)
	self.m_DescPic = self:GetDlgChild("DescPic0")
	self.m_Desc = self:GetDlgChild("Desc0")
	 
	-- 4个页面
	for i=1, 4 do
		self["m_Page"..i] = CYbEducateActionInfoPage:new(self, 1)
	end
	
	-- 两边的装饰页面
	self.m_SidePage1 = CYbEducateActionInfoPage:new(self, 2)
	self.m_SidePage2 = CYbEducateActionInfoPage:new(self, 3)
		
	self.m_OkBtn = self:GetDlgChild("OkBtn")
end

function CYbEducateActionInfoWnd:GetWnd()
	if not g_GameMain.m_YbEducateActInfoWnd then
		g_GameMain.m_YbEducateActInfoWnd = CYbEducateActionInfoWnd:new()
	end
end

local function AddImage(wnd, strTexture)
	local Flag = IMAGE_PARAM:new()
	Flag.CtrlMask = SM_BS_BK
	Flag.StateMask = IP_ENABLE	
	local DefaultImage = WND_IMAGE_LIST:new()
	DefaultImage:AddImage(wnd:GetGraphic(), -1,  strTexture, nil, CFPos:new(0,0), "0xffffffff", 0 ) 	
	wnd:SetWndBkImage(Flag, DefaultImage)
end

-- 创建一个信息页面
local function DescPage(Wnd, i, RulesText, NowGameID)
	local Num = RulesText.."_"..i
	-- 描述
	local TextColor = nil
	if Description_Client(Num) then
		if i == 0 then
			TextColor = "#cc3af7e"
		else
			TextColor = "#ca8966b"
		end
		Wnd.m_Desc:SetWndText(TextColor..GetDescriptionDisplayName(Num))
	else
		Wnd.m_Desc:SetWndText("")
	end
	-- 图片
	--if MercCardTooltip_Common(NowGameID) then
	if i==0 then
		if MercCardTooltip_Common(NowGameID, "SmallIcon") and MercCardTooltip_Common(NowGameID, "SmallIcon") ~= "" then
			g_LoadIconFromRes(MercCardTooltip_Common(NowGameID, "SmallIcon"), Wnd.m_DescPic, -1, IP_ENABLE, IP_ENABLE)
			Wnd.m_DescPic:ShowWnd(true)
		end
		if MercCardTooltip_Common(NowGameID, "SmallIcon") == "" then
			Wnd.m_DescPic:ShowWnd(false)
		end
	else
		if MercCardTooltip_Common(NowGameID, "DesPicStr"..i) then
			AddImage(Wnd.m_DescPic, MercCardTooltip_Common(NowGameID, "DesPicStr"..i))
		end
	end
	--end
end

function CYbEducateActionInfoWnd:InfoWnd(RulesText,DownTime, NowGameID)
	if Description_Client(RulesText) then
		--self.m_ActionInfo:SetWndText(GetDescriptionDisplayName(RulesText))		
--		local TargetList = ""
--		local OverTypeTbl = g_MercenaryEduActMgr[NowGameID].OverTypeTbl
--		local MercMgr = Lan_MercenaryEducateAction_Server(NowGameID)
--		local NameTbl = loadstring("return {"..MercMgr("ShowName").."}")()
--		for i, Name in pairs(NameTbl) do
--			TargetList = TargetList..Name[1].."				"..OverTypeTbl[i][2].."#r"
--		end
	end
	local Wnd = g_GameMain.m_FbActionMgrWnd.m_ListInfoWnd
	Wnd:SamllInfoWnd(NowGameID)
	DescPage(self, 0, RulesText, NowGameID)
	
	-- 检测有几页描述
	local PageNum = 0
	for i=1, 4 do
		local Num = RulesText.."_"..i
		-- 描述
		if Description_Client(Num) and Lan_Description_Client(MemH64(Num),"Content") ~= "" then
			PageNum = PageNum + 1
		else
			break
		end
	end
	
	local rt = CFRect:new()
	self:GetLogicRect(rt)
	local WndWidth = self:GetWndOrgWidth()
	local PageWidth = self.m_Page1:GetWndOrgWidth()
	local PageHeight = self.m_Page1:GetWndOrgHeight()
	local SideWidth = self.m_SidePage1:GetWndOrgWidth() -- 装饰页面的宽度
	local SideHeight = self.m_SidePage1:GetWndOrgHeight() -- 装饰页面的高度
	local SideGap = 0
	local Interval = 5	-- 页面间隔
	
	-- 计算介绍页面距离self左边缘的距离
	if PageNum < 4 then
		SideGap = (WndWidth - PageNum*PageWidth - (PageNum-1)*Interval ) / 2 -- 不够4的话，两边得加上装饰页面
	else
		SideGap = (WndWidth - 4*PageWidth - 3*Interval) / 2
	end

	local Rect = CFRect:new()
	for i=1, PageNum do
		Rect.top = rt.top + 165
		Rect.left = rt.left + SideGap + (i-1)*(PageWidth+Interval)
		Rect.right = Rect.left + PageWidth
		Rect.bottom = Rect.top + PageHeight
		DescPage(self["m_Page"..i], i, RulesText, NowGameID)
		self["m_Page"..i]:SetLogicRect(Rect)
		self["m_Page"..i]:ShowWnd(true)
	end
	
	-- 显示装饰页面
	local SideRect = CFRect:new()
	if PageNum < 4 and PageNum > 0 then
		local Rect1 = CFRect:new()
		self.m_Page1:GetLogicRect(Rect1)
	
		SideRect.right = Rect1.left - Interval
		SideRect.left = SideRect.right - SideWidth
		SideRect.top = rt.top + 165
		SideRect.bottom = SideRect.top + SideHeight
		self.m_SidePage1:SetLogicRect(SideRect)
		self.m_SidePage1:ShowWnd(true)
		
		self["m_Page"..PageNum]:GetLogicRect(Rect1)
		SideRect.left = Rect1.right + Interval
		SideRect.right = SideRect.left + SideWidth
		self.m_SidePage2:SetLogicRect(SideRect)
		self.m_SidePage2:ShowWnd(true)
	end 
	
	--前5秒,不让玩家上按扭-----------
	--local TempTime = DownTime
	--self.m_OkBtn:EnableWnd(false)
	--------------------------------
	
	local BtnName = GetStaticTextClient(1068)--开始
	local function DownTimeFun()
		DownTime = DownTime - 1
		
		--让按扭可以点击
		--if TempTime == DownTime + 5 then
		--	self.m_OkBtn:EnableWnd(true)
		--end
		
		if DownTime <= 0 then
			--关闭窗口
			self.m_IsCountDown = false
			self:CloseWnd()
		else
			self.m_OkBtn:SetWndText(BtnName.."("..DownTime..")")
		end
	end

	if DownTime > 0 then
		self.m_IsCountDown = true
		if self.m_DownTick then
			UnRegisterTick(self.m_DownTick)
		end
		self.m_OkBtn:SetWndText(BtnName.."("..DownTime..")")
		self.m_DownTick = RegisterTick("YbActionWnd",DownTimeFun,1000)
	else
		local BtnName = GetStaticTextClient(8807)--开始
		self.m_OkBtn:SetWndText(BtnName)
	end
	self:ShowWnd(true)
end


--佣兵活动中的信息介绍面板=======================
function CYbEducateActionInfoWnd:CloseWnd()
	UnRegisterTick(self.m_DownTick)
	self.m_DownTick = nil
	--关闭窗口
	for i=1, 4 do
		self["m_Page"..i]:ShowWnd(false)
	end
	self.m_SidePage1:ShowWnd(false)
	self.m_SidePage2:ShowWnd(false)
	self:ShowWnd(false)
end

function CYbEducateActionInfoWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	if uMsgID == BUTTON_LCLICK then
		if Child == self.m_OkBtn then	-- 玩家点击开始游戏按钮
			Gac2Gas:BeginYbEducateAction(g_Conn)
			self.m_IsCountDown = false
			self:CloseWnd()
		end
	end
end

--function CYbEducateActionInfoWnd.RetShowYbEducateActInfoWnd(Conn, RulesText, DownTime, NowGameID)
--	g_GameMain.m_YbEducateActInfoWnd = CYbEducateActionInfoWnd:new()
--	local Wnd = g_GameMain.m_YbEducateActInfoWnd
--	Wnd.m_RulesText = RulesText
--	Wnd.m_NowGameID = NowGameID
--	
--	Wnd:InfoWnd(RulesText, DownTime, NowGameID)
--end