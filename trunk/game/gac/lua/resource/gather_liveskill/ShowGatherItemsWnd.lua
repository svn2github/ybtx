CShowGatherItemsWnd = class (SQRDialog)
GatherItemsRect = nil

local function CreateShowGatherItemsWnd(Parent, BigId, ItemName, ItemCount, MsgId, Arg)
	local Wnd = CShowGatherItemsWnd:new()
	local SmallIcon = g_ItemInfoMgr:GetItemInfo(BigId,ItemName,"SmallIcon")
	local DisplayName = g_ItemInfoMgr:GetItemLanInfo(BigId,ItemName,"DisplayName")
	Wnd:Init(Parent)
	g_LoadIconFromRes(SmallIcon, Wnd.m_ItemIcon, -1, IP_ENABLE, IP_ENABLE)
	Wnd.m_ItemIcon:SetWndText(ItemCount)
	Wnd.m_Desc:SetWndText(DisplayName)
	g_Tooltips:LoadIconFromRes(Wnd.m_ItemIcon,SmallIcon)
	g_SetItemRichToolTips(Wnd.m_ItemIcon,BigId, ItemName, 0)
	local text = Arg and GetStaticTextClient(MsgId, GetFlowerDispalyName(Arg)) or GetStaticTextClient(MsgId)
	Wnd.m_Content:SetWndText(text)
	
	if GatherItemsRect then
		GatherItemsRect.top = GatherItemsRect.top + 10
		GatherItemsRect.bottom = GatherItemsRect.bottom + 10
		GatherItemsRect.left = GatherItemsRect.left + 10
		GatherItemsRect.right = GatherItemsRect.right + 10
		Wnd:SetLogicRect(GatherItemsRect)
		return Wnd
	end
	--±£´æ´°¿ÚÎ»ÖÃ
	GatherItemsRect = CFRect:new()
	Wnd:GetLogicRect(GatherItemsRect)
	
	return Wnd
end

function CShowGatherItemsWnd:Init(Parent)
	self:CreateFromRes("ShowGatherItemsWnd", Parent)
	self:ShowWnd( true )
end

function CShowGatherItemsWnd:AddImage(Wnd, StateStr)
	local Flag = IMAGE_PARAM:new()
	Flag.CtrlMask = SM_BS_BK
	Flag.StateMask = IP_ENABLE	
	local DefaultImage = WND_IMAGE_LIST:new()
	DefaultImage:AddImage(Wnd:GetGraphic(), -1,  StateStr, nil, CFPos:new(), "0xffffffff", 15 ) 	
	Wnd:SetWndBkImage( Flag, DefaultImage )
end

function CShowGatherItemsWnd:OnChildCreated()
	self.m_BtnOK	= self:GetDlgChild("BtnOK")
	self.m_ItemIcon	= self:GetDlgChild("ItemIcon")
	self.m_Content	= self:GetDlgChild("Content")
	self.m_Desc		= self:GetDlgChild("Desc")
end

function CShowGatherItemsWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if( uMsgID == BUTTON_LCLICK ) then
		if Child == self.m_BtnOK then
			GatherItemsRect = nil
			self:DestroyWnd()
			
			for k, v in ipairs(g_GameMain.m_tblShowGatherItemsWnd) do
				if( v == self ) then
					table.remove(g_GameMain.m_tblShowGatherItemsWnd, k)
				end
			end
			
		end
	end
end

function CShowGatherItemsWnd.ShowGatherItemsWnd(Conn, BigId, ItemName, ItemCount, MsgId, Arg)
	if( not g_GameMain.m_tblShowGatherItemsWnd ) then
		g_GameMain.m_tblShowGatherItemsWnd = {}
	end
	local wnd = CreateShowGatherItemsWnd(g_GameMain, BigId, ItemName, ItemCount, MsgId, Arg)
	table.insert(g_GameMain.m_tblShowGatherItemsWnd, wnd)
end