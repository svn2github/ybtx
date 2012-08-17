engine_require "common/Script/lunit"
lunit.import "all"

gac_require "test/testcase/GacTestLogin"
gac_require "test/testcase/GacTestSelectCharState"
gac_require "test/testcase/GacTestAllLoginState"
gac_require "test/testcase/GacTestCreateRole"
gac_require "framework/main_frame/GacEventTable"
gac_gas_require "event/Event"
gac_require "framework/main_frame/GameState"
gac_require "test/testcase/GacBagSpace"
gac_require "test/testcase/GacTestMessage"
gac_require "test/testcase/GacPlayerBagTest"
gac_require "test/testcase/GacMoneyTest"
gac_require "test/testcase/GacTestSkillPannel"
gac_require "test/testcase/GacTestDynEquip"
gac_require "test/testCase/GacTestEmail"
gac_require "test/testCase/GacTestCDG"
gac_require "test/testcase/GacTestStone"
gac_require "test/testcase/GacTestNPCShop"
gac_require "test/testcase/GacTestEquipToSoul"
gac_require "test/testcase/GacTestSkillTalentPannel"
gac_require "test/testcase/GacTestKeyMap"

AddCheckLeakFilterObj(lunit)

function RunGacTest()
	local co = coroutine.create(GacTestMain)
	coroutine.resume(co)
	return co
end

function GacTestMain()
	etc_gac_require "GacTestConfig"
	--看是否要真正运行测试用例
	if GacTestConfig.Need ~= 1 then
		return
	end
	
	RegisterTestCase()
	lunit.run()
	if not g_App:DebugConsoleOn() then
		g_Login.m_LoginAccounts:OnCtrlmsg(g_Login.m_LoginAccounts:GetDlgChild( "Btn_Back" ),BUTTON_LCLICK,0,0)
	end
	--print("after the GacTestMain end")
end

function RegisterTestCase()
	local TestSuites=GacTestConfig.TestSuites
	for i in ipairs(TestSuites) do
		TestSuites[i](TestCase(tostring(i)))
	end
end


function BlockRunScript( str )
	SetEvent( Event.Test.GMRunScriptDone , false )
	Gac2Gas:GM_RunScript( g_Conn, str )
	local result=WaitEvent(true,nil,Event.Test.GMRunScriptDone)
	return result[2][1]
end

function BlockExecute( str )
	SetEvent( Event.Test.GMRunScriptDone , false )
	Gac2Gas:GM_Execute( g_Conn, str )
	local result=WaitEvent(true,nil,Event.Test.GMRunScriptDone)
	return result[2][1]
end

