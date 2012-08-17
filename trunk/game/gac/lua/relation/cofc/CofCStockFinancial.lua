gac_require "relation/cofc/CofCStockFinancialInc"

--查看历史财报信息返回，--参数：股票名称；股票编号；期间开始时间；期间结束时间；
						--	商会等级；商会金库资金；期间收入；售出股票；期间交易总量；发出红利；我持有股数；我的持有成本；
						--	商会人数；商会活跃度；依次为第一到第五大股东的角色名和持有股数
function Gas2Gac:RetGetCofCFinancialReport(Conn,name ,no ,startTime, endTime,level,money, income,soldStock,tradeCount,bonus,myHaveStock,myHaveCosts,personCount,activity,shareholders1Name,stockCount1,shareholders2Name,stockCount2,shareholders3Name,stockCount3,shareholders4Name,stockCount4,shareholders5Name, stockCount5)
	if g_GameMain.m_CofCStockFinancialWnd == nil then
			g_GameMain.m_CofCStockFinancialWnd = CreateCofCStockFinancialWnd(g_GameMain)
			g_GameMain.m_CofCStockFinancialWnd.m_CurWeek = 0
			if g_GameMain.m_CofCStockOrderListWnd ~= nil then
				table.insert( g_GameMain.m_CofCStockOrderListWnd.TblFatherWnd, g_GameMain.m_CofCStockFinancialWnd)
			end
			g_GameMain.m_CofCStockFinancialWnd:ShowWnd(true)
	elseif g_GameMain.m_CofCStockFinancialWnd:IsShow() == false then
			if g_GameMain.m_CofCStockOrderListWnd ~= nil then
				table.insert( g_GameMain.m_CofCStockOrderListWnd.TblFatherWnd, g_GameMain.m_CofCStockFinancialWnd)
			end
			g_GameMain.m_CofCStockFinancialWnd:ShowWnd(true)
	end
	if g_GameMain.m_CofCStockOrderListWnd == nil or g_GameMain.m_CofCStockOrderListWnd:IsShow() == false then
		g_GameMain.m_CofCStockFinancialWnd.m_LastWeek:ShowWnd(false)
		g_GameMain.m_CofCStockFinancialWnd.m_ThisWeek:ShowWnd(false)
	elseif  g_GameMain.m_CofCStockOrderListWnd:IsShow() then
		g_GameMain.m_CofCStockFinancialWnd.m_LastWeek:ShowWnd(true)
		g_GameMain.m_CofCStockFinancialWnd.m_ThisWeek:ShowWnd(true)
		g_GameMain.m_CofCStockOrderListWnd.m_CheckKMap:SetWndText("大盘总览")
		g_GameMain.m_CofCStockOrderListWnd.m_FianacialInfo:SetWndText("K线图")
	end
		
	g_GameMain.m_CofCStockFinancialWnd:ShowCofCStockFinancial(name ,no ,startTime, endTime,level,money, income,soldStock,tradeCount,bonus,myHaveStock,myHaveCosts,personCount,activity,shareholders1Name,stockCount1,shareholders2Name,stockCount2,shareholders3Name,stockCount3,shareholders4Name,stockCount4,shareholders5Name, stockCount5)
end

------------------------------------------RPC--------------------------------------------------

--创建商会股票历史财报窗口
function CreateCofCStockFinancialWnd(Parent)
	local wnd	=	CCofCStockFinancialWnd:new()
	wnd:CreateFromRes("CofCFinancialInfo",Parent)
	wnd:ShowWnd( true )
	wnd:InitCofCStockFinancialChild()
	g_ExcludeWndMgr:InitExcludeWnd(wnd, 1)
	return wnd
end

--初始化商会股票财报窗口中的子控件
function CCofCStockFinancialWnd:InitCofCStockFinancialChild()
	self.m_CloseBtn			=	self:GetDlgChild("Close")
	self.m_StockNameText	=	self:GetDlgChild("StockNameText")			--显示股票名称
	self.m_StockNoText		=	self:GetDlgChild("StockNoText")				--显示股票代号
	self.m_StockPeriodText	=	self:GetDlgChild("StockPeriodText")			--显示股票有效日期
	self.m_LastWeek			=	self:GetDlgChild("LastWeek")				--显示上一周财报按钮
	self.m_ThisWeek			=	self:GetDlgChild("ThisWeek")				--显示下一周财报按钮
	self.m_Level			=	self:GetDlgChild("Level")					--显示商会等级
	self.m_PersonCount		=	self:GetDlgChild("PersonCount")				--显示商会人数
	self.m_Money			=	self:GetDlgChild("Money")					--显示商会金库资金数目
	self.m_Activity			=	self:GetDlgChild("Activity")				--显示商会活跃度
	self.m_Income			=	self:GetDlgChild("Income")					--显示商会收入
	self.m_Output			=	self:GetDlgChild("Output")					--显示商会平均每人产出
	self.m_StockSell		=	self:GetDlgChild("StockSell")				--显示售出股票总股数
	self.m_TotakDealing		=	self:GetDlgChild("TotakDealing")			--显示商会总交易量
	self.m_First			=	self:GetDlgChild("First")					--显示商会第1大股东持有股票信息
	self.m_Second			=	self:GetDlgChild("Second")					--显示商会第2大股东持有股票信息
	self.m_Third			=	self:GetDlgChild("Third")					--显示商会第3大股东持有股票信息
	self.m_Forth			=	self:GetDlgChild("Forth")					--显示商会第4大股东持有股票信息
	self.m_Fifth			=	self:GetDlgChild("Fifth")					--显示商会第5大股东持有股票信息
	self.m_Bonus			=	self:GetDlgChild("Bonus")					--显示商会发出红利多少量
	self.m_BonusPerStock	=	self:GetDlgChild("BonusPerStock")			--显示商会每股红利多少钱
	self.m_MyHave			=	self:GetDlgChild("MyHave")					--显示我执有股票的信息
end


--商会股票财报窗口事件响应函数
function CCofCStockFinancialWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if (uMsgID == BUTTON_LCLICK) then
	  	if(Child == self.m_CloseBtn) then 
	  		g_GameMain.m_CofCStockOrderListWnd:ShowWnd(false)
	  		self:ShowWnd(false)
	  	
	  	elseif(Child == self.m_LastWeek)    then 				--要求查看股票上一周财报的信息							
			self.m_CurWeek = self.m_CurWeek - 1
			Gac2Gas:GetCofCFinancialReport(g_Conn,g_GameMain.m_CofCStockOrderListWnd.m_CurStockId,self.m_CurWeek)
	  			
	  	elseif(Child == self.m_ThisWeek)      then				--要求查看股票下一周财报的信息															
			self.m_CurWeek = self.m_CurWeek +1
			Gac2Gas:GetCofCFinancialReport(g_Conn,g_GameMain.m_CofCStockOrderListWnd.m_CurStockId,self.m_CurWeek)
		end
	end	
end
 
--显示返回的商会财报历史信息，显示在客户端;
--参数：股票名称；股票编号；期间开始时间；期间结束时间；
						--	商会等级；商会金库资金；期间收入；售出股票；期间交易总量；发出红利；我持有股数；我的持有成本；
						--	商会人数；商会活跃度；依次为第一到第五大股东的角色名和持有股数
function CCofCStockFinancialWnd:ShowCofCStockFinancial(name ,no ,startTime, endTime,level,money, income,soldStock,tradeCount,bonus,myHaveStock,myHaveCosts,personCount,activity,shareholders1Name,stockCount1,shareholders2Name,stockCount2,shareholders3Name,stockCount3,shareholders4Name,stockCount4,shareholders5Name, stockCount5)
	self.m_StockNameText:SetWndText(name)
	self.m_StockNoText:SetWndText(no)
	self.m_StockPeriodText:SetWndText(startTime .. "-" .. endTime )
	self.m_Level:SetWndText("商会等级：" .. level)
	self.m_Money:SetWndText("商会金库：" .. money .. "铜")
	self.m_Income:SetWndText("期间收入：" .. income .. "铜")
	self.m_StockSell:SetWndText("售出股票：" .. soldStock .. "铜")
	self.m_TotakDealing:SetWndText("期间总交易量：" .. tradeCount .. "铜")
	self.m_Bonus:SetWndText("发出红利：" .. bonus .. "铜")
	self.m_BonusPerStock:SetWndText("每股红利：" .. bonus/soldStock .. "铜")
	self.m_MyHave:SetWndText(myHaveStock/myHaveCosts .. "股/平均成本")
	self.m_PersonCount:SetWndText("商会人数：" .. personCount)
	self.m_Activity:SetWndText("商会活跃度：" .. activity .. "点" )
	self.m_Output:SetWndText("平均每人产出：" .. money/personCount .. "铜")
	self.m_First:SetWndText("第一大股东：" .. shareholders1Name .. "/持有" .. stockCount1 .. "股" )
	self.m_Second:SetWndText("第二大股东：" .. shareholders2Name .. "/持有" .. stockCount2 .. "股" )
	self.m_Third:SetWndText("第三大股东：" .. shareholders3Name .. "/持有" .. stockCount3 .. "股" )
	self.m_Forth:SetWndText("第四大股东：" .. shareholders4Name .. "/持有" .. stockCount4 .. "股" )
	self.m_Fifth:SetWndText("第五大股东：" .. shareholders5Name .. "/持有" .. stockCount5 .. "股" )
end