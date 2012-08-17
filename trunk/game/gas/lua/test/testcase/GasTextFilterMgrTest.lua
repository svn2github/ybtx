gac_gas_require "framework/text_filter_mgr/TextFilterMgr"

function InitGasTextFilterMgrTest()
	local TestTest=TestCase("test/testcase/GasTextFilterMgrTest")
	function TestTest:setup()
	end
	
	function TestTest:TestFilterName()
		local textFltMgr = CTextFilterMgr:new()
		assert_equal(false,textFltMgr:IsValidName("毛泽东"),"")
		assert_equal(false,textFltMgr:IsValidName("邓小平"),"")		
		assert_equal(false,textFltMgr:IsValidName("江泽民"),"")
		assert_equal(false,textFltMgr:IsValidName("胡锦涛"),"")
		assert_equal(false,textFltMgr:IsValidName("GM001"),"")
		assert_equal(false,textFltMgr:IsValidName("GM0012"),"")
		assert_equal(false,textFltMgr:IsValidName("李洪志"),"")
		assert_equal(false,textFltMgr:IsValidName("陈水扁"),"")
		
		assert_equal(true,textFltMgr:IsValidName("吴俊"),"")
	end
	
	function TestTest:TestFilterMsg()
		local textFltMgr = CTextFilterMgr:new()
		assert_equal(false,textFltMgr:IsValidMsg("日"),"")
		assert_equal(false,textFltMgr:IsValidMsg("我日"),"")		
		assert_equal(false,textFltMgr:IsValidMsg("我操你妈"),"")
		assert_equal(false,textFltMgr:IsValidMsg("你奶奶的"),"")
		assert_equal(false,textFltMgr:IsValidMsg("我爱江泽民"),"")
		assert_equal(false,textFltMgr:IsValidMsg("我爱胡锦涛"),"")
		assert_equal(false,textFltMgr:IsValidMsg("我爱温家宝"),"")
		assert_equal(false,textFltMgr:IsValidMsg("取缔法轮功"),"")
		assert_equal(false,textFltMgr:IsValidMsg("共产党万岁！"),"")
		assert_equal(false,textFltMgr:IsValidMsg("反对台独"),"")
		
		assert_equal(true,textFltMgr:IsValidMsg("方舟工作室"),"")
	end
	
	function TestTest:teardown()
	end
	
end