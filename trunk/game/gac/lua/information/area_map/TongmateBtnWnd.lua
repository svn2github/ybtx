--gac_require "information/area_map/TongmateBtnWndInc"
CTongmateBtnWnd = class(SQRDialog)

--function CreateTongmateBtnWnd(Parent)
function CTongmateBtnWnd:Ctor()
	local Parent = g_GameMain.m_AreaInfoWnd.m_SceneMapWnd["m_ScenePictureWnd"]
	if not Parent then
		return
	end
	self:CreateFromRes("AreaTongmateBtnWnd", Parent )
	self.m_TongmateBtn = self:GetDlgChild("TongmateBtn")
	self:ShowWnd(true)
end

local function MenuAutoTrackToTongmate(MemberId)
	g_GameMain.m_AreaInfoWnd:AutoTrackToTongmate(MemberId)
end

local function MenuCancel()
end

local function OnClickMenuMsg(func)
	func(g_GameMain.m_AreaInfoWnd.m_Menu.m_uCharID)
	g_GameMain.m_AreaInfoWnd.m_Menu:Destroy()
	g_GameMain.m_AreaInfoWnd.m_Menu = nil
end

local function CreateTongmateRClickMenu(BtnWnd)
	if g_GameMain.m_AreaInfoWnd.m_Menu then
		g_GameMain.m_AreaInfoWnd.m_Menu:Destroy()
		g_GameMain.m_AreaInfoWnd.m_Menu = nil
	end
	local Menu = CMenu:new("TargetMenu",g_GameMain.m_AreaInfoWnd)
	Menu:InsertItem(GacMenuText(1901),OnClickMenuMsg, nil, false, false, nil, MenuAutoTrackToTongmate )
	Menu:InsertItem(GacMenuText(1),OnClickMenuMsg, nil, false, false, nil, MenuCancel )
	
	local Rect = CFRect:new()
	BtnWnd:GetLogicRect(Rect)	
	Menu:SetPos( Rect.left+20 , Rect.top )
	local uCharID = BtnWnd.m_uCharID
	Menu.m_uCharID = uCharID
	g_GameMain.m_AreaInfoWnd.m_Menu = Menu
end

function CTongmateBtnWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if uMsgID == BUTTON_LCLICK  then
		if Child == self.m_TongmateBtn then
			g_GameMain.m_AreaInfoWnd:AutoTrackToTongmate(self.m_TongmateBtn.m_uCharID)
			g_GameMain.m_AreaInfoWnd:SetTrackWndFocus()
		end
	elseif( uMsgID == BUTTON_RCLICK )then
		if Child == self.m_TongmateBtn then
			g_GameMain.m_AreaInfoWnd:SetTrackWndFocus()
			CreateTongmateRClickMenu(self.m_TongmateBtn)
		end
	end
end