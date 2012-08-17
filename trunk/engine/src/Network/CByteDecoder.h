#pragma once
#include "CQuickRand.h"

namespace sqr
{
	class CByteDecoder
	{
	private:
		CQuickRand	m_Rand;
		unsigned	m_uCounter;

	public:
		void Reset(uint32 uKey);
		uint8 Decode(uint8 uData);
		void DecodeBuffer(char* pData,size_t stSize);
	};
}

