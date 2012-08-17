gac_require "information/head_info/DirectionInfoInc"
gac_require "framework/texture_mgr/DynImageMgr"

function CreateDirectionInfo()
	local wnd = CDirectionInfo:new()
	wnd:CreateFromRes("CharDirection", g_GameMain)
	return wnd
end

function CDirectionInfo:OnChildCreated()
	self.m_Static = self:GetDlgChild("Static")
end

function CDirectionInfo:UpdateDirectionInfoWnd(Character, type)
	Character:GetRenderObject():AddChild(self, eLinkType.LT_CENTER, "")
	self:ShowWnd(true)
	local sImagePath = ""
	if( type == ECharacterType.Player) then
		sImagePath = g_ImageMgr:GetImagePath(1359)
	elseif( type == ECharacterType.Npc) then
		sImagePath = g_ImageMgr:GetImagePath(1360)
	end
	g_DynImageMgr:AddImageByIP(self.m_Static, sImagePath, IP_ENABLE)
	
	local nEntityID = Character:GetEntityID()
	self:SetCharacterEntityID( nEntityID )
end
