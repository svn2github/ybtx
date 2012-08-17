CToolsMallCommonWnd = class (SQRDialog)



CToolsMallCommonWnd.PayType = {}
CToolsMallCommonWnd.PayType["YongBingBi"] = 1
CToolsMallCommonWnd.PayType["YongBingQuan"] = 2
CToolsMallCommonWnd.PayType["JiFen"] = 3

function  CToolsMallCommonWnd:Ctor()
    
end

--输入数量时，更改购买界面里所需总价
function CToolsMallCommonWnd:UpdateBuyItemTotalPrice(count)
	self.m_BuyAccount:SetWndText(count)
	if CToolsMallCommonWnd.PayType["YongBingBi"] == self.m_CurChoosedPayType then
        self:SetBuyItemPriceInfo(self.m_Price, EGoldType.GoldBar)
	
	elseif CToolsMallCommonWnd.PayType["YongBingQuan"] == self.m_CurChoosedPayType then
	    self:SetBuyItemPriceInfo(self.m_RMBPrice, EGoldType.GoldCoin)
	
	elseif CToolsMallCommonWnd.PayType["JiFen"] == self.m_CurChoosedPayType then
        	
	end
end

--检验输入的物品数量是否合法
function CToolsMallCommonWnd:GetInputCount(inputWnd, maxNum)
    local wnd  = inputWnd
    if inputWnd == nil then
        wnd = self.m_BuyAccount
    end
	local inputStr = wnd:GetWndText()
	local findResult = string.find(inputStr, "[^0123456789]")
	if  findResult ~= nil  then
		inputStr = string.sub(inputStr, 1, findResult-1)
		wnd:SetWndText( inputStr )
	end

	local input = tonumber(inputStr)
    if input == nil or input == 0 then
        input = 1
    end
    local max = maxNum
    if max == nil then
        max = self.m_MaxNum
    end
    if input > max then
        input = max
    end
    return input 
end


function CToolsMallCommonWnd:SetChoosedPayTypeBtnState()
    local curChoosedPayType = self.m_CurChoosedPayType
    for i, v in pairs (self.m_MoneyChkBtnTbl) do
        local payType = i
        local payBtn  = v
        if payType == curChoosedPayType then
            payBtn:SetCheck(true)
        else
            payBtn:SetCheck(false) 
        end
    end
end


--根据当前选择的支付方式和购买数量，显示物品的总价
function CToolsMallCommonWnd:SetBuyItemPriceInfo(price, priceDescType)
    self:SetChoosedPayTypeBtnState()
    local c_money = CMoney:new()
    local buyCount =  self.m_BuyAccount:GetWndText()
    local TotalPrice =  price * buyCount
    self.m_TotalPrice:SetWndText(c_money:ChangeMoneyToString(TotalPrice,priceDescType))
end


--检测玩家当前选中的支付方式是否有足够的金钱支付
function CToolsMallCommonWnd:CheckMoneyEnoughOrNot(payType, itemType, itemName)
    local isMoneyEnough = true
    local msgNo = nil
    local itemPrice = 0
    if payType == CToolsMallCommonWnd.PayType["YongBingBi"] then
        itemPrice = ToolsMallItemPrice_Common(tostring(itemType), itemName, "BindingPrice")            
		if  itemPrice > g_MainPlayer.m_nBindMoney then
            isMoneyEnough = false
            msgNo = 300
        end
    
    elseif payType == CToolsMallCommonWnd.PayType["YongBingQuan"] then
        itemPrice =  g_ItemInfoMgr:GetItemInfo(itemType, itemName, "Price")    
        if itemPrice > g_MainPlayer.m_nMoney then
            isMoneyEnough = false 
            msgNo = 301
        end
         
    elseif payTyep == CToolsMallCommonWnd.PayType["JiFen"] then
        
    end    
    if isMoneyEnough == false then
        MessageBox(g_GameMain, MsgBoxMsg(msgNo), MB_BtnOK)
        return isMoneyEnough
    end
    if itemPrice == 0 then
       MessageBox(g_GameMain, MsgBoxMsg(19001), MB_BtnOK)
       return false 
    end
    return isMoneyEnough
end

function CToolsMallCommonWnd:SetChoosedPayType()
    local payType 
    if 	self.m_CurChoosedPayType == CToolsMallCommonWnd.PayType["YongBingBi"] and 
        self.m_Price == 0 then
        self.m_CurChoosedPayType = CToolsMallCommonWnd.PayType["YongBingQuan"]
        payType = EGoldType.GoldCoin
    end
    if self.m_CurChoosedPayType == CToolsMallCommonWnd.PayType["YongBingQuan"] and
        self.m_RMBPrice == 0 then
        self.m_CurChoosedPayType = CToolsMallCommonWnd.PayType["YongBingBi"]
        payType = EGoldType.GoldBar
    end
    return payType
end
--将物品信息显示在购买界面或者赠送界面
function CToolsMallCommonWnd:SetBuyOrGiveItemInfo(nBigId, itemName)
    self:SetFocus()
    local toolsMallMainWnd = g_GameMain.m_ToolsMallWnd
    self.m_Price = ToolsMallItemPrice_Common(tostring(nBigId), itemName, "BindingPrice")    
   	self.m_RMBPrice =  g_ItemInfoMgr:GetItemInfo(nBigId, itemName, "Price")    
   	self.m_JiFenPrice = ToolsMallItemPrice_Common(tostring(nBigId), itemName, "JiFenPrice")

   	if self.m_Price == 0 then
   		self.m_YongBingBiBtn:ShowWnd(false)
   		self.m_YongBingBi:ShowWnd(false)
    else
        self.m_YongBingBiBtn:ShowWnd(true)
   		self.m_YongBingBi:ShowWnd(true)
   	end
   	if self.m_RMBPrice == 0 then
   		self.m_YongBingQuan :ShowWnd(false)
   		self.m_YongBingQuanBtn :ShowWnd(false)
    else
        self.m_YongBingQuan:ShowWnd(true)
   		self.m_YongBingQuanBtn:ShowWnd(true)
   	end
   	if self.m_JiFenPrice == 0 then
        --self.m_JiFen:ShowWnd(false)
   		--self.m_JiFenBtn :ShowWnd(false)
   	end
    
    
    local SmallIcon = g_ItemInfoMgr:GetItemInfo(nBigId, itemName,"SmallIcon")
    local DisplayName = g_ItemInfoMgr:GetItemLanInfo(nBigId, itemName,"DisplayName")  
    g_LoadIconFromRes(SmallIcon, self.m_ItemIcon, -1, IP_ENABLE, IP_CLICKDOWN)
    local payType = self:SetChoosedPayType()
    self.m_BuyAccount:SetWndText(1)
    self:UpdateBuyItemTotalPrice(1)
    
    self:SetChoosedPayTypeBtnState()   
    self.m_ItemName:SetWndText(DisplayName)
    local c_money = CMoney:new()
    self.m_YongBingBi:SetWndText(c_money:ChangeMoneyToString(self.m_Price,EGoldType.GoldBar))
    self.m_YongBingQuan:SetWndText(c_money:ChangeMoneyToString(self.m_RMBPrice,EGoldType.GoldCoin))    
       
    g_SetWndMultiToolTips(self.m_ItemIcon,nBigId,itemName,0,nil,true,payType)

end

function CToolsMallCommonWnd.CloseToolsMallGiveItemWnd()
    if g_GameMain.m_ToolsMallGiveItemWnd:IsShow() then
        g_GameMain.m_ToolsMallGiveItemWnd:ShowWnd(false)
    end
    if g_GameMain.m_ToolsMallWnd.m_TakeYongBingBiWnd then
       g_GameMain.m_ToolsMallWnd.m_TakeYongBingBiWnd:ShowWnd(false) 
    end
    if g_GameMain.m_ToolsMallWnd.m_YongBingBiWnd then
       g_GameMain.m_ToolsMallWnd.m_YongBingBiWnd:ShowWnd(false) 
    end
end

function CToolsMallCommonWnd.CloseToolsMallBuyItemWnd()
    if g_GameMain.m_ToolsMallBuyWnd:IsShow() then
         g_GameMain.m_ToolsMallBuyWnd:ShowWnd(false)
    end
    if g_GameMain.m_ToolsMallWnd.m_TakeYongBingBiWnd then
       g_GameMain.m_ToolsMallWnd.m_TakeYongBingBiWnd:ShowWnd(false) 
    end
    if g_GameMain.m_ToolsMallWnd.m_YongBingBiWnd then
       g_GameMain.m_ToolsMallWnd.m_YongBingBiWnd:ShowWnd(false) 
    end
end
