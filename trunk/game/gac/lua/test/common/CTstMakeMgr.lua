gac_require "test/common/CTstMakeMgrInc"

function CTstMakeMgr:OpenFuntion()
	--g_GameMain.m_BlackSmith:OnCtrlmsg( g_GameMain.m_BlackSmith.m_BtnMakeAccouter, BUTTON_LCLICK, 0, 0 )
	--g_GameMain.m_SelClass:OnCtrlmsg( g_GameMain.m_SelClass.m_BtnTianSha, BUTTON_LCLICK, 0, 0 )
end

function CTstMakeMgr:Ctor()
	g_GameMain.m_KeyAccelerator:OnAccelerator( nil,VK_F3,nil)
end