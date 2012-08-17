gac_require "item/boxitem/BoxItemInc"

--右键点击包裹中的盒子物品，从服务端返回盒子中物品的开始
function Gas2Gac:RetGetBoxitemInfoBegin(Conn)
	g_GameMain.m_BoxitemPickupWnd.m_PickupItemInfoTbl	=	{}	--初始化用于记录盒子物品的信息表
end

--右键点击包裹中的盒子物品，从服务端返回盒子中物品信息
--每次只返回一页中的物品信息itemIndex为掉落的物品在数据表的流水号
function Gas2Gac:RetGetBoxitemInfo(Conn, itemIndex, itemType, itemName, itemCount)
	local node		=	{}
	node.ItemIndex	=	itemIndex
	node.ItemType	=	tonumber(itemType)					--物品类型
	node.ItemName	=	itemName					--物品名称
	node.ItemCount	=	itemCount					--物品数量
	
	table.insert(g_GameMain.m_BoxitemPickupWnd.m_PickupItemInfoTbl, node)
end

function Gas2Gac:RetGetBoxitemInfoEnd(Conn, totalPageNo, curPageNo, boxitemID)
	g_GameMain.m_BoxitemPickupWnd.m_CurPageNo	=	curPageNo		--盒子物品总页数
	g_GameMain.m_BoxitemPickupWnd.m_TotalPageNo	=	totalPageNo		--当前盒子物品显示的第几页
	g_GameMain.m_BoxitemPickupWnd.m_BoxitemID	=	boxitemID
	g_GameMain.m_BoxitemPickupWnd:ShowWnd(true)
	g_GameMain.m_BoxitemPickupWnd:ShowBoxitemDetails()				--显示盒子当前显示的物品信息
	--将包裹中存储盒子物品的位置置为不可点击
	g_GameMain.m_BoxitemPickupWnd:EnableBoxItemRelatedGrid(false)
end

function Gas2Gac:RetTakeAllBoxitemDrop(Conn, boxitemID)
--	if g_GameMain.m_BoxitemPickupWnd.m_FinishAnswer ~= nil then
--		if g_GameMain.m_BoxitemPickupWnd.m_FinishAnswer[boxitemID] ~= nil then
--			g_GameMain.m_BoxitemPickupWnd.m_FinishAnswer[boxitemID] = nil 
--		end
--	end
	g_GameMain.m_BoxitemPickupWnd:ShowWnd(false)
	g_GameMain.m_BoxitemPickupWnd:EnableBoxItemRelatedGrid(true)
end

---------------------------------------------------------------------------------

--创建盒子物品拾取窗口
function CreateBoxitemPickupWnd(Parent)
	local wnd	=	CBoxitemPickupWnd:new()
	wnd:CreateFromRes("WndCollObj",Parent)

	wnd:InitBoxitemPickupWndChild()
	g_ExcludeWndMgr:InitExcludeWnd(wnd, "非互斥")

	return wnd
end

--用成员变量保存盒子物品拾取窗口中的控件
function CBoxitemPickupWnd:InitBoxitemPickupWndChild()
	self.m_Close		=	self:GetDlgChild("WndClose")		--关闭按钮
	self.m_LastPage		=	self:GetDlgChild("LastPage")		--上一页按钮
	self.m_NextPage		=	self:GetDlgChild("NextPage")		--下一页按钮
	self.m_WndGetAllItem=	self:GetDlgChild("WndGetAllItem")	--提取全部物品按钮
	self.m_ItemName1	=	self:GetDlgChild("ItemName1")		--第1个物品名称显示窗口
	self.m_ItemName2	=	self:GetDlgChild("ItemName2")		--第2个物品名称显示窗口
	self.m_ItemName3	=	self:GetDlgChild("ItemName3")		--第3个物品名称显示窗口
	self.m_ItemName4	=	self:GetDlgChild("ItemName4")		--第4个物品名称显示窗口
	
	self.m_GetItemBtn1	=	self:GetDlgChild("GetItemBtn1")		--第1个物品的btn
	local PicWnd1 = CBoxitemGridCut:new()  --用于解决物品图片不居中的问题
	PicWnd1:CreateFromRes("CommonGridCut", self.m_GetItemBtn1)
	PicWnd1:ShowWnd( true )
	self.m_GetItemBtn1.m_PicWnd = PicWnd1
	
	self.m_GetItemBtn2	=	self:GetDlgChild("GetItemBtn2")		--第2个物品的btn
	local PicWnd2 = CBoxitemGridCut:new()  --用于解决物品图片不居中的问题
	PicWnd2:CreateFromRes("CommonGridCut", self.m_GetItemBtn2)
	PicWnd2:ShowWnd( true )
	self.m_GetItemBtn2.m_PicWnd = PicWnd2
	
	self.m_GetItemBtn3	=	self:GetDlgChild("GetItemBtn3")		--第3个物品的btn
	local PicWnd3 = CBoxitemGridCut:new()  --用于解决物品图片不居中的问题
	PicWnd3:CreateFromRes("CommonGridCut", self.m_GetItemBtn3)
	PicWnd3:ShowWnd( true )
	self.m_GetItemBtn3.m_PicWnd = PicWnd3
	
	self.m_GetItemBtn4	=	self:GetDlgChild("GetItemBtn4")		--第4个物品的btn
	local PicWnd4 = CBoxitemGridCut:new()  --用于解决物品图片不居中的问题
	PicWnd4:CreateFromRes("CommonGridCut", self.m_GetItemBtn4)
	PicWnd4:ShowWnd( true )
	self.m_GetItemBtn4.m_PicWnd = PicWnd4
end

function CBoxitemPickupWnd:VirtualExcludeWndClosed()
    self:EnableBoxItemRelatedGrid(true)
end

--盒子物品拾取窗口事件响应函数
function CBoxitemPickupWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if uMsgID == BUTTON_LCLICK then
		if self.m_Close == Child then
			self:ShowWnd(false)
			self:EnableBoxItemRelatedGrid(true)
		
		elseif self.m_LastPage == Child then
			self.m_CurPageNo	=	self.m_CurPageNo - 1
			Gac2Gas:GetOnePageDropItem(g_Conn, self.m_BoxitemID, self.m_CurPageNo, self.m_TotalPageNo)
			
		elseif self.m_NextPage == Child then
			self.m_CurPageNo	=	self.m_CurPageNo + 1
			Gac2Gas:GetOnePageDropItem(g_Conn, self.m_BoxitemID, self.m_CurPageNo, self.m_TotalPageNo)
		
		elseif self.m_WndGetAllItem == Child then			--提取盒子中的所有物品
			Gac2Gas:TakeAllBoxitemDrop(g_Conn, self.m_BoxitemID)
			
		elseif self.m_GetItemBtn1 == Child then
			Gac2Gas:TakeOneDropItemByIndex(g_Conn, self.m_CurPageNo, self.m_PickupItemInfoTbl[1].ItemIndex, self.m_BoxitemID)
		
		elseif self.m_GetItemBtn2 == Child then
			
			Gac2Gas:TakeOneDropItemByIndex(g_Conn, self.m_CurPageNo, self.m_PickupItemInfoTbl[2].ItemIndex, self.m_BoxitemID )
	
		elseif self.m_GetItemBtn3 == Child then
			Gac2Gas:TakeOneDropItemByIndex(g_Conn, self.m_CurPageNo, self.m_PickupItemInfoTbl[3].ItemIndex, self.m_BoxitemID)
	
		elseif self.m_GetItemBtn4 == Child then
			Gac2Gas:TakeOneDropItemByIndex(g_Conn, self.m_CurPageNo, self.m_PickupItemInfoTbl[4].ItemIndex, self.m_BoxitemID)
			
		end	
	end
end

--显示盒子当前显示的物品信息
function CBoxitemPickupWnd:ShowBoxitemDetails()
	local boxitemTbl	=	self.m_PickupItemInfoTbl
	local boxitemTblLen	=	# boxitemTbl
	for i =1, boxitemTblLen do
		local ItemType,ItemName = boxitemTbl[i].ItemType, boxitemTbl[i].ItemName
		local SmallIcon		= g_ItemInfoMgr:GetItemInfo(ItemType,ItemName,"SmallIcon")
		local DisplayName	= g_ItemInfoMgr:GetItemLanInfo(ItemType,ItemName,"DisplayName")
		local iconWnd, nameWnd = self:GetAcurateItem(i)
		local PicWnd = iconWnd.m_PicWnd
		g_DelWndImage(PicWnd, 1, IP_ENABLE, IP_ENABLE)
		g_LoadIconFromRes(SmallIcon, PicWnd, -1, IP_ENABLE, IP_ENABLE)
		PicWnd:SetWndText(boxitemTbl[i].ItemCount)
		nameWnd:SetWndText(DisplayName)
		iconWnd:ShowWnd(true)
		nameWnd:ShowWnd(true)
	end
	for i=boxitemTblLen+1, 4 do				--盒子物品拾取窗口最多显示4个物品的信息
		local iconWnd, nameWnd = self:GetAcurateItem(i)
		iconWnd:ShowWnd(false)
		nameWnd:ShowWnd(false)
	end
	
	----关于物品当前所在显示页的页数，决定上下翻页按钮是否可点
	if self.m_CurPageNo	== 1  then
		self.m_LastPage:EnableWnd(false)
	else
		self.m_LastPage:EnableWnd(true)
	end
	
	if self.m_CurPageNo < self.m_TotalPageNo then
		self.m_NextPage:EnableWnd(true)
	else
		self.m_NextPage:EnableWnd(false)
	end
end

--盒子物品拾取面板中的1-4号物品显示控件中具体的某个
--参数：根据1-4中的某个值，确定是第几个控件
--返回值:显示控件的itemIconWnd, itemNameWnd
function CBoxitemPickupWnd:GetAcurateItem(wndIndex)
	if wndIndex == 1 then
		return self.m_GetItemBtn1, self.m_ItemName1
	
	elseif wndIndex == 2 then
		return self.m_GetItemBtn2, self.m_ItemName2

	elseif wndIndex == 3 then
		return self.m_GetItemBtn3, self.m_ItemName3
	
	elseif wndIndex == 4 then
		return self.m_GetItemBtn4, self.m_ItemName4
	end
end

--根据传的形参，决定盒子物品在包裹中格子可点击状态
function CBoxitemPickupWnd:EnableBoxItemRelatedGrid( bEnable )
	local clickBagGrid  = g_GameMain.m_BoxitemPickupWnd.m_ClickBagGrid
	local roomIndex, pos = clickBagGrid[1], clickBagGrid[2]
	local SrcCt = g_GameMain.m_BagSpaceMgr:GetSpaceRelateLc( g_StoreRoomIndex.PlayerBag )
	local cols = g_PlayerMainBag.columns
	local y,x = SrcCt:GetIndexByPos(pos, roomIndex)
	local item = SrcCt:GetSubItem(x, y)
	item:EnableWnd( bEnable )
end
