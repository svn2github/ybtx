#pragma once
//-------------------------------------------------------------------------------------------------------------------------
// 2006.09.30
//
// 脚本和c++的接口的辅助头文件
//
// MAKE_FUNCTION( _class, _function, szArg )：
//		辅助用于生成被脚本回调的函数，举例bool CWnd::IsInWndArea( int x, int y )函数需要被脚本调用，则只需要在cpp定义
//		MAKE_FUNCTION( CWnd, IsInWndArea, "ii:b" )，并且将MAKE_SFUN生成的SFUN结构通过sqr::CScript::RegFunction
//		函数注册即可，可以减少手工重复的代码，其中"ii"字符串为被调函数参数类型
//
//---------------------------------------------------------------------------------------------------------------------------
#include "CScript.h"
//这里为啥使用了inl的东西，因为现在注册function的函数是用模板的，参数是需要注册函数的函数指针
//因为函数类型太多，不能再cpp文件里面显示的实例化，所以只能对外暴露了
#include "CScript.inl"
#include "CBaseObject.h"
#include "CompileTimeHelper.h"

#define CHECK_BASE_OBJECT_CLASS(_class) COMPILE_TIME_CHECK((ClassConversion<_class*, CBaseObject*>::exists == 1), IsolatedClass_NotAllowed_To_RegistedBaseObject)
#define CHECK_ISOLATED_CLASS(_class) COMPILE_TIME_CHECK((ClassConversion<_class*, CBaseObject*>::exists != 1), BaseObjectClass_NotAllowed_To_RegistedAsIsolated)
#define CHECK_CLASS_INHERIT_TYPE(_SubClass, _BaseClass) COMPILE_TIME_CHECK(_SubClass::eBaseObjectType == _BaseClass::eBaseObjectType, SubAndBaseClassTypeIsNotSame)

#define REGIST_ISOLATED_CLASS( scriptClass, _class )	\
	CHECK_ISOLATED_CLASS(_class); \
	(scriptClass).RegIsolatedClass( RegCreateIsolated<_class>, RegDeleteIsolated<_class>, sizeof(_class), typeid(_class).name(), #_class );

#define REGIST_B_CLASS( scriptClass, _class )	\
	CHECK_BASE_OBJECT_CLASS(_class); \
	(scriptClass).RegClass( _class::eBaseObjectType, RegCreate<_class>, RegDelete<_class>, sizeof(_class), typeid(_class).name(), #_class, \
	NULL );

#define REGIST_D_CLASS( scriptClass, _class, _base_class )	\
	CHECK_BASE_OBJECT_CLASS(_class); \
	CHECK_BASE_OBJECT_CLASS(_base_class); \
	CHECK_CLASS_INHERIT_TYPE(_class, _base_class); \
	(scriptClass).RegClass( _class::eBaseObjectType, RegCreate<_class>, RegDelete<_class>, sizeof(_class), typeid(_class).name(), #_class, \
	#_base_class, NULL );

#define REGIST_D_CLASS_2( scriptClass, _class, _base_class1, _base_class2 )	\
	CHECK_BASE_OBJECT_CLASS(_class); \
	CHECK_BASE_OBJECT_CLASS(_base_class1); \
	CHECK_BASE_OBJECT_CLASS(_base_class2); \
	CHECK_CLASS_INHERIT_TYPE(_class, _base_class1); \
	CHECK_CLASS_INHERIT_TYPE(_class, _base_class2); \
	(scriptClass).RegClass( _class::eBaseObjectType, RegCreate<_class>, RegDelete<_class>, sizeof(_class), typeid(_class).name(), #_class, \
	#_base_class1, \
	#_base_class2, NULL );

#define REGIST_D_CLASS_3( scriptClass, _class, _base_class1, _base_class2, _base_class3 ) \
	CHECK_BASE_OBJECT_CLASS(_class); \
	CHECK_BASE_OBJECT_CLASS(_base_class1); \
	CHECK_BASE_OBJECT_CLASS(_base_class2); \
	CHECK_BASE_OBJECT_CLASS(_base_class3); \
	CHECK_CLASS_INHERIT_TYPE(_class, _base_class1); \
	CHECK_CLASS_INHERIT_TYPE(_class, _base_class2); \
	CHECK_CLASS_INHERIT_TYPE(_class, _base_class3); \
	(scriptClass).RegClass( _class::eBaseObjectType, RegCreate<_class>, RegDelete<_class>, sizeof(_class), typeid(_class).name(), #_class, \
	#_base_class1, \
	#_base_class2, \
	#_base_class3, NULL );

#define REGIST_D_CLASS_4( scriptClass, _class, _base_class1, _base_class2, _base_class3, _base_class4 )	\
	CHECK_BASE_OBJECT_CLASS(_class); \
	CHECK_BASE_OBJECT_CLASS(_base_class1); \
	CHECK_BASE_OBJECT_CLASS(_base_class2); \
	CHECK_BASE_OBJECT_CLASS(_base_class3); \
	CHECK_BASE_OBJECT_CLASS(_base_class4); \
	CHECK_CLASS_INHERIT_TYPE(_class, _base_class1); \
	CHECK_CLASS_INHERIT_TYPE(_class, _base_class2); \
	CHECK_CLASS_INHERIT_TYPE(_class, _base_class3); \
	CHECK_CLASS_INHERIT_TYPE(_class, _base_class4); \
	(scriptClass).RegClass( _class::eBaseObjectType, RegCreate<_class>, RegDelete<_class>, sizeof(_class), typeid(_class).name(), #_class, \
	#_base_class1, \
	#_base_class2, \
	#_base_class3, \
	#_base_class4, NULL );

#define REGIST_D_CLASS_5( scriptClass, _class, _base_class1, _base_class2, _base_class3, _base_class4, _base_class5 )	\
	CHECK_BASE_OBJECT_CLASS(_class); \
	CHECK_BASE_OBJECT_CLASS(_base_class1); \
	CHECK_BASE_OBJECT_CLASS(_base_class2); \
	CHECK_BASE_OBJECT_CLASS(_base_class3); \
	CHECK_BASE_OBJECT_CLASS(_base_class4); \
	CHECK_BASE_OBJECT_CLASS(_base_class5); \
	CHECK_CLASS_INHERIT_TYPE(_class, _base_class1); \
	CHECK_CLASS_INHERIT_TYPE(_class, _base_class2); \
	CHECK_CLASS_INHERIT_TYPE(_class, _base_class3); \
	CHECK_CLASS_INHERIT_TYPE(_class, _base_class4); \
	CHECK_CLASS_INHERIT_TYPE(_class, _base_class5); \
	(scriptClass).RegClass( _class::eBaseObjectType, RegCreate<_class>, RegDelete<_class>, sizeof(_class), typeid(_class).name(), #_class, \
	#_base_class1, \
	#_base_class2, \
	#_base_class3, \
	#_base_class4, \
	#_base_class5, NULL );


#define REGIST_ABSTRACT_B_CLASS( scriptClass, _class )	\
	CHECK_BASE_OBJECT_CLASS(_class); \
	(scriptClass).RegClass( _class::eBaseObjectType, NULL, NULL, sizeof(_class), typeid(_class).name(), #_class, \
	NULL );

#define REGIST_ABSTRACT_D_CLASS( scriptClass, _class, _base_class )	\
	CHECK_BASE_OBJECT_CLASS(_class); \
	CHECK_BASE_OBJECT_CLASS(_base_class); \
	CHECK_CLASS_INHERIT_TYPE(_class, _base_class); \
	(scriptClass).RegClass( _class::eBaseObjectType, NULL, NULL, sizeof(_class), typeid(_class).name(), #_class, \
	#_base_class, NULL );

#define REGIST_ABSTRACT_D_CLASS_2( scriptClass, _class, _base_class1, _base_class2 )	\
	CHECK_BASE_OBJECT_CLASS(_class); \
	CHECK_BASE_OBJECT_CLASS(_base_class1); \
	CHECK_BASE_OBJECT_CLASS(_base_class2); \
	CHECK_CLASS_INHERIT_TYPE(_class, _base_class1); \
	CHECK_CLASS_INHERIT_TYPE(_class, _base_class2); \
	(scriptClass).RegClass( _class::eBaseObjectType, NULL, NULL, sizeof(_class), typeid(_class).name(), #_class, \
	#_base_class1, \
	#_base_class2, NULL );

#define REGIST_ABSTRACT_D_CLASS_3( scriptClass, _class, _base_class1, _base_class2, _base_class3 ) \
	CHECK_BASE_OBJECT_CLASS(_class); \
	CHECK_BASE_OBJECT_CLASS(_base_class1); \
	CHECK_BASE_OBJECT_CLASS(_base_class2); \
	CHECK_BASE_OBJECT_CLASS(_base_class3); \
	CHECK_CLASS_INHERIT_TYPE(_class, _base_class1); \
	CHECK_CLASS_INHERIT_TYPE(_class, _base_class2); \
	CHECK_CLASS_INHERIT_TYPE(_class, _base_class3); \
	(scriptClass).RegClass( _class::eBaseObjectType, NULL, NULL, sizeof(_class), typeid(_class).name(), #_class, \
	#_base_class1, \
	#_base_class2, \
	#_base_class3, NULL );

#define REGIST_ABSTRACT_D_CLASS_4( scriptClass, _class, _base_class1, _base_class2, _base_class3, _base_class4 )	\
	CHECK_BASE_OBJECT_CLASS(_class); \
	CHECK_BASE_OBJECT_CLASS(_base_class1); \
	CHECK_BASE_OBJECT_CLASS(_base_class2); \
	CHECK_BASE_OBJECT_CLASS(_base_class3); \
	CHECK_BASE_OBJECT_CLASS(_base_class4); \
	CHECK_CLASS_INHERIT_TYPE(_class, _base_class1); \
	CHECK_CLASS_INHERIT_TYPE(_class, _base_class2); \
	CHECK_CLASS_INHERIT_TYPE(_class, _base_class3); \
	CHECK_CLASS_INHERIT_TYPE(_class, _base_class4); \
	(scriptClass).RegClass( _class::eBaseObjectType, NULL, NULL, sizeof(_class), typeid(_class).name(), #_class, \
	#_base_class1, \
	#_base_class2, \
	#_base_class3, \
	#_base_class4, NULL );

#define REGIST_ABSTRACT_D_CLASS_5( scriptClass, _class, _base_class1, _base_class2, _base_class3, _base_class4, _base_class5 )	\
	CHECK_BASE_OBJECT_CLASS(_class); \
	CHECK_BASE_OBJECT_CLASS(_base_class1); \
	CHECK_BASE_OBJECT_CLASS(_base_class2); \
	CHECK_BASE_OBJECT_CLASS(_base_class3); \
	CHECK_BASE_OBJECT_CLASS(_base_class4); \
	CHECK_BASE_OBJECT_CLASS(_base_class5); \
	CHECK_CLASS_INHERIT_TYPE(_class, _base_class1); \
	CHECK_CLASS_INHERIT_TYPE(_class, _base_class2); \
	CHECK_CLASS_INHERIT_TYPE(_class, _base_class3); \
	CHECK_CLASS_INHERIT_TYPE(_class, _base_class4); \
	CHECK_CLASS_INHERIT_TYPE(_class, _base_class5); \
	(scriptClass).RegClass( _class::eBaseObjectType, NULL, NULL, sizeof(_class), typeid(_class).name(), #_class, \
	#_base_class1, \
	#_base_class2, \
	#_base_class3, \
	#_base_class4, \
	#_base_class5, NULL );


#define REGIST_CLASSMEMBER( scriptClass, _class, _member, szType ) \
(scriptClass).RegClassMember( ( string( "("#_class")" ) + szType ).c_str(), ( (char*)&((_class*)0x4000000)->_member) - ((char*)0x4000000 ), #_class, #_member );


#define REGIST_CLASSFUNCTION( scriptClass, _class, _function, szArg ) \
	(scriptClass).RegClassCallFunc( ( string( "("#_class")" ) + szArg ).c_str(), ( &_class::_function ), #_class, #_function );


#define REGIST_CLASSFUNCTION_OVERLOAD( scriptClass, _class, _fun_type, _fun_name_cpp, _fun_name_lua, szArg ) \
	(scriptClass).RegClassCallFunc( "("#_class")"szArg, (  static_cast<_fun_type>(&_class::_fun_name_cpp) ), #_class, #_fun_name_lua );


#define REGIST_GLOBALFUNCTION( scriptClass, _function, szArg ) \
	(scriptClass).RegCallFunc( szArg, ( &_function ), #_function );


#define REGIST_GLOBALFUNCTION_OVERLOAD( scriptClass,  _fun_type, _fun_name_cpp, _fun_name_lua, szArg ) \
	(scriptClass).RegCallFunc( szArg, ( static_cast<_fun_type>(&_fun_name_cpp) ), #_fun_name_lua );


#define REGIST_STATICFUNCTION( scriptClass, _class, _function, szArg ) \
	(scriptClass).RegCallFunc( szArg, ( &_class::_function ), #_class"_"#_function );


#define REGIST_STATICFUNCTION_OVERLOAD( scriptClass,  _class, _fun_type, _fun_name_cpp, _fun_name_lua, szArg ) \
	(scriptClass).RegCallFunc( szArg, (  static_cast<_fun_type>(&_class::_fun_name_cpp) ), #_class"_"#_fun_name_lua );

#define REGIST_PURE_CALLBACKFUNCTION( scriptClass, _class, _function, szArg ) \
{ \
class __class : public _class { public: \
	static void RegCallBack(CScript& Script, const char* szClass, const char* szFunction, const char* szFormat)\
{ Script.RegClassCallback( szFormat, &_class::_function, szClass, szFunction, true); } \
}; \
	__class::RegCallBack(scriptClass, #_class, #_function, szArg);\
}

#define REGIST_PURE_CALLBACKFUNCTION_OVERLOAD( scriptClass, _class, _fun_type, _fun_name_cpp, _fun_name_lua, szArg ) \
{ \
class __class : public _class { public: \
	static void RegCallBack(CScript& Script, const char* szClass, const char* szFunction, const char* szFormat)\
{ 	Script.RegClassCallback( szFormat, static_cast<_fun_type>(&_class::_fun_name_cpp), szClass, szFunction, true); } }; \
	__class::RegCallBack(scriptClass, #_class, #_fun_name_lua, szArg);\
}

#define REGIST_CALLBACKFUNCTION( scriptClass, _class, _function, szArg ) \
{ \
class __class : public _class { public: \
static void RegCallBack(CScript& Script, const char* szClass, const char* szFunction, const char* szFormat)\
{ Script.RegClassCallback( szFormat, &_class::_function, szClass, szFunction, false); } \
}; \
	__class::RegCallBack(scriptClass, #_class, #_function, szArg);\
}

#define REGIST_CALLBACKFUNCTION_OVERLOAD( scriptClass, _class, _fun_type, _fun_name_cpp, _fun_name_lua, szArg ) \
{ \
class __class : public _class { public: \
static void RegCallBack(CScript& Script, const char* szClass, const char* szFunction, const char* szFormat)\
{ 	Script.RegClassCallback( szFormat, static_cast<_fun_type>(&_class::_fun_name_cpp), szClass, szFunction, false); } }; \
	__class::RegCallBack(scriptClass, #_class, #_fun_name_lua, szArg);\
}

#define REGIST_CONSTANT( scriptClass, ValueName ) \
	(scriptClass).RegConstant( NULL, #ValueName, ValueName )


#define REGIST_ENUMTYPE( scriptClass, EnumType ) \
	(scriptClass).RegConstant( #EnumType, NULL, (int32)sizeof(EnumType) );


#define REGIST_ENUMERATION( scriptClass, EnumType, ValueName ) \
	{ EnumType e = ValueName; (scriptClass).RegConstant( #EnumType, #ValueName, e ); }


#define REGIST_PACKFUNCTIONTABLE( scriptClass, _pack_handle, pack_fun, cpp_context, szTableName ) \
	_pack_handle = (scriptClass).RegPackPakage( pack_fun, cpp_context, szTableName )


#define REGIST_PACKFUNCTION( scriptClass, _pack_handle, szfunction, id, first_arg_as_callcontext, szArg ) \
	(scriptClass).RegPackFunction( _pack_handle, szfunction, id, first_arg_as_callcontext, (szArg) )


#define REGIST_PACKCALLBACKTABLE( scriptClass, _pack_handle, adress_of_a_unpack_fun_point, szTableName ) \
	_pack_handle = (scriptClass).RegUnPackPakage( szTableName, adress_of_a_unpack_fun_point )


#define REGIST_PACKCALLBACK( scriptClass, _pack_handle, szfunction, id, first_arg_as_callcontext, szArg ) \
	(scriptClass).RegUnPackFunction( _pack_handle, szfunction, id, first_arg_as_callcontext, szArg )


#define REGIST_SERIALPACKFUN( scriptClass, szFunName, pack_fun ) \
	(scriptClass).RegSerialPackFun(szFunName, pack_fun)


#define REGIST_SERIALUNPACKFUN( scriptClass, unpack_fun ) \
	(scriptClass).RegSerialUnPackFun(unpack_fun)

