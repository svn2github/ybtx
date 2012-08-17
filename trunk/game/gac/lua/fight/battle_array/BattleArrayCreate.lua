gac_require "fight/battle_array/BattleArrayCreateInc"

function CreateBattleArrayCreateWnd(Parent)
	local Wnd = CBattleArrayCreateWnd:new()
	Wnd:Init(Parent)
	return Wnd
end

function CBattleArrayCreateWnd:Init(Parent)
	self.m_ItemId = nil
	self.m_tblArrayPosTemp = {}
	self.m_sArrayName = ""
	self.m_tblGridWnd = {} --格子上的窗体
	self.m_tblTmpBattleArrayBook = {}  --{roomIndex, nPos}
	self.m_BattleArrayBookInfo   = {} --阵法书动态信息{pos1,pos2,pos3,pos4,pos5}
	self:CreateFromRes("BattleArray", Parent)
end

function CBattleArrayCreateWnd:OnChildCreated()
	self.m_XBtn				= self:GetDlgChild("XBtn")
	self.m_ArrayList		= self:GetDlgChild("ArrayList")
	self.m_OkBtn			= self:GetDlgChild("OkBtn")
	self.m_CancelBtn		= self:GetDlgChild("CancelBtn")
	self.m_ResetBtn			= self:GetDlgChild("ResetBtn")
	self.m_ArrayNameLabel	= self:GetDlgChild("ArrayNameLabel")
	self.m_OkBtn:EnableWnd(false)
	self.m_tblIndexBtn = {}
	for i = 1, 5 do
		self.m_tblIndexBtn[i] = self:GetDlgChild("Index" .. i)
		self.m_tblIndexBtn[i]:SetDetectMouseOverWnd(true)
	end
	self:ReDrawList()
end

--消息
function CBattleArrayCreateWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if(self:IsShow()) then
		if(uMsgID == BUTTON_LCLICK) then
			if(Child == self.m_XBtn or Child == self.m_CancelBtn) then
				local cursorState = g_WndMouse:GetCursorState()
				if(cursorState == ECursorState.eCS_PickupBattleArrayIndex) then
					self:ClearCursor()
				end
				self:ShowWnd(false)
			elseif(Child == self.m_ResetBtn) then
				self:Reset()
			elseif(Child == self.m_OkBtn) then --保存阵型
				self:SaveBattleArrayPos()
			else
				self:StartDragBtn(Child)
			end
		elseif(uMsgID == BUTTON_RCLICK) then
			self:ClearCursor()
		elseif(uMsgID == BUTTON_DRAG) then
			self:StartDragBtn(Child)
		elseif(uMsgID == ITEM_LBUTTONCLICK) then
			if(Child == self.m_ArrayList) then
				local item = self.m_ArrayList:GetSubItem(uParam1, uParam2)
				local pos = uParam1*10 + uParam2
				self:LClickUpItem(item, pos)
			end
		end
	end
end

--重绘list
function CBattleArrayCreateWnd:ReDrawList()
	self.m_ArrayList:DeleteAllItem()
	for i = 0, 9 do self.m_ArrayList:InsertColumn(i, 38) end
	for i = 0, 9 do self.m_ArrayList:InsertItem(i, 38)   end
	self:DrawBG()
	self:DrawArrayListItem()
end

--开始拖拽Btn
function CBattleArrayCreateWnd:StartDragBtn(Child)
	local cursorState = g_WndMouse:GetCursorState()
	for i = 1, 5 do
		if(Child == self.m_tblIndexBtn[i] and cursorState == ECursorState.eCS_Normal) then
			g_WndMouse:SetPickupBattleArrayIndex(i, 1, Child) --1是从按钮点起的，2是从Item点起的
			Child:ShowWnd(false)
			return
		end
	end
end

--清空鼠标
function CBattleArrayCreateWnd:ClearCursor()
	local cursorState, context = g_WndMouse:GetCursorState()
	if(cursorState == ECursorState.eCS_PickupBattleArrayIndex) then
		if(1 == context[2]) then
			self.m_tblIndexBtn[context[1]]:ShowWnd(true)
		end
		g_WndMouse:ClearCursorAll()
	end
end

--判断是否将5个位置排满
function CBattleArrayCreateWnd:IsCanSave()
	for i = 1, 5 do
		if(self.m_tblIndexBtn[i]:IsShow()) then return false end
	end
	return true
end

--点击Itme事件
function CBattleArrayCreateWnd:LClickUpItem(item, pos)
	local tbl = self.m_tblArrayPosTemp
	local cursorState, context = g_WndMouse:GetCursorState()
	if(cursorState == ECursorState.eCS_PickupBattleArrayIndex) then
		for i, v in pairs(tbl) do
			if(v == pos) then return end --如果该位置已被占用,就直接返回(为判断服务器的安全性,可注释掉)
		end
		if(2 == context[2]) then
			local lastPos = tbl[context[1]]
			self.m_tblGridWnd[lastPos]:ShowWnd(false)
			self.m_tblGridWnd[lastPos]:SetWndText("")
		end
		g_WndMouse:ClearCursorAll()
		self.m_tblGridWnd[pos]:ShowWnd(true)
		self.m_tblGridWnd[pos]:SetWndText(context[1])
		tbl[context[1]] = pos
		self.m_OkBtn:EnableWnd(self:IsCanSave())
	elseif(cursorState == ECursorState.eCS_Normal) then
		for i, v in pairs(tbl) do
			if(v == pos) then
				g_WndMouse:SetPickupBattleArrayIndex(i, 2, self.m_tblGridWnd[pos])
				break
			end
		end
	end
end

--打开编辑阵型界面
function CBattleArrayCreateWnd:OpenAndReEdit(arrayName, openPlace, itemId) --openPlace：0从包裹打开，1从阵法库打开
	local tbl = g_GameMain.m_BattleArrayBase.m_tblBattleArrayPos --阵型信息表
	local nameIndex = g_GameMain.m_BattleArrayBase:GetArrayNameIndex(arrayName)
	local tblThisArrayPos = tbl[nameIndex]
	self.m_ItemId    = itemId
	self.m_openPlace = openPlace
	if(0 == openPlace) then --从背包的阵法书打开
		--已经拥有该阵法
		if(tblThisArrayPos and 1 == tblThisArrayPos[1][3]) then MsgClient(12006, arrayName) return end
		if(next(self.m_BattleArrayBookInfo)) then --该阵法书携带阵型信息
			self.m_tblArrayPosTemp = self.m_BattleArrayBookInfo
			self:DrawPos({{},self.m_BattleArrayBookInfo})
		else --该阵法书未携带阵型信息
			if(not tblThisArrayPos) then --现在、曾经都没有该阵法
				self:Reset()
			else --曾经拥有该阵法
				if(0 == tblThisArrayPos[1][3]) then --曾经拥有该阵法
					self.m_tblArrayPosTemp = tblThisArrayPos[2]
					self:DrawPos(tblThisArrayPos)
				end
			end
		end
	else --从阵法库打开
		self:DrawPos(tblThisArrayPos)
	end
	self.m_sArrayName = arrayName
	self.m_ArrayNameLabel:SetWndText(arrayName)
	self:ShowWnd(true)
	self.m_OkBtn:EnableWnd(self:IsCanSave())
end

function CBattleArrayCreateWnd:Reset()
	self:ClearCursor()
	for i = 1, 5 do self.m_tblIndexBtn[i]:ShowWnd(true) end
	for i = 0, 99 do
		self.m_tblGridWnd[i]:ShowWnd(false)
		self.m_tblGridWnd[i]:SetWndText("")
	end
	self.m_tblArrayPosTemp = {}
	self.m_OkBtn:EnableWnd(false)
end

--绘制背景层
function CBattleArrayCreateWnd:DrawBG()
	local bk = SQRDialog:new()
	bk:CreateFromResEx("BagGridBackground",self,true,true)
	bk:ShowWnd( true )
	local SIZE = bk:GetWndWidth()
	local Flag = IMAGE_PARAM:new(SM_BS_BK, IP_ENABLE)
	local SrcImage = bk:GetWndBkImage(Flag)

	for i=1, 100 do
		local x, y = self:ParsePos(i, 10)
		local pos  = CPos:new(SIZE*x,SIZE*y)
		local item = self.m_ArrayList:GetSubItem(y, x)
		local DescImage = item:GetWndBkImage(Flag)
		local ImageBk = IMAGE:new()
		SrcImage:GetImage(SrcImage:GetImageCount()-1, ImageBk)
		ImageBk:SetPosWndX(pos.x)
		ImageBk:SetPosWndY(pos.y)
		DescImage:AddImageFromImageList(SrcImage,0,-1)
	end
	bk:DestroyWnd()
end

function CBattleArrayCreateWnd:ParsePos(Pos, cols)
	Pos = Pos - 1
	local y = math.floor(Pos / cols)
	local x = Pos - y*cols
	return x, y
end

function CBattleArrayCreateWnd:DrawArrayListItem()
	for i = 0, 99 do
		local item = self.m_ArrayList:GetSubItem(i/10, i%10)
		local gridItem = self:CreateItemWnd(item)
		self.m_tblGridWnd[i] = gridItem
	end
end

function CBattleArrayCreateWnd:CreateItemWnd(parent)
	local wnd = CArrayCreateGridWnd:new()
	wnd:CreateFromRes("CommonGrid", parent)
	wnd:ShowWnd(true)
	return wnd
end

function CBattleArrayCreateWnd:DrawPos(tblThisAPos)
	self:Reset()
	for j = 1, 5 do
		self.m_tblArrayPosTemp = tblThisAPos[2]
		self.m_tblGridWnd[tblThisAPos[2][j]]:ShowWnd(true)
		self.m_tblGridWnd[tblThisAPos[2][j]]:SetWndText(j)
	end
	for i = 1, 5 do self.m_tblIndexBtn[i]:ShowWnd(false) end
end

function CBattleArrayCreateWnd:SaveBattleArrayPos()
	local tblArrayPos		= g_GameMain.m_BattleArrayBase.m_tblBattleArrayPos
	local bHaveTheArray		= false
	local tblBookInfo		= self.m_tblTmpBattleArrayBook
	local nameIndex			= g_GameMain.m_BattleArrayBase:GetArrayNameIndex(self.m_sArrayName)
	local tblArrayPosTemp = self.m_tblArrayPosTemp
	if(tblArrayPos[nameIndex] ~= nil and 1 == tblArrayPos[nameIndex][1][3]) then bHaveTheArray = true end
	if(next(tblBookInfo) and (not bHaveTheArray)) then --如果没有该阵法
		Gac2Gas:SaveBattleArrayPos(g_Conn, self.m_sArrayName, tblArrayPosTemp[1], tblArrayPosTemp[2], tblArrayPosTemp[3], tblArrayPosTemp[4], tblArrayPosTemp[5], tblBookInfo[1], tblBookInfo[2])
		tblBookInfo = {}
	else
		Gac2Gas:SaveBattleArrayPos(g_Conn, self.m_sArrayName, tblArrayPosTemp[1], tblArrayPosTemp[2], tblArrayPosTemp[3], tblArrayPosTemp[4], tblArrayPosTemp[5], -1, -1)
	end
end

--得到物品动态信息
function CBattleArrayCreateWnd:GetBattleArrayBookInfo(itemId)
	local bookInfo = g_DynItemInfoMgr:GetDynItemInfo(itemId)
	self.m_BattleArrayBookInfo = {bookInfo.Pos1, bookInfo.Pos2, bookInfo.Pos3, bookInfo.Pos4, bookInfo.Pos5}
end

function CBattleArrayCreateWnd:CloseByDelItem(itemId)
	if(self:IsShow() and self.m_ItemId == itemId) then
		self:ShowWnd(false)
	end
end


------------------------------------------------------
function CBattleArrayCreateWnd:ReDrawGrid(pos)
	self.m_tblGridWnd[pos]:Destroy()
	local item = self.m_ArrayList:GetSubItem(pos/10, pos%10)
	local gridItem = self:CreateItemWnd(item)
	self.m_tblGridWnd[pos] = self:CreateItemWnd(item)
end