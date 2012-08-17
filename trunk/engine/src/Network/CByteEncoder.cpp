#include "stdafx.h"
#include "CByteEncoder.h"
#include "CByteEncoder.inl"


uint32 CByteEncoder::Reset()
{
	m_uCounter=0;
	uint32 uKey=m_Rand.GetSeed();
	m_Rand=uKey;

	return uKey;
}


void CByteEncoder::EncodeBuffer(char* pData,size_t stSize)
{
	for( size_t i=0 ; i<stSize ; ++i,++pData )
		*pData=Encode(*pData);
}


