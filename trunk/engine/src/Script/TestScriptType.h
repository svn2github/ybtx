#pragma once
#include "CScript.h"

namespace sqr
{
	sqr::CScript& GetScriptForTest();

	bool tParamMix( bool b, int x, double d );

	int32 tInt( int32 param );
	uint32 tUint( uint32 param );
	float tFloat( float param );
	double tDouble( double param );
	bool tBool( bool param );
	const char* tString( const char* param );

	
	void TypeTestRegist( CScript& script );

	int add( int a, int b );
	int add( int a, int b, int c );
}

