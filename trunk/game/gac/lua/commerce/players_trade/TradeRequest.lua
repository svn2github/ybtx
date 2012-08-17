CPlayerTradeRequestWnd = class (SQRDialog)

---收到多个玩家发送的交易请求，显示多个请求的窗口---------------------
function CPlayerTradeRequestWnd:Ctor()
	self:CreateFromRes("PlayerTradeQuestWnd", g_GameMain)
	self:ShowWnd( true )
	self:InitWndChild()
end

function CPlayerTradeRequestWnd:InitWndChild()
	self.m_CloseBtn 		= self:GetDlgChild("Close")
	self.m_AcceptBtn		= self:GetDlgChild("Accept")
	self.m_DenyBtn			= self:GetDlgChild("Deny")
	self.m_TradeRequestList	= self:GetDlgChild("TradeQuestList")
	self.m_DenyAllBtn 		= self:GetDlgChild("DenyAll")
	self:EnableAcceptAndDenyBtn(false)
end

function CPlayerTradeRequestWnd:EnableAcceptAndDenyBtn(bEnable)
	self.m_AcceptBtn:EnableWnd(bEnable)
	self.m_DenyBtn:EnableWnd(bEnable)
end

function CPlayerTradeRequestWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if uMsgID == BUTTON_LCLICK then
		if Child == self.m_CloseBtn then
			self:ShowWnd(false)
		
		elseif 	Child == self.m_AcceptBtn then
			local choosedItem = self:GetSelectedItemOrder( )
			if choosedItem == nil then
				return
			else
				g_GameMain.m_PlayersTradeWnd.OtherPlayerID =  g_GameMain.m_PlayersTradeWnd.m_TradeRequestTbl[choosedItem][1]
				g_GameMain.m_PlayersTradeWnd:CloseTradeInvitationWnd()
				Gac2Gas:AgreeToTrade( g_Conn, g_GameMain.m_PlayersTradeWnd.m_TradeRequestTbl[choosedItem][1] )
			end
		
		elseif  Child == self.m_DenyBtn then		--拒绝某个交易请求
			local choosedItem = self:GetSelectedItemOrder( )
			if choosedItem == nil then
				return
			else
			    g_GameMain.m_PlayersTradeWnd:CloseTradeInvitationWnd()
				Gac2Gas:DenyedTrade( g_Conn, g_GameMain.m_PlayersTradeWnd.m_TradeRequestTbl[choosedItem][1] )
			end
			
		elseif Child == self.m_DenyAllBtn then		--拒绝所有交易请求
			g_GameMain.m_PlayersTradeWnd.m_TradeRequestTbl = {}
			g_GameMain.m_PlayersTradeWnd.PlayersTradeRequestItem = {}
			g_GameMain.m_PlayersTradeWnd:CloseTradeInvitationWnd()
			Gac2Gas:DenyAllPlayersTradeRequest(g_Conn)
		end	
	
	elseif uMsgID == ITEM_LBUTTONCLICK then
		local choosedItem = self:GetSelectedItemOrder( )
		if  choosedItem == nil then
			self:EnableAcceptAndDenyBtn(false)
		else
			self:EnableAcceptAndDenyBtn(true)
		end
	end
end

function CPlayerTradeRequestWnd:ShowTradeRequestListWnd()
	local requestListTbl = 	g_GameMain.m_PlayersTradeWnd.m_TradeRequestTbl
	self.m_TradeRequestList:DeleteAllItem()
	
	for i=1, # requestListTbl do
		if ( i == 1 ) then
			self.m_TradeRequestList:InsertColumn( 0, self.m_TradeRequestList:GetWndWidth())
		end
		self.m_TradeRequestList:InsertItem( i-1, 25 )
		
		local itemWnd = SQRDialog:new()
		local subItem = self.m_TradeRequestList:GetSubItem(i-1, 0)
		itemWnd:CreateFromRes("PlayerTradeRequestItem", subItem)
		itemWnd:ShowWnd( true )
		itemWnd:SetStyle( 0x60000000 )
		local itemWndChild = itemWnd:GetDlgChild("TradeItem")
		itemWndChild:SetStyle( 0x60000000 )
		itemWndChild:SetWndText(GetStaticTextClient(8317,requestListTbl[i][2]))
		table.insert(g_GameMain.m_PlayersTradeWnd.PlayersTradeRequestItem, itemWnd)		
	end
end

--到底是哪个订单被选中了
function CPlayerTradeRequestWnd:GetSelectedItemOrder( ) 
	local choosedItem = self.m_TradeRequestList:GetSelectItem( -1 ) + 1
	
	if choosedItem == 0 then
		return nil
	end
	
	return choosedItem
end