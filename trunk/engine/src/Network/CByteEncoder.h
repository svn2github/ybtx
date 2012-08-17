#pragma once
#include "CQuickRand.h"

namespace sqr
{
	class CByteEncoder
	{
	private:
		CQuickRand	m_Rand;
		unsigned	m_uCounter;

	public:
		uint32 Reset();
		uint8 Encode(uint8 uData);
		void EncodeBuffer(char* pData,size_t stSize);
	};
}
