
gac_gas_require "framework/common/CMoney"
gac_gas_require "item/SoulpearlCommonFunc"

CNPCShopPlayerSoldWnd = class ( SQRDialog )
local NotWithOneCommonShop = 0--非随身商店
---------------------------------------------------------------------------------------------RPC-----------------------------------------------------------------------------------------------
--创建npc商店购买物品窗口
local function CreateNPCShopPlayerSold(Wnd)
	Wnd:CreateFromRes("NpcShopBuyBack",g_GameMain)

	g_ExcludeWndMgr:InitExcludeWnd(Wnd, 1)
	Wnd:InitNPCShopPlayerSoldWndChild()
	return Wnd
end

function CNPCShopPlayerSoldWnd:Ctor()
    CreateNPCShopPlayerSold(self)
end

--初始化npc商店购回界面窗口中的子控件
function CNPCShopPlayerSoldWnd:InitNPCShopPlayerSoldWndChild()
	self.m_Sell				= self:GetDlgChild("Buy")
	self.m_Close			= self:GetDlgChild("Close")
	self.m_ReBuyAllBtn		= self:GetDlgChild("BuyBackAll")
	self.m_BindJin			= self:GetDlgChild("JinCount")
	self.m_BindYin			= self:GetDlgChild("YinCount")
	self.m_BindTong			= self:GetDlgChild("TongCount")
	self.m_LiuTongJin		= self:GetDlgChild("GoldCount")
	self.m_LiuTongYin		= self:GetDlgChild("SliverCount")
	self.m_LiuTongTong		= self:GetDlgChild("CopperCount")
	self.m_BindJinBtn		= self:GetDlgChild("Jin")
	self.m_BindYinBtn		= self:GetDlgChild("Yin")
	self.m_BindTongBtn		= self:GetDlgChild("Tong")
	self.m_LiuTongJinBtn	= self:GetDlgChild("Gold")
	self.m_LiuTongYinBtn	= self:GetDlgChild("Sliver")
	self.m_LiuTongTongBtn	= self:GetDlgChild("Copper")
	
	self.m_tblGoodsWnd			= {}
	self.m_tblGoodsNameDisWnd	= {}
	self.m_tblGoodsPriceWnd		= {}
	for i = 1, 12 do
		local strGoodsRes				= string.format("Goods_%02d", i)
		local strGoodsNameDisRes		= string.format("GoodsNameDis_%02d", i)
		local strGoodsPriceRes			= string.format("GoodsPrice_%02d", i)
		self.m_tblGoodsWnd[i]			= self:GetDlgChild(strGoodsRes)
		self.m_tblGoodsNameDisWnd[i]	= self:GetDlgChild(strGoodsNameDisRes)
		self.m_tblGoodsPriceWnd[i]		= self:GetDlgChild(strGoodsPriceRes)
	end
	--self:SetMouseWheel(true)
end
--从服务器返回玩家角色已卖出的物品，将其显示在购买窗口
function CNPCShopPlayerSoldWnd:ShowNPCShopPlayerSoldWnd(GoodsType,GoodsName,GoodsCount,nViewIndex,GoodsID,GoodsPosIndex)
	local SmallIcon		= g_ItemInfoMgr:GetItemInfo(GoodsType,GoodsName,"SmallIcon")
	local goodsPrice	= string.format("GoodsPrice_%02d", nViewIndex)
	local iconWnd		= self.m_tblGoodsWnd[nViewIndex]
	self.m_tblGoodsWnd[nViewIndex].GoodsPosIndex = GoodsPosIndex
	g_LoadIconFromRes(SmallIcon, iconWnd, -1, IP_ENABLE, IP_CLICKDOWN)
	g_SetWndMultiToolTips(iconWnd,GoodsType,GoodsName,GoodsID,GoodsCount)
	if GoodsCount > 1 then 
		iconWnd:SetWndText(" " .. GoodsCount)
	end
	local c_money = CMoney:new()
	local moneyStr = ""

	if g_ItemInfoMgr:IsSoulPearl(GoodsType) then
	    local item = g_DynItemInfoMgr:GetDynItemInfo(GoodsID)
	    local soulCount = item.nSoulNum
	    moneyStr = GetSoulpearlPriceInfo(GoodsName,soulCount, g_GameMain.m_NPCShopSell.m_PayType, 1/4)
	else
	    moneyStr = g_Tooltips:GetItemPriceByPayType(GoodsType, GoodsName, false)
    end
	local DisplayName = g_ItemInfoMgr:GetItemLanInfo(GoodsType,GoodsName,"DisplayName")
	self.m_tblGoodsNameDisWnd[nViewIndex]:SetWndText(DisplayName)
	self.m_tblGoodsPriceWnd[nViewIndex]:SetWndText(moneyStr)
end

function CNPCShopPlayerSoldWnd:VirtualExcludeWndClosed()
	if g_GameMain.m_NPCShopSell:IsShow() == false then
	    g_GameMain.m_NPCShopSell.m_ShopOpenFlag =false
		g_GameMain.m_WndMainBag.m_ctItemRoom:UpdateBagTips(false, nil)
		g_GameMain.m_RoleStatus:UpdataEquipTips()
	end
end

function CNPCShopPlayerSoldWnd:OnLButtonClick( nFlags,  uParam1, uParam2 )
	self:OnLButtonUp( nFlags,  uParam1, uParam2 )
end

--左键点击NPC商店，手上有物品直接卖掉
function CNPCShopPlayerSoldWnd:OnLButtonUp( nFlags,  uParam1, uParam2 )
	local state, context = g_WndMouse:GetCursorState()
	if state == ECursorState.eCS_PickupItem then --手上有物品
		g_GameMain.m_NPCShopSell:SellItem( context )
	end
end

--NPC商店购买窗口事件响应
function CNPCShopPlayerSoldWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	local state, context = g_WndMouse:GetCursorState()
	
	if uMsgID == BUTTON_LCLICK then 
		if Child == g_GameMain.m_NPCShopPlayerSold.m_Sell then
			local wnd = g_GameMain.m_NPCShopSell
			wnd:ShowWnd(true)
			if NpcShop_Common(wnd.npcName, "ShopType") == NotWithOneCommonShop then
			    local AutoCloseWnd = CAutoCloseWnd:new()
			    AutoCloseWnd:AutoCloseWnd(wnd)
			end
			g_GameMain.m_NPCShopPlayerSold:ShowWnd(false)
			if wnd.m_PayType ~= 0  then
				wnd:ShowJiFenCountInShopOrNot(true)
				wnd.m_JiFenChkBtnTbl[wnd.m_PayType]:SetCheck(true)
				wnd.m_JiFenChkBtnTbl[wnd.m_PayType]:EnableWnd(true)
			end
			
		elseif Child == g_GameMain.m_NPCShopPlayerSold.m_Close then
			g_GameMain.m_NPCShopPlayerSold:ShowWnd(false)

		elseif Child == g_GameMain.m_NPCShopPlayerSold.m_ReBuyAllBtn then
			local npcName = g_GameMain.m_NPCShopSell.npcName
			if g_MainPlayer.m_ItemBagLock then
				MsgClient(160018)
				return 
			end
			Gac2Gas:BuyBackAllItem( g_Conn, npcName )

		elseif state == ECursorState.eCS_PickupItem then --手上有物品
			g_GameMain.m_NPCShopSell:SellItem( context )
		end
	elseif uMsgID == BUTTON_RIGHTCLICKDOWN then --手上没有物品,--右键点击售出的物品，直接取回
		if  state ~= ECursorState.eCS_Normal then --手上有物品 
			g_WndMouse:ClearCursorAll()
		else
			g_GameMain.m_NPCShopPlayerSold:GetClickWndItem(Child)
		end
	end
end

function CNPCShopPlayerSoldWnd:GetClickWndItem(itemWnd)
	local itemWndChoosed = 0
	for i = 1, #(self.m_tblGoodsWnd) do
		if(itemWnd == self.m_tblGoodsWnd[i]) then
			itemWndChoosed = i
		end
	end
	local tbl = g_GameMain.m_PlayerSoldGoodsList
	local tblLength = table.maxn(tbl)
	if itemWndChoosed > 0 and itemWndChoosed <= tblLength then
		local npcName = g_GameMain.m_NPCShopSell.npcName
		if g_MainPlayer.m_ItemBagLock then
			MsgClient(160018)
			return 
		end 
		Gac2Gas:BuyBackItem( g_Conn, npcName, self.m_tblGoodsWnd[itemWndChoosed].GoodsPosIndex )
	end
end

--清除图标
function CNPCShopPlayerSoldWnd:ClearWndInfo()
	for i = 1, 12 do
		local goodsWnd = self.m_tblGoodsWnd[i]
		self.m_tblGoodsNameDisWnd[i]:SetWndText("")
		self.m_tblGoodsPriceWnd[i]:SetWndText("")
		goodsWnd:SetWndText("")
		goodsWnd:SetMouseOverDescAfter("")
		g_DelWndImage(goodsWnd, 1, IP_ENABLE, IP_CLICKDOWN)
		g_WndMouse:SetWndCursorSkillState(goodsWnd, ECursorSkillState.eCSS_Normal)
	end
end

------------------------脱穿装备时，对物品tooltip进行更新，保证装备的tooltip比较信息正确---------
function CNPCShopPlayerSoldWnd:UpdateNpcSoldItemTooltip()
    local soldListTbl = g_GameMain.m_PlayerSoldGoodsList
    for i =1, g_GameMain.NPCShopPlayerSoldCount do
        local iconWnd = self.m_tblGoodsWnd[i]
        local GoodsType = soldListTbl[i].GoodsType
        local GoodsName = soldListTbl[i].GoodsName
        local GoodsCount = soldListTbl[i].GoodsCount
        local GoodsID = soldListTbl[i].GoodsID
        g_SetWndMultiToolTips(iconWnd,GoodsType,GoodsName,GoodsID,GoodsCount)     
    end
end