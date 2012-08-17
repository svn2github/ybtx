#include "stdafx.h"
#include "CSkeletalMask.h"

uint32 SKELETAL_MASK::GetSkeletalCount()
{
	uint32 n = 0;
	for ( int32 i = 0; i < 8; i++ )
		for ( int32 j = 0; j < 32; j++ )
			if ( ( m_Mask[i]&( 1<< j ) ) != 0 )
				n++;
	return n;
}