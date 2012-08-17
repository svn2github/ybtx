--大地图窗口
gac_require( "information/world_map/Camp3MapWndInc" )
cfg_require "map/WorldMapInfo"

function CreateCamp3MapBG( Parent )
	local Wnd = CCamp3MapBG:new()
	Wnd:CreateFromRes( "Camp3Map", Parent )

	for Index=1, 4 do
		if not Wnd["m_SceneBtn"..Index] then
			Wnd["m_SceneBtn"..Index] = Wnd:GetDlgChild("SceneBtn"..Index)
			Wnd["m_SceneBtn"..Index].m_SceneName = WorldMapInfo["Camp3"][Index]
		end
	end
	
	Parent["m_Camp3Map"] = Wnd
end

--子窗口消息
function CCamp3MapBG:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if( uMsgID == BUTTON_LCLICK ) then
		g_GameMain.m_AreaInfoWnd:ClickSceneBtnOpenAreaWnd(Child.m_SceneName)
	elseif uMsgID == BUTTON_RCLICK then
		g_GameMain.m_AreaInfoWnd:InitWorldMap()
	end
end
