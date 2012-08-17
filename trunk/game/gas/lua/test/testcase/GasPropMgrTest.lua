gac_gas_require "item/item_info_mgr/ItemInfoMgr"

function InitGasPropMgrTest( TestTest )
	local TestTest=TestCase("test/testcase/GasPropMgrTest")	
	function TestTest:setup()
		
	end
	
	function TestTest:TestGetProp()
		local PropMgr = CItemInfoMgr:new()
		local PropTable = PropMgr:GetItemInfo( 1,"宫保鸡丁" )
		assert_table( PropTable, "Get Prop Must Be Table.Please Check It." )
		assert_equal("",PropTable.Description,"" )
		local NilTable = PropMgr:GetItemInfo( 1,"999999" )
		assert_equal(nil,NilTable,"" )
	end
	
	
	function TestTest:TestPropArg()
		local PropMgr = CItemInfoMgr:new()
		function func1()
			PropMgr:GetItemInfo( 1 )
		end
		assert_error( "No Error", func1)
		
		function func2()
			PropMgr:GetItemInfo( nil,1 )
		end
		assert_error( "No Error", func2)
		
		function func3()
			PropMgr:GetItemInfo( "123",1 )
		end
		assert_error( "No Error", func3)
		
		function func4()
			PropMgr:GetItemInfo( 1,{} )
		end
		assert_error( "No Error", func4)
		
		--大类的 id太大了，超过注册的表格数目
		function func5()
			PropMgr:GetItemInfo( 1111,1111 )
		end
		assert_error( "No Error", func5)
		
	end
	
	function TestTest:teardown()
	end
	
end