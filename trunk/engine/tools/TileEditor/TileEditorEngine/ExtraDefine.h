#ifndef _ExtraDefine_
#define _ExtraDefine_
#include "wtypes.h"
#include <map>
#include "BaseDefs.h"
using namespace sqr;

#include "basemath.h"

struct COLOR
{
	union
	{
		DWORD	dwColor;
		BYTE	byColor[4];
	};

	COLOR()
	{
		dwColor = 0;
	}

	COLOR( DWORD c )
	{
		dwColor = c;
	}

	COLOR( BYTE a, BYTE r, BYTE g, BYTE b )
	{
		byColor[0] = b;
		byColor[1] = g;
		byColor[2] = r;
		byColor[3] = a;
	}

	operator DWORD() const
	{
		return dwColor;
	}

	const COLOR& operator= ( const DWORD& arg )
	{
		dwColor = arg;
		return *this;
	}

	const COLOR& operator= ( const COLOR& arg )
	{
		dwColor = arg.dwColor;
		return *this;
	}

	const COLOR operator* ( float arg ) const
	{
		return COLOR( (BYTE)( byColor[3]*arg + 0.5f ), (BYTE)( byColor[2]*arg + 0.5f ), 
			(BYTE)( byColor[1]*arg + 0.5f ), (BYTE)( byColor[0]*arg + 0.5f ) );
	}

	const COLOR operator+ ( const COLOR& arg ) const
	{
		return COLOR( (BYTE)min( 255, (int)byColor[3] + arg.byColor[3] ), (BYTE)min( 255, (int)byColor[2] + arg.byColor[2] ), 
			(BYTE)min( 255, (int)byColor[1] + arg.byColor[1] ), (BYTE)min( 255, (int)byColor[0] + arg.byColor[0] ) );
	}

	const COLOR operator! () const
	{
		return COLOR( byColor[3], byColor[0], byColor[1], byColor[2] );
	}
};


#endif