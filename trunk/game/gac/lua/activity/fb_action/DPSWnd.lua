CFbDPSWnd = class(SQRDialog)

local CDpsInfoWnd = class(SQRDialog)
local CDps_Btn_Wnd = class(SQRDialog)
local CList_Btn_Wnd = class(SQRDialog)

local m_BtnType = 
{
	["ShowType"] = 1,
	["ShowDPS"] = 2,
	["SendMsg"] = 3,
}

local ShowTypeNum = {
	["伤害"] = 1,
	["治疗"] = 2,
	["承受伤害"] = 3,
	["死亡"] = 4,
}

local m_ItemHeight = 30

--创建小副本记分板
function CFbDPSWnd:Ctor()
	self:CreateFromRes("FbDps_Open_Wnd",g_GameMain)
	self.m_OpenBtn = self:GetDlgChild("Btn")
	self:CreateDpsInfoWnd()
end

function CFbDPSWnd.GetWnd()
	local Wnd = g_GameMain.m_FbDpsInfoWnd
	if not Wnd then
		Wnd = CFbDPSWnd:new()
		g_GameMain.m_FbDpsInfoWnd = Wnd
	end
	return Wnd
end

function CFbDPSWnd:MoveDpsInfo()
	local rt1 = CFRect:new()
	local rt2 = CFRect:new()
	self:GetLogicRect(rt1)
	self.m_InfoWnd:GetLogicRect(rt2)
	local width = rt2.right - rt2.left
	local height = rt2.bottom - rt2.top
	rt2.top = rt1.top
	rt2.bottom = rt2.top + height
	rt2.left = rt1.right
	rt2.right = rt2.left + width
	self.m_InfoWnd:SetLogicRect(rt2)
end

function CFbDPSWnd:CreateDpsInfoWnd()
	local Wnd = CDpsInfoWnd:new()
	Wnd:CreateFromRes("FbDps_Info_Wnd", self)
	Wnd.m_DPSList = Wnd:GetDlgChild("ListInfo")
	Wnd.m_ItemList = Wnd:GetDlgChild("ItemList")
	Wnd.m_ItemList:ShowWnd(false)
	
	Wnd.m_ItemList.m_CurrBtnNum = 0
	Wnd.m_DpsShowType = ShowTypeNum["伤害"]
	
	Wnd.m_Btn = {}
	Wnd.m_Btn[m_BtnType.ShowType] = Wnd:InItBtnWnd(Wnd:GetDlgChild("Btn2"),m_BtnType.ShowType)
	Wnd.m_Btn[m_BtnType.ShowDPS] = Wnd:InItBtnWnd(Wnd:GetDlgChild("Btn1"),m_BtnType.ShowDPS)
	Wnd.m_Btn[m_BtnType.SendMsg] = Wnd:GetDlgChild("Btn3")
	Wnd.m_Btn[m_BtnType.SendMsg].m_BtnNum = m_BtnType.SendMsg
	Wnd.m_Btn[m_BtnType.ShowType]:ShowWnd(false)
	
	Wnd:ShowWnd(false)
	
	Wnd:InitDataList()
	--调整窗口的大小
	Wnd:MoveWnd()
	
	self.m_InfoWnd = Wnd
	
	self:MoveDpsInfo()	
end

function CDpsInfoWnd:InItBtnWnd(Parent, BtnNum)
	local Wnd = CDps_Btn_Wnd:new()
	Wnd:CreateFromRes("FbDps_Btn_Wnd", Parent)
	Parent.m_Pic = Wnd:GetDlgChild("Pic")
	Parent.m_Title = Wnd:GetDlgChild("Title")
	Parent.m_BtnNum = BtnNum
	Parent.m_Wnd = Wnd
	
	local title = ""
	if BtnNum == m_BtnType.ShowType then
		title = GetStaticTextClient(8011)
	elseif BtnNum == m_BtnType.ShowDPS then
		title = GetStaticTextClient(8013)..GetStaticTextClient(8012)
	end
	Parent.m_Title:SetWndText(title)
	
	Wnd:ShowWnd(true)
	return Parent
end

function CFbDPSWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	if uMsgID == BUTTON_LCLICK then
		if Child == self.m_OpenBtn then
			if self.m_InfoWnd then
				if self.m_InfoWnd:IsShow() then
					--self.m_ShowInfoBtn:SetCheck(false)
					self.m_InfoWnd:ShowWnd(false)
					--停止让服务端发消息(省点网络带宽)
					Gac2Gas:EndSendFbDpsInfo(g_Conn)
				else
					--self.m_ShowInfoBtn:SetCheck(true)
					--让服务端发消息
					Gac2Gas:BeginSendFbDpsInfo(g_Conn)
					self.m_InfoWnd.m_ItemList:ShowWnd(false)
					self.m_InfoWnd:ShowWnd(true)
				end
			end
		end
	end
end

function CDpsInfoWnd:ChangeListBtnTitle(ItemName, ShowName)
	local BtnNum = self.m_ItemList.m_CurrBtnNum
	if BtnNum == m_BtnType.ShowType then
		--self.m_Btn[BtnNum].m_Title:SetWndText(ShowName)
		--显示其它BOSS的DPS信息(旧的信息)
		Gac2Gas:GetBossDpsInfo(g_Conn)
	elseif BtnNum == m_BtnType.ShowDPS then
		self.m_Btn[BtnNum].m_Title:SetWndText(ShowName..GetStaticTextClient(8012))
		--初始化数据列表的标题显示
		self.m_DpsShowType = ShowTypeNum[ItemName]
		self:ChangeDataList()
		--告诉服务端,我要显示其它的项值
		Gac2Gas:ChangeDpsShowType(g_Conn, ItemName)
	elseif BtnNum == m_BtnType.SendMsg then
		--告诉服务端,向其它人发我的分数信息
		self:SendFbDpsInfoList(ChannelIdTbl[ItemName])
	end
end

function CDpsInfoWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	if uMsgID == BUTTON_LCLICK then
		self:InitItemList(Child)
	elseif uMsgID == ITEM_LBUTTONCLICK then
		local Item = Child:GetSubItem(uParam1, 0)
		
		Child:ShowWnd(false)
		
		self:ChangeListBtnTitle(Item.m_ItemName, Item.m_ShowName)
	end
end

--============================start===========================
--创建可选择的下拉列表
local function CreateListBtn(IsShowWnd, Parent, ShowName)
	local Wnd = CList_Btn_Wnd:new()
	Wnd:CreateFromRes("FbDps_List_Btn_Wnd",Parent)
	Wnd.m_CheckBtn = Wnd:GetDlgChild("Btn")
	Wnd.m_Title = Wnd:GetDlgChild("Title")
	Wnd.m_Title:SetWndText(ShowName)
	Wnd.m_CheckBtn:ShowWnd(false)
	Wnd:ShowWnd( IsShowWnd )
	return Wnd
end

function CDpsInfoWnd:InsertList(ItemName, ShowName, ItemHeight)
	local itemnum = self.m_ItemList:GetItemCount()
	self.m_ItemList:InsertItem(itemnum, ItemHeight)
	local item = self.m_ItemList:GetSubItem(itemnum, 0 )
	local NameWnd = CreateListBtn(true, item, ShowName)
	item.m_ItemName = ItemName
	item.m_ShowName = ShowName
	item.m_TblWnd = NameWnd
end

function CDpsInfoWnd:Insert_ItemList(BtnNum)
	self.m_ItemList:DeleteAllItem()
	
	local tempWnd = CreateListBtn(false,self,"")
	local ItemHeight = tempWnd:GetWndOrgHeight()
	local ItemWidth = tempWnd:GetWndOrgWidth()
	tempWnd:DestroyWnd()
	
	self.m_ItemList:InsertColumn(0, ItemWidth)
	
	if BtnNum == m_BtnType.ShowType then
		self:InsertList("所有战斗", GetStaticTextClient(8011), ItemHeight)
	elseif BtnNum == m_BtnType.ShowDPS then
		self:InsertList("伤害", GetStaticTextClient(8013), ItemHeight)
		self:InsertList("治疗", GetStaticTextClient(8014), ItemHeight)
		self:InsertList("承受伤害", GetStaticTextClient(8015), ItemHeight)
		self:InsertList("死亡", GetStaticTextClient(8016), ItemHeight)
	elseif BtnNum == m_BtnType.SendMsg then
		--self:InsertList("附近", GetStaticTextClient(8018), ItemHeight)
		self:InsertList("队伍", GetStaticTextClient(8019), ItemHeight)
		self:InsertList("佣兵小队", GetStaticTextClient(8020), ItemHeight)
		self:InsertList("佣兵团", GetStaticTextClient(8021), ItemHeight)
	end
	
	return ItemHeight, ItemWidth
end 

function CDpsInfoWnd:MoveItemList(CurrBtn)
	self.m_ItemList.m_CurrBtnNum = CurrBtn.m_BtnNum
	
	--接着调用初始化功能
	local height, width = self:Insert_ItemList(CurrBtn.m_BtnNum)
	
	--初始化list高度
	local itemnum = self.m_ItemList:GetItemCount()
	local rt1 = CFRect:new()
	local rt2 = CFRect:new()
	CurrBtn:GetLogicRect(rt1)
	rt2.top = rt1.bottom + 3
	rt2.bottom = rt2.top + height * (itemnum+1)
	rt2.right = rt1.right
	rt2.left = rt1.right - width - 15
	self.m_ItemList:SetLogicRect(rt2)
	
	self.m_ItemList:ShowWnd(true)
	self.m_ItemList:SetFocus()
end

function CDpsInfoWnd:MoveWnd()
	local itemnum = self.m_DPSList:GetItemCount()
	local rt1 = CFRect:new()
	local rt2 = CFRect:new()
	self.m_DPSList:GetLogicRect(rt1)
	rt1.bottom = rt1.top + m_ItemHeight * (itemnum+1)
	self.m_DPSList:SetLogicRect(rt1)
	self:GetLogicRect(rt2)
	rt2.bottom = rt2.top + m_ItemHeight * (itemnum+4)
	self:SetLogicRect(rt2)
end
--=======================end===============================

--========================start=========================
--数据排序
function CDpsInfoWnd:SortData()
	local temptbl = {}
	local itemCount = self.m_DPSList:GetItemCount()
	--存数据
	for colNum = 1, itemCount-1 do
		local item1 = self.m_DPSList:GetSubItem(colNum, 0)
		local item2 = self.m_DPSList:GetSubItem(colNum, 1)
		local item3 = self.m_DPSList:GetSubItem(colNum, 2)
		local item4 = self.m_DPSList:GetSubItem(colNum, 3)
		if item2:GetWndText() == "" then
			return
		end
		table.insert(temptbl, {item1.m_ItemName, item1:GetWndText(),item2:GetWndText(),item3:GetWndText(),item4:GetWndText()})
	end
	--删列表信息
	for colNum = itemCount-1, 1 , -1 do
		self.m_DPSList:DeleteItem(colNum + 1)
	end
	--排序后,重新写入信息
	for i=1, #(temptbl)-1 do
		for k=i+1, #(temptbl) do
			if tonumber(temptbl[i][3]) < tonumber(temptbl[k][3]) then
				local num = temptbl[i]
				temptbl[i] = temptbl[k]
				temptbl[k] = num
			end
		end
	end
	for i=1, #(temptbl) do
		self:InsertDataList(unpack(temptbl[i]))
	end
end

--数据列表中添加数据
function CDpsInfoWnd:InsertDataList(ItemName, Data1, Data2, Data3, Data4)
	local itemNum = self.m_DPSList:GetItemCount()
	self.m_DPSList:InsertItem(itemNum, m_ItemHeight)
	local item1 = self.m_DPSList:GetSubItem(itemNum, 0)
	local item2 = self.m_DPSList:GetSubItem(itemNum, 1)
	local item3 = self.m_DPSList:GetSubItem(itemNum, 2)
	local item4 = self.m_DPSList:GetSubItem(itemNum, 3)
	item1:SetWndText(Data1)
	item2:SetWndText(Data2)
	item3:SetWndText(Data3)
	item4:SetWndText(Data4)
	
	item1.m_ItemName = ItemName
end

function CDpsInfoWnd:UpdateDataList(ItemName, Data1, Data2, Data3)
	local itemCount = self.m_DPSList:GetItemCount()
	for colNum = 0, itemCount-1 do
		local item1 = self.m_DPSList:GetSubItem(colNum, 0)
		local item2 = self.m_DPSList:GetSubItem(colNum, 1)
		local item3 = self.m_DPSList:GetSubItem(colNum, 2)
		local item4 = self.m_DPSList:GetSubItem(colNum, 3)
		if item1.m_ItemName == ItemName then
			item2:SetWndText(Data1)
			item3:SetWndText(Data2)
			item4:SetWndText(Data3)
			break
		end
	end
end

function CDpsInfoWnd:InitDataList()
	self.m_DPSList:DeleteAllItem()
	
	local ItemWidth = self.m_DPSList:GetWndOrgWidth()
	
	self.m_DPSList:InsertColumn(0, ItemWidth * 0.3)
	self.m_DPSList:InsertColumn(1, ItemWidth * 0.2)
	self.m_DPSList:InsertColumn(2, ItemWidth * 0.2)
	self.m_DPSList:InsertColumn(3, ItemWidth * 0.3)
	
	self:InsertDataList("玩家名字",GetStaticTextClient(8022),GetStaticTextClient(8023),GetStaticTextClient(8024),GetStaticTextClient(8031))
end

function CDpsInfoWnd:ChangeDataList()
	--修改第一行的信息
	local str1 = ""
	local str2 = ""
	local str3 = GetStaticTextClient(8031)
	if self.m_DpsShowType == ShowTypeNum["治疗"] then
		str1 = GetStaticTextClient(8025)
		str2 = GetStaticTextClient(8026)
		str3 = GetStaticTextClient(8031)
	elseif self.m_DpsShowType == ShowTypeNum["承受伤害"] then
		str1 = GetStaticTextClient(8027)
	elseif self.m_DpsShowType == ShowTypeNum["死亡"] then
		str1 = GetStaticTextClient(8028)
	else
		str1 = GetStaticTextClient(8023)
		str2 = GetStaticTextClient(8024)
		str3 = GetStaticTextClient(8031)
	end
	self:UpdateDataList("玩家名字",str1,str2,str3)
	
	--清数据
	local itemCount = self.m_DPSList:GetItemCount()
	for colNum = 1, itemCount-1 do--从第二行发起,第一行是列标题
		local item = self.m_DPSList:GetSubItem(colNum, 0)
		self:UpdateDataList(item:GetWndText(), "", "", "")
	end
end
--=======================end===========================


function CDpsInfoWnd:InitItemList(Btn)
	--临时功能,第一个按扭没有效果
	if Btn.m_BtnNum == m_BtnType.ShowType then
		return
	end
	
	if self.m_ItemList:IsShow() then
		self.m_ItemList:ShowWnd(false)
		if self.m_ItemList.m_CurrBtnNum ~= Btn.m_BtnNum then
			self:MoveItemList(Btn)
		end
	else
		self:MoveItemList(Btn)
	end
end



--以下是服务端信息的插入
function CFbDPSWnd:OpenWnd()
	self:ShowWnd(true)
	self.m_InfoWnd.m_StartReCordTime = os.time()
end

function CFbDPSWnd:CloseWnd()
	self:ShowWnd(false)
	self:DestroyWnd()
	g_GameMain.m_FbDpsInfoWnd = nil
	self = nil
end

function CFbDPSWnd:UpdateMemberDpsInfo(Name, Data1, Data2)
	--print(Name, Data1, Data2)
	--这个地方之后要处理一下,如果窗口为nil了,怎么办
	local InfoWnd = self.m_InfoWnd
	
	if InfoWnd.m_DpsShowType == ShowTypeNum["承受伤害"] or InfoWnd.m_DpsShowType == ShowTypeNum["死亡"] then
		Data2 = ""
	end
	
	local itemCount = InfoWnd.m_DPSList:GetItemCount()	
	
	local isupdate = false
	for colNum = 1, itemCount-1 do--从第二行发起,第一行是列标题
		local item = InfoWnd.m_DPSList:GetSubItem(colNum, 0)
		local item2 = InfoWnd.m_DPSList:GetSubItem(colNum, 3)
		if item:GetWndText() == Name then
			InfoWnd:UpdateDataList(Name, Data1, Data2, item2:GetWndText())
			isupdate = true
			break
		end
	end
	if not isupdate then
		InfoWnd:InsertDataList(Name, Name, Data1, Data2, 0)
		--调整窗口的大小
		InfoWnd:MoveWnd()
	end
	--排序
	InfoWnd:SortData()
	
	--单独更新一下百分比信息
	local currItemNum = InfoWnd.m_DPSList:GetItemCount()
	
	local Percent = 0
	for colNum = 1, currItemNum-1 do--从第二行发起,第一行是列标题
		local item = InfoWnd.m_DPSList:GetSubItem(colNum, 1)
		if item:GetWndText() == "" then
			break
		end
		Percent = Percent + tonumber(item:GetWndText())
	end
	
	for colNum = 1, currItemNum-1 do--从第二行发起,第一行是列标题
		local num = 0
		local item1 = InfoWnd.m_DPSList:GetSubItem(colNum, 1)
		local item2 = InfoWnd.m_DPSList:GetSubItem(colNum, 3)
		
		if Percent ~= 0 and item1:GetWndText() ~= "" then
			num = math.floor(tonumber(item1:GetWndText())/Percent * 100)
		end
		item2:SetWndText(num)
		
	end
end

function CFbDPSWnd:DeleteMemberDpsInfo(Name)
	--这个地方之后要处理一下,如果窗口为nil了,怎么办
	local InfoWnd = self.m_InfoWnd
	
	local itemCount = InfoWnd.m_DPSList:GetItemCount()
	for colNum = 1, itemCount-1 do--从第二行发起,第一行是列标题
		local item = InfoWnd.m_DPSList:GetSubItem(colNum, 0)
		if item:GetWndText() == Name then
			InfoWnd.m_DPSList:DeleteItem(colNum + 1)
			break
		end
	end
end

--发送战斗统计的信息
function CDpsInfoWnd:SendFbDpsInfoList(ChannelID)
	local itemCount = self.m_DPSList:GetItemCount()
	
	--先发格式消息
	local Msg = GetStaticTextClient(8029, GetSceneDispalyName(g_GameMain.m_SceneName), GetStaticTextClient(8012 + self.m_DpsShowType))
	Gac2Gas:TalkToChannel(g_Conn, Msg, ChannelID)
	Msg = GetStaticTextClient(8030, os.date("%H:%M:%S",self.m_StartReCordTime),os.date("%H:%M:%S",os.time()))
	Gac2Gas:TalkToChannel(g_Conn, Msg, ChannelID)
		
	--发出信息
	for colNum = 1, itemCount-1 do--从第二行发起,第一行是列标题
		local item1 = self.m_DPSList:GetSubItem(colNum, 0)
		local item2 = self.m_DPSList:GetSubItem(colNum, 1)
		local item3 = self.m_DPSList:GetSubItem(colNum, 2)
		local item4 = self.m_DPSList:GetSubItem(colNum, 3)
		
		Msg = item1:GetWndText().." - "..item2:GetWndText().." - "..item3:GetWndText()
		if self.m_DpsShowType == ShowTypeNum["伤害"] or self.m_DpsShowType == ShowTypeNum["治疗"] then
			Msg = Msg.." - "..item4:GetWndText().."%"
		end
		
		Gac2Gas:TalkToChannel(g_Conn, Msg, ChannelID)
	end
end
