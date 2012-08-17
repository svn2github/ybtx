gac_gas_require "skill/SkillCommon"

CRoleLevelUp  = class( SQRDialog )


--创建角色升级提醒面板
function CRoleLevelUp:Ctor(parent, tblInfo)
	self:Init(parent, tblInfo)
end

function CRoleLevelUp:Init(parent, tblInfo)
	self.m_tblSkillUp = {}
	self:CreateFromRes("RoleLevelUpWnd", parent)
	self:ShowWnd( true )
	self:UpdateLevelPanel(tblInfo)
end

function CRoleLevelUp:UpdateLevelPanel(tblInfo)
	self:ShowAttentionMag(tblInfo)
end

function CRoleLevelUp:OnChildCreated()
	self.m_HPvalueText			= self:GetDlgChild("HPvalueText")
	self.m_MPvalueText			= self:GetDlgChild("MPvalueText")
	self.m_HPvalueText:ShowWnd( false )
	self.m_MPvalueText:ShowWnd( false )
end

local function MoveLevelUpFuc()
	if not g_GameMain then
		return
	end
	local wnd = g_GameMain.m_RoleLevelAttentionWnd
	if wnd then
		local Rect = CFRect:new()
		wnd:GetWndRect(Rect)
		Rect.top  	= Rect.top - 0.5
		Rect.bottom	= Rect.bottom - 0.5
		wnd:SetWndRect(Rect)
		if (not wnd.m_HPvalueText:IsShow()) then
			if wnd.m_MoveLevelUpWndTick then
				UnRegisterTick( wnd.m_MoveLevelUpWndTick )
				wnd.m_MoveLevelUpWndTick = nil
			end
		end
	end	
end

--当角色升级后显示提示信息
local AllTime = 3000
function CRoleLevelUp:ShowAttentionMag(tblInfo)
	if self.m_MoveLevelUpWndTick then
		UnRegisterTick( self.m_MoveLevelUpWndTick )
		self.m_MoveLevelUpWndTick = nil
	end
	local Rect = CFRect:new()
	self:GetWndRect(Rect)
	Rect.top  	= 334
	Rect.bottom	= 434
	self:SetWndRect(Rect)
	local level, uDesMaxHP, uCurMaxHP, uDesMaxMP, uCurMaxMP = unpack(tblInfo)
	local hpValue = uDesMaxHP - uCurMaxHP
	local mpValue = uDesMaxMP - uCurMaxMP
	if hpValue > 0 then
		self.m_HPvalueText:ShowWnd(true)
		self.m_HPvalueText:SetWndText(hpValue)
		self.m_HPvalueText:SetTransparent(1)	
		self.m_HPvalueText:SetTransparentObj(AllTime,true)
	end
	if mpValue > 0 then
		self.m_MPvalueText:ShowWnd(true)
		self.m_MPvalueText:SetWndText(mpValue)
		self.m_MPvalueText:SetTransparent(1)	
		self.m_MPvalueText:SetTransparentObj(AllTime,true)
	end
	self.m_MoveLevelUpWndTick = RegisterTick("MoveLevelUp", MoveLevelUpFuc , 10)
end


