#include "stdafx.h"
#include "CByteDecoder.h"
#include "CByteDecoder.inl"


void CByteDecoder::Reset(uint32 uKey)
{
	m_uCounter=0;
	m_Rand=uKey;
}


void CByteDecoder::DecodeBuffer(char* pData,size_t stSize)
{
	for( size_t i=0 ; i<stSize ; ++i,++pData )
		*pData=Decode(*pData);
}

