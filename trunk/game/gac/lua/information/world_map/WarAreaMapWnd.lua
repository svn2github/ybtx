--大地图窗口
gac_require "information/world_map/WarAreaMapWndInc"
cfg_require "map/WorldMapInfo"

function CreateWarAreaMapBG( Parent )
	local Wnd = CWarAreaMapBG:new()
	Wnd:CreateFromRes( "WarAreaMap", Parent )

	for Index=1, 3 do
		if not Wnd["m_SceneBtn"..Index] then
			Wnd["m_SceneBtn"..Index] = Wnd:GetDlgChild("SceneBtn"..Index)
			Wnd["m_SceneBtn"..Index].m_SceneName = WorldMapInfo["WarArea"][Index]
		end
	end
	
	Parent["m_WarAreaMap"] = Wnd
end

--子窗口消息
function CWarAreaMapBG:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if( uMsgID == BUTTON_LCLICK ) then
		g_GameMain.m_AreaInfoWnd:ClickSceneBtnOpenAreaWnd(Child.m_SceneName)
	elseif uMsgID == BUTTON_RCLICK then	
		g_GameMain.m_AreaInfoWnd:InitWorldMap()
	end
end
