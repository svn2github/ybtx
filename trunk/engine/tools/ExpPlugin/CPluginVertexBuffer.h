#pragma once
#include "CVertexHardwareBuffer.h"

namespace sqr
{
	class CPluginVertexBuffer : public CVertexHardwareBuffer
	{
	public:
		CPluginVertexBuffer( size_t vertexSize, size_t numVertices, Usage usage );
		~CPluginVertexBuffer();

	private:
		void*	_lock(size_t offset, size_t length, LockOptions options) { return 0; }
		void	_unlock(void) {}
	};
}