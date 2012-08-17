gac_require "fight/battle_array/BattleArrayStoreInc"

function CreateBattleArrayStoreWnd(Parent)
	local Wnd = CBattleArrayStore:new()
	Wnd:Init(Parent)
	return Wnd
end

function CBattleArrayStore:Init(Parent)
	self.m_Page				= 0
	self.m_MaxCol			= 3 --4列
	self.m_MaxRow			= 2 --3行
	self.m_OnePageNum		= (self.m_MaxCol+1) * (self.m_MaxRow+1)
	self.m_TmpItem			= {}
	self.m_TmpArrayName		= ""
	self.m_tblSimpShowWnd	= {}
	--self.m_tblArrayListBtn = {} --用于存放ArrayListCwnd的表
	self.m_tblShortCutInfo	= {} --{arrayName}
	self.m_tblShortCutWnd	= {}
	self:CreateFromRes("BattleArrayStore", Parent)
	self:DrawShortCutListItem()
end

function CBattleArrayStore:OnChildCreated()
	self.m_ArrayList	= self:GetDlgChild("ArrayList")
	self.m_XBtn			= self:GetDlgChild("XBtn")
	self.m_EditPosBtn	= self:GetDlgChild("EditPosBtn")
	self.m_EditMemBtn	= self:GetDlgChild("EditMemBtn")
	self.m_RevertBtn	= self:GetDlgChild("RevertBtn")
	self.m_SimpShow		= self:GetDlgChild("SimpShow")
	self.m_ArrayName	= self:GetDlgChild("ArrayName")
	self.m_HaveArrayNum	= self:GetDlgChild("HaveArrayNum")
	self.m_ALLArrayNum	= self:GetDlgChild("ALLArrayNum")
	self.m_Icon			= self:GetDlgChild("Icon")
	self.m_ShortCutList	= self:GetDlgChild("ShortCutList")
	self.m_ArrayDes		= self:GetDlgChild("ArrayDes")
	for i = 0, 7 do self.m_ShortCutList:InsertColumn(i, 38) end
	self.m_ShortCutList:InsertItem(0, 38)
	
	self.m_tblPageBtn = {}
	for i = 1, 6 do
		self.m_tblPageBtn[i] = self:GetDlgChild("PageBtn" .. i)
	end
	self.m_EditPosBtn:EnableWnd(false)
	self.m_EditMemBtn:EnableWnd(false)
	self.m_RevertBtn:EnableWnd(false)
	self:DrawList()
	self:ReDrawSimpShow()
	self:DrawShortCutListBG()
end

function CBattleArrayStore:OnRButtonUp()
	g_WndMouse:ClearCursorAll()
end

--消息
function CBattleArrayStore:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if(uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_XBtn) then
			if(ECursorState.eCS_PickupBattleArrayIcon) then
				g_WndMouse:ClearCursorAll()
			end
			self:ShowWnd(false)
		elseif(Child == self.m_EditPosBtn) then
			self:OpenCreateWnd()
		elseif(Child == self.m_EditMemBtn) then
			self:OpenMemPosWnd()
		elseif(Child == self.m_RevertBtn) then
			self:RevertToBook()
		else
			self:LClickTblBtn(Child)
		end
	elseif(uMsgID == ITEM_LBUTTONCLICK) then
		if(Child == self.m_ArrayList) then
			self:LClickArrayList(uParam1, uParam2)
		elseif(Child == self.m_ShortCutList) then
			self:LClickShortCutList(uParam2)
		end
	elseif(uMsgID == ITEM_RBUTTONCLICK) then
		if(Child == self.m_ShortCutList) then
			self:RClickShortCutList(uParam2)
		end
	elseif(uMsgID == BUTTON_DRAG) then
		if(Child == self.m_ArrayList) then
			self:DragIcon(uParam1, uParam2)
		end
	end
end

--设置选择的翻页单选按钮
function CBattleArrayStore:SetCheckBtn(index)
	self.m_tblPageBtn[index]:SetCheck(true)
end

--点击阵法库的阵法图标
function CBattleArrayStore:LClickArrayList(uParam1, uParam2)
	local index = uParam1*(self.m_MaxCol+1) + uParam2 + self.m_Page*self.m_OnePageNum + 1
	self:ShowArrayInfo(index)
	self.m_EditMemBtn:EnableWnd(g_GameMain.m_TeamBase.m_bCaptain)
	self.m_EditPosBtn:EnableWnd(true)
	self.m_RevertBtn:EnableWnd(true)
end

--显示当前选中的阵法信息
function CBattleArrayStore:ShowArrayInfo(index)
	self.m_Icon:ShowWnd(true)
	local base = g_GameMain.m_BattleArrayBase
	local tblThisArrayPos = base.m_tblBattleArrayPos[index]
	local arrayName = tblThisArrayPos[1][2]
	local SmallIcon = g_ItemInfoMgr:GetItemInfo(26, arrayName,"SmallIcon")
	g_LoadIconFromRes(SmallIcon, self.m_Icon, -2, IP_ENABLE, IP_CLICKDOWN)
	self.m_ArrayName:SetWndText(arrayName)
	self:ReDrawSimpShowItem(tblThisArrayPos[2])
	self.m_ArrayDes:SetWndText(base:GetArraySkillDes(arrayName))
end

--清空显示的当前阵法信息
function CBattleArrayStore:ClearArrayInfoShow()
	self.m_Icon:ShowWnd(false)
	self.m_ArrayName:SetWndText("")
	self:ReDrawSimpShowItem({})
	self.m_ArrayDes:SetWndText("")
end

--点击快捷栏
function CBattleArrayStore:LClickShortCutList(uParam2)
	local state, context = g_WndMouse:GetCursorState()
	local index = uParam2+1
	if(state == ECursorState.eCS_PickupBattleArrayIcon) then
		local InfoTbl = context[1]
		local toWnd = self.m_tblShortCutWnd[index].m_Btn
		self.FunCallBackSaveShortCut = function()
			local SmallIcon = g_ItemInfoMgr:GetItemInfo(26, InfoTbl.SkillName,"SmallIcon")
			g_LoadIconFromRes(SmallIcon, toWnd, -2, IP_ENABLE, IP_CLICKDOWN)
			g_WndMouse:ClearCursorAll()
			self.m_tblShortCutInfo[index] = InfoTbl.SkillName
			self.FunCallBackSaveShortCut = nil
		end
		Gac2Gas:SaveBattleArrayShortCutIndex(g_Conn, InfoTbl.SkillName, index)
	elseif(state == ECursorState.eCS_Normal) then
		local arrayName = self.m_tblShortCutInfo[index]
		if(arrayName) then
			local memPosWnd			= g_GameMain.m_BattleArrayMemPosWnd
			local base				= g_GameMain.m_BattleArrayBase
			local nameIndex			= base:GetArrayNameIndex(arrayName)
			local tblThisArrayMem	= base.m_tblBattleArrayMem[nameIndex] --阵型信息表
			if(memPosWnd.m_bMemChanged or not tblThisArrayMem) then
				self:ShowWnd(false)
				g_GameMain.m_BattleArrayMemPosWnd:OpenWnd(arrayName)
			else
				Gac2Gas:UseBattleArraySkill(g_Conn, arrayName)
			end
		end
	end
end

--右击快捷栏
function CBattleArrayStore:RClickShortCutList(uParam2)
	local index = uParam2+1
	local toWnd = self.m_tblShortCutWnd[index].m_Btn
	if(self.m_tblShortCutInfo[index]) then
		self.FunCallBackSaveShortCut = function()
			g_WndMouse:ClearIconWnd(toWnd)
			self.m_tblShortCutInfo[index] = nil
			self.FunCallBackSaveShortCut = nil
		end
		Gac2Gas:SaveBattleArrayShortCutIndex(g_Conn, "", index)
	end
end

--重绘list
function CBattleArrayStore:DrawList()
	self.m_ArrayList:DeleteAllItem()
	for i = 0, self.m_MaxCol do self.m_ArrayList:InsertColumn(i, 64) end
	for i = 0, self.m_MaxRow do self.m_ArrayList:InsertItem(i, 64)   end
	for i = 0, self.m_MaxRow do
		for j = 0, self.m_MaxCol do
			local item = self.m_ArrayList:GetSubItem(i, j)
			--local btn = self:CreateArrayListItem(item) --为list上加Cwnd写的
			--table.insert(self.m_tblArrayListBtn, btn)  --为list上加Cwnd写的
			item:EnableWnd(false)
		end
	end
	
end

function CBattleArrayStore:UnEnableItem()
	for i = 0, self.m_MaxRow do
		for j = 0, self.m_MaxCol do
			local item = self.m_ArrayList:GetSubItem(i, j)
			item:EnableWnd(false)
			g_WndMouse:ClearIconWnd(item)
		end
	end
	self.m_EditMemBtn:EnableWnd(false)
	self.m_EditPosBtn:EnableWnd(false)
	self.m_RevertBtn:EnableWnd(false)
end

function CBattleArrayStore:ShowBattleArrayName()
	local tbl = g_GameMain.m_BattleArrayBase.m_AllBattleArrayName
	for i = 0, self.m_MaxRow do
		for j = 0, self.m_MaxCol do
			local item = self.m_ArrayList:GetSubItem(i, j)
			item:SetWndText(tbl[i*4+(j+1)+self.m_Page*self.m_OnePageNum])
		end
	end
end

function CBattleArrayStore:GetBattleArrayStoreInfo()
	local tblAllArrayName	= g_GameMain.m_BattleArrayBase.m_AllBattleArrayName
	local tblArrayPos		= g_GameMain.m_BattleArrayBase.m_tblBattleArrayPos
	local haveArrayNum		= 0
	for i = 1, self.m_OnePageNum do
		local index = i+(self.m_Page)*self.m_OnePageNum
		if(nil ~= tblArrayPos[index] and tblArrayPos[index][1][3] == 1) then
			local item = self.m_ArrayList:GetSubItem((i-1)/(self.m_MaxCol+1), (i-1)%(self.m_MaxCol+1))
			item:EnableWnd(true)
			local arrayName = tblAllArrayName[index]
			local SmallIcon = g_ItemInfoMgr:GetItemInfo(26, arrayName,"SmallIcon")
			--local wndBtn = self.m_tblArrayListBtn[i].m_Btn
			g_LoadIconFromRes(SmallIcon, item, -2, IP_ENABLE, IP_CLICKDOWN)
		end
	end
	for i, p in pairs(tblArrayPos) do
		if(p and p[1][3] == 1) then
			haveArrayNum = haveArrayNum + 1
		end
	end
	self.m_HaveArrayNum:SetWndText(haveArrayNum)
	self.m_ALLArrayNum:SetWndText(#tblAllArrayName)
end

function CBattleArrayStore:OpenWnd(index, saveFrom) --saveFrom: 0从包裹保存，1从阵法库保存
	self:UnEnableItem()
	self:ShowBattleArrayName()
	self:GetBattleArrayStoreInfo()
	self:ClearArrayInfoShow()
	if(index and 0 ~= index and 1 == saveFrom) then
		self:ShowArrayInfo(index)
	end
	self:ShowWnd(true)
end

function CBattleArrayStore:OpenCreateWnd()
	local itemIndex		= self.m_ArrayList:GetSelectItem(-1)
	if(-1 == itemIndex) then MsgClient(12005) return end
	local subItemIndex	= self.m_ArrayList:GetSelectSubItem(itemIndex, -1)
	local item			= self.m_ArrayList:GetSubItem(itemIndex, subItemIndex)
	local bFlag			= item:IsEnable()
	if(not bFlag) then MsgClient(12005) return end
	local tbl = g_GameMain.m_BattleArrayBase.m_AllBattleArrayName
	self:ShowWnd(false)
	local arrayName = tbl[itemIndex*4+subItemIndex+1 + self.m_Page*self.m_OnePageNum]
	g_GameMain.m_CreateBattleArrayWnd:OpenAndReEdit(arrayName, 1) --1从阵法库打开编辑阵型窗体
end

function CBattleArrayStore:OpenMemPosWnd()
	local itemIndex		= self.m_ArrayList:GetSelectItem(-1)
	if(-1 == itemIndex) then MsgClient(12005) return end
	local subItemIndex	= self.m_ArrayList:GetSelectSubItem(itemIndex, -1)
	local item			= self.m_ArrayList:GetSubItem(itemIndex, subItemIndex)
	local bFlag			= item:IsEnable()
	if(not bFlag) then MsgClient(12005) return end
	local tbl = g_GameMain.m_BattleArrayBase.m_AllBattleArrayName
	self:ShowWnd(false)
	local arrayName = tbl[itemIndex*4+subItemIndex+1 + self.m_Page*self.m_OnePageNum]
	g_GameMain.m_BattleArrayMemPosWnd:OpenWnd(arrayName)
end

--还原成阵法书
function CBattleArrayStore:RevertToBook()
	local itemIndex			= self.m_ArrayList:GetSelectItem(-1)
	if(-1 == itemIndex) then MsgClient(12005) return end
	local subItemIndex		= self.m_ArrayList:GetSelectSubItem(itemIndex, -1)
	local item				= self.m_ArrayList:GetSubItem(itemIndex, subItemIndex)
	local bFlag				= item:IsEnable()
	if(not bFlag) then MsgClient(12005) return end
	local index				= self.m_Page*self.m_OnePageNum + itemIndex*4 + subItemIndex + 1
	local arrayName			= g_GameMain.m_BattleArrayBase.m_AllBattleArrayName[index]
	local tblArrayPos		= g_GameMain.m_BattleArrayBase.m_tblBattleArrayPos
	local tblShortCutInfo	= self.m_tblShortCutInfo
	self.m_TmpItem			= item
	self.m_TmpArrayName		= arrayName
	
	------传说中的分割线的开始--------------------------------------------------------------------
	local wnd =  g_GameMain.m_MainSkillsToolBar.m_WndSkill
	local tblShortcutPiece = wnd:GetTblShortcutPiece()
	local tblWnd = {}
	for i, p in pairs(tblShortcutPiece) do
		if(arrayName == p:GetName()) then
			table.insert(tblWnd, p)
		end
	end
	self.FunCallBackDeleteMainShortCut = function()
		for i, p in pairs(tblWnd) do
			wnd:DeletePiece(p)
		end
		self.FunCallBackDeleteMainShortCut = nil
	end
	
	local tblIndex = {}
	for i, p in pairs(tblShortCutInfo) do
		if(arrayName == p) then
			table.insert(tblIndex, i)
		end
	end
	self.FunCallBackSaveShortCut = function()
		for i, p in pairs(tblIndex) do
			local toWnd = self.m_tblShortCutWnd[p].m_Btn
			g_WndMouse:ClearIconWnd(toWnd)
			self.m_tblShortCutInfo[p] = nil
			Gac2Gas:SaveBattleArrayShortCutIndex(g_Conn, "", p)
		end
		self.FunCallBackSaveShortCut = nil
	end
	------传说中的分割线的结束--------------------------------------------------------------------
	
	Gac2Gas:RevertToBattleArrayBook(g_Conn, tblArrayPos[index][1][1], arrayName)
end

--重绘阵型显示窗口
function CBattleArrayStore:ReDrawSimpShow()
	self.m_SimpShow:DeleteAllItem()
	self.m_tblSimpShowWnd = {}
	for i = 0, 9 do self.m_SimpShow:InsertColumn(i, 10) end
	for i = 0, 9 do self.m_SimpShow:InsertItem(i, 10)   end
	for i = 0, 9 do
		for j = 0, 9 do
			local item		= self.m_SimpShow:GetSubItem(i, j)
			local itemWnd	= self:CreateSimpShowItem(item)
			table.insert(self.m_tblSimpShowWnd, itemWnd)
			item:EnableWnd(false)
		end
	end
end

function CBattleArrayStore:ReDrawSimpShowItem(tblPos)
	for i = 1, 100 do
		local itemWnd = self.m_tblSimpShowWnd[i]
		itemWnd.m_Static:ShowWnd(false)
	end
	for i = 1, #tblPos do
		local itemWnd = self.m_tblSimpShowWnd[tblPos[i]+1]
		itemWnd.m_Static:ShowWnd(true)
	end
end

--鼠标拖动阵法图标
function CBattleArrayStore:DragIcon(uParam1, uParam2)
	local tblAllArrayName = g_GameMain.m_BattleArrayBase.m_AllBattleArrayName
	local state, context  = g_WndMouse:GetCursorState()
	local arrayIndex = uParam1*(self.m_MaxCol+1) + uParam2 + self.m_Page*self.m_OnePageNum + 1
	local arrayName  = tblAllArrayName[arrayIndex]
	local item = self.m_ArrayList:GetSubItem(uParam1, uParam2)
	if(state == ECursorState.eCS_Normal) then --手上没有物品
		local fs = {}
		fs.SkillName = arrayName
		fs.Level = 1
		g_WndMouse:SetPickupBattleArrayIcon(fs, item)
	end
end

--组合按钮控件点击消息响应的封装
function CBattleArrayStore:LClickTblBtn(Child)
	for i = 1, 6 do
		if(Child == self.m_tblPageBtn[i]) then
			self.m_Page = i-1
			self:OpenWnd(0)
			return
		end
	end
end

function CBattleArrayStore:DrawShortCutListItem()
	for i = 0, 7 do
		local item = self.m_ShortCutList:GetSubItem(0, i)
		local wnd  = self:CreateShortCutListItem(item)
		self.m_tblShortCutWnd[i+1] = wnd
	end
end

--绘制背景层
function CBattleArrayStore:DrawShortCutListBG()
	local bk = SQRDialog:new()
	bk:CreateFromResEx( "BagGridBackground",self,true,true )
	bk:ShowWnd( true )
	local SIZE = bk:GetWndWidth()
	local Flag = IMAGE_PARAM:new(SM_BS_BK, IP_ENABLE)
	local SrcImage    = bk:GetWndBkImage(Flag)
	local FlagDisable = IMAGE_PARAM:new(SM_BS_BK, IP_DISABLE)
	local SrcImageDisable = bk:GetWndBkImage(FlagDisable)

	for i = 1, 8 do
		local x, y = self:ParsePos(i, 8)
		local pos  = CPos:new(SIZE*x,SIZE*y)
		local item = self.m_ShortCutList:GetSubItem(y, x)
		local DescImage        = item:GetWndBkImage(Flag)
		local DescImageDisable = item:GetWndBkImage(FlagDisable)
		local ImageBk = IMAGE:new()
		SrcImage:GetImage(SrcImage:GetImageCount()-1, ImageBk)
		SrcImageDisable:GetImage(SrcImageDisable:GetImageCount()-1, ImageBk)
		ImageBk:SetPosWndX(pos.x)
		ImageBk:SetPosWndY(pos.y)
		DescImage:AddImageFromImageList(SrcImage,0,-1)
		DescImageDisable:AddImageFromImageList(SrcImageDisable,0,-1)
	end
	bk:DestroyWnd()
end

function CBattleArrayStore:ParsePos(Pos, cols)
	Pos = Pos - 1
	local y = math.floor(Pos / cols)
	local x = Pos - y*cols
	return x, y
end

-----------------------------------------------------------
function CBattleArrayStore:CreateSimpShowItem(parent)
	local wnd = CSimpShowItem:new()
	wnd:CreateFromRes("BattleArraySimpShowItem", parent)
	wnd:ShowWnd( true )
	return wnd
end

function CSimpShowItem:OnChildCreated()
	self.m_Static = self:GetDlgChild("Static")
	self.m_Static:ShowWnd(false)
end

function CBattleArrayStore:CreateShortCutListItem(parent)
	local wnd = CShortCutListItem:new()
	wnd:CreateFromRes("BattleArrayStoreShortCutItem", parent)
	wnd:ShowWnd( true )
	wnd.m_Btn = wnd:GetDlgChild("Btn")
	return wnd
end







-----------为list加上Cwnd-----------------------------------------------
--[[
function CBattleArrayStore:CreateArrayListItem(parent)
	local wnd = CArrayListItem:new()
	wnd:CreateFromRes("BattleArrayStoreListItem", parent)
	wnd:ShowWnd(true)
	return wnd
end

function CArrayListItem:OnChildCreated()
	self.m_Btn = self:GetDlgChild("Btn")
end
--]]