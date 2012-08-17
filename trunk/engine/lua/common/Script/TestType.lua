engine_require "common/Script/lunit"
lunit.import "all"

function InitTypeTest( TypeTest )
	local ErrPrefix 	= "Error in "
	local MaxInt 		= 100
	local MaxUint 		= 1000
	local StepFloat 	= 0.1
	local StepDouble 	= 0.01

	function TypeTest:test_int64()
		local n = tTestInt64()
		assert_equal( 99999999999999, n,						"[double][return]".. n )
		local p = tGetint64(-99999999999999)
		assert_equal( -99999999999999, p,						"[double][return]".. p )
		--print(n .. "   " .. p)
	end

		
	function TypeTest:test_cbuf()
		local buf = CreateCBuf( 100 )
		local buf_acr = CreateCBufAcr( buf )
		
		buf_acr:Write("s", "cbuf test" )
		buf_acr:Seek( "begin" )
		assert_equal( "cbuf test", buf_acr:ToString(), "[cbuf acr][tostring]" )
		
		buf_acr:Seek( "begin" )
		local i, b1, I, b2, str, d, b3, f = -34341, false, 341343, false,"cbuf", 1.2154113 , false, 1.245121
		buf_acr:Write( "ibIbsdbf", i,b1,I,b2,str,d,b3,f )
		
		buf_acr:Seek( "begin" )
		local ri, rb1, rI, rb2, rstr, rd, rb3, rf  = buf_acr:Read( "ibIbsdbf" )
		local diff = math.abs( rf - f )
		assert_equal( i, ri, "[cbuf acr][write read]" )
		assert_equal( I, rI, "[cbuf acr][write read]" )
		assert_equal( str, rstr, "[cbuf acr][write read]" )
		assert_equal( d, rd, "[cbuf acr][write read]" )
		assert_equal( b1, rb1, "[cbuf acr][write read]" )
		assert_equal( b2, rb2, "[cbuf acr][write read]" )
		assert_equal( b3, rb3, "[cbuf acr][write read]" )
		if( diff >0.0000002 )then
			assert_fail( "[cbuf acr][write read]" )
		end
	end
	
	function TypeTest:test_cbuf_seek()
		local buf_acr = CreateCBufAcr( CreateCBuf(10) )
		buf_acr:Write( "cccccccccc", 1,2,3,4,5,6,7,8,9,10 )
		
		buf_acr:Seek( "begin" )
		assert_equal( 1, buf_acr:Read("c"), "[cbuf acr][seek]" )
		assert_equal( 2, buf_acr:Read("c"), "[cbuf acr][seek]" )
		assert_equal( 3, buf_acr:Read("c"), "[cbuf acr][seek]" )
		assert_equal( 4, buf_acr:Read("c"), "[cbuf acr][seek]" )
		assert_equal( 5, buf_acr:Read("c"), "[cbuf acr][seek]" )
		assert_equal( 6, buf_acr:Read("c"), "[cbuf acr][seek]" )
		assert_equal( 7, buf_acr:Read("c"), "[cbuf acr][seek]" )
		assert_equal( 8, buf_acr:Read("c"), "[cbuf acr][seek]" )
		assert_equal( 9, buf_acr:Read("c"), "[cbuf acr][seek]" )
		assert_equal( 10, buf_acr:Read("c"), "[cbuf acr][seek]" )
		
		buf_acr:Seek( -1 )
		assert_equal( 10, buf_acr:Read("c"), "[cbuf acr][seek]" )
		buf_acr:Seek( -2 )
		assert_equal( 9, buf_acr:Read("c"), "[cbuf acr][seek]" )
		buf_acr:Seek( -2 )
		assert_equal( 8, buf_acr:Read("c"), "[cbuf acr][seek]" )
		buf_acr:Seek( -2 )
		assert_equal( 7, buf_acr:Read("c"), "[cbuf acr][seek]" )
		buf_acr:Seek( -2 )
		assert_equal( 6, buf_acr:Read("c"), "[cbuf acr][seek]" )
		buf_acr:Seek( -2 )
		assert_equal( 5, buf_acr:Read("c"), "[cbuf acr][seek]" )
		buf_acr:Seek( -2 )
		assert_equal( 4, buf_acr:Read("c"), "[cbuf acr][seek]" )
		buf_acr:Seek( -2 )
		assert_equal( 3, buf_acr:Read("c"), "[cbuf acr][seek]" )
		buf_acr:Seek( -2 )
		assert_equal( 2, buf_acr:Read("c"), "[cbuf acr][seek]" )
		buf_acr:Seek( -2 )
		assert_equal( 1, buf_acr:Read("c"), "[cbuf acr][seek]" )
		
		buf_acr:Seek( -1 )
		buf_acr:Seek( 4 )
		assert_equal( 5, buf_acr:Read("c"), "[cbuf acr][seek]" )
		buf_acr:Seek( 4 )
		assert_equal( 10, buf_acr:Read("c"), "[cbuf acr][seek]" )
		
		buf_acr:Seek( -6 )
		assert_equal( 5, buf_acr:Read("c"), "[cbuf acr][seek]" )
		buf_acr:Seek( -5 )
		assert_equal( 1, buf_acr:Read("c"), "[cbuf acr][seek]" )
	end
	
	function TypeTest:test_cbuf_copy_set()
		local buf_acr = CreateCBufAcr( CreateCBuf(10) )
		buf_acr:Set( 0, 10 )
		buf_acr:Seek( "begin" )
		buf_acr:Write( "cccccc", 0,1,2,3,4,5 )
		
		buf_acr:Seek( "begin" )
		local buf_acr_copy = CreateCBufAcr( CreateCBuf(6) )
		buf_acr_copy:Set( 0, 6 )
		buf_acr_copy:Seek( "begin" )
		buf_acr_copy:Copy( buf_acr, 4 )
		
		buf_acr_copy:Seek( "begin" )
		assert_equal( 0, buf_acr_copy:Read("c"), "[cbuf acr][copy set]" )
		assert_equal( 1, buf_acr_copy:Read("c"), "[cbuf acr][copy set]" )
		assert_equal( 2, buf_acr_copy:Read("c"), "[cbuf acr][copy set]" )
		assert_equal( 3, buf_acr_copy:Read("c"), "[cbuf acr][copy set]" )
		assert_equal( 0, buf_acr_copy:Read("c"), "[cbuf acr][copy set]" )
		assert_equal( 0, buf_acr_copy:Read("c"), "[cbuf acr][copy set]" )
		
		assert_equal( 4, buf_acr:Read("c"), "[cbuf acr][copy set][src]" )
		assert_equal( 5, buf_acr:Read("c"), "[cbuf acr][copy set][src]" )
	end

	function TypeTest:test_mix()
		local r = tParamMix(true,1234,1.009)
		assert_true( r, "[type mix]" )
	end
	
	function TypeTest:test_int()
		local r
		for i = -MaxInt,MaxInt do
			r = tInt(i)
			assert_equal( r, i,						"[int][return]" .. i )
		end
	end
		

	function TypeTest:test_uint()
		local r
		for i = 0,MaxUint do
			r = tUint( i )
			assert_equal( r, i,						"[uint][return]" .. i )
		end
	end	


	function TypeTest:test_bool()
		local r
		r = tBool( true )
		assert_true( r,								"[bool][return]" .. "true" )
		
		r = tBool( false )
		assert_false( r,							"[bool][return]" .. "false" )
	end


	function TypeTest:test_float()
		local i = 0
		local r
		while( i < 1 ) do
			r = tFloat( i )
			local diff
			
			diff =  math.abs( r - i )
			if( diff >0.0000002 )then
				assert_fail( "[float][return]" .. i)
			end
			
			i = i + StepFloat
		end
	end



	function TypeTest:test_double()
		local i = 0
		local r
		while( i < 1 ) do
			r = tDouble( i )
			assert_equal( r, i,						"[double][return]".. i )
			i = i + StepDouble
		end
	end
	

	function TypeTest:test_string()
		local strTest = "Test string."
		local r
		r = tString( strTest  )
		assert_equal( r, strTest,								"[str][return]" .. strTest )
	end	

	function TypeTest:test_no_param()
		assert_equal( 123456, tGetNum(), "[no param][return]" )
	end
	
	function TypeTest:test_no_return()
		tNoReturn(1)
	end
	
	function TypeTest:test_aysn_type_set_empty_tbl()
		local aysn = CTestAsynType:new()
		local bufacr = CreateCBufAcr( CreateCBuf(5) )
		
		aysn:bindStr( bufacr, 5-1 )
		aysn:update()
		
		assert_equal( "Hell", bufacr:ToString(),	"[aysn type][empty table][set in c++][string]")
	end
	
	function TypeTest:test_aysn_type_valued_tbl()
		local aysn = CTestAsynType:new()
		local bufacr = CreateCBufAcr( CreateCBuf(11) )
		
		aysn:bindStr( bufacr, 11-1 )
		aysn:update()
		
		assert_equal( "Hello", bufacr:ToString(),	"[aysn type][valued table][set in c++][string]")
	end
	
	
	function TypeTest:test_aysn_type_gc()
		local aysn = CTestAsynType:new()
		local bufacr = CreateCBufAcr( CreateCBuf(100) )
		
		aysn:bindStr( bufacr, 100-1 )
		aysn:update()

		bufacr = nil
		collectgarbage( "collect" )
		aysn:update()
		
		assert_equal( 1, getNumOfGlobalReferenceTable(), "[aysn type][gc][reference]" )
		
		aysn:unbindStr()
		
		collectgarbage( "collect" )
		assert_equal( 0, getNumOfGlobalReferenceTable(), "[aysn type][gc][no reference]" )
		
	end
	
		
	function TypeTest:test_global_overload_function()
		assert_equal( 28, add2( 13,15 ),			"[global over load function][2]" )
		assert_equal( 55, add3( 13,15,27 ),			"[global over load function][3]" )
	end
	
		
	function TypeTest:test_max_int()
		local r = tAllIntMax( -128, -32768, -2147483648, 255, 65535, 4294967295 )
		assert_true( r, "[all int][max]" )
	end
	
	function TypeTest:test_uint32_max_return()
		local r = tUint32Mix()
		assert_equal( 4294967295, r, "[uint32][max][return]")
	end

	function TypeTest:test_export_constant()
		assert_equal( 100, TEST_CONSTANT )
		assert_equal( 0, TestEnum.eTE_Test1 )
		assert_equal( 1, TestEnum.eTE_Test2 )
	end
	
	function TypeTest:test_cbuf_refernce_cpp()
		local obj = CTestCBuf:new()
		local acr = obj:GetBuf()
		
		local i, b1, d, s, b2= acr:Read( "ibdsb" )
		assert_equal( 1345121, i, 		"[cbuf][reference cpp][read]" )
		assert_equal( 1.2154567, d, 	"[cbuf][reference cpp][read]" )
		assert_equal( "CTestCBuf", s, 	"[cbuf][reference cpp][read]" )
		
		local wi, wb1, wd, ws, wb2 = 543121, true, 1.2154567, "has change", false
		
		acr:Seek( "begin" )
		acr:Write( "ibdsb", wi, wb1, wd, ws, wb2 )
		
		assert_true( obj:Check(), 		"[cbuf][reference cpp][write]" )
		
	end
	
	-----------------------------------------------------------------------	
	---------	下面的函数测试内存泄露，必须放在最后			-----------
	-----------------------------------------------------------------------
	function TypeTest:test_memory_leap()
		collectgarbage( "collect" )
		collectgarbage( "collect" )
		assert_equal( 0 , getNumOfObjTable(), "[class][memory leap]" )
	end

end

