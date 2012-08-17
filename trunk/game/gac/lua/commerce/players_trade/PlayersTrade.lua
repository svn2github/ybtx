CPlayersTradeWnd = class ( SQRDialog )



CPlayerItemListWnd = class (CPlayersTradeWnd)
function CPlayerItemListWnd:Ctor()

end

function CPlayersTradeWnd.GetPlayersTradeRequestEnd()
	if g_GameMain.m_InviteTradeWnd ~= nil and g_GameMain.m_InviteTradeWnd:IsShow() then
		g_GameMain.m_InviteTradeWnd:ShowWnd( false )
	end
	if g_GameMain.m_TradeRequestWnd == nil then
		g_GameMain.m_TradeRequestWnd = CPlayerTradeRequestWnd:new()
	elseif g_GameMain.m_TradeRequestWnd:IsShow() == false then
		g_GameMain.m_TradeRequestWnd:ShowWnd(true)
	end
	g_GameMain.m_TradeRequestWnd:EnableAcceptAndDenyBtn(false)
	g_GameMain.m_TradeRequestWnd:ShowTradeRequestListWnd()
	g_GameMain.m_PlayersTradeWnd.m_WaitingRequestMsg = false
end

function CPlayersTradeWnd.RetGetTradeCess(Conn,recieverid,exchange_money,cess)
	if exchange_money > 0 then
		local function CallBack(g_GameMain,uButton)
			if uButton == MB_BtnOK then
				g_GameMain.m_ExchangeMsgBox = nil
				Gac2Gas:SubmitedTrade(g_Conn,recieverid)
			else
				g_GameMain.m_ExchangeMsgBox = nil
				g_GameMain.m_PlayersTradeWnd:ShowWnd(false)
				g_GameMain.m_WndMainBag:ShowWnd(false)
				g_GameMain.m_PlayersTradeWnd:VirtualExcludeWndClosed()
				g_GameMain.m_PlayersTradeWnd.IsActive = false
			end
			return true
		end
		local str = MsgBoxMsg(203,cess .. "%", g_MoneyMgr:ChangeMoneyToString(exchange_money,EGoldType.GoldCoin))
		g_GameMain.m_ExchangeMsgBox = MessageBox( g_GameMain, str, BitOr( MB_BtnOK, MB_BtnCancel),CallBack,g_GameMain)
	else
		Gac2Gas:SubmitedTrade(g_Conn,recieverid)
	end
end


--交易受邀者接受了交易请求
function CPlayersTradeWnd.ReturnAgreeToTrade( InvitorID ) 
	if g_GameMain.m_InviteTradeWnd ~= nil and g_GameMain.m_InviteTradeWnd:IsShow() then
		g_GameMain.m_InviteTradeWnd:ShowWnd( false )
	end
	if g_GameMain.m_TradeRequestWnd ~= nil and g_GameMain.m_TradeRequestWnd:IsShow() then
		g_GameMain.m_TradeRequestWnd:ShowWnd(false)
	end
	
	g_GameMain.m_PlayersTradeWnd:InitPlayersTrade( InvitorID ) 
	if g_GameMain.m_PlayersTradeWnd:IsShow() == false then
		if ( g_GameMain.m_WndMainBag:IsShow() == false ) then
			g_GameMain.m_WndMainBag:ShowWnd(true)
		end
		local invitor	=	g_GetPlayerInfo(InvitorID)
		local tradeName	=	invitor.m_Properties:GetCharName()
		local selfName	=	g_MainPlayer.m_Properties:GetCharName()
	
		g_GameMain.m_PlayersTradeWnd:ShowTradeWnd(tradeName ,selfName)		
	end
	g_GameMain.m_PlayersTradeWnd:SetFocus()
	--SetEvent( Event.Test.InviteeAcceptedTrade, true, true )
end

function CPlayersTradeWnd:ShowTradeWnd(tradeName ,selfName)

		g_GameMain.m_PlayersTradeWnd:ClearWndInfo()
		g_GameMain.m_PlayersTradeWnd.IsActive = true
		g_GameMain.m_PlayersTradeWnd.m_Other:SetWndText(tradeName..GetStaticTextClient(8346))
		g_GameMain.m_PlayersTradeWnd:ShowWnd(true)
end


function CPlayersTradeWnd.ReturnDenyedTrade() --交易受邀者拒绝了交易请求
	g_GameMain.m_PlayersTradeWnd:ShowWnd(false)
	if g_GameMain.m_InviteTradeWnd ~= nil and g_GameMain.m_InviteTradeWnd:IsShow() then
		g_GameMain.m_InviteTradeWnd:ShowWnd( false )
	end
	g_GameMain.m_PlayersTradeWnd.IsActive = false
	g_GameMain.m_PlayersTradeWnd:VirtualExcludeWndClosed()
	g_GameMain.m_PlayersTradeWnd:ShowWnd(false) 
	--  SetEvent( Event.Test.InviteeDenyedTrade, true, true )
end

function CPlayersTradeWnd.GotTradeInvitation( InvitorID, InvitorName )
	if g_GameMain.m_InviteTradeWnd == nil then
		g_GameMain.m_InviteTradeWnd = CTradeInvitationWnd:new( )
	end
	if g_GameMain.m_TradeRequestWnd ~= nil and g_GameMain.m_TradeRequestWnd:IsShow() then
		g_GameMain.m_TradeRequestWnd:ShowWnd(false)
	end

	if  g_GameMain.m_InviteTradeWnd:IsShow() == false then
		g_GameMain.m_InviteTradeWnd:ShowWnd( true )
		local invitationTextWnd = g_GameMain.m_InviteTradeWnd:GetDlgChild( "RequestText" )
		invitationTextWnd:SetWndText( GetStaticTextClient(8317,InvitorName))
		g_GameMain.m_PlayersTradeWnd.OtherPlayerID = InvitorID
	end
	g_GameMain.m_PlayersTradeWnd.m_WaitingRequestMsg = false
	g_GameMain.m_InviteTradeWnd:SetFocus()
	--SetEvent( Event.Test.GotTradeInvitation, true, true )
end

function CPlayersTradeWnd.ReturnCanceledTrade( )  --对方取消了交易
	g_GameMain.m_WndMainBag:ShowWnd(false)
	g_GameMain.m_PlayersTradeWnd.IsActive = false
	g_GameMain.m_PlayersTradeWnd:VirtualExcludeWndClosed()
	g_GameMain.m_PlayersTradeWnd:ShowWnd(false)
	--SetEvent( Event.Test.ReturnCanceledTrade, true, true )
end

function CPlayersTradeWnd.ReturnChoosedTradeMoney( moneyCount ) --对方输入了交易金币数目
	gac_gas_require "framework/common/CMoney"
	local money = CMoney:new()
	local wnd = g_GameMain.m_PlayersTradeWnd
	local tblWnd = {wnd.m_MineGold,wnd.m_Gold,wnd.m_MineSilver,wnd.m_Silver,wnd.m_MineTong,wnd.m_Tong}
	money:ShowMoneyInfo(moneyCount,tblWnd)
	
	g_GameMain.m_PlayersTradeWnd:LockTrade()
	--SetEvent( Event.Test.ReturnChoosedTradeMoney, true, true )
end

function CPlayersTradeWnd.ReturnChoosedInviteeTradeMoney( moneyCount )
	gac_gas_require "framework/common/CMoney"
	local money = CMoney:new()
	local wnd = g_GameMain.m_PlayersTradeWnd
	local tblWnd = {wnd.m_OtherGold,wnd.m_OtherGoldImage,wnd.m_OtherSilver,wnd.m_OtherSilverImage,wnd.m_OtherTong,wnd.m_OtherTongImage}
	money:ShowMoneyInfo(moneyCount,tblWnd)
end

--对方锁定了交易，服务器调用此函数
function  CPlayersTradeWnd.ReturnLockedTrade(  )
	local otherLockWnd = g_GameMain.m_PlayersTradeWnd.m_OtherLockBtn
	otherLockWnd:SetWndText( GetStaticTextClient(8318))
	g_Conn.OtherPlayerLock = true
	if g_Conn.MainPlayerLock == true then
		g_GameMain.m_PlayersTradeWnd.m_OkBtn:EnableWnd( true )
	end

	--SetEvent( Event.Test.ReturnLockedTrade, true, true )
end

--玩家提交了交易结果
function CPlayersTradeWnd.RetSubmitedTrade()
	g_GameMain.m_PlayersTradeWnd.IsActive = false
	g_GameMain.m_PlayersTradeWnd:ShowWnd(false)
	g_GameMain.m_PlayersTradeWnd:VirtualExcludeWndClosed()
	if g_GameMain.m_InviteTradeWnd ~= nil and g_GameMain.m_InviteTradeWnd:IsShow() then
		g_GameMain.m_InviteTradeWnd:ShowWnd( false )
	end
	g_GameMain.m_WndBagSlots.m_lcAddBagSlots:UpdateBagSlotsPos()
	--SetEvent( Event.Test.ReturnSubmitedTrade, true, true )  
end

--决定了交易的东西，服务器返回将交易物品显示在交易窗口
function CPlayersTradeWnd.ReturnChoosedTradeItem( viewIndex, itemType, itemName, itemCount, itemID )
	local tbl = g_GameMain.m_PlayersTradeWnd.MainPlayerItemList
	tbl[viewIndex] = {itemType, itemName, itemCount, itemID}
	local m_PlayersTradeWnd = g_GameMain.m_PlayersTradeWnd
	m_PlayersTradeWnd.MainPlayerItemWnd = m_PlayersTradeWnd.MainPlayerItemWnd + 1
	local SmallIcon = g_ItemInfoMgr:GetItemInfo( tonumber(itemType), itemName,"SmallIcon" )

	local iconWnd = m_PlayersTradeWnd.m_MineTradeItemListWnd.m_ItemBtnListTbl[viewIndex]
	g_LoadIconFromRes(SmallIcon, iconWnd, -1, IP_ENABLE, IP_ENABLE)
	if itemCount > 1 then
		iconWnd:SetWndText( "   " .. itemCount )
	end

	g_SetWndMultiToolTips(m_PlayersTradeWnd.m_MineTradeItemListWnd.m_ItemStaticListTbl[viewIndex],tonumber(itemType), itemName , itemID, itemCount)
	--SetEvent( Event.Test.ReturnChoosedTradeItem, true, true )
end

function CPlayersTradeWnd.ReturnChoosedTradeItemError( roomIndex, pos )
    local SrcCt = g_GameMain.m_BagSpaceMgr:GetSpaceRelateLc( roomIndex )
	local y, x = SrcCt:GetIndexByPos(pos, roomIndex)
	SrcCt:SetClickedWndState(x, y, roomIndex, true)
end

--对方放入了要交易的东西，服务端返回将交易物品显示在交易窗口
function CPlayersTradeWnd.ReturnInviteeChoosedItem( viewIndex, itemType, itemName, itemCount, itemID )
	local m_PlayersTradeWnd = g_GameMain.m_PlayersTradeWnd
	m_PlayersTradeWnd.OtherPlayerItemWnd = m_PlayersTradeWnd.OtherPlayerItemWnd + 1
	local tbl = g_GameMain.m_PlayersTradeWnd.OtherPlayerItemList 
	tbl[viewIndex] = {itemType, itemName, itemCount, itemID}
	local SmallIcon = g_ItemInfoMgr:GetItemInfo( tonumber(itemType), itemName,"SmallIcon" )
	
	local iconWnd = m_PlayersTradeWnd.m_OtherTradeItemListWnd.m_ItemBtnListTbl[viewIndex]
	g_WndMouse:ClearIconWnd(iconWnd)
	g_LoadIconFromRes(SmallIcon, iconWnd, -1, IP_ENABLE, IP_ENABLE)
	if itemCount > 1 then
		iconWnd:SetWndText( "   " .. itemCount )
	end

	g_SetWndMultiToolTips(m_PlayersTradeWnd.m_OtherTradeItemListWnd.m_ItemStaticListTbl[viewIndex], tonumber(itemType), itemName, itemID , itemCount)
	--SetEvent( Event.Test.ReturnInviteeChoosedItem, true, true )
end

--成功取消了要交易的物品,
function CPlayersTradeWnd.ReturnCancelChoosedItemSuc()
	local tbl = g_GameMain.m_PlayersTradeWnd.MainPlayerItemList
	--清除原有的交易物品信息显示
	for i =1,table.maxn( tbl ) do
		local iconWnd = g_GameMain.m_PlayersTradeWnd.m_MineTradeItemListWnd.m_ItemBtnListTbl[i]

		g_WndMouse:ClearIconWnd(iconWnd)
		
		g_GameMain.m_PlayersTradeWnd.m_MineTradeItemListWnd.m_ItemStaticListTbl[i]:SetMouseOverDescAfter( "" )
		iconWnd:SetWndText( "" )
	end

	g_GameMain.m_PlayersTradeWnd.MainPlayerItemWnd = 0
	g_GameMain.m_PlayersTradeWnd.MainPlayerItemList = {}
end

--取消某个交易的物品后，服务器返回重新排放的交易物品列表
function CPlayersTradeWnd.ReturnResetItemList( viewIndex, itemType, itemName, itemCount, itemID)
	local tbl = g_GameMain.m_PlayersTradeWnd.MainPlayerItemList
	node = {}
	node.itemType = itemType
	node.itemName = itemName
	node.itemCount = itemCount
	node.itemID		= itemID

	tbl[viewIndex] = node
	--table.insert( tbl, node )  
end

--在客户端显示交易的物品的信息，图标、名称、数量
function CPlayersTradeWnd.ReturnResetItemListEnd( itemWndChoosed)
	local m_PlayersTradeWnd = g_GameMain.m_PlayersTradeWnd
	local tbl = g_GameMain.m_PlayersTradeWnd.MainPlayerItemList
	for i, p in pairs( tbl ) do
		m_PlayersTradeWnd.MainPlayerItemWnd = m_PlayersTradeWnd.MainPlayerItemWnd + 1
		local SmallIcon = g_ItemInfoMgr:GetItemInfo( tonumber(p["itemType"]), p["itemName"],"SmallIcon" )

		local iconWnd = m_PlayersTradeWnd.m_MineTradeItemListWnd.m_ItemBtnListTbl[i]
		g_LoadIconFromRes(SmallIcon, iconWnd, -1, IP_ENABLE, IP_ENABLE)
		g_SetWndMultiToolTips(m_PlayersTradeWnd.m_MineTradeItemListWnd.m_ItemStaticListTbl[i],tonumber(p["itemType"]), p["itemName"], p["itemID"])
		if p["itemCount"] > 1 then
			iconWnd:SetWndText( "   " .. p["itemCount"] )
		end
	end
	local SrcCt = g_GameMain.m_BagSpaceMgr:GetSpaceRelateLc( g_StoreRoomIndex.PlayerBag )
	local gridTbl =  m_PlayersTradeWnd.SelectGridTable
	
	local x , y, roomIndex =  unpack(gridTbl[itemWndChoosed])

	table.remove( m_PlayersTradeWnd.SelectGridTable, itemWndChoosed ) 
	SrcCt:SetClickedWndState(x, y, roomIndex, true)
	g_GameMain.m_PlayersTradeWnd:EnableItemInBagFalse()
end



local function SetReplaceItemInfo(replacedRoomIndex, replacedPos, viewIndex, roomIndex, pos)
	local srcRoomCt = g_GameMain.m_BagSpaceMgr:GetSpaceRelateLc( roomIndex )
	local replacedRoomCt = g_GameMain.m_BagSpaceMgr:GetSpaceRelateLc( replacedRoomIndex )
    local y, x = replacedRoomCt:GetIndexByPos(replacedPos, replacedRoomIndex)
	replacedRoomCt:SetClickedWndState(x, y, replacedRoomIndex, true)
	local posY , posX = srcRoomCt:GetIndexByPos(pos, roomIndex)
	g_GameMain.m_PlayersTradeWnd.SelectGridTable[viewIndex] = {posX, posY, roomIndex}
end

--交易栏已满，玩家手中有物品，点击交易栏中有物品的地方，替换现有交易物品
function CPlayersTradeWnd.ReturnReplaceItem(  viewIndex, itemType, itemName, itemCount, itemID, roomIndex, pos, replacedRoomIndex, replacedPos )
	local m_PlayersTradeWnd = g_GameMain.m_PlayersTradeWnd
	local tbl = g_GameMain.m_PlayersTradeWnd.MainPlayerItemList
	tbl[viewIndex] = { itemType, itemName, itemCount, itemID }

	local iconWnd = m_PlayersTradeWnd.m_MineTradeItemListWnd.m_ItemBtnListTbl[viewIndex]
	local SmallIcon = g_ItemInfoMgr:GetItemInfo( itemType, itemName,"SmallIcon")

	g_WndMouse:ClearIconWnd(iconWnd)
	g_GameMain.m_PlayersTradeWnd.m_MineTradeItemListWnd.m_ItemStaticListTbl[viewIndex]:SetMouseOverDescAfter( "" )
	iconWnd:SetWndText( "" )

	g_LoadIconFromRes(SmallIcon, iconWnd, -1, IP_ENABLE, IP_ENABLE)
	g_SetWndMultiToolTips(g_GameMain.m_PlayersTradeWnd.m_MineTradeItemListWnd.m_ItemStaticListTbl[viewIndex],tonumber(itemType), itemName, itemID, itemCount)
	if itemCount > 1 then
		iconWnd:SetWndText( "   " .. itemCount)
	end
	
	SetReplaceItemInfo(replacedRoomIndex, replacedPos, viewIndex, roomIndex, pos)
end

function CPlayersTradeWnd.ReturnReplaceItemError(roomIndex, pos)
    local SrcCt = g_GameMain.m_BagSpaceMgr:GetSpaceRelateLc( roomIndex )
	local y, x = SrcCt:GetIndexByPos(pos, roomIndex)
	SrcCt:SetClickedWndState(x, y, roomIndex, true)
end

--创建玩家交易窗口
function CPlayersTradeWnd:CreatePlayersTradeWnd()
    self:CreateFromRes( "PlayerDealingWnd", g_GameMain )
	self.m_Other = self:GetDlgChild("Other")
	    
	self.m_OtherTradeItemListWnd = CPlayerItemListWnd:new()	--显示对方玩家的交易物品、金钱信息窗口
	self.m_OtherTradeItemListWnd:CreateFromRes("OtherPlayersDealingItemList", self)
	self.m_OtherTradeItemListWnd:ShowWnd( true )
	self.m_OtherTradeItemListWnd.m_ItemBtnListTbl = {}				--装物品格子Btn的tbl
	self.m_OtherTradeItemListWnd.m_ItemStaticListTbl = {}
	self.m_OtherTradeItemListWnd:SetStyle(0x40040000)				--设为子窗口

	self.m_MineTradeItemListWnd = CPlayerItemListWnd:new()		--显示自己的交易物品、金钱信息窗口
	self.m_MineTradeItemListWnd:CreateFromRes("PlayersDealingItemList", self)	
	self.m_MineTradeItemListWnd:ShowWnd( true )
	self.m_MineTradeItemListWnd.m_ItemBtnListTbl = {}   			--装物品格子Btn的tbl
	self.m_MineTradeItemListWnd.m_ItemStaticListTbl = {}
	self.m_MineTradeItemListWnd:SetStyle(0x40040000)				--设为子窗口
	self:LoadItemListBtn()
	self:InitPlayersTradeWndChild()
	self.m_OtherLockBtn:EnableWnd( false )
	self.m_OkBtn:EnableWnd( false )

	g_ExcludeWndMgr:InitExcludeWnd(self, "非互斥")
end

function CPlayersTradeWnd:Ctor()
    self:CreatePlayersTradeWnd()
end


function CPlayersTradeWnd:ParsePos(Pos, cols)
	Pos = Pos - 1
	local y = math.floor(Pos / cols)
	local x = Pos - y*cols
	return x , y
end

function CPlayersTradeWnd:LoadItemListBtn()
	local DefaultImage = WND_IMAGE_LIST:new()  
	local myDealingWnd = self.m_MineTradeItemListWnd
	local otherDealingWnd= self.m_OtherTradeItemListWnd
	local pos = CPos:new(30, 30)
    local itemBtnNum = 16		--交易物品栏要做多可交易的物品数目
    
    for i = 1, itemBtnNum do             
        local mineItemStatic = myDealingWnd:GetDlgChild("ItemStatic" .. i)
        local otherItemStatic = otherDealingWnd:GetDlgChild("ItemStatic" .. i)
   
        local mineItemBtn = SQRDialog:new()
        mineItemBtn:CreateFromRes("PlayerTradeItem", mineItemStatic)
        mineItemBtn:ShowWnd( true )
        local otherItemBtn = SQRDialog:new()
        otherItemBtn:CreateFromRes("PlayerTradeItem", otherItemStatic)
        otherItemBtn:ShowWnd( true )

		otherDealingWnd.m_ItemStaticListTbl[i] = otherItemStatic
		local rt = CFRect:new()
		otherDealingWnd:GetWndRect(rt)			
		otherDealingWnd:ResizeRectOneChild(rt,otherItemStatic)
		otherDealingWnd.m_ItemBtnListTbl[i] = otherItemBtn 
    
		myDealingWnd.m_ItemStaticListTbl[i] = mineItemStatic
		local rt = CFRect:new()
		myDealingWnd:GetWndRect(rt)			
		myDealingWnd:ResizeRectOneChild(rt,mineItemStatic)
		myDealingWnd.m_ItemBtnListTbl[i] = mineItemBtn 
    end
end

--初始化交易窗口中的子控件
function CPlayersTradeWnd:InitPlayersTradeWndChild()
	self.m_MeLockBtn			=	self:GetDlgChild("MeLockBtn")
	self.m_OtherLockBtn			=	self:GetDlgChild("OtherLockBtn")
	self.m_OkBtn				=	self:GetDlgChild("OkBtn")
	self.m_Close				=	self:GetDlgChild("Close")
	self.m_CancelBtn			=	self:GetDlgChild("CancelBtn") 
	self.m_OtherGold			=	self.m_OtherTradeItemListWnd:GetDlgChild("JinCount")
	self.m_OtherSilver			=	self.m_OtherTradeItemListWnd:GetDlgChild("YinCount")
	self.m_OtherTong			=	self.m_OtherTradeItemListWnd:GetDlgChild("TongCount")	
	self.m_MineGold				=	self.m_MineTradeItemListWnd:GetDlgChild("JinCount")
	self.m_MineSilver			=	self.m_MineTradeItemListWnd:GetDlgChild("YinCount")
	self.m_MineTong				=	self.m_MineTradeItemListWnd:GetDlgChild("TongCount")	
	
	self.m_Gold				=	self.m_MineTradeItemListWnd:GetDlgChild("Jing")
	self.m_Silver			=	self.m_MineTradeItemListWnd:GetDlgChild("Yin")
	self.m_Tong				=	self.m_MineTradeItemListWnd:GetDlgChild("Tong")	
    self.m_OtherGoldImage		=	self.m_OtherTradeItemListWnd:GetDlgChild("Jing")
	self.m_OtherSilverImage	    =	self.m_OtherTradeItemListWnd:GetDlgChild("Yin")
	self.m_OtherTongImage		=	self.m_OtherTradeItemListWnd:GetDlgChild("Tong")	
	
end

--接受交易请求后初始化; 参数:所点玩家的ID
function CPlayersTradeWnd:InitPlayersTrade( OtherPlayerID )
	g_Conn.OtherPlayerLock = false
	g_Conn.MainPlayerLock = false
	self.MainPlayerItemWnd = 0
	self.OtherPlayerItemWnd = 0
	self.MainPlayerItemList = {}
	self.OtherPlayerItemList = {}
	self.SelectGridTable = {}
	self.IsActive = false
	self.OtherPlayerID = OtherPlayerID
	
	self.m_Gold:ShowWnd(true)
	self.m_Silver:ShowWnd(true)
	self.m_Tong:ShowWnd(true)
	self.m_OtherGoldImage:ShowWnd(true)
	self.m_OtherSilverImage:ShowWnd(true)
	self.m_OtherTongImage:ShowWnd(true)
	
    self.m_MineGold:ShowWnd(true)
	self.m_MineSilver:ShowWnd(true)
	self.m_MineTong:ShowWnd(true)
	self.m_OtherGold:ShowWnd(true)
	self.m_OtherSilver:ShowWnd(true)
	self.m_OtherTong:ShowWnd(true)
end

function CPlayersTradeWnd:ClearWndInfo()
    local wnd = g_GameMain.m_PlayersTradeWnd
	for i=1, 16 do 		--一次最多可交易16个物品
		local mineItemBtn = wnd.m_MineTradeItemListWnd.m_ItemBtnListTbl[i]
		mineItemBtn:SetWndText("")
		wnd.m_MineTradeItemListWnd.m_ItemStaticListTbl[i]:SetMouseOverDescAfter( "" )
		g_WndMouse:ClearIconWnd(mineItemBtn)
		
		local otherItemBtn = wnd.m_OtherTradeItemListWnd.m_ItemBtnListTbl[i]
		otherItemBtn:SetWndText("")
		g_WndMouse:ClearIconWnd(otherItemBtn)
		wnd.m_OtherTradeItemListWnd.m_ItemStaticListTbl[i]:SetMouseOverDescAfter( "" )
	end
	
	self.m_MeLockBtn:SetWndText("   " .. GetStaticTextClient(1048))
	self.m_OtherLockBtn:SetWndText("   " .. GetStaticTextClient(1048))
	self.m_MeLockBtn:EnableWnd( true )
	self.m_OtherLockBtn:EnableWnd( false )
	self.m_OtherGold:SetWndText("")
	self.m_OtherSilver:SetWndText("")
	self.m_OtherTong:SetWndText("")
	self.m_MineGold:SetWndText("")
	self.m_MineSilver:SetWndText("")
	self.m_MineTong:SetWndText("")
	self.m_MineGold:EnableWnd(true)
	self.m_MineSilver:EnableWnd(true)
	self.m_MineTong:EnableWnd(true)
	self.m_OkBtn:EnableWnd( false )
end

function CPlayersTradeWnd:CloseTradeInvitationWnd()
    if g_GameMain.m_TradeRequestWnd ~= nil and g_GameMain.m_TradeRequestWnd:IsShow() then
        g_GameMain.m_TradeRequestWnd:ShowWnd(false)
    end
    if g_GameMain.m_InviteTradeWnd ~= nil and g_GameMain.m_InviteTradeWnd:IsShow() then
       g_GameMain.m_InviteTradeWnd:ShowWnd(false) 
    end
end


--手上有物品时，点击玩家交易窗口，将物品放在交易窗口里
function CPlayersTradeWnd:SellItem( context, Child)
	local function Callback(g_GameMain,uButton)
		g_GameMain.m_MsgBox = nil
		return true
	end
	local fromRoom, fromRow, fromCol  = context[1], context[2], context[3]
	if fromRoom == g_AddRoomIndexClient.PlayerExpandBag then --如果是从背包栏拿出的包裹
		g_WndMouse:ClearCursorAll()
		return
	end
	local SrcCt = g_GameMain.m_BagSpaceMgr:GetSpaceRelateLc( fromRoom )
	local srcPos = SrcCt:GetPosByIndex( fromRow, fromCol, fromRoom )	
	local grid = g_GameMain.m_BagSpaceMgr:GetGridByRoomPos(fromRoom, srcPos)
	local nBigId, Index, bindingType = grid:GetType()
	local itemID = grid:GetItem():GetItemID()
	if g_ItemInfoMgr:IsQuestItem(nBigId) then		--不能出售任务物品
		g_WndMouse:ClearCursorAll()
		self.m_MsgBox = MessageBox(self, MsgBoxMsg(200), MB_BtnOK)
		return
	end
	local dynInfo =  g_DynItemInfoMgr:GetDynItemInfo(itemID)
    if dynInfo and dynInfo.BindingType == 2 then --物品绑定类型为：绑定
		g_WndMouse:ClearCursorAll()
		g_GameMain.m_MsgBox = MessageBox(g_GameMain, MsgBoxMsg(201), MB_BtnOK, Callback, g_GameMain)
		return
	end

	local count = grid:Size()
	g_WndMouse:ClearCursorAll()
	local cols = 6
	local wnd = g_GameMain.m_PlayersTradeWnd
	if wnd.MainPlayerItemWnd < 16 then --如果交易栏未满，则将手中的物品放入交易栏
		local y, x = SrcCt:GetIndexByPos(srcPos, fromRoom)
		SrcCt:SetClickedWndState(x, y, fromRoom, false)
		table.insert( wnd.SelectGridTable, {x, y, fromRoom})
		Gac2Gas:ChoosedTradeItem( g_Conn, wnd.OtherPlayerID, fromRoom, srcPos, count, itemID )
	else
		local viewIndex = wnd:WhichItemClicked( Child )
		--不管是右键点击包裹栏里的物品还是将物品拖到交易窗口，只要没有选中具体替换的物品就不做任何响应
		if viewIndex == 0 then
			return
		end
   
	    local y, x = SrcCt:GetIndexByPos(srcPos, fromRoom)
	    SrcCt:SetClickedWndState(x, y, fromRoom, false) 
		Gac2Gas:ReplaceItem( g_Conn, wnd.OtherPlayerID, viewIndex, fromRoom, srcPos, count, itemID )
	end
	g_ItemInfoMgr:PlayItemSound(nBigId, Index)
end	 

--交易的钱可以正常交易
function CPlayersTradeWnd:LockTrade()
	g_Conn.MainPlayerLock = true
	self.m_MeLockBtn:SetWndText("      " .. GetStaticTextClient(1049))
	self.m_MeLockBtn:EnableWnd(false)
	self.m_MineGold:EnableWnd(false)
	self.m_MineSilver:EnableWnd(false)
	self.m_MineTong:EnableWnd(false)
	Gac2Gas:HaveLockedTrade( g_Conn, self.OtherPlayerID ) 
	if g_Conn.OtherPlayerLock == true then
		self.m_OkBtn:EnableWnd( true )
	end
end

--玩家交易面板关闭时，将包裹中相应置为不可点状态的格子置为可点状态
function CPlayersTradeWnd:VirtualExcludeWndClosed()
	if g_GameMain.m_PlayersTradeWnd.IsActive then
		Gac2Gas:CanceledTrade( g_Conn, g_GameMain.m_PlayersTradeWnd.OtherPlayerID )
	end
	
	if g_GameMain.m_PlayersTradeWnd.SelectGridTable == nil then
		return
	end

	for i=1, table.maxn( g_GameMain.m_PlayersTradeWnd.SelectGridTable ) do
		if g_GameMain.m_PlayersTradeWnd.SelectGridTable[i] ~= nil then
			local x, y, roomIndex = unpack(g_GameMain.m_PlayersTradeWnd.SelectGridTable[i])
			local SrcCt = g_GameMain.m_BagSpaceMgr:GetSpaceRelateLc( g_StoreRoomIndex.PlayerBag )
			SrcCt:SetClickedWndState(x, y, roomIndex, true)
		end
	end
end

function CPlayersTradeWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	local state, context = g_WndMouse:GetCursorState()

	if uMsgID == BUTTON_LCLICK then
		if Child == self.m_Close  and state == ECursorState.eCS_Normal  then
			self:ShowWnd(false)
			g_GameMain.m_WndMainBag:ShowWnd(false)
			self:VirtualExcludeWndClosed()
			self.IsActive = false

		elseif Child == self.m_CancelBtn  and state == ECursorState.eCS_Normal  then
			self:ShowWnd(false)
			g_GameMain.m_WndMainBag:ShowWnd(false)
			self:VirtualExcludeWndClosed()
			self.IsActive = false
		
		elseif Child == self.m_MeLockBtn   and state == ECursorState.eCS_Normal  then
			local bEnough, tradeMoneyCount = self:CheckTradeMoneyEnough()
			local money = tonumber(tradeMoneyCount)
			if bEnough then
				Gac2Gas:ChoosedTradeMoney( g_Conn, self.OtherPlayerID, money )
			else
				self.m_MsgBox = MessageBox(self, MsgBoxMsg(202), MB_BtnOK)
				return
			end

		elseif Child == self.m_OkBtn and state == ECursorState.eCS_Normal  then
			Gac2Gas:GetTradeCess(g_Conn,self.OtherPlayerID)
		elseif state == ECursorState.eCS_PickupItem then --手上有物品
			self:SellItem( context, Child )
		end
	
	elseif (uMsgID == WND_NOTIFY ) then
		if (WM_IME_CHAR == uParam1 or WM_CHAR == uParam1) then
			if Child == g_GameMain.m_PlayersTradeWnd.m_MineGold then
                local money = CMoney:new()
                money:CheckInputMoneyValidate(Child, 1) 
			   
			elseif Child == g_GameMain.m_PlayersTradeWnd.m_MineSilver 
			        or Child == g_GameMain.m_PlayersTradeWnd.m_MineTong then
        	    local money = CMoney:new()
        	    money:CheckInputMoneyValidate(Child, 2)
			end
		end
	
	elseif uMsgID == BUTTON_RIGHTCLICKDOWN then		--右键点击取回物品
		if g_Conn.MainPlayerLock == false  then  --未锁定交易时，可将物品取回，否则不可取回
			local itemWndChoosed = self:WhichItemClicked( Child )
			self:CancelChoosedItem( itemWndChoosed )
		end
	end
end

function CPlayersTradeWnd:CheckTradeMoneyEnough()
	local goldCount = tonumber( self.m_MineGold:GetWndText() )
	local silverCount = tonumber( self.m_MineSilver:GetWndText() )
	local tongCount = tonumber( self.m_MineTong:GetWndText() )
	
	gac_gas_require "framework/common/CMoney"
	local money = CMoney:new()
	local moneyCount = money:ChangeGoldAndArgentToMoney(goldCount, silverCount, tongCount)
	if moneyCount > g_MainPlayer.m_nMoney then
		return false
	end
	return true, moneyCount
end

--交易栏中的那个物品放置的位置被点击了
function CPlayersTradeWnd:WhichItemClicked( ClickedItemWnd )
	local wnd = g_GameMain.m_PlayersTradeWnd
	local itemWndChoosed = 0 
	for i=1, 16 do
		if ClickedItemWnd == wnd.m_MineTradeItemListWnd.m_ItemStaticListTbl[i] then
			itemWndChoosed = i
			break
		end
	end
	
	return itemWndChoosed
end

--点击的控件是否有要交易的物品，如果有则取消要交易的物品
function CPlayersTradeWnd:CancelChoosedItem( itemWndChoosed )
	local wnd = g_GameMain.m_PlayersTradeWnd
	if itemWndChoosed > 0 and itemWndChoosed <= wnd.MainPlayerItemWnd then --表明点击的控件上已放有物品
		Gac2Gas:CancelChoosedItem( g_Conn, wnd.OtherPlayerID, itemWndChoosed )
	end
end

--将玩家交易物品格子中添加的物品，在物品栏中对应的位置置为不可点击状态
function CPlayersTradeWnd:EnableItemInBagFalse()
	for i=1, table.maxn( g_GameMain.m_PlayersTradeWnd.SelectGridTable ) do
		if g_GameMain.m_PlayersTradeWnd.SelectGridTable[i] ~= nil then
			local x, y, roomIndex = unpack(g_GameMain.m_PlayersTradeWnd.SelectGridTable[i])
			local SrcCt = g_GameMain.m_BagSpaceMgr:GetSpaceRelateLc( g_StoreRoomIndex.PlayerBag )
			SrcCt:SetClickedWndState(x, y, roomIndex, false)
		end
	end
end

--------------------脱穿装备时，对物品tooltip进行更新，保证装备的tooltip比较信息正确---------

function CPlayersTradeWnd:UpdatePlayersTradeToo1tip()
    local mineItemsTbl = self.MainPlayerItemList  ---itemType, itemName, itemCount, itemID
    local otherItemsTbl = self.OtherPlayerItemList
    local mineIconWndsTbl = self.m_MineTradeItemListWnd.m_ItemStaticListTbl
    local otherIconWndsTbl = self.m_OtherTradeItemListWnd.m_ItemStaticListTbl
    for i=1, # mineItemsTbl do
       local itemType = mineItemsTbl[i][1] 
       local itemName = mineItemsTbl[i][2] 
       local itemCount = mineItemsTbl[i][3]
       local itemID = mineItemsTbl[i][4]  
       local iconWnd = mineIconWndsTbl[i]
       g_SetWndMultiToolTips(iconWnd,tonumber(itemType), itemName , itemID, itemCount)
    end
    for i=1, # otherItemsTbl do
       local itemType = otherItemsTbl[i][1] 
       local itemName = otherItemsTbl[i][2] 
       local itemCount = otherItemsTbl[i][3]
       local itemID = otherItemsTbl[i][4]  
       local iconWnd = otherIconWndsTbl[i]
       g_SetWndMultiToolTips(iconWnd,tonumber(itemType), itemName , itemID, itemCount)
    end 
end
