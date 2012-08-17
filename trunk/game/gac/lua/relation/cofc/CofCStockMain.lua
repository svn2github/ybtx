gac_require "relation/cofc/CofCStockMainInc"
gac_require "relation/cofc/CofCStockOrderlist"

--准备开始传股票信息，初始化相关数据
function Gas2Gac:RetGetCofCStockInfoBegin(Conn)
	g_GameMain.m_CofCMainStockWnd.m_AllStockInfoTbl 	=	{}		--用来记录所有股票信息（大盘中的股票信息）的table
	g_GameMain.m_CofCMainStockWnd.m_MyStockInfoTbl		=	{}		--用来记录我执有的股票信息的table
	g_GameMain.m_CofCMainStockWnd.m_StockListWndTbl		=	{}		--用来存储显示股票信息的listctrl中的item
	g_GameMain.m_CofCMainStockWnd.m_CurStockType		=	1		--表明当前显示的是大盘股票信息，2表明是个人股票信息,3表明是我的交易信息
	g_GameMain.m_CofCStockOrderListWnd:ShowWnd(false)
end

--开始传股票相关信息,参数：股票代号，名称，执有量，成本，涨幅，交易量，叫卖价（所有中最低的），叫买价（所有中最高的），最高价，最低价
function Gas2Gac:RetGetCofCStockInfo(Conn, no, name, haveCount, costs, range, dealingCount, sellPrice, buyPrice, highestPrice, lowestPrice)
	local node			=	{}
	node.No				=	no
	node.Name			=	name
	node.HaveCount		=	haveCount
	node.Costs			=	costs
	node.Range			=	range
	node.DealingCount	=	dealingCount
	node.SellPrice		=	sellPrice
	node.BuyPrice		=	buyPrice
	node.HighestPrice	=	highestPrice
	node.LowestPrice	=	lowestPrice
	
	table.insert(g_GameMain.m_CofCMainStockWnd.m_AllStockInfoTbl, node)		
	
	if haveCount > 0 then				--如果持有量大于0，表明是自己股票，单独记录在一个表里，有助于显示
		table.insert(g_GameMain.m_CofCMainStockWnd.m_MyStockInfoTbl, node)	
	end
end

--股票信息传输结束，将其显示在客户端
function Gas2Gac:RetGetCofCStockInfoEnd(Conn)
	g_GameMain.m_CofCMainStockWnd:ShowAllStockInfo()
	local AutoCloseWnd = CAutoCloseWnd:new()
	AutoCloseWnd:AutoCloseWnd(g_GameMain.m_CofCMainStockWnd)
end

--准备开始传我的交易面板中的相关信息，初始化相关数据
function Gas2Gac:RetGetCofCStockMyDealingInfoBegin(Conn)
	g_GameMain.m_CofCMainStockWnd.m_MyDealingInfoTbl	=	{}			--记录我的交易相关信息
end

--传我的交易面板中的相关信息，参数：股票流水号，股票代号，名称，挂单数量，类型，挂单价格，涨幅，叫卖价（所有中最低的），叫买价（所有中最高的），执有，成本
function Gas2Gac:RetGetCofCStockMyDealingInfo(Conn, stockId, no, name, count, type, price, range, sellPrice, buyPrice, haveCount, costs)
	local node			=	{}
	node.ID				=	stockId
	node.No				=	no
	node.Name			=	name
	node.Count			=	count
	if type == 0 then
		node.Type 		=	"买单"
	elseif type == 1 then
		node.Type 		=	"卖单"
	end
	node.Price			=	price
	node.Range			=	range
	node.SellPrice		=	sellPrice
	node.BuyPrice		=	buyPrice
	node.HaveCount		=	haveCount
	node.Costs			=	costs
	
	table.insert(g_GameMain.m_CofCMainStockWnd.m_MyDealingInfoTbl,node)
end

function Gas2Gac:RetGetCofCStockMyDealingInfoEnd(Conn)
	g_GameMain.m_CofCMainStockWnd:ShowMyDealingInfo()
end

------------------------------------以上是Rcp函数，从服务端的返回-----------------------------------------------------------

--创建商会股票大盘的窗口（我的股票窗口和大盘窗口一模一样，所以共用资源和代码）
function CreateStockMainWnd(Parent)
	local wnd	=	CCofCStockMainWnd:new()
	wnd:CreateFromRes("CofCStockMain",Parent)
	wnd:ShowWnd( true )
	g_ExcludeWndMgr:InitExcludeWnd(wnd, 1)
	wnd:InitStockMainChild()
	
	return wnd
end

--初始化股票大盘窗口上的子控件
function CCofCStockMainWnd:InitStockMainChild()
	self.m_CloseBtn		=	self:GetDlgChild("Close")				--关闭按钮
	self.m_AllStock		=	self:GetDlgChild("MainStock")			--切换到大盘股票的窗口的按钮
	self.m_MyStock		=	self:GetDlgChild("MyStock")				--切换到我的股票窗口的按钮
	self.m_MyDeal		=	self:GetDlgChild("MyDeal")				--切换到我的交易窗口的按钮
	self.m_StockList	=	self:GetDlgChild("StockList")			--显示股票信息的窗口listctrl
	self.m_Title		=	self:GetDlgChild("Title")				--显示窗口中的title信息
end

--股票大盘窗口事件响应函数
function CCofCStockMainWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if (uMsgID == BUTTON_LCLICK) then
	  	if(Child == self.m_CloseBtn) then 				--关闭按钮
	  		--self:CloseStockOrderlitstWnd()	
	  		self:ShowWnd(false)
	  	
	  	elseif(Child == self.m_AllStock)    then 		--显示所有股票信息	
	  		self.m_CurStockType	= 1			
	  		--self:CloseStockOrderlitstWnd()	
	  		g_GameMain.m_CofCStockOrderListWnd:ShowWnd(false)
			self:ShowAllStockInfo()
	  	
	  	elseif(Child == self.m_MyStock)      then		--显示我执有的股票信息
	  		self.m_CurStockType	= 2	
	  		--self:CloseStockOrderlitstWnd()																
			g_GameMain.m_CofCStockOrderListWnd:ShowWnd(false)
			self:ShowMyStockInfo()
	  	
	  	elseif(Child == self.m_MyDeal)       then		--显示我的股票的挂单情况
	  		self.m_CurStockType	= 3		
	  		--self:CloseStockOrderlitstWnd()					
			g_GameMain.m_CofCStockOrderListWnd:ShowWnd(false)
			self:SetMyStockWndInfo()	
		end
		
	elseif ( uMsgID == ITEM_LBUTTONCLICK ) then
		self:CheckWhichStock(self)
								
	end
end

--判断到底是哪知股票被选中，看详细信息了
--返回值：被选中的股票的id
function CCofCStockMainWnd:GetWhichStockChoosed()
	local choosedItem = nil 
	choosedItem = self.m_StockList:GetSelectItem( -1 ) + 1
  
	if choosedItem == 0 then
		return nil
	end
	
	local stockInfoTbl,wndIndex= self:GetCurShowWndTbl()
	
  	local choosedStockNo = stockInfoTbl[choosedItem].No
	return choosedStockNo,wndIndex
end

--大盘股票、我的股票、我的交易面板公用函数：筛选信息到底是哪支股票被选中查看
--参数：用来区别到底是哪个窗口对象
function CCofCStockMainWnd:CheckWhichStock(wndObject)
	local choosedStockNo,wndIndex = self:GetWhichStockChoosed()
	if choosedStockNo == nil then
		return 
	end
	if g_GameMain.m_CofCStockOrderListWnd == nil then
		g_GameMain.m_CofCStockOrderListWnd = CreateStockOrderlistWnd(g_GameMain)
		g_GameMain.m_CofCStockOrderListWnd:ShowWnd(true)
	elseif g_GameMain.m_CofCStockOrderListWnd:IsShow() == false then
		g_GameMain.m_CofCStockOrderListWnd:ShowWnd(true)
	end
	if wndIndex == 2 then
		g_GameMain.m_CofCStockOrderListWnd.m_BuyThisStock:SetWndText("撤回挂单")
		g_GameMain.m_CofCStockOrderListWnd.m_SellThisStock:SetWndText("新增交易")
	else
		g_GameMain.m_CofCStockOrderListWnd.m_BuyThisStock:SetWndText("购进该股")
		g_GameMain.m_CofCStockOrderListWnd.m_SellThisStock:SetWndText("售出该股")		
	end
	g_GameMain.m_CofCStockOrderListWnd.m_FianacialInfo:SetWndText("历史财报")
	g_GameMain.m_CofCStockOrderListWnd.m_CheckKMap:SetWndText("K线图")
	g_GameMain.m_CofCStockOrderListWnd:ShowExtraWndFalse(true)
	
	Gac2Gas:GetCofCStockOrderList(g_Conn,choosedStockNo)
end

--显示股票信息，通用函数，因为所有股票和个人股票的信息量和显示格式一模一样，所以用这个函数做基础，通过参数来区别
function CCofCStockMainWnd:ShowStockInfoCommon(PersonnalOrAll)
	local stockTbl = {}
	local type  = 0
	if PersonnalOrAll == self.m_AllStockInfoTbl then			--如果是所有股票信息
		stockTbl = self.m_AllStockInfoTbl
		type = 1
	elseif PersonnalOrAll == self.m_MyStockInfoTbl then			--如果是个人股票信息
		stockTbl = self.m_MyStockInfoTbl
		type = 1
	elseif PersonnalOrAll == self.m_MyDealingInfoTbl then		--如果是个人交易信息
		stockTbl = self.m_MyDealingInfoTbl
		type = 2
	end
	local stockTblLen	= 	# stockTbl
	if stockTblLen >= 1 then
		self.m_FirstStockId = 	stockTbl[1].No						--记录第一支股票id
		self.m_LastStockId	=	stockTbl[stockTblLen].No			--记录最后一支股票id
	end
	self.m_StockList:DeleteAllItem()
	for i=1, stockTblLen do
		if ( i == 1 ) then
		  self.m_StockList:InsertColumn( 0, self.m_StockList:GetWndWidth())
		end
		self.m_StockList:InsertItem( i-1, 25 )
		
		local stockItemWnd = SQRDialog:new()
		local item = self.m_StockList:GetSubItem( i-1, 0)
		stockItemWnd:CreateFromRes( "CofCMainStockItem", item )
		stockItemWnd:ShowWnd( true )
		stockItemWnd:SetStyle( 0x60000000 )
		--显示股票的具体信息
		stockItemWnd:GetDlgChild("No"):SetWndText(stockTbl[i].No)
		stockItemWnd:GetDlgChild("Name"):SetWndText(stockTbl[i].Name)
		stockItemWnd:GetDlgChild("SellPrice"):SetWndText(stockTbl[i].SellPrice)
		stockItemWnd:GetDlgChild("BuyPrice"):SetWndText(stockTbl[i].BuyPrice)

		self:SetStockInfoInStockWnd(stockItemWnd,type,stockTbl,i)
		
		table.insert(self.m_StockListWndTbl, stockItemWnd)
	end

end

--区分（大盘、我的股票）和（我的交易）这两者，然后按需求给出的信息排列顺序正确显示
function CCofCStockMainWnd:SetStockInfoInStockWnd(stockItemWnd,type,stockTbl,index)
	if type == 1 then					--按照大盘、股票的信息排列顺序显示信息
		stockItemWnd:GetDlgChild("HaveNo"):SetWndText(stockTbl[index].HaveCount)
		stockItemWnd:GetDlgChild("Costs"):SetWndText(stockTbl[index].Costs)
		stockItemWnd:GetDlgChild("Range"):SetWndText(stockTbl[index].Range .. "%")
		stockItemWnd:GetDlgChild("DealingCount"):SetWndText(stockTbl[index].DealingCount)
		stockItemWnd:GetDlgChild("HighestPrice"):SetWndText(stockTbl[index].HighestPrice)
		stockItemWnd:GetDlgChild("LowestPrice"):SetWndText(stockTbl[index].LowestPrice)
	elseif type == 2 then
		stockItemWnd:GetDlgChild("HaveNo"):SetWndText(stockTbl[index].Count)
		stockItemWnd:GetDlgChild("Costs"):SetWndText(stockTbl[index].Type)
		stockItemWnd:GetDlgChild("Range"):SetWndText(stockTbl[index].Price)
		stockItemWnd:GetDlgChild("DealingCount"):SetWndText(stockTbl[index].Range .. "%")
		stockItemWnd:GetDlgChild("HighestPrice"):SetWndText(stockTbl[index].HaveCount)
		stockItemWnd:GetDlgChild("LowestPrice"):SetWndText(stockTbl[index].Costs)
	end	
end

--显示所有股票信息
function CCofCStockMainWnd:ShowAllStockInfo()	
	self.m_Title:SetWndText("  代码    名称    执有     成本    涨幅   交易量    叫卖     叫买   最高    最低")
	self:ShowStockInfoCommon(self.m_AllStockInfoTbl)
end

--显示所有我执有的股票信息
function CCofCStockMainWnd:ShowMyStockInfo()
	self.m_Title:SetWndText("  代码    名称    执有     成本    涨幅   交易量    叫卖     叫买   最高    最低")
	self:ShowStockInfoCommon(self.m_MyStockInfoTbl)
end

--显示所有我的交易信息
function CCofCStockMainWnd:ShowMyDealingInfo()
	self:ShowStockInfoCommon(self.m_MyDealingInfoTbl)
end

--因为大盘股票、我的股票和我的交易三个界面想公用一个窗口，但是界面显示的内容不太一样
--所以通过此函数更改一些显示信息
function CCofCStockMainWnd:SetMyStockWndInfo()
	self.m_Title:SetWndText(" 代码    名称   挂单数量   类型  挂单价格   涨幅    叫卖     叫买   执有    成本")
	Gac2Gas:GetCofCStockMyDealingInfo(g_Conn)
end


--大盘股票、我的股票和我的交易三个界面相互切换时，关闭原先打开的查看某支股票的窗口
function CCofCStockMainWnd:CloseStockOrderlitstWnd()
	if g_GameMain.m_CofCStockOrderListWnd ~= nil and 
		g_GameMain.m_CofCStockOrderListWnd:IsShow() then
			g_GameMain.m_CofCStockOrderListWnd:ShowWnd(false)
	end
end

--得到当前打开窗口对应的股票信息缓存表
function CCofCStockMainWnd:GetCurShowWndTbl()
	local stockInfoTbl = {}
	local wndIndex = 1
	if self.m_CurStockType	== 1 then
		stockInfoTbl = self.m_AllStockInfoTbl
	elseif self.m_CurStockType	== 2 then
		stockInfoTbl = self.m_MyStockInfoTbl
	elseif self.m_CurStockType == 3 then
		stockInfoTbl = self.m_MyDealingInfoTbl
		wndIndex = 2
	end
	return stockInfoTbl,wndIndex
end

--通过股票的id得到该股票在listctrl中的显示位置
--参数：当前显示在StockOrderlist窗口的股票信息对应的股票id， 上一支或下一支股票（0代表上一支，1代表下一支）
function CCofCStockMainWnd:GetStockIndexById(stockId, lastOrNext)
	local stockInfoTbl = self:GetCurShowWndTbl()

	local index = self:GetShowIndexByStockId(stockId)
	
	local choosedStockNo = 0				
	if lastOrNext == 0 then						  	--如果是想要得到上一支股票的信息，则得到上一支股票的id
		if index == 1 then 
			return
		end
		choosedStockNo = stockInfoTbl[index-1].No	--如果是想要得到下一支股票的信息，则得到下一支股票的id
	elseif lastOrNext == 1 then
		local stockInfoTblLen = # stockInfoTbl
		if index == stockInfoTblLen then
			return 
		end
		choosedStockNo = stockInfoTbl[index+1].No
	end
	
	Gac2Gas:GetCofCStockOrderList(g_Conn,choosedStockNo)
end

--通过股票id得到股票在窗口中对应的显示的index
function CCofCStockMainWnd:GetShowIndexByStockId(stockId,wndInfo)
	local stockInfoTbl = {}
	if wndInfo == nil then
		stockInfoTbl = self:GetCurShowWndTbl()
	else
		stockInfoTbl = wndInfo
	end
	local stockInfoTblLen = # stockInfoTbl
	
	local index = 0
	
	--通过id得到index
	for i =1, stockInfoTblLen do
		if stockInfoTbl[i].No == stockId then
			index = i
			return index
		end
	end
end

