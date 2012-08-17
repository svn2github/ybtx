
CBaseServantBuffWnd		 	= class ( CBuffWnd )
local OneLineBuffNum = 8
---------------- Create Servant buff ------------------

function CBaseServantBuffWnd:Ctor( Parent )
	self:Init( Parent, Parent )
	self.m_Parent = Parent
end

function CBaseServantBuffWnd:InitData()
	self.m_RowNum			= 2
	self.m_ColNum			= 8
	self.m_IconSize			= 22
	self.m_OneTypeLineNum	= 1
	self.m_tblBuffs = {{}, {}}
	self.m_tblBuffs[1].n = 0
	self.m_tblBuffs[2].n = 0
end

function CBaseServantBuffWnd:InitPos(ParentRectWnd)
	local Rect	= CFRect:new()
	ParentRectWnd:GetLogicRect(Rect)
	self:SetPos(self, Rect.right, Rect.top)
end

function CBaseServantBuffWnd:GetRemainTime(ID, buff_name)
	local Servant = g_MainPlayer:GetServant(self.m_Parent.eNpcType)
	if(Servant) then
		local RemainTime = Servant:GetRemainTime(buff_name, ID)
		return RemainTime or 0
	end
	return 0
end

