CToolsMallBuyWnd = class ( CToolsMallCommonWnd )
local MoneyTypeTbl = {}
MoneyTypeTbl[1] = 2 --绑定金
MoneyTypeTbl[2] = 1 --流通金
MoneyTypeTbl[3] = 3 --积分

local function CreatToolsMallBuyWnd(wnd)
	wnd:CreateFromRes("ToolsMallBuyWnd", g_GameMain)
	wnd:InitBuyWnd()
    g_ExcludeWndMgr:InitExcludeWnd(wnd, "非互斥")
end

function CToolsMallBuyWnd:Ctor()
    CreatToolsMallBuyWnd(self)
    self.m_CurChoosedPayType = 1 --默认选择佣兵币
end
	
function CToolsMallBuyWnd:InitBuyWnd()
	self.m_ItemIcon 		= self:GetDlgChild("ObjImage")
	self.m_ItemName 		= self:GetDlgChild("ObjName")
	self.m_PayWay 			= self:GetDlgChild("PayWay")
	self.m_YongBingBiBtn 	= self:GetDlgChild("check1")
	self.m_YongBingQuanBtn 	= self:GetDlgChild("check2")
	self.m_JiFenBtn 		= self:GetDlgChild("check3")
	self.m_YongBingBi 		= self:GetDlgChild("text1")
	self.m_YongBingQuan 	= self:GetDlgChild("text2")
	self.m_text3 			= self:GetDlgChild("text3")
	self.m_text4 			= self:GetDlgChild("text4")
	self.m_BuyAccount 		= self:GetDlgChild("BuyAccount")
	self.m_SubBtn 			= self:GetDlgChild("SubBtn")
	self.m_AddBtn 			= self:GetDlgChild("AddBtn")
	self.m_TotalPrice		= self:GetDlgChild("TotalPrice")
	self.m_ConfirmBtn 		= self:GetDlgChild("ConfirmBtn")
	self.m_CancelBtn 		= self:GetDlgChild("CancelBtn")
	self.m_CloseBtn 		= self:GetDlgChild("CloseBtn")
	self:SetStyle(0x00040000)
	self.m_BuyAccount:SetWndText(1)
	self.m_MaxNum = 100
	self.m_MoneyChkBtnTbl 	= {self.m_YongBingBiBtn, self.m_YongBingQuanBtn, self.m_JiFenBtn}
	self.m_JiFenBtn:ShowWnd(false)
	self.m_text3:ShowWnd(false)
end

function CToolsMallBuyWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	 if uMsgID == BUTTON_LCLICK then
        local toolsMallWnd = g_GameMain.m_ToolsMallWnd
	 	
        if (Child == self.m_YongBingBiBtn) then
            self.m_CurChoosedPayType = CToolsMallCommonWnd.PayType["YongBingBi"]    
            self:SetBuyItemPriceInfo(self.m_Price, 8319)
			
        elseif (Child == self.m_YongBingQuanBtn) then
            self.m_CurChoosedPayType = CToolsMallCommonWnd.PayType["YongBingQuan"]
            self:SetBuyItemPriceInfo(self.m_RMBPrice, 8320)
        
        elseif (Child == self.m_JiFenBtn) then --积分价格部分还未完成
            self.m_CurChoosedPayType = CToolsMallCommonWnd.PayType["JiFen"] 
            self:SetChoosedPayTypeBtnState()
            
        elseif (Child == self.m_CloseBtn) then
        	self:ShowWnd(false)
        	
        elseif (Child == self.m_CancelBtn) then
        	self:ShowWnd(false)
        
        elseif (Child == self.m_SubBtn) then
            local amount = tonumber(self.m_BuyAccount:GetWndText()) or 1 
        	if amount > 1 then
	        	amount = amount - 1
        	else
        		amount = 1
        	end
            self:UpdateBuyItemTotalPrice(amount)

        elseif (Child == self.m_AddBtn) then
            local amount = tonumber(self.m_BuyAccount:GetWndText()) or 1 
        	if amount < self.m_MaxNum then
        	    amount = amount + 1
        	else
        	    amount = self.m_MaxNum
            end 
            self:UpdateBuyItemTotalPrice(amount)

        elseif (Child == self.m_ConfirmBtn) then
        	local buyCount =  self.m_BuyAccount:GetWndText()
			local buyCountNum = tonumber(buyCount)
            if buyCountNum == nil or  buyCountNum == 0  then
                buyCountNum = 1
            end
            if self:CheckMoneyEnoughOrNot(self.m_CurChoosedPayType, toolsMallWnd.m_ChoosedItemType, toolsMallWnd.m_ChoosedItemName) then
                Gac2Gas:BuyItemInToolsMall(g_Conn, toolsMallWnd.m_ChoosedItemType, toolsMallWnd.m_ChoosedItemName, buyCountNum, MoneyTypeTbl[self.m_CurChoosedPayType])  
            end
            self:ShowWnd(false)        
        end
      
      elseif (uMsgID == WND_NOTIFY ) then
		if (WM_IME_CHAR == uParam1 or WM_CHAR == uParam1) then
		    if Child == self.m_BuyAccount then
                local  count = self:GetInputCount()
                self:UpdateBuyItemTotalPrice(count)
            end
		end
    end
end
