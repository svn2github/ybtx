CBuyBacthWnd = class ( SQRDialog )


--批量购买窗口
local function CreateBuyBatchWnd(Wnd)
	Wnd:CreateFromRes("NpcBuyItemBatch", g_GameMain)
	Wnd:ShowWnd( true )
	g_ExcludeWndMgr:InitExcludeWnd(Wnd, "非互斥")
	Wnd:InitNPCShopSellWndBatchChild()
	Wnd:SetStyle(0x00040000)
	Wnd.m_MaxNum = 100
end

function CBuyBacthWnd:Ctor()
    CreateBuyBatchWnd(self)
end

--初始化npc商店出售窗口相关的批量购买子窗口中的控件
function CBuyBacthWnd:InitNPCShopSellWndBatchChild()
	self.m_Close		=	self:GetDlgChild("BtnCancel")
	self.m_OKBtn		=	self:GetDlgChild("BtnOk")
	self.m_InputWnd		=	self:GetDlgChild("EditMoney")
	self.m_NeedMoney	=	self:GetDlgChild("NeedMoney")
end

--批量购买窗口事件响应
function CBuyBacthWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if uMsgID == BUTTON_LCLICK then
		if Child == self.m_Close then
			self:ShowWnd(false)

		elseif Child == self.m_OKBtn then
			local buyCount =  self.m_InputWnd:GetWndText()
			local buyCountNum = tonumber(buyCount)
			if buyCount == "" or  buyCountNum == 0 or IsNumber(buyCountNum) == false then
			    MessageBox(g_GameMain, MsgBoxMsg(111), MB_BtnOK)
			    return 
			end
			local m_NPCShopSellWnd = g_GameMain.m_NPCShopSell
			m_NPCShopSellWnd.batchCount = tonumber( buyCount )
			g_GameMain.m_NPCShopSell:BuyItem()
	
			self:ShowWnd(false)
		end

	elseif (uMsgID == WND_NOTIFY ) then
		if (WM_IME_CHAR == uParam1 or WM_CHAR == uParam1) then
			local inputStr = self.m_InputWnd:GetWndText()
			local findResult = string.find(inputStr, "[^0123456789]")
			if  findResult ~= nil  then
				inputStr = string.sub(inputStr, 1, findResult-1)
				self.m_InputWnd:SetWndText( inputStr )
			end

			local input = tonumber(inputStr)
			if input == nil or input == 0 then
				input = 1
				self.m_InputWnd:SetWndText( "" )
			end
			if input > self.m_MaxNum then
				self.m_InputWnd:SetWndText(tostring(self.m_MaxNum))
			end
			self:ResetTotalPrice(input)
		end
	end
end

function CBuyBacthWnd:ResetTotalPrice(input)
	local itemType, itemName = g_GameMain.m_NPCShopSell:GetChoosedItemTypeAndName( g_GameMain.m_NPCShopSell.clickedItemIndex) --获得类型，名字
	local Price = 0
	local c_money = CMoney:new()
	if g_GameMain.m_NPCShopSell.m_PayType ~= 0 then     --该商店的支付方式是积分
		local payTypeMoney = g_MainPlayer:GetJiFenCountByType(g_GameMain.m_NPCShopSell.m_PayType)
	
		if g_ItemInfoMgr:IsSoulPearl(itemType) then
		    itemName, g_GameMain.m_NPCShopSell.SoulCount =  g_ItemInfoMgr:GetSoulpearlInfo(itemName)
			Price = GetSoulpearlPriceByTypeAndCount(itemName, g_GameMain.m_NPCShopSell.SoulCount, g_GameMain.m_NPCShopSell.m_PayType)
		else
			Price = GetItemJiFenPrice(itemType, itemName, 1, g_GameMain.m_NPCShopSell.m_PayType) 
		end

		local jiFenType = g_GameMain.m_NPCShopSell.m_PayType
		
		if( input > self.m_MaxNum) then
			MoneyNumber=Price* self.m_MaxNum
		else
			MoneyNumber=Price* input
		end
		self.m_NeedMoney:SetWndText(MoneyNumber..GetJiFenTypeStr(jiFenType))
	
	else
		if g_ItemInfoMgr:IsSoulPearl(itemType) then
		    itemName, g_GameMain.m_NPCShopSell.SoulCount =  g_ItemInfoMgr:GetSoulpearlInfo(itemName)
			Price = GetSoulpearlPriceByTypeAndCount(itemName,g_GameMain.m_NPCShopSell.SoulCount, g_GameMain.m_NPCShopSell.m_PayType)		
		else
			Price = g_ItemInfoMgr:GetItemInfo(itemType, itemName,"Price")
		end			
		if( input > self.m_MaxNum) then
			MoneyNumber=Price* self.m_MaxNum
		else
			MoneyNumber=Price* input
		end
		local MoneyStr
		if(g_GameMain.m_NPCShopSell.m_LiuTongShopFlag == 1 ) then
			MoneyStr = c_money:ChangeMoneyToString( MoneyNumber,EGoldType.GoldCoin )
		else	
			MoneyStr = c_money:ChangeMoneyToString( MoneyNumber,EGoldType.GoldBar )
		end
		self.m_NeedMoney:SetWndText(MoneyStr)
	end

end
