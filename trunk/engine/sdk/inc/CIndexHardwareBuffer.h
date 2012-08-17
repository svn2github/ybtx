#pragma once
#include "CHardwareBuffer.h"

namespace sqr
{
	class CIndexHardwareBuffer : public CHardwareBuffer
	{
	public:
		CIndexHardwareBuffer(size_t numIndexes, Usage usage,
			bool useSystemMem = false, bool useShadowBuffer = true);
		virtual ~CIndexHardwareBuffer();

		void read(size_t offset, size_t length, void* pDest);
		void write(size_t offset, size_t length, const void* pSource,
			bool discardBuffer = false);

		size_t getNumIndexes(void) const
		{
			return m_sNumIndexes;
		}
		size_t getIndexSize(void) const
		{
			return sizeof(WORD);
		}

	protected:
		size_t m_sNumIndexes;
	};
}