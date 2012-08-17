
gac_require "toolbar/hide_player/HidePlayerInc"

function CreateHidePlayerWnd(Parent)
	local Wnd = CHidePlayerWnd:new()
	Wnd:CreateFromRes("HideAllPlayerWnd",Parent)
	return Wnd
end 

function CHidePlayerWnd:HideOrShowPlayer()
	if g_GameMain.HidePlayer then
		g_GameMain.m_HidePlayerExceptTeammateWnd:ShowWnd(false)
		self:ShowWnd(true)
	else
		self:ShowWnd(false)
	end
end


function CreateHidePlayerExceptTeammateWnd(Parent)
	local Wnd = CHidePlayerExceptTeammateWnd:new()
	Wnd:CreateFromRes("HidePlayerExceptTeammateWnd",Parent)
	return Wnd
end

function CHidePlayerExceptTeammateWnd:HideOrShowPlayerExceptTeammate()
	if g_GameMain.HideAllPlayerExceptTeammate == true then
		g_GameMain.m_HidePlayerWnd:ShowWnd(false)
		self:ShowWnd(true)
	else
		self:ShowWnd(false)
	end
end
