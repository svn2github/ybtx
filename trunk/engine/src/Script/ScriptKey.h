#pragma once

namespace sqr
{
	/*
		因为以前我们索引lua里面的一些东西是通过
		字符串访问的，lua对于字符串的处理虽然很高效，但是lua对于外部传入的字符串，
		首先会进行一次hash匹配查找，同样的代码
		lua_getfield(L, t, "field") //取得t索引对应table里面field字段的东西

		比起
		static int field; //这个代表的是field
		lua_pushlightuserdata(L, &key)
		lua_gettable(L, t)
		
		效率要低一点，所以我们可以使用一些key来代替string的字段

		下面这些函数就是提供一些关键字符串的key
	*/

	inline void* GetCppObjPtrKey()  //用于对象上面的cpp obj ptr
	{
		static int ls_CppObjPtr = 0;
		return &ls_CppObjPtr;
	}

	
	inline void* GetGlobalObjTblKey()  //用于global table里面的obj table
	{
		static int ls_ObjTbl = 0;
		return &ls_ObjTbl;
	}

	inline const char* GetGlobalObjTblKeyName()
	{
		return "GlobalObjTbl";
	}

	inline void* GetClassPtrKey()
	{
		static int ls_ClassPtr = 0;
		return &ls_ClassPtr;
	}

	inline void* GetCppMemLeakTblKey()
	{
		static int ls_CppMemLeakTbl = 0;
		return &ls_CppMemLeakTbl;
	}

	inline const char* GetCppMemLeakTblKeyName()
	{
		return "CppObjMemLeakTbl";
	}

	inline void* GetSerialRefTblKey()
	{
		static int ls_SerialRefTbl = 0;
		return &ls_SerialRefTbl;
	}

	inline void* GetGlobalRefTableKey()
	{
		static int ls_RefTableKey = 0;
		return &ls_RefTableKey;
	}

	inline const char* GetGlobalRefTableKeyName()
	{
		return "__global_reference_table";;
	}
}
