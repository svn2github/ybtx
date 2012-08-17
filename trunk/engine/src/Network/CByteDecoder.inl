#include "stdafx.h"
#include "CByteDecoder.h"


inline uint8 CByteDecoder::Decode(uint8 uData)
{
	if( (m_uCounter++&0xbf) == 0 )
	{
		(int32)m_Rand;
	}
	return uint8(m_Rand.GetSeed()^uData);
}

