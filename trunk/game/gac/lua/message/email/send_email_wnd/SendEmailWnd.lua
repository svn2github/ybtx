gac_require "message/message_box/MsgBoxMsg"

CSendEmailWnd = class(SQRDialog) 

--创建发件箱窗口
function CSendEmailWnd:Ctor()
	self:CreateFromRes("SendEmail",g_GameMain) 
	self:InitSendEmailWndChild()
	self.m_EmailTitle = self:GetDlgChild("EmailTitle")
	self.m_EmailTitle:SetMaxTextLenght(16)
	self.m_ReceiverName:SetIsSendMsgToParent(true)
	self.m_EmailTitle:SetIsSendMsgToParent(true)
	self.m_EmailText = self:GetDlgChild("EmailText")
	self.m_EmailText:SetMouseWheel(true)
	self.m_SendBtn = self.m_Send
	self.m_SendBtn:EnableWnd(false)
	self.Goods_count = nil
	self.m_emailGoodsTbl = {}--元素{格子Email_index,物品名,物品id<唯一>}
	self.GoodsID = {}        --识别同一个物品不能重复添加
	self.Goodsname = nil     --识别同种物品才可以添加到同一个格子中
	self.GoodsType = nil
	self.m_emailGoods = nil	--目前选中的物品
	self.m_ImagineList = CReceiveImagineList:new()
	self.m_ImagineList:CreateFromRes("SendEmailImagineList",self)
	self.m_ImagineList:ShowWnd( true )
	self.m_ImagineList:InitReceiveImagineListChild()
	self.m_ImagineList:ShowWnd(false)
	self.m_SenderList = CFriendsListWnd:new()
	self.m_SenderList:CreateFromRes("senderList",self)

	g_ExcludeWndMgr:InitExcludeWnd(self, 2, false, {g_GameMain.m_EmailBox})
end

--用成员变量存储发件箱窗口中的子控件
function CSendEmailWnd:InitSendEmailWndChild()
	self.m_Cancel			=	self:GetDlgChild("Cancel")
	self.m_Close			=	self:GetDlgChild("Close")
	self.m_Send				=	self:GetDlgChild("Send")
	self.m_FriendsList		=	self:GetDlgChild("FriendsList")
	self.m_Goods_01			=	self:GetDlgChild("Goods_01")
	self.m_Goods_02			=	self:GetDlgChild("Goods_02")
	self.m_Goods_03			=	self:GetDlgChild("Goods_03")
	self.m_Goods_04			=	self:GetDlgChild("Goods_04")
	self.m_GoodsName_01			=	self:GetDlgChild("ItemName1")
	self.m_GoodsName_02			=	self:GetDlgChild("ItemName2")
	self.m_GoodsName_03			=	self:GetDlgChild("ItemName3")
	self.m_GoodsName_04			=	self:GetDlgChild("ItemName4")
	self.m_ReceiverName		=	self:GetDlgChild("ReceiverName")
	self.m_Jin				=	self:GetDlgChild("Jin")
	self.m_Yin				=	self:GetDlgChild("Yin")
	self.m_Tong				=	self:GetDlgChild("Tong")
	self.m_OpenBag = self:GetDlgChild("OpenBag")
end	

--发件箱窗口事件响应
function CSendEmailWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	if(not self.m_ReceiverName:IsFocus() )then
		self.m_ImagineList:ShowWnd(false)
	end
	if uMsgID == VK_TAB then
		if self.m_ReceiverName:IsFocus() then
			self.m_EmailTitle:SetFocus()
		elseif self.m_EmailTitle:IsFocus() then
			self.m_ReceiverName:SetFocus()
		else
			self.m_ReceiverName:SetFocus()
		end
	end
	if(uMsgID == BUTTON_LCLICK)then
		if g_MainPlayer.m_ItemBagLock and (not (Child == self.m_Cancel or Child == self.m_Close)) then
				MsgClient(160032)
				return 
		end
		if(Child == self.m_Cancel or Child == self.m_Close) then
			self.m_SenderList:ShowWnd(false)
			self:ShowWnd(false)
			Gac2Gas:GetMailList(g_Conn)
		elseif(Child == self.m_Send)then
			if self:CheckAllCharAreBlankChar(self.m_EmailTitle)  then
				self:GetSendBoxContent()
				self.m_Send:EnableWnd(false)
			else
				self.m_MsgBox = MessageBox(self, MsgBoxMsg(251), MB_BtnOK)
			end

			if self.m_SenderList:IsShow() then
				self.m_SenderList:ShowWnd(false)
			end
			Gac2Gas:GetMailList(g_Conn)
		elseif(Child == self.m_OpenBag) then
			if not g_GameMain.m_WndMainBag:IsShow() then
				g_GameMain.m_WndMainBag:ShowWnd(true)
			end
		elseif(Child == self.m_FriendsList)then
			if(self.m_SenderList:IsShow() == false)then

				self.m_SenderList:InitFriendListChild()
				self.m_SenderList:ShowWnd(true)
			else
				self.m_SenderList:ShowWnd(false)
			end
		elseif(Child == self.m_Goods_01)then
			self:ClickGoodsButton("Goods_01", nil)
		elseif(Child == self.m_Goods_02)then
			self:ClickGoodsButton("Goods_02", nil)
		elseif(Child == self.m_Goods_03)then
			self:ClickGoodsButton("Goods_03", nil)
		elseif(Child == self.m_Goods_04)then
			self:ClickGoodsButton("Goods_04", nil)
		end
	elseif uMsgID == BUTTON_RIGHTCLICKDOWN then
		if(Child == self.m_Goods_01)then
			self:RClickGoodsButton("Goods_01")
		elseif(Child == self.m_Goods_02)then
			self:RClickGoodsButton("Goods_02")
		elseif(Child == self.m_Goods_03)then
			self:RClickGoodsButton("Goods_03")
		elseif(Child == self.m_Goods_04)then
			self:RClickGoodsButton("Goods_04")
		end
	elseif (uMsgID == WND_NOTIFY ) then
		if (WM_IME_CHAR == uParam1 or WM_CHAR == uParam1) then
			if(Child == self.m_ReceiverName)then
				local Rect=CFRect:new()
				self.m_ImagineList:GetWndRect(Rect)
				local uWidth = Rect.right - Rect.left
				local uHeight = Rect.bottom - Rect.top
				local ParentRect=CFRect:new()
				self:GetWndRect(ParentRect)
				Rect.left 	= 93 + ParentRect.left
				Rect.top  	= 70 + ParentRect.top
				Rect.right	= Rect.left + uWidth
				Rect.bottom	= Rect.top + uHeight
				self.m_ImagineList:SetWndRect(Rect)
				self.WndMailInfo = self.m_ImagineList.m_friendslist
				self.WndMailInfo:InsertColumn(0,self.WndMailInfo:GetWndWidth())
				local name = self.m_ReceiverName:GetWndText()
				if(self.WndMailInfo:GetItemCount()==0) then
					self.m_ImagineList:ShowWnd(false)
				end
				if(name ~= "") then
					for i=1,self.WndMailInfo:GetItemCount() do
						local friendname = string.sub(self.WndMailInfo:GetSubItem(i-1,0):GetWndText(),0,string.len(name))
						if(name == friendname) then
							--Do Nothing
						else
							self.WndMailInfo:DeleteItem(i)
							return
						end
					end
					if(self.WndMailInfo:GetItemCount() == 0) then--没有相匹配的 ,关闭
						self.m_ImagineList:ShowWnd(false)
					end
					local tblFriendList = g_GameMain.m_AssociationBase.m_tblFriendListClass
					for i = 1, #tblFriendList do
						for j = 1, #(tblFriendList[i]) do
							local friendName = string.sub(tblFriendList[i][j].playerName, 0, string.len(name))
							if(name ==friendName) then
								for j=1, self.WndMailInfo:GetItemCount() do
									if(self.WndMailInfo:GetSubItem(j-1,0):GetWndText() == friendName) then
										return
									end
								end
								local itemCount = self.WndMailInfo:GetItemCount()
								self.WndMailInfo:InsertItem(itemCount, 20)
								self.WndMailInfo:GetSubItem( itemCount, 0 ):SetWndText(friendName)
								self.m_ImagineList:ShowWnd(true)
								self.m_ImagineList:SetFocus()
							end
						end
					end
					if(self.WndMailInfo:GetItemCount()==0) then
						self.m_ImagineList:ShowWnd(false)
					end
					self.m_ReceiverName:SetFocus()
				end
			elseif (Child == self.m_Jin )then
				local money = CMoney:new()
				money:CheckInputMoneyValidate(Child, 1,"SendMail")
			elseif ( Child == self.m_Yin or Child == self.m_Tong)then
				local money = CMoney:new()
				money:CheckInputMoneyValidate(Child, 2,"SendMail")
			end
		end
	end
	local strReceiverName = self.m_ReceiverName:GetWndText()
	local strEmailTitle = self.m_EmailTitle:GetWndText()
	if(string.len(strReceiverName) > 0 and string.len(strEmailTitle) > 0) then
		if(string.len(self.m_EmailText:GetWndText()) >0 or tonumber(self.m_Jin:GetWndText()) ~= nil or
			tonumber(self.m_Jin:GetWndText()) ~= nil or
			tonumber(self.m_Jin:GetWndText()) ~= nil or
			 #(g_GameMain.m_SendBox.m_emailGoodsTbl) > 0)then
			self.m_SendBtn:EnableWnd(true)
		else
			self.m_SendBtn:EnableWnd(false)
		end
	else
		self.m_SendBtn:EnableWnd(false)
	end
end
--邮件物品附件
function CSendEmailWnd:CSMEmailGoods(wndObject,context)
	local function Callback(g_GameMain,uButton)
		g_GameMain.m_MsgBox = nil
		return true
	end
	local fromRoom, fromRow, fromCol = context[1], context[2], context[3]
	if fromRoom == g_AddRoomIndexClient.PlayerExpandBag then --如果是从背包栏拿出的包裹
		g_WndMouse:ClearCursorAll()
		return
	end
	local SrcCt = g_GameMain.m_BagSpaceMgr:GetSpaceRelateLc( g_StoreRoomIndex.PlayerBag )
	local srcPos = SrcCt:GetPosByIndex( fromRow, fromCol, fromRoom ) 
	local grid = g_GameMain.m_BagSpaceMgr:GetGridByRoomPos(fromRoom, srcPos)
	local nItemID = grid:GetItem():GetItemID()
	local BigId, Index, bindingType = grid:GetType()
	if g_ItemInfoMgr:IsQuestItem(BigId) then		--不能邮寄任务物品
		g_WndMouse:ClearCursorAll()
		g_GameMain.m_SendBox.m_MsgBox = MessageBox(g_GameMain.m_SendBox, MsgBoxMsg(259), MB_BtnOK)
		return
	end

	if bindingType == 2 then
		--物品绑定类型为：绑定
		g_WndMouse:ClearCursorAll()
		g_GameMain.m_MsgBox = MessageBox(g_GameMain, MsgBoxMsg(260), MB_BtnOK,Callback,g_GameMain)
		return
	end
	local count = grid:Size()
	g_WndMouse:ClearCursorAll()
	local mailTitle = g_GameMain.m_SendBox.m_EmailTitle
	local DisplayName = g_ItemInfoMgr:GetItemLanInfo(BigId, Index,"DisplayName")
	if mailTitle:GetWndText() == ""  then
		mailTitle:SetWndText(DisplayName)
	end
	g_GameMain.m_SendBox.Goods_count = count --物品数量
	g_GameMain.m_SendBox.GoodsID = grid:GetItemID(count)

	if not grid:Empty() then
	    g_GameMain.m_SendBox.GoodsType =BigId
		g_GameMain.m_SendBox.Goodsname =Index
		local SmallIcon = grid:GetIcon()
		g_GameMain.m_SendBox.m_emailGoods = SmallIcon
	else
		g_GameMain.m_SendBox.m_emailGoods = nil
		g_GameMain.m_SendBox.Goodsname = nil
		g_GameMain.m_SendBox.GoodsType =nil
	end
end

function CSendEmailWnd:CheckSend()
	local strReceiverName = g_GameMain.m_SendBox.m_ReceiverName:GetWndText()
	local strEmailTitle = g_GameMain.m_SendBox.m_EmailTitle:GetWndText()
	if(string.len(strReceiverName) > 0 and string.len(strEmailTitle) > 0) then
		if(string.len(g_GameMain.m_SendBox.m_EmailText:GetWndText()) >0 or tonumber(g_GameMain.m_SendBox.m_Jin:GetWndText()) ~= nil 
			or tonumber(g_GameMain.m_SendBox.m_Yin:GetWndText()) ~= nil
			or tonumber(g_GameMain.m_SendBox.m_Tong:GetWndText()) ~= nil
			or #(g_GameMain.m_SendBox.m_emailGoodsTbl) > 0)then
			g_GameMain.m_SendBox.m_SendBtn:EnableWnd(true)
		else
			g_GameMain.m_SendBox.m_SendBtn:EnableWnd(false)
		end
	else
		g_GameMain.m_SendBox.m_SendBtn:EnableWnd(false)
	end
	g_GameMain.m_SendBox.m_emailGoods = nil
	g_GameMain.m_SendBox.GoodsID = {}
end

function CSendEmailWnd:RClickGoodsButton(buttonname)
	g_GameMain.m_WndMainBag:SetFocus()
	local Email_index
	local itemname_wnd_str
	local WndName
	if(buttonname == "Goods_01") then
		Email_index =1
		item_name_wnd= "ItemName1"
		WndName = g_GameMain.m_SendBox.m_GoodsName_01
	elseif(buttonname == "Goods_02") then
		Email_index =2
		item_name_wnd= "ItemName2"
		WndName = g_GameMain.m_SendBox.m_GoodsName_02
	elseif(buttonname == "Goods_03") then
		Email_index =3
		item_name_wnd= "ItemName3"
		WndName = g_GameMain.m_SendBox.m_GoodsName_03
	elseif(buttonname == "Goods_04") then
		Email_index =4
		item_name_wnd= "ItemName4"
		WndName = g_GameMain.m_SendBox.m_GoodsName_04
	end
	local itemnameWnd=g_GameMain.m_SendBox:GetDlgChild(item_name_wnd)
	local Wnd = g_GameMain.m_SendBox:GetDlgChild(buttonname)
	
	if WndName and WndName.m_ItemType and WndName.m_ItemName then
		g_ItemInfoMgr:PlayItemSound(WndName.m_ItemType, WndName.m_ItemName)
	end
		
	local count={}
	for i=1, #(g_GameMain.m_SendBox.m_emailGoodsTbl) do
		if(g_GameMain.m_SendBox.m_emailGoodsTbl[i][1] == Email_index)then
			table.insert(count,i)
		end
	end
	for i =1, #(count)do
		table.remove(g_GameMain.m_SendBox.m_emailGoodsTbl,count[1])    --从表中删除
	end
	g_DelWndImage(Wnd, 1, IP_ENABLE, IP_CLICKDOWN)   --取消添加物品附件
	g_GameMain.m_SendBox:CSMEnableEmailGoods( Wnd,Email_index )
	Wnd:SetWndText("")
	itemnameWnd:SetWndText("") 
	Wnd:SetMouseOverDesc("")
	g_GameMain.m_SendBox:CheckSend()
	
end
--检查邮件标题或者邮件正文是否全为空格符
--参数：标题窗口或者邮件正文窗口
function CSendEmailWnd:CheckAllCharAreBlankChar(textWnd)
	local contentNotAllBlankChar = false
	local content = textWnd:GetWndText()
	local contentLen = string.len(content)
	for i=1, contentLen do
		local char = string.sub(content,i, i)
		if char ~= ' ' and char~= '\n' then
			contentNotAllBlankChar = true 
			break
		end
	end
	return contentNotAllBlankChar
end


function CSendEmailWnd:CleanAllGoodsInformation()
	g_GameMain.m_SendBox.m_Goods_01:SetWndText("")
	g_GameMain.m_SendBox.m_Goods_02:SetWndText("")
	g_GameMain.m_SendBox.m_Goods_03:SetWndText("")
	g_GameMain.m_SendBox.m_Goods_04:SetWndText("")
	g_GameMain.m_SendBox.m_GoodsName_01:SetWndText("")
	g_GameMain.m_SendBox.m_GoodsName_02:SetWndText("")
	g_GameMain.m_SendBox.m_GoodsName_03:SetWndText("")
	g_GameMain.m_SendBox.m_GoodsName_04:SetWndText("")
	g_GameMain.m_SendBox.m_Goods_01:SetMouseOverDesc("")
	g_GameMain.m_SendBox.m_Goods_02:SetMouseOverDesc("")
	g_GameMain.m_SendBox.m_Goods_03:SetMouseOverDesc("")
	g_GameMain.m_SendBox.m_Goods_04:SetMouseOverDesc("")
	g_DelWndImage(g_GameMain.m_SendBox.m_Goods_01, 1, IP_ENABLE, IP_CLICKDOWN)   --取消添加物品附件
	g_DelWndImage(g_GameMain.m_SendBox.m_Goods_02, 1, IP_ENABLE, IP_CLICKDOWN)
	g_DelWndImage(g_GameMain.m_SendBox.m_Goods_03, 1, IP_ENABLE, IP_CLICKDOWN)
	g_DelWndImage(g_GameMain.m_SendBox.m_Goods_04, 1, IP_ENABLE, IP_CLICKDOWN)
	g_GameMain.m_SendBox:ReleaseLockedItem()
	g_GameMain.m_SendBox.m_emailGoods = nil
	g_GameMain.m_SendBox.GoodsID = {}
	g_GameMain.m_SendBox.m_emailGoodsTbl = {}
	g_GameMain.m_SendBox.Goodsname = nil
	g_GameMain.m_SendBox.GoodsType = nil
end

--10是手续费
--得到输入的金钱数目
function CSendEmailWnd:GetMoneyCount()
	local goldCount = tonumber( self.m_Jin:GetWndText() )
	local silverCount = tonumber( self.m_Yin:GetWndText() )
	local tongCount = tonumber( self.m_Tong:GetWndText() )
	
	gac_gas_require "framework/common/CMoney"
	local money = CMoney:new()
	local moneyCount = money:ChangeGoldAndArgentToMoney(goldCount, silverCount, tongCount)
	return moneyCount
end

function CSendEmailWnd:RetGetSendMoneyCess(exchange_money,cess)
	if exchange_money > 0 then
		local function CallBack(g_GameMain,uButton)
			if uButton == MB_BtnOK then
				Gac2Gas:SendMail(g_Conn,self.m_ReceiverName:GetWndText() ,self.m_EmailTitle:GetWndText(),self.m_EmailText:GetWndText().." ",self:GetMoneyCount(),g_MainPlayer:GetMoneyType())
			else
				self.m_SenderList:ShowWnd(false)
				self:ShowWnd(false)
			end
			g_GameMain.m_ExchangeMsgBox = nil
			return true
		end
		local str = MsgBoxMsg(267,cess .. "%", g_MoneyMgr:ChangeMoneyToString(exchange_money,EGoldType.GoldCoin))
		g_GameMain.m_ExchangeMsgBox = MessageBox( g_GameMain, str, BitOr( MB_BtnOK, MB_BtnCancel),CallBack,g_GameMain)
	else
		Gac2Gas:SendMail(g_Conn,self.m_ReceiverName:GetWndText() ,self.m_EmailTitle:GetWndText(),self.m_EmailText:GetWndText().." ",self:GetMoneyCount(),g_MainPlayer:GetMoneyType())
	end
end

function CSendEmailWnd:IsMoneyEnough()
	local nYouZi = #(g_GameMain.m_SendBox.m_emailGoodsTbl)*10 + 10
	local send_money = self:GetMoneyCount()
	local curMoneyType = g_MainPlayer:GetMoneyType()
	if send_money >  g_MainPlayer.m_nMoney then
	    self.m_MsgBox  = MessageBox( self, MsgBoxMsg(263), MB_BtnOK )
        return  
	end
	if (nYouZi+send_money) > g_MainPlayer.m_nMoney +  g_MainPlayer.m_nBindMoney then
	    self.m_MsgBox  = MessageBox( self, MsgBoxMsg(263), MB_BtnOK )
        return 
	end
	if send_money > 0 then
		Gac2Gas:GetSendMoneyCess(g_Conn,send_money)
	else
		Gac2Gas:SendMail(g_Conn,self.m_ReceiverName:GetWndText() ,self.m_EmailTitle:GetWndText(),self.m_EmailText:GetWndText().." ",tonumber(send_money),curMoneyType)
	end
end

function CSendEmailWnd:CheckGoodsAndMoney()
	local function callback(Context,Button)
		if(Button == MB_BtnOK) then
			self:IsMoneyEnough()
		end
		return true
	end
	local function CheckSendMoney()
		local jinCount = self.m_Jin:GetWndText() 
		local yinCount = self.m_Yin:GetWndText()
		local tongCount = self.m_Tong:GetWndText()
		if( jinCount ~= "" or  yinCount ~= "" or  tongCount ~= "") then	
			--如果要发送的金币数目合法，则询问玩家是否真的要发送如此数目的金币给收件人
			gac_gas_require "framework/common/CMoney"
			local money = CMoney:new()
			local moneyCount = money:ChangeGoldAndArgentToMoney(jinCount, yinCount, tongCount)
		
			local moneyStr = money:ChangeMoneyToString(moneyCount,EGoldType.GoldCoin)
			self.m_MsgBox = MessageBox( self, MsgBoxMsg(258, moneyStr), BitOr( MB_BtnOK, MB_BtnCancel),callback)
			return
		end
		self:IsMoneyEnough()
	end
	local function callback2(Context,Button)
		if(Button == MB_BtnOK) then
			CheckSendMoney()
		end
		return true
	end
	
	local GoodsTbl = g_GameMain.m_SendBox.m_emailGoodsTbl
	if IsTable(GoodsTbl) and #GoodsTbl > 0 then
		self.m_MsgBox = MessageBox( self, MsgBoxMsg(262, self.m_ReceiverName:GetWndText()), BitOr( MB_BtnOK, MB_BtnCancel),callback2)
		return
	else
		CheckSendMoney()
	end
end

--得到发件箱的内容，并将内容发送给服务器端
function CSendEmailWnd:GetSendBoxContent()
	local strReceiverName = self.m_ReceiverName:GetWndText()
	if(GetCharCount(strReceiverName) > 64 )then
		self.m_MsgBox  = MessageBox( self, MsgBoxMsg(252), MB_BtnOK )
		self.m_ReceiverName:SetWndText("")
		return
	end
	local strEmailTitle = self.m_EmailTitle:GetWndText()
	if(GetCharCount(strEmailTitle) > 16)then
		self.m_MsgBox  = MessageBox( self, MsgBoxMsg(253), MB_BtnOK )
		self.m_EmailTitle:SetWndText("")
		return
	end
	local strEmailText = self.m_EmailText:GetWndText().." "
	if(GetCharCount(strEmailText) > 500)then
		self.m_MsgBox  = MessageBox( self, MsgBoxMsg(254), MB_BtnOK )
		return
	end
		--检查收件人姓名是否是玩家自己
	if(strReceiverName == g_MainPlayer.m_Properties:GetCharName()) then
		self.m_MsgBox  = MessageBox( self, MsgBoxMsg(257), MB_BtnOK )
		self.m_ReceiverName:SetWndText("")
		self:CleanAllGoodsInformation()
		return
	end
	local moneyAmount = self:GetMoneyCount()
	if( moneyAmount > 999999 )then
		self.m_MsgBox  = MessageBox( self, MsgBoxMsg(255), MB_BtnOK )
		return
	elseif(moneyAmount < 0)then
		self.m_MsgBox  = MessageBox( self, MsgBoxMsg(256), MB_BtnOK )
		return
	end
	local function callback(Context,Button)
		if(Button == MB_BtnOK) then
			self:CheckGoodsAndMoney()
		end
		return true
	end
	local nYouZi = #(g_GameMain.m_SendBox.m_emailGoodsTbl)*10 + 10
	gac_gas_require "framework/common/CMoney"
	local money_mgr = CMoney:new()
	local sYouZi = money_mgr:ChangeMoneyToString(nYouZi)
	self.m_MsgBox = MessageBox( self, MsgBoxMsg(265,sYouZi), BitOr( MB_BtnOK, MB_BtnCancel),callback)
end

---------------------------------------------------
--释放包裹中被锁定的格子
function CSendEmailWnd:ReleaseLockedItem()
	if g_GameMain.m_SendBox.SelectGridTable == nil then
		return
	end
	for i=1, table.maxn( g_GameMain.m_SendBox.SelectGridTable ) do
		if g_GameMain.m_SendBox.SelectGridTable[i] ~= nil then
			local x, y, roomIndex = unpack(g_GameMain.m_SendBox.SelectGridTable[i])
			local SrcCt = g_GameMain.m_BagSpaceMgr:GetSpaceRelateLc( g_StoreRoomIndex.PlayerBag )
			SrcCt:SetClickedWndState(x, y, roomIndex, true)
		end
	end
	g_GameMain.m_SendBox.SelectGridTable = {}
end

function CSendEmailWnd:VirtualExcludeWndClosed()
	self:CleanAllGoodsInformation()
	g_GameMain.m_WndMainBag:ShowWnd(false)
end

function CSendEmailWnd:CSMEnableEmailGoods( wndObject, Goods_Index )
	local a = g_GameMain.m_SendBox.SelectGridTable
	if wndObject:GetWndText() == "" and
		g_GameMain.m_WndMainBag:IsShow() == false then
		g_GameMain.m_WndMainBag:ShowWnd(true)
	end
	local SrcCt = g_GameMain.m_BagSpaceMgr:GetSpaceRelateLc( g_StoreRoomIndex.PlayerBag )
	if(g_GameMain.m_SendBox.SelectGridTable == nil)then
		g_GameMain.m_SendBox.SelectGridTable = {}
		return
	end
	if g_GameMain.m_SendBox.SelectGridTable[Goods_Index] ~= nil then
		local x, y, roomIndex = unpack(g_GameMain.m_SendBox.SelectGridTable[Goods_Index])
		SrcCt:SetClickedWndState(x, y, roomIndex, true)
		g_GameMain.m_SendBox.SelectGridTable[Goods_Index] = nil
	end
end

--点击物品附件框的事件
function CSendEmailWnd:ClickGoodsButton(buttonname, Rcontext)
	g_GameMain.m_WndMainBag:SetFocus()
	local WndName 
	local Email_index
	if(buttonname == "Goods_01") then
		Email_index =1
		WndName = g_GameMain.m_SendBox.m_GoodsName_01
	elseif(buttonname == "Goods_02") then
		Email_index =2
		WndName = g_GameMain.m_SendBox.m_GoodsName_02
	elseif(buttonname == "Goods_03") then
		Email_index =3
		WndName = g_GameMain.m_SendBox.m_GoodsName_03
	elseif(buttonname == "Goods_04") then
		Email_index =4
		WndName = g_GameMain.m_SendBox.m_GoodsName_04
	end
	local Wnd = g_GameMain.m_SendBox:GetDlgChild(buttonname)
	local state, context = g_WndMouse:GetCursorState()
	if state == ECursorState.eCS_PickupSplitItem then --手上的物品是要拆分的
		return
	elseif state == ECursorState.eCS_PickupItem then --手上有物品
		self:CSMEmailGoods( Wnd,context )
	elseif  Rcontext~=nil then
		context = Rcontext
		self:CSMEmailGoods( Wnd,context )
	end
	
	if(g_GameMain.m_SendBox.m_emailGoods~=nil)then--添加物品附件
		local SmallIcon = g_GameMain.m_SendBox.m_emailGoods
		for i=1,#(g_GameMain.m_SendBox.m_emailGoodsTbl)do
			if(g_GameMain.m_SendBox.m_emailGoodsTbl[i][1] == Email_index)then
				local count={}
				for m=1, #(g_GameMain.m_SendBox.m_emailGoodsTbl) do
					if(g_GameMain.m_SendBox.m_emailGoodsTbl[m][1] == Email_index)then
						table.insert(count,m)
					end
				end
				for m =1, #(count)do
					table.remove(g_GameMain.m_SendBox.m_emailGoodsTbl,count[1])    --从表中删除
				end
				g_DelWndImage(Wnd, 1, IP_ENABLE, IP_CLICKDOWN)   --取消添加物品附件
				g_GameMain.m_SendBox:CSMEnableEmailGoods( Wnd,Email_index )
				Wnd:SetWndText("")
				WndName:SetWndText("")
				Wnd:SetMouseOverDesc("")
				break
			end
		end
		g_LoadIconFromRes(SmallIcon, Wnd, -1, IP_ENABLE, IP_CLICKDOWN)  
		local wnd = g_GameMain.m_SendBox
		Wnd:SetWndText(wnd.Goods_count)--显示物品数量
		local DisplayName = g_ItemInfoMgr:GetItemLanInfo(wnd.GoodsType, wnd.Goodsname,"DisplayName")
		WndName:SetWndText(DisplayName)
		WndName.m_ItemType = wnd.GoodsType
		WndName.m_ItemName = wnd.Goodsname
		g_SetWndMultiToolTips(Wnd,wnd.GoodsType, wnd.Goodsname, wnd.GoodsID[1], wnd.Goods_count)
		for i = 1 ,#(g_GameMain.m_SendBox.GoodsID) do
			table.insert(g_GameMain.m_SendBox.m_emailGoodsTbl, {Email_index,g_GameMain.m_SendBox.Goodsname,g_GameMain.m_SendBox.GoodsID[i]}) 
			--格子Email_index,物品名,物品id<唯一>
		end
		--将物品在物品栏对应的格子置为不可点击状态
		local fromRoom, fromRow, fromCol = context[1], context[2], context[3]
		local SrcCt = g_GameMain.m_BagSpaceMgr:GetSpaceRelateLc( g_StoreRoomIndex.PlayerBag )
		local srcPos = SrcCt:GetPosByIndex( fromRow, fromCol, fromRoom )
		local cols = g_PlayerMainBag.columns
		if(g_GameMain.m_SendBox.SelectGridTable == nil)then
			g_GameMain.m_SendBox.SelectGridTable = {}
		end 
		local y, x = SrcCt:GetIndexByPos(srcPos, fromRoom)
		g_GameMain.m_SendBox.SelectGridTable[Email_index] = {x, y, fromRoom}
		SrcCt:SetClickedWndState(x, y, fromRoom, false)
		g_WndMouse:ClearCursorAll()
		
		g_ItemInfoMgr:PlayItemSound(wnd.GoodsType, wnd.Goodsname)
	end
	g_GameMain.m_SendBox:CheckSend()
end
function CSendEmailWnd:AddSenderList(friendsname)
	--最近联系人保存到本地文件"var/gac/Emailsendlist/EmailSenderList_ncharID.txt"
	if(g_MainPlayer.m_Properties:GetCharName() == friendsname)then
		return
	end
	local nCharID = g_MainPlayer.m_uID
	
	MakeDirectoryUTF8(g_RootPath .. "var/gac/Emailsendlist")
	
	local strpath = g_RootPath .. "var/gac/Emailsendlist/EmailSenderList_" .. nCharID .. ".txt"
	local fi = CLuaIO_Open(strpath, "rb")
	local data = ""
	local allfriendsname = {}
	table.insert(allfriendsname,friendsname)
	if fi then
		data = fi:ReadAll()
	
		while(true) do
			if(data=="")then break end
			local pos = string.find(data, "\n")
			if not pos then break end
			if(string.sub(data,1,pos-2) == "")then
				data = string.sub(data,pos+1,string.len(data))
				break
			end
			if(string.sub(data,1,pos-2) ~= friendsname)then  --判断是否已经存在
				table.insert(allfriendsname,string.sub(data,1,pos-2)) --不同于friendsname的加入allfriendsname中
			end
			data = string.sub(data,pos+1,string.len(data))
		end
		fi:Close()
	end
	local fo = CLuaIO_Open(strpath, "w+")
	if fo then
		local data = fo:WriteString("")
		fo:Close()
		local f = CLuaIO_Open(strpath, "a")
		if f then

			local len = math.min(#allfriendsname, 20)
			for i = 1, len do
				f:WriteString(allfriendsname[i].."\n")
			end

			f:Close()
		end
	end
end



