gac_require "relation/cofc/CofCStockKMapInc"

function Gas2Gac:RetGetCofCStockKMapBegin(Conn)
	g_GameMain.m_CofCStockKMapWnd.m_CofCStockKMapInfoTbl	=	{}
end

--股票交易记录信息：交易时间、交易量、交易累计总额
function Gas2Gac:RetGetCofCStockKMap(Conn, tradeTime, tradeCount, tradeMoney)
	local node		=	{}
	node.TradeTime	=	tradeTime -- 该字段类型为 unix_timestamp
	node.TradeCount	=	tradeCount
	node.TradeMoney	=	tradeMoney
	if tradeMoney ~= 0 then
		node.TradePrice =	tradeMoney/tradeCount
	else
		node.TradePrice	=	0
	end
	
	table.insert(g_GameMain.m_CofCStockKMapWnd.m_CofCStockKMapInfoTbl,node)
end

--股票交易k线图部分传输数据结束，在客户端显示k线图
function Gas2Gac:RetGetCofCStockKMapEnd(Conn, time_begin, time_end)
	g_GameMain.m_CofCStockKMapWnd:ShowCofCStockKMap(time_begin, time_end) 
end

----------------------------------------------RPC-----------------------------------------

--创建商会股票K线图窗口
function CreateCofCStockKMapWnd(Parent)
	local wnd	=	CCofCStockKMapWnd:new()
	wnd:CreateFromRes("CofCKeyMap",Parent)
	wnd:ShowWnd( true )
	g_ExcludeWndMgr:InitExcludeWnd(wnd, 1)
	wnd:InitKMapInterval()
	wnd:InitCofCStockKMapChild()
	
	return wnd	
end	

--用成员变量保存商会股票k线图窗口中的控件
function CCofCStockKMapWnd:InitCofCStockKMapChild()
	self.m_Close			=	self:GetDlgChild("Close")
	self.m_KMap				=	self:GetDlgChild("KMap")
	self.m_DayBtn			=	self:GetDlgChild("DayBtn")
	self.m_WeekBtn			=	self:GetDlgChild("WeekBtn")
	self.m_MonthBtn			=	self:GetDlgChild("MonthBtn")
	
	local spanX = 20
	
	self.m_GraphicRectItem	=	      ClassCast(CChartHistogram, self.m_KMap:AppendGraphicItem(1))
	self.m_GraphicLineItem	= 	      ClassCast(CChartLineGraph, self.m_KMap:AppendGraphicItem(0))
end

--初始化商会股票k线图上时间间隔和传给服务端的查看k线图的时间范围
function CCofCStockKMapWnd:InitKMapInterval()
	self.m_DayTimePeriod		=	-24*60				--日时间差
	self.m_WeekTimePeriod		=	-7*24*60			--周时间差
	self.m_MonthTimePeriod		=	-30*24*60			--月时间差
	self.m_DayTimeInterval		=	15					--日时间刻度
	self.m_WeekTimeInterval		=	2*60				--周时间刻度
	self.m_MonthTimeInterval	=	8*60				--月时间刻度
	self.m_CurKMapType			=	1
end

--商会股票k线图窗口事件响应函数
function CCofCStockKMapWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if (uMsgID == BUTTON_LCLICK) then
		if Child == self.m_Close then
			self:ShowWnd(false)
			g_GameMain.m_CofCStockOrderListWnd:ShowWnd(false)
		
		elseif Child == self.m_DayBtn then
			self.m_CurKMapType	=	1
			self:GetCofCStockKMapInfo()
		
		elseif Child == self.m_WeekBtn then
			self.m_CurKMapType	=	2
			self:GetCofCStockKMapInfo()
			
		elseif Child == self.m_MonthBtn then
			self.m_CurKMapType	=	3
			self:GetCofCStockKMapInfo()
		end	
	end
end

--点击商会股票k线图日线图、周线图、月线图
function CCofCStockKMapWnd:GetCofCStockKMapInfo()
	if self.m_CurKMapType == 1 then						--查看日线图信息
		Gac2Gas:GetCofCStockKMap(g_Conn, g_GameMain.m_CofCStockOrderListWnd.m_CurStockId, self.m_DayTimePeriod, self.m_DayTimeInterval)
	
	elseif self.m_CurKMapType == 2 then					--查看周线图信息
		Gac2Gas:GetCofCStockKMap(g_Conn, g_GameMain.m_CofCStockOrderListWnd.m_CurStockId, self.m_WeekTimePeriod, self.m_WeekTimeInterval)

	elseif self.m_CurKMapType == 3 then					--查看月线图信息
		Gac2Gas:GetCofCStockKMap(g_Conn, g_GameMain.m_CofCStockOrderListWnd.m_CurStockId, self.m_MonthTimePeriod, self.m_MonthTimeInterval)
	end
end
--- @brief 补全数据中的空点
function CCofCStockKMapWnd:InsertEmptyPoint(time_begin, time_end)
	local data = self.m_CofCStockKMapInfoTbl
	if #data > 0 then
		local delta = self:GetCofCStockKMapTimeDelta() * 60
		-- 中间空缺点的填充
		local nextTime = data[1].TradeTime
		local i = 1
		while true do
			if i > #data then
				break
			end
			local thisTime = data[i].TradeTime
			local j = i
			while nextTime < thisTime do
				table.insert(data, j, {TradeTime = nextTime, TradeCount = 0, TradeMoney = 0, TradePrice = 0})
				nextTime = nextTime + delta
				j = j + 1
			end
			nextTime = thisTime + delta
			i = i + 1
		end
		-- 起点缺失点的填充
		if data[1].TradeTime > time_begin then
			table.insert(data, 1, {TradeTime = time_begin, TradeCount = 0, TradeMoney = 0, TradePrice = 0})
		end
		i = 1
		while data[i].TradeTime + delta < data[i + 1].TradeTime do
			table.insert(data, i + 1, {TradeTime = data[i].TradeTime + delta, TradeCount = 0, TradeMoney = 0, TradePrice = 0})
			i = i + 1
		end
		-- 终点缺失点的填充
		while data[#data].TradeTime < time_end do
			table.insert(data, {TradeTime = data[#data].TradeTime + delta, TradeCount = 0, TradeMoney = 0, TradePrice = 0})
		end
	end
end

-- @brief 线图的0值线性插值
function CCofCStockKMapWnd:LineGraphInterpolation()
	local data = self.m_CofCStockKMapInfoTbl
	
	-- 前缀0点填充
	local i = 1
	while i <= #data do
		if data[i].TradePrice ~= 0 then
			for j = 1, i - 1 do
				data[j].TradePrice = data[i].TradePrice
			end
			break
		else
			i = i + 1
		end
	end
	-- 中间0点线性插值
	local nonZeroIndex = nil
	while i <= #data do
		if data[i].TradePrice ~= 0 then
			if nonZeroIndex ~= nil and i - nonZeroIndex > 1 then
				local d = (data[i].TradePrice - data[nonZeroIndex].TradePrice) / (i - nonZeroIndex)
				for j = nonZeroIndex + 1, i - 1 do
					data[j].TradePrice = data[nonZeroIndex].TradePrice + (j - nonZeroIndex) * d
				end
			end
			nonZeroIndex = i
		end
		i = i + 1
	end
	-- 后缀0点的填充
	if nonZeroIndex ~= nil then
		i = nonZeroIndex + 1
		while i <= #data do
			data[i].TradePrice = data[nonZeroIndex].TradePrice
			i = i + 1
		end
	end
end

--在客户端显示k线图的线和柱状图
function CCofCStockKMapWnd:ShowCofCStockKMap(time_begin, time_end)
	local kMapInfoTbl		=	self.m_CofCStockKMapInfoTbl
	local kMapInfoTblLen	=	# kMapInfoTbl
	local timeSpan = self:GetCofCStockKMapTimeDelta() * 60
	
	
	-- 清除历史数据
	self.m_GraphicRectItem:Clear()
	self.m_GraphicLineItem:Clear()
	
	if kMapInfoTblLen > 0 then
		self.m_GraphicLineItem:SetColor(0xffff0000)
		-- 空缺点的补齐
		self:InsertEmptyPoint(time_begin, time_end)
		
		-- 线图的0值线性插值
		self:LineGraphInterpolation()
		
		-- 用以调整缩放比例的计数器
		local line_min = 0
		local line_max = 0
		local rect_min = 0
		local rect_max = 0

		local color = nil
		
		for	i=1, # self.m_CofCStockKMapInfoTbl	do
			if i % 2 ==	0 then
				color =	0xff00ff00
			else
				color =	0xffffff00
			end
			
			-- 柱状图数据插入
			self.m_GraphicRectItem:AppendData(kMapInfoTbl[i].TradeCount, color)		--插入柱状图数据
			if kMapInfoTbl[i].TradeCount > rect_max	then rect_max =	kMapInfoTbl[i].TradeCount end
			if kMapInfoTbl[i].TradeCount < rect_min	then rect_min =	kMapInfoTbl[i].TradeCount end
			
			-- 线图数据的插入
			self.m_GraphicLineItem:AppendData(kMapInfoTbl[i].TradePrice)
			if kMapInfoTbl[i].TradePrice > line_max	then line_max =	kMapInfoTbl[i].TradePrice end
			if kMapInfoTbl[i].TradePrice < line_min	then line_min =	kMapInfoTbl[i].TradePrice end
		end
		
		-- 设置k线图的缩放比例
		if rect_max - rect_min == 0 then
			self.m_GraphicRectItem:SetZoomY(1)
		else
			self.m_GraphicRectItem:SetZoomY(self.m_KMap:GetWndHeight() * 0.8 / (rect_max - rect_min))
		end
		if line_max - line_min == 0 then
			self.m_GraphicLineItem:SetZoomY(1)
		else
			self.m_GraphicLineItem:SetZoomY(self.m_KMap:GetWndHeight() * 0.8 / (line_max - line_min))
		end
		self.m_GraphicLineItem:SetDeltaY(self.m_KMap:GetWndHeight() * 0.1)
		-- 设置k线图水平间距
		local spanX = self.m_KMap:GetWndWidth() / #self.m_CofCStockKMapInfoTbl
		self.m_GraphicRectItem:SetSpanX(spanX)
		self.m_GraphicRectItem:SetItemWidth(self.m_GraphicRectItem:GetSpanX() - 1)
		self.m_GraphicLineItem:SetSpanX(spanX)
		self.m_GraphicLineItem:SetDeltaX(spanX / 2)
	end
end

--根据k线图的类型，日线图返回时间差为15分钟，周线图返回时间差为2*60分钟，月线图返回的时间差为8*60分钟
function CCofCStockKMapWnd:GetCofCStockKMapTimeDelta()
	if self.m_CurKMapType == 1 then						--得到日线图时间差
		return 15
	
	elseif self.m_CurKMapType == 2 then					--得到周线图时间差
		return 2*60

	elseif self.m_CurKMapType == 3 then					--得到月线图时间差
		return 8*60
	end
end
