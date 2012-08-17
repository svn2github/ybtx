#pragma once
#include "TSingleton.h"
#include "CHardwareBuffer.h"
#include "CIndexHardwareBuffer.h"
#include "CVertexHardwareBuffer.h"
#include "CShadowBuffer.h"

namespace sqr
{
	class CHardwareBufferMgr 
		: public Singleton<CHardwareBufferMgr>
		, public CGraphicMallocObject
	{
	public:
		CHardwareBufferMgr();
		virtual ~CHardwareBufferMgr();
		CHardwareBuffer* creatBuffer(size_t bufferSize);
		bool DestroyBuffer(CHardwareBuffer* buffer);
		void Clear();

		void Release();
		void Update();
		
		size_t	GetBufferSize();
		CVertexHardwareBuffer* createVertexBuffer(size_t vertexSize, size_t numVerts, Usage usage, 
				void* pInitData = NULL, bool useSysMem = false , bool useShadowBuffer = false);
		CIndexHardwareBuffer* createIndexBuffer(size_t numIndexes, Usage usage,
				void* pInitData = NULL, bool useSysMem = false , bool useShadowBuffer = false);
	protected:
		typedef	  GSet<CHardwareBuffer*> BufferSet;
		BufferSet m_bufferlist;
		bool	  m_isNeedUpdate;
		size_t	  m_sBufferSize;
	};

	inline size_t	CHardwareBufferMgr::GetBufferSize()
	{
		return m_sBufferSize;
	}
}