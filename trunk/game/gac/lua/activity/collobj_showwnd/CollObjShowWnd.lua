CCollObjShowWnd = class(SQRDialog)
CCollObjItemGridCut = class(SQRDialog)

function CCollObjShowWnd:Ctor()
	self:CreateFromRes("WndCollObj",g_GameMain) 
	self.m_CloseButton = self:GetDlgChild("WndClose")
	self.m_CanCollItemTbl={}
	self.m_DeleteIndexNum = 0
	self.m_CollItem = self:GetDlgChild("WndGetAllItem")
	self.m_BtnPageUp = self:GetDlgChild("LastPage")
	self.m_BtnPageDown = self:GetDlgChild("NextPage")
	--Wnd.m_CancelColl = Wnd:GetDlgChild("WndCancel")
end

function CCollObjShowWnd:InitCollObjWnd(CollItemTbl)
	if self.StartIndex>self.EndIndex then
		return
	end
	local GetItemWnd={"GetItemBtn1","GetItemBtn2","GetItemBtn3","GetItemBtn4"}
	local ItemNameWnd={"ItemName1","ItemName2","ItemName3","ItemName4"}
	self.m_SelectItemWnd={}
	self.m_ItemNameWnd={}
	for i=1,4 do
		table.insert(self.m_SelectItemWnd,self:GetDlgChild(GetItemWnd[i]))
		local PicWnd = CCollObjItemGridCut:new()
		PicWnd:CreateFromRes("CommonGridCut", self.m_SelectItemWnd[i])
		PicWnd:ShowWnd( true )
		self.m_SelectItemWnd[i].m_Picture = PicWnd
		table.insert(self.m_ItemNameWnd,self:GetDlgChild(ItemNameWnd[i]))
	end
	for i=1,4 do
		if i+self.StartIndex-1>self.EndIndex or CollItemTbl[i+self.StartIndex-1]==nil then
			self.m_SelectItemWnd[i]:ShowWnd(false)
			self.m_ItemNameWnd[i]:ShowWnd(false)
			self.m_SelectItemWnd[i]:EnableWnd(false)
			self.m_SelectItemWnd[i]:SetWndText("")
			self.m_ItemNameWnd[i]:SetWndText("")
			--self.m_BtnGetItemWnd[i]:ShowWnd(false)
		else
			local ItemType,ItemName,ItemNum= CollItemTbl[i+self.StartIndex-1][1],CollItemTbl[i+self.StartIndex-1][2],CollItemTbl[i+self.StartIndex-1][3]
			--print("ItemType,ItemName,ItemNum",ItemType,ItemName,ItemNum)
			local SmallIcon		= g_ItemInfoMgr:GetItemInfo(ItemType,ItemName,"SmallIcon")
			local picwnd = self.m_SelectItemWnd[i].m_Picture
			g_LoadIconFromRes(SmallIcon, picwnd, -1, IP_ENABLE, IP_ENABLE)
			picwnd:SetWndText(ItemNum)
			self.m_SelectItemWnd[i]:ShowWnd(true)
			self.m_SelectItemWnd[i]:EnableWnd(true)
			self.m_ItemNameWnd[i]:SetWndText(g_ItemInfoMgr:GetItemLanInfo(ItemType,ItemName,"DisplayName"))
			self.m_ItemNameWnd[i]:ShowWnd(true)
			--self.m_BtnGetItemWnd[i]:EnableWnd(true)
		end
	end
	if self.EndIndex>4 then
		self.m_BtnPageUp:EnableWnd(true)
	else
		self.m_BtnPageUp:EnableWnd(false)
	end
	if table.getn(CollItemTbl)-self.EndIndex>0 then
		self.m_BtnPageDown:EnableWnd(true)
	else
		self.m_BtnPageDown:EnableWnd(false)
	end
	if table.getn(CollItemTbl)==0 then
		self.m_CollItem:EnableWnd(false)
	end
	self:ShowWnd(true)
	self.m_IsInOpened = true
end

function CCollObjShowWnd:ChangePageItem()
	if table.getn(self.m_CanCollItemTbl)-g_GameMain.m_CollObjWnd.StartIndex>=4 then
		self.EndIndex=g_GameMain.m_CollObjWnd.StartIndex+3
	else
		self.EndIndex=table.getn(self.m_CanCollItemTbl)
	end
	self:InitCollObjWnd(self.m_CanCollItemTbl)
	self:ShowWnd(true)
end
function CCollObjShowWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	if(uMsgID == BUTTON_LCLICK) then
		if ( Child == self.m_CloseButton) then
			Gac2Gas:CancelCollItems(g_Conn,self.GlobalID)
			self.m_CanCollItemTbl={}
			self.m_DeleteIndexNum = 0
			self:ShowWnd(false)
			self.m_IsInOpened = nil
		elseif(Child == self.m_CollItem) then
			--print("进行全部采集")
			Gac2Gas:SelectAllGridItems(g_Conn,self.GlobalID)
			return
		elseif(Child == g_GameMain.m_CollObjWnd:GetDlgChild("LastPage")) then
			--print("提取物品1")
			self.StartIndex=self.StartIndex-4
			self:ChangePageItem()
			return
		elseif(Child == g_GameMain.m_CollObjWnd:GetDlgChild("NextPage")) then
			--print("提取物品1")
			self.StartIndex=self.StartIndex+4
			self:ChangePageItem()
			return
		elseif(Child == g_GameMain.m_CollObjWnd:GetDlgChild("GetItemBtn1")) then
			--print("提取物品1")
			local GetIndex = self.StartIndex
			Gac2Gas:SelectSingleGridItem(g_Conn,GetIndex,self.GlobalID)
			return
		elseif(Child == g_GameMain.m_CollObjWnd:GetDlgChild("GetItemBtn2")) then
			--print("提取物品2")
			local GetIndex = self.StartIndex+1
			Gac2Gas:SelectSingleGridItem(g_Conn,GetIndex,self.GlobalID)
			return
		elseif(Child == g_GameMain.m_CollObjWnd:GetDlgChild("GetItemBtn3")) then
			--print("提取物品3")
			local GetIndex = self.StartIndex+2
			Gac2Gas:SelectSingleGridItem(g_Conn,GetIndex,self.GlobalID)
			return
		elseif(Child == g_GameMain.m_CollObjWnd:GetDlgChild("GetItemBtn4")) then
			--print("提取物品4")
			local GetIndex = self.StartIndex+3
			Gac2Gas:SelectSingleGridItem(g_Conn,GetIndex,self.GlobalID)
			return
		else
			return
		end
	end
	return
end

function CCollObjShowWnd:RemoveOneGridItem(Grid)
	g_DelWndImage(self.m_SelectItemWnd[Grid], 1, IP_ENABLE, IP_CLICKDOWN)
	self.m_SelectItemWnd[Grid]:ShowWnd(false)
	self.m_ItemNameWnd[Grid]:ShowWnd(false)
	--self.m_CollItem:EnableWnd(false)
	local DropItemTbl=self.m_CanCollItemTbl
	if table.getn(DropItemTbl)>0 then
		if table.getn(DropItemTbl)-self.StartIndex>=4 then
			self.EndIndex=self.StartIndex+3
		else
			self.EndIndex=table.getn(DropItemTbl)
		end
		self:InitCollObjWnd(DropItemTbl)
	end
end

local m_CollItemTbl = {}

function CCollObjShowWnd.RetCanCollItem(Conn,ItemType,ItemName,ItemNum)
	table.insert(m_CollItemTbl,{ItemType,ItemName,ItemNum})
end

function CCollObjShowWnd.RetCanCollItemEnd(Conn,GlobalID,PerfectType)
	local CollObj = CIntObjClient_GetIntObjByID(GlobalID)
	if CollObj==nil then
		return
	end
	if PerfectType >= 3 then
		MsgClient(3150)
	elseif PerfectType > 1 then
		MsgClient(3155)
	end
	local CollObjName = CollObj.m_Properties:GetCharName()
	if not next(m_CollItemTbl) then
		g_GameMain.m_CollObjWnd.StartIndex=1
		g_GameMain.m_CollObjWnd.EndIndex=1
	else
		g_GameMain.m_CollObjWnd.StartIndex=1
		if table.getn(m_CollItemTbl)>4 then
			g_GameMain.m_CollObjWnd.EndIndex=4
		else
			g_GameMain.m_CollObjWnd.EndIndex=table.getn(m_CollItemTbl)
		end
	end
	g_GameMain.m_CollObjWnd:InitCollObjWnd(m_CollItemTbl)
	g_GameMain.m_CollObjWnd.m_CanCollItemTbl = m_CollItemTbl
	m_CollItemTbl = {}
	g_GameMain.m_CollObjWnd.GlobalID=GlobalID
end

function CCollObjShowWnd.RetCollAllItemGrid(Conn,GetIndex)
	local removeindex = GetIndex - g_GameMain.m_CollObjWnd.m_DeleteIndexNum
	--print(GetIndex ,removeindex,  "GetIndex, removeindex")
	table.remove(g_GameMain.m_CollObjWnd.m_CanCollItemTbl,removeindex)
	if removeindex <= 4 then
		g_GameMain.m_CollObjWnd:RemoveOneGridItem(removeindex)
	end
	g_GameMain.m_CollObjWnd.m_DeleteIndexNum = g_GameMain.m_CollObjWnd.m_DeleteIndexNum + 1
end

function CCollObjShowWnd.NotifyCloseCollWnd(Conn)
	g_GameMain.m_CollObjWnd:ShowWnd(false)
	g_GameMain.m_CollObjWnd.m_IsInOpened = nil
	g_GameMain.m_CollObjWnd.m_CanCollItemTbl={}
	g_GameMain.m_CollObjWnd.m_DeleteIndexNum = 0
end
--
--function Gas2Gac:RetCollGetGridItem(Conn,GetIndex)
--	table.insert(g_GameMain.m_CollObjWnd.m_DeleteIndex,GetIndex)
--end

function CCollObjShowWnd.RetCollGetGridItem(Conn,GetIndex,Succ)
	if not Succ then
		return
	end
	local Grid = GetIndex%4
	if Grid == 0 then
		Grid = 4
	end
	--print(Grid ,GetIndex,  "Grid, GetIndex")
	table.remove(g_GameMain.m_CollObjWnd.m_CanCollItemTbl,GetIndex)
	g_GameMain.m_CollObjWnd:RemoveOneGridItem(Grid)	
	--[[
	--print(Grid ,GetIndex,  "Grid, GetIndex")
	local removeindex = GetIndex - g_GameMain.m_CollObjWnd.m_DeleteIndexNum
	table.remove(g_GameMain.m_CollObjWnd.m_CanCollItemTbl,removeindex)
	--if Grid>0 and Grid <= 4 then
		g_DelWndImage(g_GameMain.m_CollObjWnd.m_SelectItemWnd[Grid], 1, IP_ENABLE, IP_CLICKDOWN)
	--end
	g_GameMain.m_CollObjWnd.m_SelectItemWnd[Grid]:ShowWnd(false)
	g_GameMain.m_CollObjWnd.m_ItemNameWnd[Grid]:ShowWnd(false)
	local CollItemTbl=g_GameMain.m_CollObjWnd.m_CanCollItemTbl
	if table.getn(CollItemTbl)>0 then
		--print(g_GameMain.m_CollObjWnd.StartIndex, "g_GameMain.m_CollObjWnd.StartIndex")
		if table.getn(CollItemTbl)-g_GameMain.m_CollObjWnd.StartIndex>=4 then
			g_GameMain.m_CollObjWnd.EndIndex=g_GameMain.m_CollObjWnd.StartIndex+3
		else
			g_GameMain.m_CollObjWnd.EndIndex=table.getn(CollItemTbl)
		end
		g_GameMain.m_CollObjWnd:InitCollObjWnd(CollItemTbl)
	end
	g_GameMain.m_CollObjWnd.m_DeleteIndexNum = g_GameMain.m_CollObjWnd.m_DeleteIndexNum + 1
	--]]
end

function CCollObjShowWnd.RetSelectAllGridItems(Conn,Succ)
	if not Succ then
		g_GameMain.m_CollObjWnd.m_DeleteIndexNum = 0
		return
	end
	g_GameMain.m_CollObjWnd.m_CanCollItemTbl={}
	g_GameMain.m_CollObjWnd:ShowWnd(false)
	g_GameMain.m_CollObjWnd.m_IsInOpened = nil
	g_GameMain.m_CollObjWnd.m_DeleteIndexNum = 0
end
