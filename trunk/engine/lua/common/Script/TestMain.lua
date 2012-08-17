
__print_on = true		--控制lunit输出信息
engine_require "common/Script/lunit"
engine_require "common/Script/ScriptLib"
engine_require "common/Script/TestType"
engine_require "common/Script/TestClass"
engine_require "common/Script/TestOther"

lunit.import "all"

function TestRun( pFactory, x, y )
	Factory = pFactory
	Wnd = Factory:CreateWnd()
	assert_equal( "ImplementWnd",Wnd:WndName(), "[RunScriptMediator]" )	--throw exception
	return 10254, "Test", 1.025;
end

--main
function TestMain()
	InitTypeTest( TestCase("Type test") )
	InitClassTest( TestCase("Class test") )
	InitTestOther( TestCase("Other test") )
	local r = lunit.run()
	return r
end

