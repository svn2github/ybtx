#include "stdafx.h"
#include "ScriptRegComm.h"
#include "ScriptCommon.h"
#include "ScriptDebugHelper.h"

namespace sqr
{
	void RegDefClass(lua_State* pState)
	{
		const TCHAR* szDefClass=
		"local function GetMember( Class, Name )\n"
		"	local value = rawget(Class, Name)\n"
		"	if value then\n"
		"		return value\n"
		"	else\n"
		"		local base_list = Class.__base_list\n"
		"		local n = #base_list\n"
		"		for i = 1, n do\n"
		"			value = rawget(base_list[i], Name)\n"
		"			if value then\n"
		"				return value"
		"			end\n"
		"		end\n"
		"	end\n"
		"	return nil\n"
		"end\n"

		//获得拥有Name字段的那个table
		"local function GetClass(Class, Name)\n"
		"	if rawget(Class, Name) then\n"
		"		return Class\n"
		"	else\n"
		"		local base_list = Class.__base_list\n"
		"		local n = #base_list\n"
		"		for i = 1, n do\n"
		"			if rawget(base_list[i], Name) then\n"
		"				return base_list[i]\n"
		"			end\n"
		"		end\n"
		"	end\n"
		"	return nil\n"
		"end\n"

		"__mt_class = {}\n"
		"__mt_class.__index = function(Class, Name)\n"
		"	return GetMember(Class, Name)\n"
		"end\n"

		"local eSCT_Isolated = EScriptClassType.eSCT_Isolated \n"
		"local eSCT_Share = EScriptClassType.eSCT_Share \n"
		"local eSCT_Static = EScriptClassType.eSCT_Static \n"
		"local eSCT_Dynamic = EScriptClassType.eSCT_Dynamic \n"

		"local function SetClassMetaTable(NewClass)\n"
		"	local ClassMt = {}\n"
		"	ClassMt.__index = function(Class, Name)\n"
		"		return GetMember(Class, Name)\n"
		"	end\n"
		"	setmetatable(NewClass, ClassMt)\n"
		"end\n"

		//isolated class 的meta method
		"local function SetIsolatedClassMeta(NewClass) \n"
		"	NewClass.__index = function( Object, Name )\n"
		"		local f = rawget( NewClass, Name )\n"
		"		if( is_cpp_member_var( f ) )then\n"
		"			return f( Object, nil )\n"
		"		end\n"
		"		return f\n"
		"	end\n"

		"	NewClass.__newindex = function( Object, Name, Value )\n"
		"		local f = rawget( NewClass, Name )\n"
		"		if( is_cpp_member_var( f ) )then\n"
		"			f( Object, Value )\n"
		"			return\n" 
		"		end\n"
		"		error(\"unsafe类对象只允许给注册的class member进行赋值\")"
		"		rawset( Object, Name, Value )\n"
		"	end\n"
		"end\n"

		////对于isolated_class是不允许继承的
		"function isolated_class()\n"
		"	local NewClass = {}\n"
		"	setmetatable(NewClass, {})\n"

		"	function NewClass:new(...) \n"
		"		local NewInstance = self.ctor( self )\n"
		"		if self.Ctor then\n"
		"			self.Ctor( NewInstance, ... )\n"
		"		end\n"
		"		return NewInstance\n"
		"	end\n"
		"	SetIsolatedClassMeta(NewClass) \n"

		"	return NewClass\n"
		"end\n"

		"local function CheckDiamondAndSetBaseList(...)\n"
		"	local arg = {...}\n"
		"	local n = select(\"#\", ...)\n"
		"	local base_tbl = {} \n"
		"	local __base_list = {}\n"
		"	for i = 1, n do\n"
		"		if arg[i] then\n"
		"			table.insert(__base_list, arg[i])\n"
		"			local base = arg[i].__base_list\n"
		"			assert(base)\n"
		"			for j = 1, #base do\n"
		"				local class_obj = base[j]\n"
		"				if base_tbl[class_obj] then\n"
		"					error(\"注册类的基类有菱形继承关系\")\n"
		"				else\n"
		"					base_tbl[class_obj] = true\n"
		"					table.insert(__base_list, class_obj)\n"
		"				end\n"
		"			end\n"
		"		end\n"
		"	end\n"
		"	return __base_list\n"
		"end\n"

		"local function SetClassMetaMethod(NewClass, eClassType)\n"
		"	NewClass.__index = function( Object, Name )\n"
		"		local f = GetMember( NewClass, Name )\n"
		"		return f\n"
		"	end\n"

		"	if eClassType == eSCT_Dynamic then\n"
		"		NewClass.__newindex = function( Object, Name, Value )\n"
		"			rawset( Object, Name, Value )\n"
		"		end\n"
		"	else\n"
		"		NewClass.__newindex = function( Object, Name, Value )\n"
		"			error(\"attempt to index a read only userdata\")\n"
		"		end\n"
		"	end\n"
		"end\n"

		"function cpp_class( eClassType, ... )\n"
		"	local arg = {...}\n"
		"	local NewClass = {}\n"
		"	SetClassMetaTable(NewClass)\n"
		"	NewClass.__base_list = CheckDiamondAndSetBaseList(...)\n"

		"	SetClassMetaMethod(NewClass, eClassType) \n"

		"	if eClassType == eSCT_Share then\n"
		"		return NewClass\n"
		"	end\n"

		"	function NewClass:ctor( )\n"
		"		local inst = {}\n"
		"		setmetatable(inst, NewClass)\n"
		"		return inst\n"
		"	end\n"

		"	function NewClass:new( ... )\n"
		"		local NewInstance = self.ctor( self )\n"
		"		if self.Ctor then\n"
		"			self.Ctor( NewInstance, ... )\n"
		"		end\n"
		"		return NewInstance\n"
		"	end\n"
		
		"	return NewClass\n"
		"end\n"

		"function class(...) \n"
		"	local arg = {...}\n"
		"	local n = select(\"#\", ...)\n"
		"	local CppClass = {} \n"
		"	local Num = 0 \n"
		"	for i = 1, n do \n"
		"		if arg[i] then\n"
		"			local Info = GetClass(arg[i], \"__info\") \n"
		"			if Info then \n"
		"				CppClass = Info\n"
		"				if CppClass[\"__ClassType\"] == eSCT_Isolated then\n"
		"					local err = \"父类\" .. CppClass[\"__ClassName\"] .. \" 是isolated类，不允许继承\" \n"
		"					error(err) \n"
		"				end\n"
		"				if CppClass[\"__ClassType\"] == eSCT_Share then\n"
		"					local err = \"父类\" .. CppClass[\"__ClassName\"] .. \" 是Share类，不允许继承\" \n"
		"					error(err) \n"
		"				end\n"
		"				Num = Num + 1 \n"
		"			end \n"
		"		end\n"
		"	end \n"

		"	if Num > 1 then \n"
		"		local msg = \"在Lua中声明的类不允许继承多个不同的c++类\" \n"
		"		error(msg) \n"
		"	end \n"

		"	local NewClass = cpp_class(eSCT_Dynamic, ...) \n"

		//如果有一个base cpp class，那么我们使用这个class的ctor和gc
		"	if Num == 1 then\n"
		"		NewClass.ctor = rawget(CppClass, \"ctor\") \n"
		"		NewClass.__gc = rawget(CppClass, \"__gc\") \n"
		"		NewClass.init_obj = rawget(CppClass, \"init_obj\") \n"
		"	end\n"

		"	assert(NewClass)\n"
		"	return NewClass \n"
		"end \n"


		"__mt_readonly = {}\n"
		"__mt_readonly.__index = __mt_readonly\n"
		"function __mt_readonly.__newindex( t, k, v )\n"
		"	error( \"attempt to index a read only table \" )\n"
		"end\n"

		//在继承树上检查check_node是否cur_node的基类，不考虑lua和c++的类的差异问题
		"function IsBaseClass( CurClass, BaseClass )\n"
		"	if CurClass == BaseClass then\n"
		"		return true\n"
		"	end\n"
		"	local base_list = CurClass.__base_list\n"
		"	for i = 1, #base_list do\n"
		"		if base_list[i] == BaseClass then\n"
		"			return true\n"
		"		end\n"
		"	end\n"
		"	return false\n"
		"end\n"


		"function ClassCast( Class, obj )\n"
		"	local obj_class = getmetatable( obj )\n"
		//如果obj是子类对象就直接返回
		"	if( IsBaseClass( obj_class, Class ) ) then\n"
		"		return obj\n"
		"	end\n"
		"	local bCppObjBase = rawget(obj_class, \"__info\")\n"
		"	local bCppObjDev = rawget(Class, \"__info\")\n"
		//只允许obj为c++对象，class为lua类
		
		"	if(bCppObjBase and (not bCppObjDev)) then\n"
			//Class和obj_class如果没有继承关系也是不允许转换的
		"		if IsBaseClass(Class, obj_class) == false then\n"
		"			error(\"两个对象之间没有继承关系不能转换！\")\n"
		"		else\n"
		"			ChangeMetaTable( obj, Class )\n"
		"			return obj\n"
		"		end\n"
		"	else\n"
		"		error(\"Class不是lua类，而obj不是c++创建对象，不允许进行classcast\")\n"
		"	end\n"
		"end\n"

		
		"";

		luaL_dostring( pState, szDefClass );
	}

	void RegDebugPrint(lua_State* pState)
	{
		const TCHAR* szDebugPrint = 
		"function DebugPrint( n, ... )\n"
		"	local arg = {...}\n"
		"	for k, v in ipairs( arg ) do\n"
		"		if type(n) ~= \"table\" then\n"
		"			print(\"The Last Type is not a table, can not index \")\n"
		"			return \n"
		"		end\n"
		"		n = n[v]\n"
		"	end\n"
		"	if( type( n ) == \"table\" ) then\n"
		"		print( n, \"\\n{\" )\n"
		"		for k, v in pairs( n ) do\n"
		"			print( \"\", k, \"=\", v )\n"
		"		end\n"
		"		print( \"}\" )\n"
		"	else\n"
		"		print( n )\n"
		"	end\n"
		"end\n";

		luaL_dostring( pState, szDebugPrint );
	}

	void RegExtend(lua_State* pState)
	{
		const TCHAR* szExtend =
		"function apcall( func, ... )\n"
		"	local arg = {...}\n"
		"	local argc = select(\"#\", ...)\n"
		"	return xpcall(	function() return func( unpack(arg, 1, argc) ) end, ExpLogHandler )\n"
		"end\n"
		"\n"
		"\n"
		
		"local CoExpTbl = {}\n"
		"setmetatable(CoExpTbl, {__mode = \"k\"})\n"

		"local function HandleCoFunc(ret, ...) \n"
		"	if ret == true then \n"
		"		return ...\n"
		"	else\n"
		"		CoExpTbl[coroutine.running()] = true"
		"		local arg = {...}\n"
		"		error(arg[1])\n"
		"	end\n"
		"end\n"
		
		"local oldcreateco = coroutine.create\n"
		"coroutine.create=\n"
		"function ( Func )\n"
		"	local function RoutineFunc( ... )\n"
#ifdef _WIN32
		"	SetCoroutineESH()\n"
#endif
		"	return HandleCoFunc(apcall(Func, ...))\n"
		"	end\n"
		"\n"
		"	return oldcreateco( RoutineFunc, 61440 )\n"
		"end\n"
		"\n"
	
		"local oldresume = coroutine.resume\n"
		"local function HandleResume( pSaveStack, co, ret, ...)\n"
		"	if ret == false then\n"
		"		local arg = {...}\n"
		/*
			如果是coroutine调用的函数出现错误，我们在函数出错的地方已经记录了堆栈
			不需要在这里记录了
		*/
		"		if not CoExpTbl[co] then\n"
		"			LogErr(arg[1])\n"
		"		else\n"
		"			CoExpTbl[co] = nil\n"
		"		end\n"
		"	end\n"
		"	AfterResume(co, pSaveStack)\n"
		"	return ret, ...\n"
		"end\n"

		"coroutine.resume = \n"
		"function( co, ... )\n"
		"	assert(co)\n"
		"	local pSaveStack = BeforeResume(co)\n"
		"	return HandleResume( pSaveStack, co, oldresume(co, ... ) )"
		"end"

		"\n"
		
		"function pack( target, ... )\n"
		"	local arg = {...}\n"
		"	for i,v in ipairs(arg) do\n"
		"		 table.insert( target , v )\n"
		"	end\n"
		"end\n"
		
		"LoadPathTbl = {}"

		"function RequireFile( alias_path, file_name )\n"
		"	if type(package.preload[file_name]) == \"nil\"  then\n"
		"		local function PreLoadFunc()\n"
		"			local chunk, msg =  AliasLoadFile(alias_path, file_name) \n"
		"			if not chunk then\n"
		"				error(msg) \n"	
		"			end\n"
		"			return chunk(file_name) \n"
		"		end\n"
		"		package.preload[file_name] = PreLoadFunc\n"
		"	end\n"
		"	return require(file_name)\n"
		"end\n"

		"function Require(file_name)\n"
		"	return RequireFile(\"\", file_name) \n"
		"end\n"

		"function GetLoadPath( alias_path )\n"
		"	return LoadPathTbl[alias_path]\n"
		"end\n"

		//我们在lua里面只允许调用count和collect，其他gc的设置都不允许调用，只能由c++调用	

		"local _collectgarbage = collectgarbage\n"
		"function collectgarbage(opt, arg)\n"
		"	if opt == \"count\" or opt == \"collect\" then\n"
		"		return _collectgarbage(opt, arg)\n"
		"	end\n"
		"	error(\"collectgarbage 在lua里面只允许传入count或collect\")\n"
		"end\n"

		"";

		luaL_dostring( pState, szExtend );
	}

}
