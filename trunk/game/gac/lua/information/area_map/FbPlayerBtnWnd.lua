CFbPlayerBtnWnd = class(SQRDialog)

function CFbPlayerBtnWnd:Ctor(Parent, TeamID, MemberId, isLive)
	--print("   "..TeamID)
	if TeamID == 1 then
		--print("ºì")
		if isLive then
			self:CreateFromRes("AreafbplayerBtnRedWnd", Parent )
		else
			self:CreateFromRes("AreafbplayerBtnDeadRedWnd", Parent )
		end
	elseif TeamID == 2 then
		--print("À¶")
		if isLive then
			self:CreateFromRes("AreafbplayerBtnBlueWnd", Parent )
		else
			self:CreateFromRes("AreafbplayerBtnDeadBlueWnd", Parent )
		end
	elseif TeamID == 3 then
		--print("»Æ")
		if isLive then
			self:CreateFromRes("AreaFbPlayerBtnyellowWnd", Parent )
		else
			self:CreateFromRes("AreaFbPlayerBtnDeadyellowWnd", Parent )
		end
	elseif TeamID == 4 then
		--print("ÂÌ")
		if isLive then
			self:CreateFromRes("AreaFbPlayerBtngreenWnd", Parent )
		else
			self:CreateFromRes("AreaFbPlayerBtnDeadgreenWnd", Parent )
		end
	end
	--self.m_FbPlayerBtn = self:GetDlgChild("FbPlayerBtn")
	self.m_FbPlayerBtn = self:GetDlgChild("FbPlayerBtn")
	self:ShowWnd(true)
	if self and self.m_FbPlayerBtn then
		self.m_FbPlayerBtn.m_uCharID = MemberId
	end
end

local function MenuAutoTrackToFbPlayer(MemberId)
	g_GameMain.m_AreaInfoWnd:AutoTrackToFbPlayer(MemberId)
	
end

local function MenuCancel()

end

local function OnClickMenuMsg(func)
	func(g_GameMain.m_AreaInfoWnd.m_Menu.m_uCharID)
	g_GameMain.m_AreaInfoWnd.m_Menu:Destroy()
	g_GameMain.m_AreaInfoWnd.m_Menu = nil
end

local function CreateFbPlayerRClickMenu(BtnWnd)
	if g_GameMain.m_AreaInfoWnd.m_Menu then
		g_GameMain.m_AreaInfoWnd.m_Menu:Destroy()
		g_GameMain.m_AreaInfoWnd.m_Menu = nil
	end
	local Menu = CMenu:new("TargetMenu",g_GameMain.m_AreaInfoWnd)
	Menu:InsertItem(GacMenuText(1901),OnClickMenuMsg, nil, false, false, nil, MenuAutoTrackToFbPlayer )
	Menu:InsertItem(GacMenuText(1),OnClickMenuMsg, nil, false, false, nil, MenuCancel )
	local Rect = CFRect:new()
	BtnWnd:GetLogicRect(Rect)	
	Menu:SetPos( Rect.left+20 , Rect.top )
	local uCharID = BtnWnd.m_uCharID
	Menu.m_uCharID = uCharID
	g_GameMain.m_AreaInfoWnd.m_Menu = Menu
end

function CFbPlayerBtnWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if uMsgID == BUTTON_LCLICK  then
		if Child == self.m_FbPlayerBtn then
			g_GameMain.m_AreaInfoWnd:AutoTrackToFbPlayer(self.m_FbPlayerBtn.m_uCharID)
			g_GameMain.m_AreaInfoWnd:SetTrackWndFocus()
		end
	elseif( uMsgID == BUTTON_RCLICK )then
		if Child == self.m_FbPlayerBtn then
			g_GameMain.m_AreaInfoWnd:SetTrackWndFocus()
			CreateFbPlayerRClickMenu(self.m_FbPlayerBtn)
		end
	end
end
