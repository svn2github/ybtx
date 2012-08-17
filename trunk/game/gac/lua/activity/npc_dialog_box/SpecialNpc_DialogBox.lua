CSpecialNpc_DialogBox = class(SQRDialog)

function CSpecialNpc_DialogBox:Ctor(parent, WndName, FunType,TriggerType,TargetEntityID)
	self:CreateFromRes("NpcDialogBoxListCtrlItemListItem", parent)
	self.m_Text = self:GetDlgChild("Text")
	self.m_Icon = self:GetDlgChild("Icon")
	self.m_Text:SetWndText(WndName)
	self.m_Type = FunType
	self.m_Name = TriggerType
	self.m_EntityID = TargetEntityID
	self:GetWnd()
end

function CSpecialNpc_DialogBox:GetWnd()
	self:ShowWnd(true)
	return self
end

local function Transfer(Name,EntityID)
	--local EntityID = g_MainPlayer.m_LockCenter.m_LockingObj:GetEntityID()
	Gac2Gas:RemitSpecScene(g_Conn,EntityID, Name)
end

local function CreateNpc(Name,EntityID)
	--local EntityID = g_MainPlayer.m_LockCenter.m_LockingObj:GetEntityID()
	Gac2Gas:RenoSpecNpc(g_Conn, EntityID, Name)
end

function CSpecialNpc_DialogBox:RunSpecialFun()
	if self.m_Type == "Transmit" then
		Transfer(self.m_Name,self.m_EntityID)
	elseif self.m_Type == "Renovate" then
		CreateNpc(self.m_Name,self.m_EntityID)
	end
end
