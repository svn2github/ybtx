#pragma once
#include "GraphicBase.h"

namespace sqr
{
	struct SKELETAL_MASK
	{
		GVector<uint32> m_Mask;

		SKELETAL_MASK()
			: m_Mask(8,0)
		{
			//memset( m_Mask, 0, sizeof(m_Mask) );
		}

		inline const SKELETAL_MASK operator + ( const SKELETAL_MASK& Mask ) const
		{
			SKELETAL_MASK re;
			for ( int32 i = 0; i < 8; i++ )
				re.m_Mask[i] = m_Mask[i]|Mask.m_Mask[i];
			return re;
		}

		inline const SKELETAL_MASK& operator += ( const SKELETAL_MASK& Mask )
		{
			for ( int32 i = 0; i < 8; i++ )
				m_Mask[i] |= Mask.m_Mask[i];
			return *this;
		}

		inline void Empty()
		{
			fill(m_Mask.begin(),m_Mask.end(),0);
		}
		inline void AddSkeletal( uint32 nIndex )
		{
			m_Mask[nIndex>>5] |= 1<<(nIndex&31);
		}
		inline void DelSkeletal( uint32 nIndex )
		{
			m_Mask[nIndex>>5] &=~(1<<(nIndex&31));
		}
		inline bool HaveSkeletal( uint32 nIndex ) const
		{
			return ( m_Mask[nIndex>>5]&( 1<<(nIndex&31) ) ) != 0;
		}
		uint32 GetSkeletalCount();
	};
}