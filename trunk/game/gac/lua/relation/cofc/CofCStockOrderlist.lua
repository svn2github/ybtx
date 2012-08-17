gac_require "relation/cofc/CofCStockOrderlistInc"
gac_require "relation/cofc/CofCStockMyDealing"
gac_require "relation/cofc/CofCStockFinancial"
gac_require "relation/cofc/CofCStockKMap"

--撤回挂单成功的话，请求刷新我的交易列表，并且关闭orderlist窗口
function Gas2Gac:RetDeleteOrder(Conn, suc)
	if suc then
		Gac2Gas:GetCofCStockMyDealingInfo(g_Conn)
		g_GameMain.m_CofCStockOrderListWnd:ShowWnd(false)
	end
end

--返回某支股票的详细信息,
function Gas2Gac:RetGetCofCStockOrderListBegin(Conn, stockId)
	g_GameMain.m_CofCStockOrderListWnd.m_SellOrderlistTbl	=	{}			--记录卖单价钱和股数的表
	g_GameMain.m_CofCStockOrderListWnd.m_BuyOrderlistTbl	=	{}			--记录买单价钱和股数的表
	g_GameMain.m_CofCStockOrderListWnd.m_CurStockId			=	stockId		--记录当前显示的股票id
end

--得到股票的订单列表，参数：类型（0为买单，1为卖单）、订单号、价格、数量
function Gas2Gac:RetGetCofCStockOrderList(Conn, type, orderlistId, price, count)
	local node			=	{}
	node.OrderlistId	=	orderlistId
	node.Price			=	price
	node.Count			=	count
	
	if type == 0 then	
		table.insert(g_GameMain.m_CofCStockOrderListWnd.m_BuyOrderlistTbl,node)
	elseif type == 1 then
		table.insert(g_GameMain.m_CofCStockOrderListWnd.m_SellOrderlistTbl,node)
	end
end

--传数据结束，开始显示在客户端
function Gas2Gac:RetGetCofCStockOrderListEnd(Conn, totalBuyCount, totalSellCount)
	local wnd = g_GameMain.m_CofCStockOrderListWnd
	wnd.m_TotalBuyCount = totalBuyCount
	wnd.m_TotalSellCount = totalSellCount
	wnd:ShowStockOrderlistWnd(totalBuyCount, totalSellCount)

	if wnd.UpdateCofCStockMyDealingInfoCallBack ~= nil then
		wnd:UpdateCofCStockMyDealingInfoCallBack()
	end
end

---------------------------------------股票详细信息显示窗口相关代码-----------------------------------------------------
--创建股票详细信息显示窗口
function CreateStockOrderlistWnd(Parent)
	local wnd = CStockOrderlistWnd:new()
	wnd:CreateFromRes("CofCStockDetails",Parent)
	wnd:ShowWnd( true )
	g_ExcludeWndMgr:InitExcludeWnd(wnd, 3, false, {g_GameMain.m_CofCMainStockWnd})
	wnd:InitCofCStockMainDetailChild()
	
	return wnd
end

--初始化股票详细信息窗口中的子控件
function CStockOrderlistWnd:InitCofCStockMainDetailChild()
	self.m_StockNoAndName	=	self:GetDlgChild("NoName")			--显示股票名称和编号
	self.m_LastStock		=	self:GetDlgChild("LastStock")		--要求显示上一只股票详细信息的按钮
	self.m_NextStock		=	self:GetDlgChild("NextStock")		--要求显示下一只股票详细信息的按钮
	self.m_SellCount		=	self:GetDlgChild("Sell")			--显示卖单总股数
	self.m_BuyCount			=	self:GetDlgChild("Buy")				--显示买单总股数
	self.m_BuyThisStock		=	self:GetDlgChild("BuyThisStock")	--点击购买此支股票
	self.m_SellThisStock	=	self:GetDlgChild("SellThisStock")	--点击出售此支股票
	self.m_CheckKMap		=	self:GetDlgChild("KMap")			--点击查看k线图
	self.m_FianacialInfo	=	self:GetDlgChild("FianacialInfo")	--点击查看历史财报
	--self.m_CancelOrder		=	self:GetDlgChild("CancelOrder")		--点击取消订单按钮
end

--股票详细信息窗口事件响应函数
function CStockOrderlistWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if (uMsgID == BUTTON_LCLICK) then
	  	if(Child == self.m_LastStock) then 						--要求查看上一支股票的信息
	  		self:GetLastStockInfo()
	  	
	  	elseif(Child == self.m_NextStock)    then 				--要求查看下一支股票的信息							
			self:GetNextStockInfo()
	  			
	  	elseif(Child == self.m_BuyThisStock)      then			--购买选中的股票															
			self:BuyCurShowStock()
	  	
	  	elseif(Child == self.m_SellThisStock)       then		--出售选中的股票							
			self:SellCurShowStock()	
		
		elseif(Child == self.m_CheckKMap)       then 			--查看k线图
			self:CheckKMapChart()
			
		elseif(Child == self.m_FianacialInfo)       then 		--查看历史财报	
			self:CheckFinancialInfo()
			
		--[[
		elseif(Child == self.m_CancelOrder)       then 			--取消某支股票
			for i =1 ,# self.m_BuyOrderlistTbl do
				if self.m_BuyOrderlistTbl[i].OrderlistId == self.m_CurStockId then
					table.remove(self.m_BuyOrderlistTbl, i)
					break
				end
			end
			Gac2Gas:CofcStockDeleteOrder(g_Conn, self.m_CurStockId)
		--]]
		end
	end
end

--将从服务端得到的股票信息显示在股票详细信息窗口中
function CStockOrderlistWnd:ShowStockOrderlistWnd(totalBuyCount, totalSellCount)
	self = g_GameMain.m_CofCStockOrderListWnd
	local buyTbl  = self.m_BuyOrderlistTbl
	local buyTblLen  = # buyTbl
	local sellTbl = self.m_SellOrderlistTbl
	local sellTblLen = # sellTbl
	
	if self.m_CurStockId == g_GameMain.m_CofCMainStockWnd.m_FirstStockId then		--如果已经是第一支股票了，则"上一支"的按钮不可点
		self.m_LastStock:EnableWnd(false)
	else 
		self.m_LastStock:EnableWnd(true)
	end

	if self.m_CurStockId == g_GameMain.m_CofCMainStockWnd.m_LastStockId then		--如果已经是最后一支股票了，则"下一支"的按钮不可点
		self.m_NextStock:EnableWnd(false)
	else 
		self.m_NextStock:EnableWnd(true)
	end
	
	self.m_SellCount:SetWndText("卖单(总" .. totalSellCount .. "股)")
	self.m_BuyCount:SetWndText("买单(总" .. totalBuyCount .. "股)")
	
	--显示所有卖单信息,不足5条卖单的将显示挂单信息的button控件不显示
	for i =1, buyTblLen do
		local buyStr = "Buy" .. i
		self:GetDlgChild(buyStr):ShowWnd(true)
		self:GetDlgChild(buyStr):SetWndText(buyTbl[i].Price .. "/" .. buyTbl[i].Count .. "股"  )
	end	
	for i = buyTblLen +1 , 5 do
		local buyStr = "Buy" .. i
		self:GetDlgChild(buyStr):ShowWnd(false)
	end
	
	--显示所有买单信息，不足5条买单的将显示挂单信息的button控件不显示
	for i = 1, sellTblLen do
		local sellStr = "Sell" .. i
		self:GetDlgChild(sellStr):ShowWnd(true) 
		self:GetDlgChild(sellStr):SetWndText(sellTbl[i].Price .. "/" .. sellTbl[i].Count .. "股"  )
	end
	for i = sellTblLen +1 , 5 do
		local sellStr = "Sell" .. i
		self:GetDlgChild(sellStr):ShowWnd(false)
	end
end

--“上一支”按钮被点中，将被点中的订单id传回给CofCStockMain.lua中的函数GetStockIndexById，
--用来筛选信息，到底需要将那支股票id传给服务端，
--参数：当前股票的id，用于判断上下股票的信息
function CStockOrderlistWnd:GetLastStockInfo()
	g_GameMain.m_CofCMainStockWnd:GetStockIndexById(self.m_CurStockId, 0)
end

--“下一支”按钮被点中，将被点中的订单id传回给CofCStockMain.lua中的函数GetStockIndexById，
--用来筛选信息，到底需要将那支股票id传给服务端，
--参数：当前股票的id，用于判断上下股票的信息
function CStockOrderlistWnd:GetNextStockInfo()
	g_GameMain.m_CofCMainStockWnd:GetStockIndexById(self.m_CurStockId, 1)
end

--查看历史财报函数
function CStockOrderlistWnd:CheckFinancialInfo()
	if self.m_FianacialInfo:GetWndText() == "K线图" then
		self:CofcOrderlistFinancialBtnClicked()
	elseif self.m_FianacialInfo:GetWndText() == "历史财报" then
		local financialCurWeek = 0
		Gac2Gas:GetCofCFinancialReport(g_Conn,self.m_CurStockId, financialCurWeek)
	end		
end

function CStockOrderlistWnd:GetWhichOrderChoosed()
	local choosedItem = nil 
	choosedItem = g_GameMain.m_CofCMainStockWnd.m_StockList:GetSelectItem( -1 ) + 1
  
	if choosedItem == 0 then
		return nil
	end
	
	local choosedStockNo = g_GameMain.m_CofCMainStockWnd.m_MyDealingInfoTbl[choosedItem].ID		--挂单流水号
	return choosedStockNo
end
--点击“买进该股”按钮后，如果我的交易窗口不存在则创建，否则显示我的交易窗口
function CStockOrderlistWnd:BuyCurShowStock()
	
	--是否取消挂单的回调函数
	local function callback(Context,Button)
		if(Button == MB_BtnOK) then		
			local mydealingId = self:GetWhichOrderChoosed()
			if mydealingId == nil then
				return
			end
			Gac2Gas:CofcStockDeleteOrder(g_Conn,mydealingId)
		end
		return true
	end
	if self.m_BuyThisStock:GetWndText() == "撤回挂单" then
		g_GameMain.m_CofCMainStockWnd:ShowWnd(true)
		self.m_MsgBox = MessageBox( self, MsgBoxMsg(16008), BitOr( MB_BtnOK, MB_BtnCancel),callback)
		return
	end
	self:MyDealingWndOrderlistBuy()
end

function CStockOrderlistWnd:MyDealingWndOrderlistBuy()
	if g_GameMain.m_CofCStockMyDealing == nil then
		g_GameMain.m_CofCStockMyDealing = CreateCofCMyDealingWnd(g_GameMain)
		table.insert( self.TblFatherWnd, g_GameMain.m_CofCStockMyDealing)
		g_GameMain.m_CofCStockMyDealing:ShowWnd(true)
	else 
		g_GameMain.m_CofCStockMyDealing:ShowWnd(true)
	end
	g_GameMain.m_CofCStockMyDealing.m_CurDealingType 	=	1 
	
	self:ShowExtraWndFalse(false)
	local price = ""
	local count = ""
	if # self.m_BuyOrderlistTbl ~= 0 then
		price = self.m_BuyOrderlistTbl[1].Price
		count = self.m_BuyOrderlistTbl[1].Count
	end												
	self:SetCofCMyDealingWndEvent()
	g_GameMain.m_CofCStockMyDealing:ShowCofCStockDealingInfo(self.m_TotalBuyCount,price,count)

end

--点击“售出该股”按钮后，如果我的交易窗口不存在则创建，否则显示我的交易窗口
function CStockOrderlistWnd:SellCurShowStock()
	if self.m_SellThisStock:GetWndText() == "新增交易" then
		self:MyDealingWndOrderlistBuy()
		return
	end
	if g_GameMain.m_CofCStockMyDealing == nil then
		g_GameMain.m_CofCStockMyDealing = CreateCofCMyDealingWnd(g_GameMain)
		table.insert( self.TblFatherWnd, g_GameMain.m_CofCStockMyDealing)
		g_GameMain.m_CofCStockMyDealing:ShowWnd(true)
	else
		g_GameMain.m_CofCStockMyDealing:ShowWnd(true)
	end
	g_GameMain.m_CofCStockMyDealing.m_CurDealingType 	=	2
	self:ShowExtraWndFalse(false)
	local price = ""
	local count = ""
	if # self.m_SellOrderlistTbl ~= 0 then
		price = self.m_SellOrderlistTbl[1].Price
		count = self.m_SellOrderlistTbl[1].Count
	end
	self:SetCofCMyDealingWndEvent()
	g_GameMain.m_CofCStockMyDealing:ShowCofCStockDealingInfo(self.m_TotalSellCount,price,count)
end

--在大盘股票、我的股票、我的交易面板之间切换时，股票信息orderlist窗口中的上一支、下一支等窗口根据传进的参数选择显示或不显示
function CStockOrderlistWnd:ShowExtraWndFalse(showOrNotShow)
	self.m_LastStock:ShowWnd(showOrNotShow)
	self.m_NextStock:ShowWnd(showOrNotShow)
	self.m_BuyThisStock:ShowWnd(showOrNotShow)
	self.m_SellThisStock:ShowWnd(showOrNotShow)
	self.m_CheckKMap:ShowWnd(showOrNotShow)
	self.m_FianacialInfo:ShowWnd(showOrNotShow)
end

--用这个函数标志我的交易窗口是打开的，那么当更新股票orderlist窗口时，也更新我的交易窗口中的信息
function CStockOrderlistWnd:SetCofCMyDealingWndEvent()
	if g_GameMain.m_CofCStockMyDealing.m_CurDealingType == 1 then
		UpdateCofCStockMyDealingInfoCallBack  = self.BuyCurShowStock
	elseif g_GameMain.m_CofCStockMyDealing.m_CurDealingType == 2 then
		UpdateCofCStockMyDealingInfoCallBack  = self.SellCurShowStock
	end
end

--点击查看k线图button，查看k线图，或者如果打开的界面是历史财报界面，则应打开大盘界面
function CStockOrderlistWnd:CheckKMapChart()
	if self.m_CheckKMap:GetWndText() == "大盘总览" then
		self.m_LastStock:ShowWnd(false)
		self.m_NextStock:ShowWnd(false)
		self:ShowWnd(false)
		g_GameMain.m_CofCMainStockWnd:ShowWnd(true)
		g_GameMain.m_CofCMainStockWnd.m_CurStockType = 1
		g_GameMain.m_CofCMainStockWnd:ShowAllStockInfo()
	elseif self.m_CheckKMap:GetWndText() == "K线图" then
		self.m_CheckKMap:SetWndText("大盘总览")
		self.m_FianacialInfo:SetWndText("历史财报")
		if g_GameMain.m_CofCStockKMapWnd == nil then
			g_GameMain.m_CofCStockKMapWnd = CreateCofCStockKMapWnd(g_GameMain)
			table.insert( self.TblFatherWnd, g_GameMain.m_CofCStockKMapWnd)
			g_GameMain.m_CofCStockKMapWnd:ShowWnd(true)
		elseif g_GameMain.m_CofCStockKMapWnd:IsShow() == false then
			g_GameMain.m_CofCStockKMapWnd:ShowWnd(true)
		end	
		Gac2Gas:GetCofCStockKMap(g_Conn,self.m_CurStockId,g_GameMain.m_CofCStockKMapWnd.m_DayTimePeriod,g_GameMain.m_CofCStockKMapWnd.m_DayTimeInterval)
	end
end

function CStockOrderlistWnd:CofcOrderlistFinancialBtnClicked()
	if g_GameMain.m_CofCStockKMapWnd == nil then
		g_GameMain.m_CofCStockKMapWnd = CreateCofCStockKMapWnd(g_GameMain)
		g_GameMain.m_CofCStockKMapWnd:ShowWnd(true)
	elseif g_GameMain.m_CofCStockKMapWnd:IsShow() == false then
		g_GameMain.m_CofCStockKMapWnd:ShowWnd(true)
	end	
	self.m_CheckKMap:SetWndText("大盘总览")
	self.m_FianacialInfo:SetWndText("历史财报")
	Gac2Gas:GetCofCStockKMap(g_Conn,self.m_CurStockId,g_GameMain.m_CofCStockKMapWnd.m_DayTimePeriod,g_GameMain.m_CofCStockKMapWnd.m_DayTimeInterval)
end