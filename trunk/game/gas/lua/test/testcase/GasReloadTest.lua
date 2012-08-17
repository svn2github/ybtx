gac_gas_require "reload/Update"

function InitGasReloadTest()
	local TestTest=TestCase("test/testcase/GasReloadTest")
	local Reload,Ret,Error
	
	function TestTest:setup()
		Reload = CUpdate:new()
	end
	
	function TestTest:TestReloadNoFile()
		Ret,Error = Reload:Reload("dddd")
		assert_equal( false , Ret,"" )
	end
	
	--[[
	function TestTest:TestReloadErrorFile()
		Ret,Error = Reload:Reload("test/testcase/reload_testfile/error")
		assert_equal( false,Ret,"" )
	end
	
	function TestTest:TestPcallErrorFile()
		Ret,Error = Reload:Reload("test/testcase/reload_testfile/pcallerror")
		assert_equal( false,Ret,"" )
	end
	
	function TestTest:TestReloadSuccess()
		Ret,Error = Reload:Reload("test/testcase/reload_testfile/success")
		assert_equal( true,Ret,"" )
	end
	--]]
	
	function TestTest:TestIncludeInc()
		Ret,Error = Reload:Reload("successInc")
		assert_equal( false,Ret,"" )
		assert_match( "include",Error,"" )
		
		Ret,Error = Reload:Reload("successInc")
		assert_equal( false,Ret,"" )
		assert_match( "include",Error,"" )
		
		Ret,Error = Reload:Reload("successinc")
		assert_equal( false,Ret,"" )
		assert_match( "include",Error,"" )
	end
	
	function TestTest:TestNoIncludeInc()
		Ret,Error = Reload:Reload("successInckk")
		assert_equal( false,Ret,"" )
		assert_not_match( "Reload file include \"inc\". ",Error,"" )
	end
	
	function TestTest:teardown()
	end
	
end
