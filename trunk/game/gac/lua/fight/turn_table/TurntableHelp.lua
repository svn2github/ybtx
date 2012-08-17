
gac_require "fight/turn_table/TurntableHelpInc"
function createturntableHelpWnd( Parent )
	local Wnd = turntablehelpWnd:new()
	Wnd:CreateFromRes( "WndInfo", Parent )
	Wnd:ShowWnd( true )
	Wnd.list = Wnd:GetDlgChild( "list" )		
	return Wnd
end

function turntablehelpWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	
end

