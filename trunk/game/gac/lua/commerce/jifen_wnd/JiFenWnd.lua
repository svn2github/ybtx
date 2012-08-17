gac_require "commerce/jifen_wnd/JiFenWndInc"
--积分表；按顺序显示，第一列是积分类型，第二列是积分名称对应的id，第三列是积分tooltips对应的id
local JiFenListTbl = {
{11, 1199, 1211, 1},
{10, 1200, 1210, 10},
{8,  299,  1208, 13},
{9,  300,  1209, 20},
{5,  255,  1205, 25},
{6,  256,  1206, 25},
{7,  298,  1207, 30},
{1,  251,  1201, 65},
{2,  252,  1202, 65},
{3,  253,  1203, 65},
{4,  254,  1204, 65}
}


function CJiFenWnd:Ctor()
    self:CreateFromRes("JiFenDetailWnd", g_GameMain.m_PlayerManageWnd)
    self.m_CloseBtn = self:GetDlgChild("CloseBtn")
    self.m_CloseBtn:ShowWnd(false)
    self:InitChild()
end

function CJiFenWnd:UpdateJiFenDetail()
    for i=1, # JiFenListTbl  do
        local jifenType = JiFenListTbl[i][1]
        local jifenCount = 0
        if jifenType ~= 11 then
            jifenCount = g_MainPlayer:GetJiFenCountByType(jifenType)
        end
        g_GameMain.m_JiFenDetailWnd.m_JiFenCountBtnTbl[i]:SetWndText(jifenCount)
    end
end

function CJiFenWnd:InitChild()
    self.m_JiFenCountBtnTbl = {}
    self.m_JiFenDetailBtnTbl = {}
    self.m_JiFenStaticBtnTbl = {}
    local baseNo = 1200
    for i=1, #JiFenListTbl do
        local childStr = string.format("%s%d", "JiFenCount", i)
        local child = self:GetDlgChild(childStr)
        local detailStr = string.format("%s%d", "JiFenDetail", i)
        local detail = self:GetDlgChild(detailStr)
        table.insert(self.m_JiFenCountBtnTbl, child)
        local staticStr = string.format("%s%d", "JiFenStatic", i)
        local staticChild = self:GetDlgChild(staticStr)
        staticChild:SetMouseOverDescAfter(ToolTipsText(JiFenListTbl[i][3]))
        table.insert(self.m_JiFenStaticBtnTbl, staticChild)
        table.insert(self.m_JiFenDetailBtnTbl, detail)
		staticChild:SetWndText(ToolTipsText(JiFenListTbl[i][2]))
    end
    
    self.m_WithOneJiFenShopListWnd = CWithOneJiFenShopListWnd:new(self)
    
	local Rt = CFRect:new()
	g_GameMain.m_PlayerManageWnd.m_ActionCheckBtnTbl[1]:GetLogicRect(Rt)
	Rt.top = Rt.bottom + 3
	Rt.bottom = Rt.top + self:GetWndOrgHeight()
	Rt.right = Rt.left +  self:GetWndOrgWidth()
	self:SetLogicRect(Rt)	    
end

function CJiFenWnd:UpdateJiFenWnd()
    local playerLevel = g_MainPlayer:CppGetLevel()
    for i=1, #JiFenListTbl do
        local bFalg = true
		if JiFenListTbl[i][4] > playerLevel then
            bFalg = false
		end
		self.m_JiFenCountBtnTbl[i]:ShowWnd(bFalg)
		self.m_JiFenDetailBtnTbl[i]:ShowWnd(bFalg)
		self.m_JiFenStaticBtnTbl[i]:ShowWnd(bFalg)
    end
end

function CJiFenWnd:SetWithOneJiFenShopList(Child)
    local jiFenType = 1
    for i, v in pairs (self.m_JiFenDetailBtnTbl) do
        if v  == Child then
            jiFenType  = JiFenListTbl[i][1]
            break
        end
    end
    local jiFenShopList = g_WithOneJiFenNpcShopMgr[jiFenType]
    if # jiFenShopList == 1 then
        local npcName = g_WithOneJiFenNpcShopMgr[jiFenType][1]
        g_GameMain.m_NPCShopSell:NPCShopSellWnd(npcName) 
    else
        self.m_WithOneJiFenShopListWnd:InitWithOneJiFenShopList(jiFenShopList)
        local Rt = CFRect:new()
        Child:GetLogicRect(Rt)
        Rt.left = Rt.right + 3
        self.m_WithOneJiFenShopListWnd:ShowWnd(true)
        self.m_WithOneJiFenShopListWnd:SetLogicRect(Rt)
    end
end

function CJiFenWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
    if uMsgID == BUTTON_LCLICK  then
        self:SetWithOneJiFenShopList(Child)
    end
end
