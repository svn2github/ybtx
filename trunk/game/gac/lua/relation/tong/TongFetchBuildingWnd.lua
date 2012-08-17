gac_require "relation/tong/TongFetchBuildingWndInc"
cfg_load "item/TongChallengeItem_Common"
lan_load "item/Lan_TongChallengeItem_Common"

local BuildingNameTbl = {}
BuildingNameTbl[1] = "艾米挑战副本防御设施"
BuildingNameTbl[2] = "神圣挑战副本防御设施"
BuildingNameTbl[3] = "修斯挑战副本防御设施"

--帮会挑战领取建筑界面
function CreateTongFetchBuildingWnd(Parent)
	local Wnd = CTongFetchBuildingWnd:new()
	Wnd:Init(Parent)
	return Wnd
end

function CTongFetchBuildingWnd:Init(Parent)
	self:CreateFromRes("TongFetchBuilding", Parent)
	g_ExcludeWndMgr:InitExcludeWnd(self, 2, false, nil, 1)
end

function CTongFetchBuildingWnd:OnChildCreated()
	self.m_OkBtn = self:GetDlgChild("OkBtn")
	self.m_CancelBtn = self:GetDlgChild("CancelBtn")
	self.m_XBtn = self:GetDlgChild("XBtn")
	self.m_Introduction = self:GetDlgChild("Introduction")
	self.m_BuildingName = self:GetDlgChild("BuildingName")
	self.m_BuildingIcon = self:GetDlgChild("BuildingIcon")
end

function CTongFetchBuildingWnd:OpenPanel()
	local PlayerCamp = g_MainPlayer:CppGetCamp()
	local BuildingName = BuildingNameTbl[PlayerCamp]
	local BuildingDisplayName = Lan_TongChallengeItem_Common(MemH64(BuildingName), "DisplayName")
	self.m_BuildingName:SetWndText(BuildingDisplayName)
	self.m_Introduction:SetWndText(GetStaticTextClient(9113))
	local SmallIcon = TongChallengeItem_Common(BuildingName,"SmallIcon")
	g_LoadIconFromRes(SmallIcon, self.m_BuildingIcon, -1, IP_ENABLE, IP_ENABLE)
	self:ShowWnd(true)
end

function CTongFetchBuildingWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	if uMsgID == BUTTON_LCLICK then
		if Child == self.m_OkBtn then
			Gac2Gas:TongFetchBuilding(g_Conn)
			self:ShowWnd(false)
		elseif Child == self.m_CancelBtn or Child == self.m_XBtn  then
			self:ShowWnd(false)
		end
	end
end