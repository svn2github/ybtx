GacTestConfig =
{
	OneStepLoginAccount = "onestep",
	OneStepLoginName = "onestep",
	Need = 0 , --[[
			0, 为正常启动
			1, 跑自动测试
			2, 一步登陆
		--]]
	TestSuites =
	{       
		InitGacTestNPCShop,
		InitGacTestEmail,
		InitGacPlayerBag,
		InitGacTestStone,
		InitGacTestLogin,
		InitGacTestCDG,
		InitGacTestSelectCharState,
		InitGacTestAllLoginState,
		InitGacTestCreateRole,
		InitGacBagSpace, 
		InitGacTestMessage,
		InitGacPlayerMoney,
		--[[
		InitGacTestDynEquip,
		InitGacTestSkillTalentPannel,
		InitGacPlayerSkillPannel,
		--]]
	}
}
