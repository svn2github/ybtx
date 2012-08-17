gac_require "relation/cofc/CofCStockMyDealingInc"

function Gas2Gac:RetGetCofcMyHaveInfo(Conn,myHave,costs)
	g_GameMain.m_CofCStockMyDealing:ShowUpdataMyDealingInfo(myHave,costs)
end


-------------------------------------RPC--------------------------------------------

--创建我的交易窗口
function CreateCofCMyDealingWnd(Parent)
	local wnd = CofCStockMyDealingWnd:new()
	wnd:CreateFromRes("CofCStockDealing", Parent)
	wnd:ShowWnd( true )
	g_ExcludeWndMgr:InitExcludeWnd(wnd, 1)
	wnd:InitCofCStockMyDealingChild()
	wnd:SetStyle(0x40040000)
	return wnd
end

--初始化商会股票中我的交易窗口中的子控件
function CofCStockMyDealingWnd:InitCofCStockMyDealingChild()
	self.m_CloseBtn			=	self:GetDlgChild("Close")
	self.m_NameText			=	self:GetDlgChild("NameText")					--股票名称
	self.m_NoText			=	self:GetDlgChild("NoText")						--股票代号
	self.m_BuyStock 		=	self:GetDlgChild("BuyStock")					--买入股票
	self.m_SellStock		=	self:GetDlgChild("SellStock")					--售出股票
	self.m_CurStockCount	=	self:GetDlgChild("CurStockCount")				--当前总卖单
	self.m_CurSellPrice		=	self:GetDlgChild("CurSellPrice")				--当前最低出售价
	self.m_Text				=	self:GetDlgChild("Text")						--目前申购股数相关详细信息
	self.m_MyHave			=	self:GetDlgChild("HaveText")					--执有股数
	self.m_CostsText		=	self:GetDlgChild("CostsText")					--成本
	self.m_OkBtn			=	self:GetDlgChild("Ok")							--确定按钮
	self.m_UpdateBtn		=	self:GetDlgChild("Update")						--刷新按钮
	self.m_Cancel			=	self:GetDlgChild("Cancel")						--取消按钮
	self.m_InputCountText	=	self:GetDlgChild("InputCountText")				--输入要交易的数量
	self.m_InputPriceText	=	self:GetDlgChild("InputPriceText")				--输入要交易的价格	
end

--我的交易窗口事件响应函数
function CofCStockMyDealingWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	if (uMsgID == BUTTON_LCLICK) then
	  	if(Child == self.m_CloseBtn or Child == self.m_Cancel) then 				--关闭按钮
	  		self:ShowWnd(false)
	  		g_GameMain.m_CofCStockOrderListWnd:ShowWnd(false)
	  		g_GameMain.m_CofCStockOrderListWnd.UpdateCofCStockMyDealingInfoCallBack = nil	--将判断等待刷新我的交易面板的窗口函数置为nil，因为一旦关闭我的交易面板窗口，这个函数就不需要了
	  	
	  	elseif(Child == self.m_BuyStock)    	then 			--买入股票按钮，显示相关买入股票信息
			if self.m_CurDealingType  == 1 then
				return
			end
			self.m_CurDealingType 	=	1
			local wnd = g_GameMain.m_CofCStockOrderListWnd
			if table.getn(wnd.m_SellOrderlistTbl) > 0 then
				self:ShowBestOrderlistInfoWnd(true)
				self:ShowCofCStockDealingInfo(wnd.m_TotalSellCount,wnd.m_SellOrderlistTbl[1].Price,wnd.m_SellOrderlistTbl[1].Count)
			else
				self:ShowBestOrderlistInfoWnd(false)
			end
		
	  	elseif(Child == self.m_SellStock)      	then			--卖出股票按钮，显示相关卖出股票信息
			if self.m_CurDealingType  == 2 then					--如果已经在当前页面，则点了该按钮没有响应
				return
			end
			self.m_CurDealingType 	=	2
			local wnd = g_GameMain.m_CofCStockOrderListWnd
			if table.getn(wnd.m_BuyOrderlistTbl) > 0 then		--有挂单的时候，显示挂单中最合适的股票价格和数目
				self:ShowBestOrderlistInfoWnd(true)
				self:ShowCofCStockDealingInfo(wnd.m_TotalBuyCount,wnd.m_BuyOrderlistTbl[1].Price,wnd.m_BuyOrderlistTbl[1].Count)
			else												--没有任何挂单的情况下，我的买卖面板显示的股价和数目是股票本身的
				self:ShowBestOrderlistInfoWnd(false)
			end
			
	  	elseif(Child == self.m_OkBtn)       	then			--确定按钮
	  		local stockCount = self.m_InputCountText:GetWndText()
	  		if stockCount == nil or stockCount == "" then
	  			self.m_MsgBox = MessageBox(self, MsgBoxMsg(16006), MB_BtnOK )
	  			return 
	  		end
	  		local stockPrice = self.m_InputPriceText:GetWndText()
	  		if stockPrice == nil or stockPrice == "" then
	  			self.m_MsgBox = MessageBox(self, MsgBoxMsg(16007), MB_BtnOK)
	  			return
	  		end
	  		self:ShowWnd(false)
	  		g_GameMain.m_CofCStockOrderListWnd:ShowWnd(false)
	  		g_GameMain.m_CofCStockOrderListWnd.UpdateCofCStockMyDealingInfoCallBack = nil
	  		
	  		if self.m_CurDealingType == 1 then
	  			Gac2Gas:CofcStockBuy(g_Conn, g_GameMain.m_CofCStockOrderListWnd.m_CurStockId, tonumber(stockCount), tonumber(stockPrice))	
	  		elseif self.m_CurDealingType == 2 then
	  			Gac2Gas:CofcStockSell(g_Conn, g_GameMain.m_CofCStockOrderListWnd.m_CurStockId, tonumber(stockCount), tonumber(stockPrice))	
	  		end
	  	
	  	elseif(Child == self.m_UpdateBtn)       then		--刷新按钮
	  		Gac2Gas:GetCofcMyHaveInfo(g_Conn, g_GameMain.m_CofCStockOrderListWnd.m_CurStockId)
	  		Gac2Gas:GetCofCStockOrderList(g_Conn, g_GameMain.m_CofCStockOrderListWnd.m_CurStockId)
		end				
	
	elseif (uMsgID == WND_NOTIFY ) then							--如果交易数量输入的是非数字的东西，直接清空
		if (WM_IME_CHAR == uParam1 or WM_CHAR == uParam1) then
			local count = self.m_InputCountText:GetWndText()
			local price = self.m_InputPriceText:GetWndText()
			if Child == self.m_InputCountText  then
				if count ~= nil and IsNumber(tonumber(count)) == false then
					self.m_InputCountText:SetWndText("")
					return 
				end
			end	
			if Child == self.m_InputPriceText  then				--如果交易价格输入的是非数字的东西，直接清空
				if price ~= nil and IsNumber(tonumber(price)) == false then
					self.m_InputPriceText:SetWndText("")
					return
				end
			end
			if count ~= nil and count ~= "" and price ~= nil and price ~= "" then			--当输入的交易股票数量和数目均不为空时，则显示交易提示信息
				self:ShowDetailInfo()
			end
		end	
	end
end

--显示股票当前最新信息
function CofCStockMyDealingWnd:ShowCofCStockDealingInfo(stockCount,price,priceStockCount)
	self.m_InputCountText:SetWndText("")
	self.m_InputPriceText:SetWndText("")
	self.m_Text:SetWndText("")
	--如果没有股票信息，则默认初始为0
	if stockCount == nil then
		stockCount = 0
	end
	if price == nil then
		price = 0 
	end
	if priceStockCount == nil then
		priceStockCount = 0
	end
	if self.m_CurDealingType == 1 then
		self.m_CurStockCount:SetWndText("当前总卖单：" .. stockCount .. "股")
		self.m_CurSellPrice:SetWndText("当前最低出售价："  .. price .. "铜/" .. priceStockCount .. "股")
	elseif self.m_CurDealingType == 2 then
		self.m_CurStockCount:SetWndText("当前总买单：" .. stockCount .. "股")
		self.m_CurSellPrice:SetWndText("当前最高出售价："  .. price .. "铜/" .. priceStockCount .. "股")
	end
	local tblIndex = g_GameMain.m_CofCMainStockWnd:GetShowIndexByStockId( g_GameMain.m_CofCStockOrderListWnd.m_CurStockId,g_GameMain.m_CofCMainStockWnd.m_MyStockInfoTbl)	
	if tblIndex ~= nil then
		local myHave, costs = g_GameMain.m_CofCMainStockWnd.m_MyStockInfoTbl[tblIndex].HaveCount,g_GameMain.m_CofCMainStockWnd.m_MyStockInfoTbl[tblIndex].Costs
		self:ShowUpdataMyDealingInfo(myHave,costs)
	end
end

--当输入的股票购买或出售数量和价格都不为空时，显示相关的买卖信息
function CofCStockMyDealingWnd:ShowDetailInfo()
	if self.m_CurDealingType == 1 then
		self.m_Text:SetWndText("您当前申购" .. self.m_InputCountText:GetWndText() .. "股".. "在最高收购价为" .. self.m_InputPriceText:GetWndText() .. "时，可以直接购买而无需挂单。")
	elseif self.m_CurDealingType == 2 then
		self.m_Text:SetWndText("您当前出售" .. self.m_InputCountText:GetWndText() .. "股".. "在最低出售价为" .. self.m_InputPriceText:GetWndText() .. "时，可以直接出售而无需挂单。")
	end
end

function CofCStockMyDealingWnd:ShowUpdataMyDealingInfo(myHave,costs)
	self.m_MyHave:SetWndText(myHave .. "股")
	self.m_CostsText:SetWndText(costs)
end

--如果要买如或卖出股票，但是股市中没有该只股票的卖单或买单，则我的股票交易窗口中的最合适价钱和数目窗口不显示
function CofCStockMyDealingWnd:ShowBestOrderlistInfoWnd(showOrNot)
	self.m_CurStockCount:ShowWnd(showOrNot)
	self.m_CurSellPrice:ShowWnd(showOrNot)
end