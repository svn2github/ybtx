CSelectRoleIAccelerator = class( IAccelerator )

function CSelectRoleIAccelerator:OnAccelerator( Msg, wParam, lParam )
	if(g_SelectChar.m_DisAcc) then return end
	
	if( g_SelectChar.m_SelectCharWnd:IsShow() ) then
		if wParam == VK_RETURN then
			if( g_SelectChar.m_SelectCharWnd.m_EnterGame:IsEnable() ) then --表明有已选中进游戏的角色
				g_SelectChar.m_SelectCharWnd:IsEnterGame()
			end
		elseif wParam == VK_ESCAPE then
			g_SelectChar.m_SelectCharWnd:BackUpAnimation(EGameState.eToLogin)
		end
	end
end
