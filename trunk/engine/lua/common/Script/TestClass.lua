engine_require "common/Script/lunit"
lunit.import "all" 

function InitClassTest( ClassTest )
	local ErrPrefix 	= "Error in "
	
	
	function ClassTest:test_VariantFun()
		local cObj = class(TestVarObject)
			local obj = cObj:new()
			
		function TestVarObject:DoVarCallBack(object, ...)
			local arg = {...}
			print("Call Back")
			print(object)
			
			for i, v in ipairs (arg) do
				print(v)
			end
		end
	

		
		local argList, argList2 = obj:TestRetArgList()
		
		obj:TestVariantCallBack()
		
		obj:DoVarObject(10)
		obj:DoVarObject("asdf")
		obj:DoVarObject(false)
		
		local num = obj:GetVarObjectNum()
		print(num)
		local str = obj:GetVarObjectStr()
		print(str)
		obj:DoVarListObject(100, 10, "adsfdsa", 1, true)
		obj:DoVarListObject("Hello World", "Hi Hi")
	end


	function ClassTest:test_NormalFun()
		local obj = CTestNormalFunClass:new()
		local value = 54123
		obj:SetInt( value )
		assert_equal( value, obj:GetInt(), ErrPrefix .. "class function normal function test" )
	end

	function ClassTest:test_VirtualFun()
		local objBase = CTestClassBase:new()
		assert_equal( 0, objBase:GetIndex(), ErrPrefix .. "base virtual function" )
		assert_equal( 100, objBase:Get100(), ErrPrefix .. "base normal function Get100" )
		assert_equal( 1000, objBase:Get1000(), ErrPrefix .. "base normal function Get1000" )
		
		local objChild = CTestClassChild:new()
		assert_equal( 1, objChild:GetIndex(), ErrPrefix .. "child virtual function" )
		assert_equal( 100, objChild:Get100(), ErrPrefix .. "child normal function Get100" )
		assert_equal( 1000, objChild:Get1000(), ErrPrefix .. "child normal function Get1000" )		
	end

	function ClassTest:test_StaticFun()
		assert_equal( "CStaticFunClass", CStaticFunClass_GetName(), ErrPrefix .. "class static function" )
		assert_equal( 35, CStaticFunClass_GetNum(), ErrPrefix .. "class static function" ) 
	end
	
	--function ClassTest:test_AttachClassRelease()
	--	--print( IWnd_GetNum() )
	--	--print( IMessage_GetNum() )
	--end

	function ClassTest:test_CallBack()
			
		local cBase = class(CCallBackBase)
		local Base = cBase:new()
		--local Base = CCallBackBase:new()
		
		function Base:Environment()
			return "Lua";
		end
		
		assert_equal( "Lua", Base:GetEnvironment(), ErrPrefix .. "call back, base" )
		
		local cChildNotOver = class(CCallBackChildNotOver)
		local ChildNotOver = cChildNotOver:new()
		
		function ChildNotOver:Environment()
			return "Lua";
		end
		
		assert_equal( "Lua", ChildNotOver:GetEnvironment(), ErrPrefix .. "call back, child not over load." )
		
		local cChileOver = class(CCallBackChildOver)
		local ChileOver = cChileOver:new()
		
		assert_equal( "C++, child", ChileOver:GetEnvironment(), ErrPrefix .. "call back, base" )
		
		
	end
	
	function ClassTest:test_AbstractClass()
			
		local Factory = GetFactory()

		local Wnd = Factory:CreateWnd()

		for i=1,10 do
			local tempWnd = Wnd:Clone()
		end		
		

		
		local Message = Factory:CreateMessage()

		assert_equal( "ImplementWnd", Wnd:WndName(), ErrPrefix .. "abstract class , Wnd, WndName" )
		assert_equal( "ImplementMessage", Message:MessageName(), ErrPrefix .. "pure virtual class , Message, MessageName" ) 
		assert_equal( 0, Wnd:WndProc( Message ), ErrPrefix .. "abstract class , Wnd, WndProc" )
		
	end
	

	function ClassTest:test_AbstactClassCallBack()	
		
		local Factory = GetFactory()

		local Base = Factory:CreateMessage()
		
		function Base:Environment()
			return "Lua";
		end
		
		--由C++创建并传递给lua的对象不允许修改虚表
		assert_equal( "C++", Base:GetEnvironment(), ErrPrefix .. "abstract call back, base" )
		
		local ChildNotOver = Factory:CreateMessageNotOver()
		
		assert_equal( "C++", ChildNotOver:GetEnvironment(), ErrPrefix .. "abstract call back, base" )
		
		function ChildNotOver:Environment()
			return "Lua";
		end
		
		--由C++创建并传递给lua的对象不允许修改虚表
		assert_equal( "C++", ChildNotOver:GetEnvironment(), ErrPrefix .. "abstract call back, child not over load." )
		
		local ChileOver =  Factory:CreateMessageOver()
		assert_equal( "C++, child", ChileOver:GetEnvironment(), ErrPrefix .. "abstract call back, base" )
	end


	function ClassTest:test_MultiClass()	
		local child = CMultiChild:new()
		assert_equal( "who", child:who(), ErrPrefix .. "multi class , who" )
		assert_equal( "how", child:how(), ErrPrefix .. "multi class , how" )
		assert_equal( "what", child:what(), ErrPrefix .. "multi class , what" )
		assert_equal( "where", child:where(), ErrPrefix .. "multi class , where" )
		assert_equal( "when", child:when(), ErrPrefix .. "multi class , when" )
	end
	
	
	function ClassTest:test_PureClass()
		
		
		local Derived_IPureClass = class(IPureClass)
	
		local PureClass = Derived_IPureClass:new()
		
		assert_equal( nil, IPureClass_GetName( PureClass ), ErrPrefix .. "PureClass , GetName" )
		
		function PureClass:OnGetName( a, b, c )
			assert_equal( 3, a, ErrPrefix .. "Test , ongoto" )
			assert_equal( 4, b, ErrPrefix .. "Test , ongoto" )
			assert_equal( "ABCDEFG", c, ErrPrefix .. "Test , ongoto" )
			return "IPureClass"
		end
			
		assert_equal( "IPureClass", IPureClass_GetName( PureClass ), ErrPrefix .. "PureClass , GetName" )
	end
	

	function ClassTest:test_pack()
	
		local child = CMultiChild:new()
		function Test_Pack:ongoto( c, x, y, s )
			assert_equal( child, c, ErrPrefix .. "Test , ongoto" )
			assert_equal( 4, x, ErrPrefix .. "Test , ongoto" )
			assert_equal( 5, y, ErrPrefix .. "Test , ongoto" )
			assert_equal( "abcdef", s, ErrPrefix .. "Test , ongoto" )
			return 1;			
		end
		
		Test_Pack:goto( child, 4, 5, "abcdef" )
		Test_Pack.ongoto = nil
	end

	
	function ClassTest:test_Ref_obj()
		collectgarbage( "collect" )
		
		local BindObj1 = CTestRefObj:new()
		local BindObj2 = CTestRefObj:new()
		local BindObj3 = CTestRefObj:new()
		local Obj = CTestNormalFunClass:new()
		
		BindObj1:bindObj( Obj )
		BindObj2:bindObj( Obj )
		BindObj3:bindObj( Obj )
		
		Obj:SetInt( 465213 )
		Obj =nil
		collectgarbage( "collect" )
		
		assert_equal( 1, getNumOfGlobalReferenceTable(),	"[Obj][advance reference][bind]" )
		
		BindObj1:unbindObj()
		assert_equal( 1, getNumOfGlobalReferenceTable(),	"[Obj][advance reference][unbind][1]" )
		
		collectgarbage( "collect" )
		local Obj2 = BindObj2:getObj()
		assert_equal( 465213, Obj2:GetInt(),				"[Obj][advance reference][bind obj][obj value][2]" )
		
		Obj2 =nil
		local Obj3 = BindObj3:getObj()
		assert_equal( 465213, Obj3:GetInt(),				"[Obj][advance reference][bind obj][obj value][3]" )
		Obj3 = nil
		
		BindObj2:unbindObj()
		BindObj3:unbindObj()
		
		nilObj = BindObj2:getObj()
		assert_equal( nil, nilObj,							"[Obj][return][nil][in function test_Ref_obj]")
		
		assert_equal( 0, getNumOfGlobalReferenceTable(),	"[Obj][advance reference][unbind][3]" )
		
		local Obj4 = CTestNormalFunClass:new()
		local BindObj4 = CTestRefObj:new()
		BindObj4:bindObj( Obj4 )
		assert_equal( 1, getNumOfGlobalReferenceTable(),	"[Obj][advance reference][bind][4]" )
		Obj4 = nil
		BindObj4 = nil
		collectgarbage( "collect" )
		assert_equal( 0, getNumOfGlobalReferenceTable(),	"[Obj][advance reference][obj release][4]" )
	end

	function ClassTest:test_class_over_load_function_simple()
		local obj = CTestClassOverLoadSimple:new()
		assert_equal( 12354, obj:get1(12354) ,					"[class][over load][function]"   )
		assert_equal( 12354+1235421, obj:get2(12354,1235421),	"[class][over load][function]"   )
	end
	
	function ClassTest:test_class_function_over_load_advance()
		local obj = CTestClassOverLoadChild:new()
		obj:set_int( 1234895 )
		obj:set_bool( false )
		obj:set_float( 2.015 )
		obj:set_double( 3.012456 )
		obj:set_str( "class function over load" )
		obj:set_int8( 124 )
		
		local diff = math.abs( obj:get_float() - 2.015 )
		
		assert_equal( 1234895, obj:get_int(),						"[class][over load][function][advance][int]"   )
		assert_false( obj:get_bool(),								"[class][over load][function][advance][bool]"   )
		assert_equal( 3.012456, obj:get_double(),					"[class][over load][function][advance][double]"   )
		assert_equal( "class function over load", obj:get_str(),	"[class][over load][function][advance][str]"   )
		assert_equal( 124, obj:get_int8(),							"[class][over load][function][advance][in8]"   )
		if( diff > 0.0000002 ) then
			assert_fail(											"[class][over load][function][advance][float]"   )
		end
	end
	
	
	function ClassTest:test_AbstracClass()	
	
	
	local IAbsClass3 = class(IAbstracClass3)
	local obj1 = IAbsClass3:new()
	
	local obj2 = CPrintAbstra:new()

		obj1.OnPrint1 = function( self, s )
			assert_equal( "IAbstracClass1", s )
		end
		obj1.OnPrint2 = function( self, s )
			assert_equal( "IAbstracClass2", s )
		end
		obj1.OnPrint3 = function( self, s )
			assert_equal( "IAbstracClass3", s )
		end
		
		obj2:PrintAbstract( obj1 )
	end
	
	
	function ClassTest:test_class_release_function()
		local release_obj_factory = CTestReleaseClassFactory:new()
		local release_obj = release_obj_factory:createCTestReleaseClass()
		
		assert_false( release_obj_factory:hasCreateFromTheSameAddr(), "[class][release]" )
		
		assert_false( release_obj:IsAfterInit(), "[class][release][init value error]" )
		release_obj:setAfterInitState()
		assert_true( release_obj:IsAfterInit(), "[class][release][init value error]" )
		db()
		release_obj:Release()
		assert_equal( nil, release_obj.IsAfterInit,		"[class][release]" )
		
		local function Test()
			for i=1,10000 do
				release_obj = release_obj_factory:createCTestReleaseClass()
				assert_false( release_obj:IsAfterInit(), "[class][release][init value error]" .. i )
				release_obj:setAfterInitState()
				release_obj:Release()
				assert_equal( nil, release_obj.IsAfterInit,		"[class][release]" .. i )
			end
		end
		
		Test()
		
		assert_true( release_obj_factory:hasCreateFromTheSameAddr(), "[class][release]" )
	end
	
	function ClassTest:test_callback_param_mix()
		local cObj = class(CTestCallBackParamMix)
		local obj = cObj:new()
		local errstr = "[class][callback][param mix]"
		function obj:CallBack( i, f, b, d, s )
			local diff = math.abs( f - 1.25451 )
			assert_equal( i, 13434, errstr )
			assert_false( b,		errstr )
			assert_equal( d, 4278124286, errstr )
			assert_equal( s, "Hello", errstr )
			
			if( diff > 0.0000004 ) then 
				assert_fail( errstr )
			end
			
			return 903748914
		end
		
		local r = obj:Call()
		assert_true( r, errstr )
	end
	

	function ClassTest:test_class_member()
		local pos = CPos:new()
		assert_equal( -23, pos.x )
		assert_equal( 12, pos.y )
		pos.x = 54
		pos.y = 63
		assert_true( pos:Check() )
	end
	
	------------------------	not work	------------------------	
	function ClassTest:class_passbyvalue()
		local obj = CTestClassPassByValue:new()
		function obj:OnCheck( v, o )
			assert_equal( 3, v )
			assert_equal( 6, o.n )
		end
		obj:Check( obj )
	end
	
	
	function ClassTest:test_callback_overload()

		local cObj = class(CTestCallBackOverLoad)
		local obj = cObj:new()

		
		function obj:add_i( a, b )
			return a + b + 1
		end
		
		function obj:add_d( a, b )
			return a + b + 1.11
		end
		
		assert_true( obj:callback(), "[class][call back][overload]" )
		
	end
	
	
	function ClassTest:test_static_fun_overload()
		assert_equal( 8, CTestStaticOverLoad_add_i( 3, 5 ),				"[class][static][overload]" )
		local diff = math.abs( 8.88 - CTestStaticOverLoad_add_d( 3.33, 5.55 ) )
		if diff >0.000001 then
			assert_fail( "[class][static function][overload]" )
		end
	end
	
	function ClassTest:test_callback_return()
	
			--cObj = CTestCallBackReturn:new()
	
		function CTestCallBackReturn:Onri()
			return 45612313
		end
		
		function CTestCallBackReturn:Onrb()
			return false
		end

		function CTestCallBackReturn:Onrf()
			return 1.02487415
		end

		function CTestCallBackReturn:Onrd()
			return 3.1542454451
		end

		function CTestCallBackReturn:Onrstr()
			return "rlua"
		end

		local cObj = class(CTestCallBackReturn)

		local obj = cObj:new()

		assert_true( obj:callback(), "[class][callback][return]" )
		
	end

	-----------------------------------------------------------------------	
	---------	下面的函数测试内存泄露，必须放在最后			-----------
	-----------------------------------------------------------------------

	
	function ClassTest:test_memory_leap()
		collectgarbage( "collect" )
	end

end
