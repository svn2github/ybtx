gac_require "information/area_map/CreateNpcPointRClickMenu"

CNpcPointBtnWnd = class(SQRDialog)

--function CreateNpcPointBtn(Parent, wndres, childwndres, strTexture)
function CNpcPointBtnWnd:Ctor(Parent, wndres, childwndres, strTexture)
	if wndres then
		self:CreateFromRes(wndres, Parent)
	end
	if childwndres then
		self.m_NpcSignBtn = self:GetDlgChild(childwndres)
	end
	
	if strTexture then
		local Flag = IMAGE_PARAM:new()
		Flag.CtrlMask = SM_BS_BK
		Flag.StateMask = IP_ENABLE	
		local DefaultImage = WND_IMAGE_LIST:new()
		--strTexture = "guitex/tex/ybtx/gui/2ndEdition/Icon/NpcIcon/npcicon_paimaihang.tex"
		DefaultImage:AddImage(self:GetGraphic(), -1,  strTexture, nil, CFPos:new(0,0), "0xffffffff", 0 ) 	
		self:SetWndBkImage( Flag, DefaultImage )
	end
	self:ShowWnd(true)
end

function CNpcPointBtnWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if uMsgID == BUTTON_LCLICK or uMsgID == BUTTON_CLICKUP then
		if Child == self.m_NpcSignBtn then
			local PointName = self.m_NpcSignBtn.m_PointName
			local SceneName = self.m_NpcSignBtn.m_SceneName
			local PosTbl = self.m_NpcSignBtn.m_PosTbl
			if PointName == nil then
				return
			end
			-- 如果点的是Trap的图标，就直接寻路到Trap传送的位置
			if self.m_IsTrap then				
				PlayerAutoTrack(PointName,self.m_EndSceneName, self.m_EndScenePosX, self.m_EndScenePosY)
			else
				PlayerAutoTrack(PointName,SceneName,PosTbl[1],PosTbl[2])
			end
			g_GameMain.m_AreaInfoWnd:SetTrackWndFocus()
			--g_GameMain.m_AreaInfoWnd:AutoTrackToPoint(PointName,SceneName,PosTbl[1],PosTbl[2])
		end
	elseif( uMsgID == BUTTON_RCLICK )then
		if Child == self.m_NpcSignBtn then
			local PointName = self.m_NpcSignBtn.m_PointName
			if PointName == nil then
				return
			end
			g_GameMain.m_AreaInfoWnd:SetTrackWndFocus()
			CreateNpcPointRClickMenu(self.m_NpcSignBtn)
		end
	end
end
