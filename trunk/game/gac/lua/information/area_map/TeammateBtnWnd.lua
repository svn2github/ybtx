--gac_require "information/area_map/TeammateBtnWndInc"
CTeammateBtnWnd = class(SQRDialog)
--function CreateTeammateBtnWnd(Parent)
function CTeammateBtnWnd:Ctor(Parent)
	self:CreateFromRes("AreaTeammateBtnWnd", Parent )
	self.m_TeammateBtn = self:GetDlgChild("TeammateBtn")
	self:ShowWnd(true)
end

local function MenuAutoTrackToTeammate(MemberId)
	g_GameMain.m_AreaInfoWnd:AutoTrackToTeammate(MemberId)
end

local function MenuCancel()
end

local function OnClickMenuMsg(func)
	func(g_GameMain.m_AreaInfoWnd.m_Menu.m_uCharID)
	g_GameMain.m_AreaInfoWnd.m_Menu:Destroy()
	g_GameMain.m_AreaInfoWnd.m_Menu = nil
end

local function CreateTeammateRClickMenu(BtnWnd)
	if g_GameMain.m_AreaInfoWnd.m_Menu then
		g_GameMain.m_AreaInfoWnd.m_Menu:Destroy()
		g_GameMain.m_AreaInfoWnd.m_Menu = nil
	end
	local Menu = CMenu:new("TargetMenu",g_GameMain.m_AreaInfoWnd)
	Menu:InsertItem(GacMenuText(1901),OnClickMenuMsg, nil, false, false, nil, MenuAutoTrackToTeammate )
	Menu:InsertItem(GacMenuText(1),OnClickMenuMsg, nil, false, false, nil, MenuCancel )
	
	local Rect = CFRect:new()
	BtnWnd:GetLogicRect(Rect)	
	Menu:SetPos( Rect.left+20 , Rect.top )
	local uCharID = BtnWnd.m_uCharID
	Menu.m_uCharID = uCharID
	g_GameMain.m_AreaInfoWnd.m_Menu = Menu
end

function CTeammateBtnWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if uMsgID == BUTTON_LCLICK  then
		if Child == self.m_TeammateBtn then
			g_GameMain.m_AreaInfoWnd:AutoTrackToTeammate(self.m_TeammateBtn.m_uCharID)
			g_GameMain.m_AreaInfoWnd:SetTrackWndFocus()
		end
	elseif( uMsgID == BUTTON_RCLICK )then
		if Child == self.m_TeammateBtn then
			g_GameMain.m_AreaInfoWnd:SetTrackWndFocus()
			CreateTeammateRClickMenu(self.m_TeammateBtn)
		end
	end
end