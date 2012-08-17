gac_gas_require "scene_mgr/SceneCfg"
lan_load "scene/Lan_Scene_Basic_Common"

local WarSceneNameBasic = "ª®”Ô∆Ω‘≠"

CWarTransportWnd = class(SQRDialog)

function CWarTransportWnd:Ctor(partent)
	self:CreateFromRes("WarTransport",partent)
	self.m_CloseBtn = self:GetDlgChild("CloseBtn")
	self.m_ConfirmBtn = self:GetDlgChild("ConfirmBtn")
	self.m_CancelBtn = self:GetDlgChild("CancelBtn")
	self.m_Introduce = self:GetDlgChild("Introduce")
	self.m_WarZoneList = self:GetDlgChild("WarZoneList")
	self.m_Introduce:SetWndText(GetStaticTextClient(320000))
	g_ExcludeWndMgr:InitExcludeWnd(self, 1)
	self:InitSelList()
	self:GetWnd()
end

function CWarTransportWnd:GetWnd()
	self:ShowWnd(true)
	g_GameMain.m_WarTransportWnd = self
	return self 
end

function CWarTransportWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if(uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_CloseBtn) then
			self:ShowWnd(false)
		elseif(Child == self.m_CancelBtn) then
			self:ShowWnd(false)
		elseif (Child == self.m_ConfirmBtn) then
			if self.m_WarZoneList:GetSelectItem(-1) == -1 then
				return
			end
			local warZoneId = self.m_WarZoneList:GetSelectItem(-1) + 1
			local sceneName = WarSceneNameBasic .. warZoneId
			Gac2Gas:JudgeWarTransport(g_Conn, sceneName)
			self:ShowWnd(false)
		end
	elseif(uMsgID == ITEM_LBUTTONCLICK) then
		
	end
end


function CWarTransportWnd:InitSelList()
	self.m_WarZoneList:DeleteAllItem()
	self.m_WarZoneList:InsertColumn(0, self.m_WarZoneList:GetWndWidth())
	local warZoneId = 1
	while true do
		local sceneName = WarSceneNameBasic .. warZoneId
		if not Scene_Common[sceneName] then
			break
		end
		self.m_WarZoneList:InsertItem(warZoneId -1, 30)
		local displaySceneName = Lan_Scene_Basic_Common(MemH64(sceneName),"DisplayName")
		self.m_WarZoneList:GetSubItem(warZoneId -1, 0):SetWndText(displaySceneName)
		self.m_WarZoneList:GetSubItem(warZoneId -1, 0):SetMouseWheel(true)
		warZoneId = warZoneId + 1
	end
end

function CWarTransportWnd:Open()
	self:ShowWnd(true)
end


