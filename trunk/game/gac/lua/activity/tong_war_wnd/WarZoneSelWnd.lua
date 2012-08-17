gac_require "activity/tong_war_wnd/WarZoneSelWndInc"
gac_gas_require "scene_mgr/SceneCfg"
lan_load "scene/Lan_Scene_Basic_Common"

local WarSceneNameBasic = "ª®”Ô∆Ω‘≠"

function CreateWarZoneSelWnd(partent)
	local Wnd = CWarZoneSelWnd:new()
	Wnd:CreateFromRes("WarZoneSelWnd",partent)
	Wnd.m_CloseBtn = Wnd:GetDlgChild("CloseBtn")
	Wnd.m_EnterBtn = Wnd:GetDlgChild("EnterBtn")
	Wnd.m_WarZoneList = Wnd:GetDlgChild("WarZoneList")

	g_ExcludeWndMgr:InitExcludeWnd(Wnd, 1)
	Wnd:ShowWnd(false)
	return Wnd
end

function CWarZoneSelWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
	if(uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_CloseBtn) then
			self:ShowWnd(false)
		elseif (Child == self.m_EnterBtn) then
			if self.m_WarZoneList:GetSelectItem(-1) == -1 then
				return
			end
			local warZoneId = self.m_WarZoneList:GetSelectItem(-1) + 1
			local sceneName = WarSceneNameBasic .. warZoneId
			Gac2Gas:EnterWarZone(g_Conn, self.m_CurNpcId, sceneName)
			self:ShowWnd(false)
		end
	elseif(uMsgID == ITEM_LBUTTONCLICK) then
		
	end
end


function CWarZoneSelWnd:InitSelList(NpcID)
	self.m_CurNpcId = NpcID
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

function ShowWarZoneSelWnd(NpcID)
	g_GameMain.m_WarZoneSelWnd:InitSelList(NpcID)
	g_GameMain.m_WarZoneSelWnd:ShowWnd(true)
	local AutoCloseWnd = CAutoCloseWnd:new()
	AutoCloseWnd:AutoCloseWnd(g_GameMain.m_WarZoneSelWnd)
end

function Gas2Gac:RetEnterWarZone(Conn, success, message)
	if not success then
	end
end
