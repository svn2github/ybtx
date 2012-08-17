CConsignmentCommon = class (SQRDialog)


local Panel = {}
Panel.SellOrder = 1
Panel.CharSellOrder = 3

--购买物品或增加订单成功，清空对应面板的信息
function CConsignmentCommon.RetCSMAddOrderSuc( wndNo, IsSuc )
	if wndNo == Panel.CharSellOrder then
		
		if IsSuc == true then
		    g_GameMain.m_CSMSellCharOrderWnd:InitSellOrderWndInfo()
		    g_GameMain.m_CSMSellCharOrderWnd:VirtualExcludeWndClosed()
			local wnd = g_GameMain.m_CSMSellCharOrderWnd
			if wnd.OrderType == nil then
				wnd.OrderType = 0 
			end
			local page = 0 
			
			if wnd.CurPage == 0 then
				page = 1
				Gac2Gas:CSMSearchCharSellOrder(g_Conn, wnd.OrderType, page)
			else
				Gac2Gas:CSMSearchCharSellOrder(g_Conn, wnd.OrderType, wnd.CurPage)
			end  		 
		else
            g_GameMain.m_CSMSellCharOrderWnd.m_CSMTipsWnd:ShowWnd(false)
		end 
	end	
end

--取消订单成功，清空对应面板的信息
function CConsignmentCommon.RetCSMCancelOrderSucc( wndNo, IsSuc )
	if wndNo == Panel.CharSellOrder then
		if IsSuc == true then
			local wnd = g_GameMain.m_CSMSellCharOrderWnd   		
			if # wnd.m_CSMOrderInfoTbl == 1 then
				wnd.CurPage = wnd.CurPage - 1
			end
			wnd:InitSellOrderWndInfo()
			local page = 0 
			if wnd.CurPage == 0 then
				page = 1
				Gac2Gas:CSMSearchCharSellOrder(g_Conn, wnd.OrderType, page)
			else
				Gac2Gas:CSMSearchCharSellOrder(g_Conn, wnd.OrderType, wnd.CurPage)
			end  		 
		end
	end	
end

--从服务端得到出售或收购的最后一次价格
function CConsignmentCommon.RetGetCSMRememberPrice(pannelNo, price)
	gac_gas_require "framework/common/CMoney"
	local money = CMoney:new()
	local jinCount, yinCount, tongCount = money:ChangeMoneyToGoldAndArgent(price)
	if pannelNo == Panel.CharSellOrder then
		g_GameMain.m_CSMSellCharOrderWnd.m_Jin:SetWndText(jinCount)
		g_GameMain.m_CSMSellCharOrderWnd.m_Yin:SetWndText(yinCount)
		g_GameMain.m_CSMSellCharOrderWnd.m_Tong:SetWndText(tongCount)
        g_GameMain.m_CSMSellCharOrderWnd:SetSysFee()
		g_GameMain.m_CSMSellCharOrderWnd.m_Sell:EnableWnd(true)
	end
end

function CConsignmentCommon.RetCloseCSMWnd()
	g_GameMain.m_CSMSellOrderWnd:ShowWnd(false)
	if g_GameMain.m_CSMSellCharOrderWnd then
	    g_GameMain.m_CSMSellCharOrderWnd:ShowWnd(false)
	end
end

--寄售交易所出售列表，选择购买列表中的某个物品
--参数：1表示购买成功；2表示由于持有金钱不够，购买失败；3表示由于其他玩家已经购买了此物品，购买失败
function CConsignmentCommon.RetCSMBuyOrderError( IsSuc )
	--重新显示所选购买物品当前页的物品列表信息
	if IsSuc == 1 then
    	if # g_GameMain.m_CSMSellOrderWnd.m_CSMOrderInfoTbl == 1 then
			g_GameMain.m_CSMSellOrderWnd.CurPage = g_GameMain.m_CSMSellOrderWnd.CurPage - 1
		end
		g_GameMain.m_WndBagSlots.m_lcAddBagSlots:UpdateBagSlotsPos()
        g_GameMain.m_CSMSellOrderWnd:SearchAddTreeListInfo(1161)
	elseif IsSuc == 2 then
		g_GameMain.m_CSMSellOrderWnd.m_MsgBox  = MessageBox( g_GameMain.m_CSMSellOrderWnd, MsgBoxMsg(150), MB_BtnOK )

	elseif IsSuc == 3 then                                                                  
		g_GameMain.m_CSMSellOrderWnd.m_MsgBox  = MessageBox( g_GameMain.m_CSMSellOrderWnd, MsgBoxMsg(151), MB_BtnOK )
	end 
end

--服务器返回要显示的物品列表结束，将其显示在寄售交易所相对应的界面
function CConsignmentCommon.RetCSMOrderListEnd( wndNo )
	local wnd = nil
	if wndNo == Panel.SellOrder then 
		wnd = g_GameMain.m_CSMSellOrderWnd
	elseif wndNo == Panel.CharSellOrder then
		wnd = g_GameMain.m_CSMSellCharOrderWnd
	end  
	wnd:ShowConsignmentItemList()
end

--得到现有物品列表
function CConsignmentCommon.RetCSMOrderList( wndNo, orderID, playerName, itemType, itemName, itemPrice, itemCount,  leftTime, itemID )
	local showLeftTime = math.ceil(leftTime)
	if showLeftTime <= 0 then
	   showLeftTime = GetStaticTextClient(1287)
	end
	local wnd = nil
	if wndNo == Panel.SellOrder then 
		wnd = g_GameMain.m_CSMSellOrderWnd
	elseif wndNo == Panel.CharSellOrder then
		wnd = g_GameMain.m_CSMSellCharOrderWnd
	end	
	wnd:SetCSMOrderInfoTbl( orderID, itemType, itemName, itemCount, playerName, itemPrice, showLeftTime, itemID )
end

local function SetCurPage(wndObj, itemTotalNo)
	wndObj.MaxPageNo = math.ceil( itemTotalNo/50 )
	if wndObj.MaxPageNo >  0  and
		wndObj.CurPage == 0  then
		wndObj.CurPage = 1    
	end
	if wndObj.MaxPageNo == 0 then
        wndObj.CurPage = 0
	end
	if wndObj.CurPage > wndObj.MaxPageNo then
        wndObj.CurPage = 1
	end
end

--得到要显示的该类物品的总数目
function CConsignmentCommon.RetCSMTotalNo( wndNo, itemTotalNo )
    local wnd = nil 
	if wndNo == Panel.SellOrder then 
		wnd = g_GameMain.m_CSMSellOrderWnd

	elseif wndNo == Panel.CharSellOrder then
		wnd = g_GameMain.m_CSMSellCharOrderWnd
	end	
	wnd:InitSellOrderWndInfo()
	SetCurPage(wnd, itemTotalNo)
end

--初始化物品列表
function CConsignmentCommon:SetCSMOrderInfoTbl( orderID, itemType, itemName, itemCount, playerID, itemPrice, leftTime, itemID )
	local node = {}
	node.orderID = orderID
	node.itemType = itemType
	node.itemName = itemName
	node.itemCount = itemCount
	node.seller = playerID
	node.sellPrice = itemPrice 
	node.leftTime = leftTime
	node.itemID		=	itemID

	table.insert( self.m_CSMOrderInfoTbl, node )
end

function CConsignmentCommon.RetCSMAddSellOrderOverAvgPrice(itemType, itemName, avgPrice)
    local displayName = g_ItemInfoMgr:GetItemLanInfo(itemType, itemName,"DisplayName")
    local str =g_MoneyMgr:ChangeMoneyToString(avgPrice * 2, EGoldType.GoldCoin )
    MsgClient(8037, displayName, str)
end 

function CConsignmentCommon:ShowConsignmentItemList()
	if self.CurPage <= 1 then
		self.m_PrePageBtn:EnableWnd(false)
    else
		self.m_PrePageBtn:EnableWnd( true )
	end

	if self.CurPage == self.MaxPageNo or self.MaxPageNo <= 1  then
		self.m_NextPageBtn:EnableWnd(false)
	end
	if self.CurPage < self.MaxPageNo and self.MaxPageNo > 1 then
		self.m_NextPageBtn:EnableWnd( true )
	end 

	self.m_PageNo:SetWndText(string.format("%d/%d", self.CurPage, self.MaxPageNo ))
	self:ShowSellList()  
end

--订单列表中显示各个项的内容
function CConsignmentCommon:SetStrInItem(item, ItemData)
	for i = 1, # ItemData do
		if i == 2 then	 --如果是显示金钱的话，则需显示金银铜
			local itemPrice = ItemData[i]
			gac_gas_require "framework/common/CMoney"
			local money = CMoney:new()
--			local tblWnd = {item:GetDlgChild("GoldCount"),item:GetDlgChild("GoldImage"),item:GetDlgChild("YinCount"),item:GetDlgChild("YinImage"),item:GetDlgChild("TongCount"),item:GetDlgChild("TongImage")}
--			money:ShowMoneyInfo(itemPrice,tblWnd)
			local gold, yin, tong = money:ChangeMoneyToGoldAndArgent(itemPrice)
			item:GetDlgChild("GoldCount"):SetWndText(gold)
			item:GetDlgChild("YinCount"):SetWndText(yin)
			item:GetDlgChild("TongCount"):SetWndText(tong)
		else
			local itemStr = "Item" .. i
			item:GetDlgChild(itemStr):SetWndText(ItemData[i])
		end
	end
end

--------脱穿装备时，对物品tooltip进行更新，保证装备的tooltip比较信息正确---------
function CConsignmentCommon:UpdateCSMSellListTooltip()
    for i=1, # self.m_CSMOrderInfoTbl do
        local iconWnd = self.m_CSMOrderItemWndTbl[i].IconWnd 
        local itemBtn = self.m_CSMOrderItemWndTbl[i].ItemBtn 
        local itemType = tonumber(self.m_CSMOrderInfoTbl[i].itemType)
        local itemName = self.m_CSMOrderInfoTbl[i].itemName
        local index = string.find(itemName, "*", 1, true)
		if index ~= nil then
			itemName = string.sub(itemName, index + 1)
		end
        local itemCount =self.m_CSMOrderInfoTbl[i].itemCount
        local itemID =  self.m_CSMOrderInfoTbl[i].itemID
        g_SetWndMultiToolTips(iconWnd,itemType,itemName,itemID,itemCount)     
        g_SetWndMultiToolTips(itemBtn,itemType,itemName,itemID,itemCount)     
    end
end

--物品类型、物品名称、物品ID
function CConsignmentCommon:GetItemShowColorInCSM(nItemType, sItemName, nItemID)
    local colorStr = ""
    if nItemType >= 5 and nItemType <= 9 then   --读取装备名称显示颜色
    		local itemDynInfo = g_DynItemInfoMgr:GetDynItemInfo(nItemID)
        colorStr = g_Tooltips:GetSuitNameColor(itemDynInfo,nItemType, sItemName)
    else    --普通物品显示颜色
        colorStr = g_Tooltips:GetQualityColor(nItemType, sItemName)      
    end
    return colorStr
end

-----------------------寄售交易所出售物品列表 物品配置 ------------------------
function CConsignmentCommon:GetCSMTreeListTbl()
    self.m_TreeDataInLan = {}   --读取多国语言表中关于寄售交易所树形列表的内容，用于显示在界面上
    self.m_TreeDataInPro = {}   --读取程序表中的寄售交易所树形列表内容, 用于程序中
    self.tree_config_data= {}
    local tbl = self.m_TreeDataInLan    
    local keys = Lan_CSMTreeList_Client:GetKeys()
    for i,v in pairs( keys ) do
        local sortTypeIndex = 1 --大的分类下的小分类数目  
        local Node = {}
        self.m_TreeDataInPro[i] = {}
        for k=1, 12 do
            local nodeStr
            local lanNodeStr 
            if k ~= 1 then
                nodeStr = string.format("%s%d", "ChildNode", k-1)
                lanNodeStr = string.format("%s%d", "DisChildNode", k-1)
            else
                lanNodeStr = "DisParentTreeNode"
                nodeStr = "ParentTreeNode"
            end
            local nodeInLan =   Lan_CSMTreeList_Client(i,lanNodeStr)
            local nodeInPro =   CSMTreeList_Client(i,nodeStr)
            if IsNumber(nodeInLan) ~= true then
                if nodeInLan == "" or nodeInLan == nil  then           
                    break
                else
                    local subNode = {i, sortTypeIndex}
                    tbl[nodeInLan] = subNode
                    self.m_TreeDataInPro[i][sortTypeIndex] = nodeInPro
                    if sortTypeIndex == 1 then
                        Node.text =  nodeInLan
                        Node.sub = {}
                    else
                        table.insert(Node.sub, {text = nodeInLan})
                    end
                    sortTypeIndex = sortTypeIndex + 1
                end
            end
        end
        table.insert(self.tree_config_data, Node)
    end    
end