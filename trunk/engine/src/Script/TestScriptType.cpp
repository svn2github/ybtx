#include "stdafx.h"
#include "ScriptX.h"
#include "ScriptDef.h"
#include "TestScriptType.h"
#include "float.h"
#include "math.h"
#include <string>

namespace sqr
{
	sqr::CScript& GetScriptForTest()
	{
		return *CScript::Inst();
	}
	
#ifdef _WIN32
#pragma optimize("", off)
#endif
	bool tParamMix( bool b, int x, double d )
	{
		if( d != 1.009 )
			return false;
		if( x != 1234 )
			return false;
		if( !b )
			return false;
		return true;
	}

	bool tBool( bool param )
	{
		return param;
	}

	int32 tInt( int32 param )
	{
		return param;
	}

	uint32 tUint( uint32 param )
	{
		return param;
	}


	float tFloat( float param )
	{
		return param;
	}

	double tDouble( double param )
	{
		return param;
	}

	const char* tString( const char* param )
	{
		return param; 
	}

	int tGetNum()
	{
		return 123456;
	}

	void tNoReturn( int x )
	{
		x = 1;
	}

	int add( int a, int b )
	{
		return a + b;
	}

	int add( int a, int b, int c )

	{
		return a + b + c;
	}

	int64  tTestInt64()
	{
		return (int64) 99999999999999ll;
	}

	int64 tGetint64(int64 values)
	{
		return values;
	}

	unsigned int tUint32Mix()
	{
		return 4294967295u;
	}

#ifdef _WIN32
#pragma warning( disable: 4146 )
#endif
	bool tAllIntMax( char i8, short i16, int i32, uint8 I8, unsigned short I16, unsigned int I32 )
	{
		if( i8 != -128 )
			return false;
		if( i16 != -32768 )
			return false;
		if( i32 != (int)-2147483648U )
			return false;
		if( I8 != 255 )
			return false;
		if( I16 != 65535 )
			return false;
		if( I32 != 4294967295u )
			return false;

		return true;
	}
#ifdef _WIN32
#pragma warning( default: 4146 )
#endif

#define TEST_CONSTANT	100

	enum TestEnum
	{
		eTE_Test1 = 0,
		eTE_Test2 = 1
	};

	void TypeTestRegist( CScript& script )
	{
		REGIST_GLOBALFUNCTION( script, tParamMix, "bid:b" );

		REGIST_GLOBALFUNCTION( script, tInt, "i:i" );
		REGIST_GLOBALFUNCTION( script, tUint, "I:I" );
		REGIST_GLOBALFUNCTION( script, tFloat, "f:f" );
		REGIST_GLOBALFUNCTION( script, tDouble, "d:d" );
		REGIST_GLOBALFUNCTION( script, tBool, "b:b" );
		REGIST_GLOBALFUNCTION( script, tString, "s:s" );

		REGIST_GLOBALFUNCTION( script, tGetNum, ":i" );
		REGIST_GLOBALFUNCTION( script, tNoReturn, "i" );

		REGIST_GLOBALFUNCTION( script, tTestInt64, ":z" );
		REGIST_GLOBALFUNCTION( script, tGetint64, "z:z" );

		REGIST_GLOBALFUNCTION_OVERLOAD( script, int(*)(int,int), add, add2, "ii:i" );
		REGIST_GLOBALFUNCTION_OVERLOAD( script, int(*)(int,int,int), add, add3, "iii:i" );
		REGIST_GLOBALFUNCTION( script, tAllIntMax, "chiCHI:b" );
		REGIST_GLOBALFUNCTION( script, tUint32Mix, ":I" );

		REGIST_CONSTANT( script, TEST_CONSTANT );
		REGIST_ENUMERATION( script, TestEnum, eTE_Test1 );
		REGIST_ENUMERATION( script, TestEnum, eTE_Test2 );
	}
#ifdef _WIN32
#pragma optimize("", on)
#endif
}

