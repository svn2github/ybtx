CBreakItemWnd = class(SQRDialog)
CBreakItemGridCut = class ( SQRDialog )

function CBreakItemWnd.CreateBreakItemWnd(Parent)
	local wnd	=	CBreakItemWnd:new()
	wnd:CreateFromRes("WndCollObj",Parent)

	wnd:InitBreakItemWndChild()
	g_ExcludeWndMgr:InitExcludeWnd(wnd, "·Ç»¥³â")

	return wnd
end

function CBreakItemWnd:InitBreakItemWndChild()
	self.m_Close		=	self:GetDlgChild("WndClose")	
	self.m_LastPage		=	self:GetDlgChild("LastPage")		
	self.m_NextPage		=	self:GetDlgChild("NextPage")	
	self.m_WndGetAllItem=	self:GetDlgChild("WndGetAllItem")
	self.m_WndGetAllItem:EnableWnd(false)
	self.m_WndGetAllItem:ShowWnd(false)
	self.m_ItemName1	=	self:GetDlgChild("ItemName1")		
	self.m_ItemName2	=	self:GetDlgChild("ItemName2")		
	self.m_ItemName3	=	self:GetDlgChild("ItemName3")		
	self.m_ItemName4	=	self:GetDlgChild("ItemName4")		
	
	self.m_GetItemBtn1	=	self:GetDlgChild("GetItemBtn1")		
	local PicWnd1 = CBreakItemGridCut:new()  
	PicWnd1:CreateFromRes("CommonGridCut", self.m_GetItemBtn1)
	PicWnd1:ShowWnd( true )
	self.m_GetItemBtn1.m_PicWnd = PicWnd1
	--self.m_GetItemBtn1:EnableWnd(false)
	
	self.m_GetItemBtn2	=	self:GetDlgChild("GetItemBtn2")		
	local PicWnd2 = CBreakItemGridCut:new()  
	PicWnd2:CreateFromRes("CommonGridCut", self.m_GetItemBtn2)
	PicWnd2:ShowWnd( true )
	self.m_GetItemBtn2.m_PicWnd = PicWnd2
	--self.m_GetItemBtn2:EnableWnd(false)
	
	self.m_GetItemBtn3	=	self:GetDlgChild("GetItemBtn3")	
	local PicWnd3 = CBreakItemGridCut:new()  
	PicWnd3:CreateFromRes("CommonGridCut", self.m_GetItemBtn3)
	PicWnd3:ShowWnd( true )
	self.m_GetItemBtn3.m_PicWnd = PicWnd3
	--self.m_GetItemBtn3:EnableWnd(false)
	
	self.m_GetItemBtn4	=	self:GetDlgChild("GetItemBtn4")		
	local PicWnd4 = CBreakItemGridCut:new()  
	PicWnd4:CreateFromRes("CommonGridCut", self.m_GetItemBtn4)
	PicWnd4:ShowWnd( true )
	self.m_GetItemBtn4.m_PicWnd = PicWnd4
	--self.m_GetItemBtn4:EnableWnd(false)
end

function CBreakItemWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if uMsgID == BUTTON_LCLICK then
		if self.m_Close == Child then
			self:ShowWnd(false)
		elseif self.m_LastPage == Child then
			self.m_CurPageNo	=	self.m_CurPageNo - 1
		elseif self.m_NextPage == Child then
			self.m_CurPageNo	=	self.m_CurPageNo + 1		
		end	
	end
end

function CBreakItemWnd:ShowBreakProductsDetails()
	local breakProductTbl	=	self.m_tblBreakItemProducts
	local breakProductTblLen	=	#breakProductTbl
	for i =1, breakProductTblLen do
		local SmallIcon = g_ItemInfoMgr:GetItemInfo(breakProductTbl[i].ItemType, breakProductTbl[i].ItemName,"SmallIcon")
		local iconWnd, nameWnd = self:GetAcurateItem(i)
		local PicWnd = iconWnd.m_PicWnd
		g_DelWndImage(PicWnd, 1, IP_ENABLE, IP_ENABLE)
		g_LoadIconFromRes(SmallIcon, PicWnd, -1, IP_ENABLE, IP_ENABLE)
		PicWnd:SetWndText(breakProductTbl[i].ItemCount)
		local DisplayName = g_ItemInfoMgr:GetItemLanInfo(breakProductTbl[i].ItemType, breakProductTbl[i].ItemName,"DisplayName")
		nameWnd:SetWndText(DisplayName)
		iconWnd:ShowWnd(true)
		nameWnd:ShowWnd(true)
	end
	for i=breakProductTblLen+1, 4 do				
		local iconWnd, nameWnd = self:GetAcurateItem(i)
		iconWnd:ShowWnd(false)
		nameWnd:ShowWnd(false)
	end
	
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

function CBreakItemWnd:GetAcurateItem(wndIndex)
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

