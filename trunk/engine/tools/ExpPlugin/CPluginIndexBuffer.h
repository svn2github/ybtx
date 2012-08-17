#pragma once
#include "CIndexHardwareBuffer.h"

namespace sqr
{
	class CPluginIndexBuffer : public CIndexHardwareBuffer
	{
	public:
		CPluginIndexBuffer( size_t numIndexes, Usage usage );
		~CPluginIndexBuffer();

	private:
		void*	_lock(size_t offset, size_t length, LockOptions options) { return 0; }
		void	_unlock(void) {}
	};
}