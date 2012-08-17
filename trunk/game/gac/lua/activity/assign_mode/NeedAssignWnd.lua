CNeedAssignWnd = class (SQRDialog)
NeedAssignRect = nil
function CNeedAssignWnd.CreateNeedAssignWnd(Parent, BigId, ItemName, ItemCount, ServerId, IntObjID, Class, Time)
	local Wnd = CNeedAssignWnd:new()
	local SmallIcon = g_ItemInfoMgr:GetItemInfo(BigId,ItemName,"SmallIcon")
	Wnd:Init(Parent)
	Wnd.m_IntObjID = IntObjID
	Wnd.m_ServerId = ServerId
	Wnd.m_PlayerClass = Class
	g_LoadIconFromRes(SmallIcon, Wnd.m_ItemIcon, -1, IP_ENABLE, IP_ENABLE)
	Wnd.m_ItemIcon:SetWndText(ItemCount)
	g_Tooltips:LoadIconFromRes(Wnd.m_ItemIcon,SmallIcon)
	g_SetWndMultiToolTips(Wnd.m_ItemIcon,BigId, ItemName, 0)
	Wnd:ShowCountDownText(Time)
	
	if BigId == 16 or (BigId >=5 and BigId <=9) then
		local res = g_ItemInfoMgr:BeEquipClass(BigId,ItemName,Class)
		Wnd.m_BtnNeed:EnableWnd(res)
	end
	
	if NeedAssignRect then
		NeedAssignRect.top = NeedAssignRect.top + 60
		NeedAssignRect.bottom = NeedAssignRect.bottom + 60
--		NeedAssignRect.left = NeedAssignRect.left + 70
--		NeedAssignRect.right = NeedAssignRect.right + 70
		Wnd:SetLogicRect(NeedAssignRect)
		return Wnd
	end
	NeedAssignRect = CFRect:new()
	Wnd:GetLogicRect(NeedAssignRect)
	
	return Wnd
end

function CNeedAssignWnd:Init(Parent)
	self:CreateFromRes("NeedAssignWnd", Parent)
	self:ShowWnd(true)
end

function CNeedAssignWnd:SetCountText(Num)
	self.m_CountText:SetWndText(GetStaticTextClient(13001, Num))
end

function CNeedAssignWnd:ShowCountDownText(Time)
	
	local function TickFun()
		self.m_CountNum = self.m_CountNum - 1
		self:SetCountText(self.m_CountNum)
		if self.m_CountNum <= 0 then
			self:Destroy()
		end
	end
	self.m_CountNum = Time
	self:SetCountText(Time)
	self.m_CountDownTick = RegisterTick( "NeedAssignCountDownTick", TickFun, 1000)	
end

function CNeedAssignWnd:AddImage(Wnd, StateStr)
	local Flag = IMAGE_PARAM:new()
	Flag.CtrlMask = SM_BS_BK
	Flag.StateMask = IP_ENABLE	
	local DefaultImage = WND_IMAGE_LIST:new()
	DefaultImage:AddImage(Wnd:GetGraphic(), -1,  StateStr, nil, CFPos:new(), "0xffffffff", 15 ) 	
	Wnd:SetWndBkImage( Flag, DefaultImage )
end

function CNeedAssignWnd:OnChildCreated()
	self.m_ItemIcon		= self:GetDlgChild("ItemIcon")
	self.m_BtnNeed		= self:GetDlgChild("BtnNeed")
	self.m_BtnGreed		= self:GetDlgChild("BtnGreed")
	self.m_BtnCancel	= self:GetDlgChild("BtnCancel")
	self.m_CountText	= self:GetDlgChild("Label")
end

function CNeedAssignWnd:Destroy()
	g_GameMain.m_NeedAssignTbls[self.m_IntObjID] = nil
	if self.m_CountDownTick then
		UnRegisterTick(self.m_CountDownTick)
	end
	self:ShowWnd(false)
	self:DestroyWnd()
	NeedAssignRect = nil
end

function CNeedAssignWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if( uMsgID == BUTTON_LCLICK ) then
		if Child == self.m_BtnNeed then
			self:Destroy()
			Gac2Gas:SelectNeedAssign(g_Conn, self.m_ServerId, self.m_PlayerClass, self.m_IntObjID, 1)
		elseif Child == self.m_BtnGreed then
			self:Destroy()
			Gac2Gas:SelectNeedAssign(g_Conn, self.m_ServerId, self.m_PlayerClass, self.m_IntObjID, 2)
		elseif Child == self.m_BtnCancel then
			self:Destroy()
			Gac2Gas:SelectNeedAssign(g_Conn, self.m_ServerId, self.m_PlayerClass, self.m_IntObjID, 3)
		end
	end
end


