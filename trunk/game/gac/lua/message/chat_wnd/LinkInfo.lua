gac_require "message/chat_wnd/LinkInfoInc"

local function CreateNonEquipLinkWnd(uBigID,uIndex,uGlobalID)
	local ToolTipsStr = {}
	ToolTipsStr = g_Tooltips:GetItemToolTips(uBigID,uIndex,uGlobalID)
	if g_GameMain.m_LindWnd ~= nil then
		g_GameMain.m_LindWnd:ShowWnd(false)
		g_GameMain.m_LindWnd = nil 
	end
	if g_GameMain.m_LindWnd == nil then
		local Wnd = CNonEquipLinkWnd:new()
		Wnd:CreateFromRes("LinkWnd",g_GameMain)
		g_ExcludeWndMgr:InitExcludeWnd(Wnd, 2)
		Wnd:ShowWnd(true)
		Wnd.m_RichTextWnd = Wnd:GetDlgChild("RichTextWnd")
		Wnd.m_RichTextWnd:SetAutoWidth(0, 300)
		Wnd.m_CloseBtn = Wnd:GetDlgChild("CloseBtn")
		Wnd.m_RichTextWnd:SetWndText(ToolTipsStr)
		local rt = CFRect:new()
		Wnd.m_RichTextWnd:GetLogicRect(rt)
		Wnd:SetLogicRect(rt)
		rt.left = rt.right - 20
		rt.bottom = rt.top + 20 
		Wnd.m_CloseBtn:SetLogicRect(rt)
		Wnd.m_CloseBtn:InserBefore(Wnd.m_RichTextWnd)
		g_GameMain.m_LindWnd = Wnd
	end
end

local function CreateEquipLinkWnd(uBigID,uIndex,uGlobalID)
	local ToolTipsTbl = {}
	ToolTipsTbl.n = 3
	ToolTipsTbl[1],ToolTipsTbl[3],ToolTipsTbl[2] = g_Tooltips:GetEquipItemAllTips(uBigID,uIndex,uGlobalID)

	if g_GameMain.m_EquipLinkWnd ~= nil then
		g_GameMain.m_EquipLinkWnd:ShowWnd(false)
		g_GameMain.m_EquipLinkWnd = nil 
	end
	if g_GameMain.m_EquipLinkWnd == nil then
		local Wnd = CEquipLinkWnd:new()
		Wnd:CreateFromRes("EquipLinkWnd",g_GameMain)
		g_ExcludeWndMgr:InitExcludeWnd(Wnd, 2)
		Wnd:ShowWnd(true)
		
		Wnd.m_CloseBtn = Wnd:GetDlgChild("CloseBtn")
		g_GameMain.m_EquipLinkWnd = Wnd
		local TipsInfo = {}
		TipsInfo.m_BigID = uBigID
		TipsInfo.m_Index = uIndex
		TipsInfo.m_ItemID = uGlobalID	
		g_Tooltips:CreateChatEquipWnd(Wnd,TipsInfo,false)
		
		local ParentWnd = g_GameMain.m_EquipLinkWnd
		for i = 2 ,ToolTipsTbl.n do
			if ToolTipsTbl[i] ~= nil then
				local ChildWnd = CEquipLinkWnd:new()
				ChildWnd:CreateFromRes("EquipLinkWnd",ParentWnd)
				ChildWnd:ShowWnd( true )
				ChildWnd.m_CloseBtn = ChildWnd:GetDlgChild("CloseBtn")
				ChildWnd.m_CloseBtn:ShowWnd(false)
				local uBigID,uIndex,uGlobalID  = unpack(ToolTipsTbl[i])
				local TipsInfo = {}
				TipsInfo.m_BigID = uBigID
				TipsInfo.m_Index = uIndex
				TipsInfo.m_ItemID = uGlobalID		
				g_Tooltips:CreateChatEquipWnd(ChildWnd,TipsInfo,true)				
				local ParentRect = CFRect:new()
				ParentWnd:GetLogicRect(ParentRect)			
				local ChildRect = CFRect:new()
				ChildRect.left = ParentRect.right  + 1
				ChildRect.right = ChildRect.left + ChildWnd:GetWndOrgWidth()
				ChildRect.top = ParentRect.top 
				ChildRect.bottom = ChildRect.top + ChildWnd:GetWndOrgHeight() 
				ChildWnd:SetLogicRect(ChildRect)
				ParentWnd.m_ChildWnd = ChildWnd
				ParentWnd = ParentWnd.m_ChildWnd
			end
		end
	end	
end

function CEquipLinkWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	if ( Child ==self.m_CloseBtn and uMsgID == BUTTON_LCLICK  ) then
		self:ShowWnd(false)
	end	
end

function CNonEquipLinkWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	if ( Child ==self.m_CloseBtn and uMsgID == BUTTON_LCLICK  ) then
		self:ShowWnd(false)
	end
end

function CreateItemLinkWnd(uGlobalID,uBigID,uIndex)
	if g_ItemInfoMgr:IsEquip(uBigID) then
		if g_GameMain.m_LindWnd ~= nil then
			g_GameMain.m_LindWnd:ShowWnd(false)
			g_GameMain.m_LindWnd = nil 
		end
		CreateEquipLinkWnd(uBigID,uIndex,uGlobalID)
	else
		if g_GameMain.m_EquipLinkWnd ~= nil then
			g_GameMain.m_EquipLinkWnd:ShowWnd(false)
			g_GameMain.m_EquipLinkWnd = nil 
		end
		CreateNonEquipLinkWnd(uBigID,uIndex,uGlobalID)
	end
end


function Gas2Gac:RetInfoByID(g_Conn,uGlobalID,uBigID,uIndex)
	CreateItemLinkWnd(uGlobalID,uBigID,uIndex)
end