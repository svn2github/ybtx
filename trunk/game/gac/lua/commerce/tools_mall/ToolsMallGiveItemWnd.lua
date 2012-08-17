CToolsMallGiveFriendsListWnd = class ( SQRDialog )
CToolsMallGiveItemWnd = class ( CToolsMallCommonWnd )
local c_money = CMoney:new()

local function CreateToolsMallGiveItemWnd(wnd)
    wnd:CreateFromRes("ToolsMallGiveWnd", g_GameMain)
    wnd:ShowWnd( false )
    wnd.m_SenderList = CToolsMallGiveFriendsListWnd:new()
	wnd.m_SenderList:CreateFromRes("senderList",wnd)
    wnd:InitChildWnd()
    g_ExcludeWndMgr:InitExcludeWnd(wnd, "非互斥")
end

function CToolsMallGiveItemWnd:Ctor()
      CreateToolsMallGiveItemWnd(self)
      self.m_CurChoosedPayType = 1 --默认选择佣兵币
end

function CToolsMallGiveItemWnd:InitChildWnd()
    self.m_CloseBtn     	= self:GetDlgChild("Close")
    self.m_ItemIcon     	= self:GetDlgChild("ItemIcon")
    self.m_ItemName     	= self:GetDlgChild("Name")
    self.m_EditLeftMsg 		= self:GetDlgChild("EditLeftMsg")
    self.m_Receiver     	= self:GetDlgChild("Receiver")
    self.m_YongBingBi   	= self:GetDlgChild("YongBingBi")
    self.m_YongBingQuan 	= self:GetDlgChild("BDYongBingBi")
    self.m_JiFen        	= self:GetDlgChild("JiFen")
    self.m_YongBingBiBtn 	= self:GetDlgChild("check1")
	self.m_YongBingQuanBtn 	= self:GetDlgChild("check2")
	self.m_JiFenBtn 		= self:GetDlgChild("check3")
	self.m_PayerName        = self:GetDlgChild("PayerName")
	self.m_PayWay			= self:GetDlgChild("PayWay")
	self.m_text				= self:GetDlgChild("text")
	self.m_BuyAccount   	= self:GetDlgChild("Input")
	self.m_SubBtn 			= self:GetDlgChild("SubBtn")
	self.m_AddBtn 			= self:GetDlgChild("AddBtn")
	self.m_ConfirmBtn 		= self:GetDlgChild("ConfirmBtn")
	self.m_CancelBtn 		= self:GetDlgChild("CancelBtn")
	self.m_TotalPrice		= self:GetDlgChild("TotalPrice")
	self.m_TotalPriceText 	= self:GetDlgChild("text1")
	self.m_FriendsNameList 	= self:GetDlgChild("FriendsNameList")
	self.m_MoneyChkBtnTbl 	= {self.m_YongBingBiBtn, self.m_YongBingQuanBtn, self.m_JiFenBtn}
	self:SetStyle(0x00040000)
	self.m_BuyAccount:SetWndText(1)
	self.m_MaxNum = 100     
	self.m_JiFenBtn:ShowWnd(false)
	self.m_JiFen :ShowWnd(false)
end


function CToolsMallGiveItemWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
    if uMsgID == BUTTON_LCLICK then
    	local toolsMallWnd = g_GameMain.m_ToolsMallWnd
   
        if (Child == self.m_YongBingBiBtn) then
            self.m_CurChoosedPayType = CToolsMallCommonWnd.PayType["YongBingBi"]    
            self:SetBuyItemPriceInfo(c_money:ChangeMoneyToString(self.m_Price,EGoldType.GoldBar))
			
        elseif (Child == self.m_YongBingQuanBtn) then
            self.m_CurChoosedPayType = CToolsMallCommonWnd.PayType["YongBingQuan"]
            self:SetBuyItemPriceInfo(c_money:ChangeMoneyToString(self.m_RMBPrice,EGoldType.GoldCoin))
            
        elseif	(Child == self.m_FriendsNameList) then
        	if	self.m_SenderList:IsShow() then
	        	self.m_SenderList:ShowWnd(false)
	        else
	        	self.m_SenderList:ShowWnd(true)
	        	self.m_SenderList:DrawSenderList()
       		end
       		
        elseif (Child == self.m_JiFenBtn) then
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
    			amount = self.m_Max
    		end
    		self:UpdateBuyItemTotalPrice(amount)
    		
       elseif (Child == self.m_ConfirmBtn) then
			local payerName = self.m_PayerName:GetWndText()
            local leftMsgToPayer = self.m_EditLeftMsg:GetWndText()
        	local buyCount =  self.m_BuyAccount:GetWndText()
			local buyCountNum = tonumber(buyCount)
			if buyCountNum == nil or  buyCountNum == 0 then
				buyCountNum = 1
			end
			if self:CheckMoneyEnoughOrNot(self.m_CurChoosedPayType, toolsMallWnd.m_ChoosedItemType, toolsMallWnd.m_ChoosedItemName) then
		 		Gac2Gas:GiveItemInToolsMall(g_Conn, toolsMallWnd.m_ChoosedItemType, toolsMallWnd.m_ChoosedItemName, buyCountNum, MoneyTypeTbl[self.m_CurChoosedPayType], payerName, leftMsgToPayer) 
			end
            self:ShowWnd(false)        	
       	end
       	
      elseif (uMsgID == WND_NOTIFY ) then
      	local wnd = g_GameMain.m_ToolsMallWnd
		if (WM_IME_CHAR == uParam1 or WM_CHAR == uParam1) then
		    if Child == self.m_BuyAccount then
		        local inputCount = self:GetInputCount()
		        self:UpdateBuyItemTotalPrice(inputCount)
		    end
		end
    end
end
--赠送给他人或者要求他人付款;
--参数：1.支付方式（1表示赠送他人，2表示要求他人付款）；2.赠送时为true，要求他人时为false
function CToolsMallGiveItemWnd:GiveItemOrRequestOthersFunc(itemType, itemName)	
    self.m_EditLeftMsg:SetWndText("")
    self.m_PayerName:SetWndText("")
    self:SetBuyOrGiveItemInfo(itemType, itemName)
end


--初始化最近联系人列表
function CToolsMallGiveFriendsListWnd:DrawSenderList()
	self:GetDlgChild( "emailsenderlist" ):DeleteAllItem()
	local strpath = g_RootPath .. "var/gac/EmailSenderList.txt"
	local data = ""
	local f = CLuaIO_Open(strpath, "rb")
	if f then
		data = f:ReadAll()
		if(data == "") then
		else
			self:GetDlgChild("emailsenderlist"):InsertColumn(0,self:GetDlgChild("emailsenderlist"):GetWndWidth())
			while(true) do
				if(data == "") then 
					break
				end
				local pos1 = string.find(data, " ")
				local pos2 = string.find(data, "\n")
				if(pos1 == nil or pos2 == nil) then
					break
				end
				if(string.sub(data,1,pos1-2) == "")then
					data = string.sub(data,pos2+1,string.len(data))
					break;
				end
				if(string.sub(data,pos1+1,pos2-2) == g_MainPlayer.m_Properties:GetCharName())then
					local itemCount = self:GetDlgChild("emailsenderlist"):GetItemCount()
					self:GetDlgChild("emailsenderlist"):InsertItem( itemCount, 20)
					self:GetDlgChild("emailsenderlist"):GetSubItem( itemCount, 0 ):SetWndText(string.sub(data,1,pos1-1))
				end
				data = string.sub(data,pos2+1,string.len(data))
			end
		end
		f:Close()
	else
		local fo = CLuaIO_Open(strpath, "w+")
		if fo then
			local data = fo:WriteString("")
			fo:Close()
		end
	end
end
--好友列表窗口事件响应函数
function CToolsMallGiveFriendsListWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	if(uMsgID == ITEM_LBUTTONCLICK)then
		local index = self:GetDlgChild( "emailsenderlist" ):GetSelectItem( -1 )
		local wnd = self:GetDlgChild( "emailsenderlist" ):GetSubItem(index,0)
		db()
		g_GameMain.m_ToolsMallGiveItemWnd.m_PayerName:SetWndText(wnd:GetWndText())
		g_GameMain.m_ToolsMallGiveItemWnd.m_SenderList:ShowWnd(false)
	end
end