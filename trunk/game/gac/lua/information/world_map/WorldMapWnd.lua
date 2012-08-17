--大地图窗口
gac_require( "information/world_map/WorldMapWndInc" )

function CreateWorldMapBG(Parent)
	local Wnd = CWorldMapBG:new()
	Wnd:CreateFromRes( "WorldMapPicture", Parent )	
	for Index, SceneName in pairs(WorldMapInfo["World"]) do
		if not Wnd["m_SceneBtn"..Index] then
			Wnd["m_SceneBtn"..Index] = Wnd:GetDlgChild("SceneBtn"..Index)
			Wnd["m_SceneName"..Index] = SceneName
			--Wnd["m_SceneBtn"..Index]:SetWndText(SceneName)	
		end
	end

	Parent["m_WorldMap"] = Wnd
end

function OpenWorldMapWnd()
	if not g_GameMain.m_AreaInfoWnd:IsShow() then		
		g_GameMain.m_AreaInfoWnd.UseNkey = true
		g_GameMain.m_AreaInfoWnd:InitWorldMap()
		g_GameMain.m_AreaInfoWnd:ShowWnd(true)
	else
		-- 如果已经用M打开了区域面板
		if g_GameMain.m_AreaInfoWnd.UseMkey then
			-- 如果用M打开区域面板，并返回到世界地图，此时应该关掉区域面板
			if g_GameMain.m_AreaInfoWnd.m_MapState == "World" then
				g_GameMain.m_AreaInfoWnd.UseMkey = false
				g_GameMain.m_AreaInfoWnd.UseNkey = false
				g_GameMain.m_AreaInfoWnd:CloseAreaInfoWnd()	
			else
				g_GameMain.m_AreaInfoWnd:InitWorldMap()
				g_GameMain.m_AreaInfoWnd.UseMkey = false
				g_GameMain.m_AreaInfoWnd.UseNkey = true	
			end
		else 
			g_GameMain.m_AreaInfoWnd.UseNkey = false
			g_GameMain.m_AreaInfoWnd:CloseAreaInfoWnd()
		end
	end
end

--子窗口消息
function CWorldMapBG:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if uMsgID == BUTTON_LCLICK then
		
		local MiddleMap
		if  Child == self.m_SceneBtn1 then
			MiddleMap = "Camp1"
		elseif Child == self.m_SceneBtn2 then
			MiddleMap = "Camp2"
		elseif Child == self.m_SceneBtn3 then
			MiddleMap = "Camp3"	
		elseif Child == self.m_SceneBtn4 then
			MiddleMap = "WarArea"		
		end

		g_GameMain.m_AreaInfoWnd:InitMiddleMap(MiddleMap)
	end
end
