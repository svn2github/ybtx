gac_require "activity/fb_action/join_fb_wnd/DrinkWndInc"

cfg_load "fb_game/DrinkName_Common"
lan_load "fb_game/Lan_DrinkName_Common"

local TirValue = 0
local DrunkSign

function CreateDrinkWnd(partent)
	local Wnd = CDrinkWnd:new()
	Wnd:CreateFromRes("DrinkWnd",partent)
	
	Wnd.m_CloseBtn = Wnd:GetDlgChild("CloseBtn")
	Wnd.m_OkBtn = Wnd:GetDlgChild("OkBtn")
	Wnd.m_Introduction = Wnd:GetDlgChild("Introduction")
	Wnd.m_ItemListCtrl = Wnd:GetDlgChild("ItemList")
	Wnd.m_Price = Wnd:GetDlgChild("Price")
	Wnd.m_GoldPic = Wnd:GetDlgChild("GoldPic")
	Wnd.m_Silver = Wnd:GetDlgChild("Silver")
	Wnd.m_SilverPic = Wnd:GetDlgChild("SilverPic")
	Wnd.m_Copper = Wnd:GetDlgChild("Copper")
	Wnd.m_CopperPic = Wnd:GetDlgChild("CopperPic")
	Wnd.m_PerNum = Wnd:GetDlgChild("PerNum")

	Wnd.m_ClickItemIndex = 1
	Wnd.m_ClickedItemName = ""
	Wnd.m_DrinkTbl = {}
	Wnd:InitDrinkList()
	
	g_ExcludeWndMgr:InitExcludeWnd(Wnd, 1)
	Wnd:ShowWnd(false)
	return Wnd
end

local function GetChoosedItemIndex(uParam1, uParam2)
	local index = uParam1 + uParam2 + 1
	return index
end

function CDrinkWnd:InitWindow()
	self.m_DrinkTbl = {}
	for _, drinkName in pairs(DrinkName_Common:GetKeys()) do
		table.insert(self.m_DrinkTbl, drinkName)
	end
	
	local drinkName = self.m_DrinkTbl[1]
	self.m_ClickedItemName = drinkName
	
	Gac2Gas:GetTiredNess(g_Conn, self.m_ClickedItemName)
	self:ShowWnd(true)
	local AutoCloseWnd = CAutoCloseWnd:new()
	AutoCloseWnd:AutoCloseWnd(self)
end

function CDrinkWnd:InitDrinkList()
	self.m_ItemListCtrl:DeleteAllItem()
	self.m_ItemListCtrl:InsertColumn(0, self.m_ItemListCtrl:GetWndOrgWidth())
	
	for _, drinkName in pairs(DrinkName_Common:GetKeys()) do
		table.insert(self.m_DrinkTbl, drinkName)
		local drink = Lan_DrinkName_Common(MemH64(drinkName))
		self:InsertListInfo(self.m_ItemListCtrl, drink("DisplayName"), drinkName)
	end
end

function CDrinkWnd:CreateListItem(parent,DrinkShowName, DrinkName)
	local Wnd = CDrinkWndListItem:new()
	Wnd:CreateFromRes("NpcShopBuyItem", parent)
	Wnd:ShowWnd(true)
	Wnd.m_ItemBtn = Wnd:GetDlgChild("ItemBtn")
	Wnd.m_ItemDis = Wnd:GetDlgChild("ItemDis")
	Wnd.m_OverIconBtn = Wnd:GetDlgChild("OverIconBtn")
	Wnd.m_ItemDis:SetWndText(DrinkShowName)
	Wnd.m_OverIconBtn:SetBackImageColor(0x52ff0000)
	Wnd.m_OverIconBtn:SetMouseWheel(true)
	Wnd.m_OverIconBtn:ShowWnd(false)
	local drink = DrinkName_Common(DrinkName)
	g_LoadIconFromRes(drink("Icon"), Wnd.m_ItemBtn, -1, IP_ENABLE, IP_CLICKDOWN)
	
	return Wnd
end

function CDrinkWnd:InsertListInfo(ListCtrl, DrinkShowName, DrinkName)
	local ItemCount = ListCtrl:GetItemCount()
	ListCtrl:InsertItem(ItemCount, 40)
	local Item = ListCtrl:GetSubItem(ItemCount, 0)
	Item:SetMouseWheel(true)
	local itemWnd = self:CreateListItem(Item, DrinkShowName, DrinkName)
	Item.m_ItemWnd = itemWnd
end

function CDrinkWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )	
	if uMsgID == BUTTON_LCLICK then
		if Child == self.m_CloseBtn then
			self:ShowWnd(false)
		elseif Child == self.m_OkBtn then
			Gac2Gas:IsDrunk(g_Conn)
		end
	elseif uMsgID == ITEM_LBUTTONCLICK or uMsgID == ITEM_RBUTTONCLICK then
		local drinkIndex = self.m_ItemListCtrl:GetSelectItem(-1) + 1
		local drinkName = self.m_DrinkTbl[drinkIndex]
		self.m_ClickedItemName = drinkName
		Gac2Gas:GetTiredNess(g_Conn, self.m_ClickedItemName)
	end
end

function CDrinkWnd:ShowIntroduction(tirednessValue)
	TirValue = tirednessValue
	local drink = DrinkName_Common(self.m_ClickedItemName)
	local drinkShowInfo = Lan_DrinkName_Common(MemH64(self.m_ClickedItemName))
	local drinkName = drinkShowInfo("DisplayName")
	local Introduction = drinkShowInfo("Introduction")
	local Effect = drinkShowInfo("Effect_introduction")
	
	
	local Consistence = GetStaticTextClient(9501, drink("Consistence"))
	local Tiredness = GetStaticTextClient(9502, tirednessValue)
	local PriceNum = drink("Price")
	local Price = GetStaticTextClient(9503, math.floor(PriceNum / 10000))
	PriceNum = PriceNum % 10000
	
	local str = drinkName.."\n"..GetStaticTextClient(9504).."\n"..
							Introduction.."\n"..GetStaticTextClient(9505).."\n"..Effect..
							"\n"..Consistence.."\n"..Tiredness.."\n"
	self.m_Introduction:SetWndText(str)
	self.m_Price:SetWndText(Price)
	self.m_Silver:SetWndText(math.floor(PriceNum / 100))
	PriceNum = PriceNum % 100
	self.m_Copper:SetWndText(math.floor(PriceNum))
	--self.m_PerNum:SetWndText("/杯")
end

function Gas2Gac:RetShowDrinkWnd(Conn)
	g_GameMain.m_DrinkWnd:InitWindow()
end

function Gas2Gac:IsDrunk(Conn, Sign)
	g_GameMain.m_DrinkWnd:ShowNotify(Sign)
end

function CDrinkWnd:OnDrink()
    local function IsContinue(GameWnd, uButton)
		if uButton == MB_BtnOK then
			Gac2Gas:HaveDrink(g_Conn, self.m_ClickedItemName)
		end
		g_GameMain.m_DrinkWnd.m_LiuTongTip = 0 
		return true
	end
    MessageBox(g_GameMain, GetStaticTextClient(9507, TirValue), BitOr(MB_BtnOK, MB_BtnCancel), IsContinue, g_GameMain)
end

local LiuTongBiTipFlag      = 1     --流通金币使用提示，1为提示，0为不提示

function CDrinkWnd:ShowNotify(Sign)
	local function DrunkNotify(GameWnd, uButton)
		return true
	end
	
	if Sign then
		MessageBox(g_GameMain, GetStaticTextClient(9506), MB_BtnOK, DrunkNotify, g_GameMain)
	else
	    if g_GameMain.m_NPCShopSell.m_LiuTongShopTipFlag == LiuTongBiTipFlag then
            local wnd = CNpcShopMsgWnd:new()
            wnd:ShowWnd(true)
            g_GameMain.m_DrinkWnd.m_LiuTongTip = 1 --使用流通金币
        else
            self:OnDrink()        
        end
	end
end

function Gas2Gac:GetTiredNess(Conn, tirednessValue)
	g_GameMain.m_DrinkWnd:ShowIntroduction(tirednessValue)
end