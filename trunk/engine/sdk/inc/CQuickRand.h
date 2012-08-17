#pragma once
#include "time.h"

namespace sqr
{
	class CQuickRand
	{
		int32 nRandSeed;

	public:
		CQuickRand( int32 nSeed )
		{
			nRandSeed = nSeed;
		}

		CQuickRand()
		{
			nRandSeed = int32(time(NULL));
		}

		operator int32()
		{
			return (((nRandSeed = nRandSeed * 214013 + 2531011) >> 16) & 0x7fff);
		}

		int32 operator = ( int32 nSeed )
		{
			return nRandSeed = nSeed;
		}

		int32 GetSeed() const
		{
			return nRandSeed;
		}

		template<class T>
		static T Rand1( const T& Min, const T& Max )
		{
			static CQuickRand sGlob;
			int32 nFirst = sGlob;
			int32 nSecond = sGlob;
			double f = (nFirst<<15)|nSecond;
			return (T)( f*( Max - Min )/0x40000000 + Min );
		}

		template<class T>
		static T Rand2( const T& Min, const T& Max )
		{
			static CQuickRand sGlob;
			return (T)( sGlob*( Max - Min )/0x8000 + Min );
		}
	};

	inline int32 RandRC1( int32 nMin, int32 nMax )
	{
		return CQuickRand::Rand1( nMin, nMax + 1 );
	}

	inline int32 RandRC2( int32 nMin, int32 nMax )
	{
		return CQuickRand::Rand2( nMin, nMax + 1 );
	}
}

;
