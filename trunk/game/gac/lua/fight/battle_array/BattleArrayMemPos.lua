gac_require "fight/battle_array/BattleArrayMemPosInc"

function CreateBattleArrayMemPosWnd(Parent)
	local Wnd = CBattleArrayMemPosWnd:new()
	Wnd:Init(Parent)
	return Wnd
end

function CBattleArrayMemPosWnd:Init(Parent)
	self.m_MemNum				= 0
	self.m_bMemChanged			= true
	self.m_tblThisArrayPosInfo	= {}
	self.m_tblMemberInfo		= {}
	self.m_tblMemPosInfoTemp	= {{0,"",-1},{0,"",-1},{0,"",-1},{0,"",-1},{0,"",-1}}
	self.m_sArrayName			= ""
	self.m_tblGridWnd			= {}
	self:CreateFromRes("BattleArrayMemPos", Parent)
	self:ReDrawList()
end

function CBattleArrayMemPosWnd:OnChildCreated()
	self.m_XBtn				= self:GetDlgChild("XBtn")
	self.m_ArrayList		= self:GetDlgChild("ArrayList")
	self.m_OkBtn			= self:GetDlgChild("OkBtn")
	self.m_ResetBtn			= self:GetDlgChild("ResetBtn")
	self.m_CancelBtn		= self:GetDlgChild("CancelBtn")
	self.m_ArrayDes			= self:GetDlgChild("ArrayDes")
	self.m_PosByOrderBtn	= self:GetDlgChild("PosByOrderBtn")
	self.m_tblMemBtn		= {}
	self.m_tblNameLabel		= {}
	for i = 1, 5 do
		self.m_tblNameLabel[i]	= self:GetDlgChild("NameLabel" .. i)
		self.m_tblMemBtn[i]		= self:GetDlgChild("Mem" .. i)
		self.m_tblNameLabel[i]:ShowWnd(false)
		self.m_tblMemBtn[i]:ShowWnd(false)
	end
end

--消息
function CBattleArrayMemPosWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if(uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_XBtn or Child == self.m_CancelBtn) then
			local cursorState = g_WndMouse:GetCursorState()
			if(cursorState == ECursorState.eCS_PickupBattleArrayMember) then
				self:ClearCursor()
			end
			self:ShowWnd(false)
		elseif(Child == self.m_ResetBtn) then
			self:Reset()
		elseif(Child == self.m_OkBtn) then --保存列阵
			self:SaveBattleArrayMemPos()
		elseif(Child == self.m_PosByOrderBtn) then --顺序排列
			self:Reset()
			self:PosByOrder()
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

--重绘list
function CBattleArrayMemPosWnd:ReDrawList()
	self.m_ArrayList:DeleteAllItem()
	for i = 0, 9 do self.m_ArrayList:InsertColumn(i, 32) end
	for i = 0, 9 do self.m_ArrayList:InsertItem(i, 32)   end
	self:DrawBG()
	self:DrawArrayListItem()
	for i = 0, 9 do
		for j = 0, 9 do
			local item = self.m_ArrayList:GetSubItem(i, j)
			item:EnableWnd(false)
		end
	end
end

--打开本面板
function CBattleArrayMemPosWnd:OpenWnd(arrayName)
	if(not g_GameMain.m_TeamBase.m_bCaptain) then return end
	self.m_sArrayName = arrayName
	self:SetWndText(arrayName)
	self:FreshGridBtn()
	local base					= g_GameMain.m_BattleArrayBase
	local tblPos				= base.m_tblBattleArrayPos --阵型信息表
	local tblMemPos				= base.m_tblBattleArrayMem --列阵信息表
	local nameIndex				= base:GetArrayNameIndex(arrayName)
	local tblThisArrayPos		= tblPos[nameIndex]    --当前的阵型信息
	local tblThisArrayMemPos	= tblMemPos[nameIndex] --当前的列阵信息
	local memberInfo			= self.m_tblMemberInfo
	local tblBtnBeShow			= {}
	self.m_tblThisArrayPosInfo	= tblPos[nameIndex]
	self.m_tblMemPosInfoTemp	= {{0,"",-1},{0,"",-1},{0,"",-1},{0,"",-1},{0,"",-1}}
	if(tblThisArrayMemPos) then --如果有列阵信息
		for i = 1, 5 do
			for j = 1, 5 do
				if(0 ~= tblThisArrayMemPos[2][i] and tblThisArrayMemPos[2][i] == memberInfo[j][1]) then --用Id来查找，得该到成员信息
--					local id	= tblThisArrayMemPos[2][i]
--					local name	= memberInfo[j][2]
--					local pos	= tblThisArrayPos[2][i]
--					self.m_tblMemPosInfoTemp[i] = {id, name, pos} --初始化该列阵的成员信息
					tblBtnBeShow[j] = true
				end
			end
		end
	end
	self.m_MemNum = 0
	for i = 1, 5 do
		if(tblThisArrayPos) then --如果有阵型信息
			local item = self.m_ArrayList:GetSubItem(tblThisArrayPos[2][i]/10, tblThisArrayPos[2][i]%10)
			item:SetWndText(i)
			item:EnableWnd(true)
		end
		local isOn = false
		if(tblThisArrayMemPos) then --如果有列阵信息
			local id = tblThisArrayMemPos[2][i]
			if(0 ~= id) then		--如果i位置有人
				local isLeaveNow = true
				for j = 1, #(memberInfo) do
					if(id == memberInfo[j][1]) then --判断此人是否还在队伍中
						isLeaveNow = false
						g_WndMouse:SetPickupBattleArrayMember(i, 1, memberInfo[j][1], memberInfo[j][2], self.m_tblMemBtn[j])
						break
					end
				end
				local nPos = tblThisArrayPos[2][i]
				if(isLeaveNow) then
					tblThisArrayMemPos[2][i] = 0 --清空离开的队员列阵信息
				else
					self:PutMemOn(nPos)
				end
				self.m_tblGridWnd[nPos]:ShowWnd(not isLeaveNow)	--itemWnd是否可见
			end
		end
		
		--对小队成员按钮的控制
		local bFlag = (0 ~= memberInfo[i][1])
		self.m_tblNameLabel[i]:ShowWnd(bFlag)
		self.m_tblNameLabel[i]:SetWndText(memberInfo[i][2])
		if(bFlag) then 
			self.m_MemNum = self.m_MemNum + 1
		end
		if(tblBtnBeShow[i]) then bFlag = false end
		self.m_tblMemBtn[i]:ShowWnd(bFlag)
	end
	self.m_ArrayDes:SetWndText(base:GetArraySkillDes(arrayName))
	self.m_OkBtn:EnableWnd(self:IsCanSave())
	self:ShowWnd(true)
end

--传入小队成员信息
function CBattleArrayMemPosWnd:SetMemInfo(tblInfo)
	local name	= g_MainPlayer.m_Properties:GetCharName()
	local id	= g_MainPlayer.m_Properties:GetCharID()
	if(not next(tblInfo)) then
		self.m_tblMemberInfo = {}
		return
	end
	self:SetIsMemChange(tblInfo)
	self.m_tblMemberInfo = {{id, name, true}, unpack(tblInfo)} -- id name bOnline
end

--队员是否有变化
function CBattleArrayMemPosWnd:SetIsMemChange(tblInfo)
	local tbl = self.m_tblMemberInfo
	if(#(tblInfo) > #(tbl)-1) then --队员增加,就要重新列阵
		self.m_bMemChanged = true
		return
	end

---------以下注释掉的代码是小队人员有变化（数量、顺序）就要重新列阵。
--	if(#(tblInfo) ~= #(tbl)-1) then
--		self.m_bMemChanged = true
--		return
--	end
--	for i = 1, #(tblInfo) do
--		if(tblInfo[i][1] ~= tbl[i+1][1]) then
--			self.m_bMemChanged = true
--			return
--		end
--	end

	self.m_bMemChanged = false
end


--开始拖拽Btn
function CBattleArrayMemPosWnd:StartDragBtn(Child)
	local tbl = self.m_tblMemberInfo
	local cursorState = g_WndMouse:GetCursorState()
	for i = 1, 5 do
		if(Child == self.m_tblMemBtn[i] and cursorState == ECursorState.eCS_Normal) then
			g_WndMouse:SetPickupBattleArrayMember(i, 1, tbl[i][1], tbl[i][2], Child) --1是从按钮点起的，2是从Item点起的
			Child:ShowWnd(false)
			return
		end
	end
end

--清空鼠标
function CBattleArrayMemPosWnd:ClearCursor()
	local cursorState, context = g_WndMouse:GetCursorState()
	if(cursorState == ECursorState.eCS_PickupBattleArrayIndex) then
		if(1 == context[2]) then
			self.m_tblMemBtn[context[1]]:ShowWnd(true)
		end
		g_WndMouse:ClearCursorAll()
	end
end

--判断是否已将所有成员列阵
function CBattleArrayMemPosWnd:IsCanSave()
	for i = 1, 5 do
		if(self.m_tblMemBtn[i]:IsShow()) then return false end
	end
	return true
end


function CBattleArrayMemPosWnd:PutMemOn(pos)
	local tblMemPos	= self.m_tblMemPosInfoTemp
	local tbl		= self.m_tblThisArrayPosInfo
	local cursorState, context = g_WndMouse:GetCursorState()
	if(not context) then return end
	self.m_tblGridWnd[pos]:ShowWnd(true)
	g_WndMouse:CopySkillIconToWnd(self.m_tblGridWnd[pos])
	g_WndMouse:ClearCursorAll()
	for i = 1, 5 do
		if(tbl[2][i] == pos) then 
			tblMemPos[i] = {context[3], context[4], pos} --id, name, pos
		end
	end
end

--点击Itme事件
function CBattleArrayMemPosWnd:LClickUpItem(item, pos)
	local tblMemPos	= self.m_tblMemPosInfoTemp
	local tbl		= self.m_tblThisArrayPosInfo
	local cursorState, context = g_WndMouse:GetCursorState()
	if(cursorState == ECursorState.eCS_PickupBattleArrayMember) then
		for i, v in pairs(tblMemPos) do
			if(v[3] == pos) then return end --如果该位置已经有人,就直接返回(为判断服务器的安全性,可注释掉)
		end
		if(2 == context[2]) then
			local lastPos = tbl[2][context[1]]
			self.m_tblGridWnd[lastPos]:ShowWnd(false)
			tblMemPos[context[1]] = {0,"",-1}
		end
		self:PutMemOn(pos)
		self.m_OkBtn:EnableWnd(self:IsCanSave())
	elseif(cursorState == ECursorState.eCS_Normal) then
		for i, v in pairs(tblMemPos) do
			if(v[3] == pos) then
				g_WndMouse:SetPickupBattleArrayMember(i, 2, tblMemPos[i][1], tblMemPos[i][2], self.m_tblGridWnd[pos])
				break
			end
		end
	end
end

--重置
function CBattleArrayMemPosWnd:Reset()
	g_WndMouse:ClearCursorAll()
	local tbl = self.m_tblGridWnd
	for i = 1, 5 do
		local bFlag = (0 ~= self.m_tblMemberInfo[i][1])
		local index = self.m_tblMemPosInfoTemp[i][3]
		self.m_tblMemBtn[i]:ShowWnd(bFlag)--对小队成员按钮的控制
		if(-1 ~= index) then tbl[index]:ShowWnd(false) end
	end
	self.m_tblMemPosInfoTemp = {{0,"",-1},{0,"",-1},{0,"",-1},{0,"",-1},{0,"",-1}}
	self.m_OkBtn:EnableWnd(self:IsCanSave())
end

--顺序排列
function CBattleArrayMemPosWnd:PosByOrder()
	local base				= g_GameMain.m_BattleArrayBase
	local tblPos			= base.m_tblBattleArrayPos --阵型信息表
	local nameIndex			= base:GetArrayNameIndex(self.m_sArrayName)
	local tblThisArrayPos	= tblPos[nameIndex]
	local tblMemPos			= self.m_tblMemPosInfoTemp
	local tbl				= self.m_tblMemberInfo
	for i = 1, self.m_MemNum do
		local pos = tblThisArrayPos[2][i]
		self.m_tblMemBtn[i]:ShowWnd(false)
		self.m_tblGridWnd[pos]:ShowWnd(true)
		g_WndMouse:SetPickupBattleArrayMember(i, 1, tbl[i][1], tbl[i][2], self.m_tblMemBtn[i])
		self:PutMemOn(pos)
	end
	g_WndMouse:ClearCursorAll()
	self.m_OkBtn:EnableWnd(self:IsCanSave())
end

function CBattleArrayMemPosWnd:SaveBattleArrayMemPos()
	local tblPos	= g_GameMain.m_BattleArrayBase.m_tblBattleArrayPos --阵型信息表
	local arrayId	= 0
	local nameIndex	= g_GameMain.m_BattleArrayBase:GetArrayNameIndex(self.m_sArrayName)
	local arrayId	= tblPos[nameIndex][1][1]
	local tblMemPos	= self.m_tblMemPosInfoTemp
	Gac2Gas:SaveBattleArrayMemPos(g_Conn,arrayId,self.m_sArrayName,tblMemPos[1][1],tblMemPos[2][1],tblMemPos[3][1],tblMemPos[4][1],tblMemPos[5][1])
end

--绘制背景层
function CBattleArrayMemPosWnd:DrawBG()
	local bk = SQRDialog:new()
	bk:CreateFromResEx( "BagGridBackground",self,true,true )
	bk:ShowWnd( true )
	local SIZE = bk:GetWndWidth()
	local Flag = IMAGE_PARAM:new(SM_BS_BK, IP_ENABLE)
	local SrcImage    = bk:GetWndBkImage(Flag)
	local FlagDisable = IMAGE_PARAM:new(SM_BS_BK, IP_DISABLE)
	local SrcImageDisable = bk:GetWndBkImage(FlagDisable)

	for i=1, 100 do
		local x, y = self:ParsePos(i, 10)
		local pos  = CPos:new(SIZE*x,SIZE*y)
		local item = self.m_ArrayList:GetSubItem(y, x)
		local DescImage			= item:GetWndBkImage(Flag)
		local DescImageDisable	= item:GetWndBkImage(FlagDisable)
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

function CBattleArrayMemPosWnd:ParsePos(Pos, cols)
	Pos = Pos - 1
	local y = math.floor(Pos / cols)
	local x = Pos - y*cols
	return x, y
end

--清空所有的ItemWnd
function CBattleArrayMemPosWnd:FreshGridBtn()
	for i = 0, 99 do
		self.m_tblGridWnd[i]:ShowWnd(false)
		local item = self.m_ArrayList:GetSubItem(i/10, i%10)
		item:SetWndText("")
		item:EnableWnd(false)
	end
end

function CBattleArrayMemPosWnd:DrawArrayListItem()
	for i = 0, 99 do
		local item = self.m_ArrayList:GetSubItem(i/10, i%10)
		local gridItem = self:CreateItemWnd(item)
		self.m_tblGridWnd[i] = gridItem
	end
end

function CBattleArrayMemPosWnd:CreateItemWnd(parent)
	local wnd = CArrayMemPosGridWnd:new()
	wnd:CreateFromRes("CommonGrid", parent)
	return wnd
end