gac_require "framework/main_frame/GameState"
CUserAgreement = class ( CGameState )

--创建用户协议界面
function CUserAgreement:OnInit()
	local param = WndCreateParam:new()
	param.szTitle = ""
	param.uStyle = 0x40000000
	param.width = 1024
	param.height = 768
	param:SetParentWnd(g_App:GetRootWnd())
	param.font_size = 12
	self:Create( param )
	self:SetLockFlag( BitOr( LOCK_LEFT, LOCK_RIGHT, LOCK_TOP, LOCK_BOTTOM ) )
	local Image = WND_IMAGE_LIST:new()
	local Flag = IMAGE_PARAM:new()
	Flag.CtrlMask = SM_BS_BK
	Flag.StateMask = IP_ENABLE
	self:SetWndBkImage( Flag, Image )
	self:ShowWnd(true)
	
	self.m_UserAgreementWnd = CUserAgreementWnd:new(self)
	self.m_UserAgreementWnd:Animation()
end

function CUserAgreement:OnMainSceneCreated(CoreScene)
	g_CoreScene = CoreScene
end

function CUserAgreement:OnDestroyMainScene()
end

function CUserAgreement:OnMainSceneDestroyed()
	g_CoreScene = nil
end

--退出用户协议界面
function CUserAgreement:OnExit()
	self:DestroyWnd()
end

--用户协议主函数
function CUserAgreement:main()
	self:OnInit()
	local re = self:WaitMessage()
	self:OnExit()
	return re
end