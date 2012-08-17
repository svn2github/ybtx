local function OneStepQuickLogin()
	etc_gac_require "GacTestConfig"
	if GacTestConfig.Need ~= 2 then
		return
	end
	
	--[[
	if not g_App:DebugConsoleOn() then
		g_Login.m_LoginAccounts:OnCtrlmsg(g_Login.m_LoginAccounts:GetDlgChild( "Btn_Back" ),BUTTON_LCLICK,0,0)
	end
	--]]
	
	g_loginIndex = GetRandomOneStepLoginIndex()
	if g_loginIndex < 1 then
		return
	end
	local login_account = GacTestConfig.OneStepLoginAccount .. g_loginIndex
	local login_name = GacTestConfig.OneStepLoginName .. g_loginIndex
	
	gac_require "test/common/CTstLoginControler"
	controler = CTstLoginControler:new()
	controler:OneStepReLoginEx( login_account, login_name )
end	

function One_Step_Quick_Login()
	local co = coroutine.create(OneStepQuickLogin)
	coroutine.resume(co)
	return co
end

