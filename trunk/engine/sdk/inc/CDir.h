#pragma once
/*
*	2D的方向，只有8个方向,需要重载一些运算符号
*/

#include "CVector2.h"
#include <math.h>
#include "BaseMath.h"
#include "CMath.h"
#include "CMathMallocObject.h"

namespace sqr
{
	struct CDir : public CMathMallocObject
	{
		uint8	uDir;

		CDir( uint8 uDir = 0 ) : uDir(uDir) {}

		void Get( CVector2f& vecDir )const
		{
			vecDir = *this;
		}
		void Set( const CVector2f& vecDir )
		{
			*this = vecDir;
		}

		void SetDir( uint8 fDir )
		{
			uDir = fDir;
		}

		//得到方向
		operator CVector2f() const
		{
			float fAng = (float)( uDir*MATH_PI*2/256 );
			return CVector2f( CMath::SinTable( fAng ), CMath::CosTable( fAng ) );
		}

		const CDir& operator = ( CVector2f vecDir )
		{
			vecDir.Normalize();
			uDir = (uint8)( asinf( fabs( vecDir.x ) )*256/( MATH_PI*2 ) );
			if ( vecDir.y < 0 )
				uDir = (uint8)( 128 - uDir );
			if ( vecDir.x < 0 )
				uDir = (uint8)(-uDir);
			return *this;
		}
	};
}// end of namespace sqr
