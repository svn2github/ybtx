#pragma once
#include "CHardwareBufferMgr.h"
#define DefaultSize 2097152 
namespace sqr
{
	class CDynamicPool : public CGraphicMallocObject
	{
	public:
		CDynamicPool(size_t BlockSize = DefaultSize,LockOptions LockOp = HBL_NORMAL );
		~CDynamicPool();

		CVertexHardwareBuffer*	LockVertexBuffer(size_t AllocSize,void** pBuffer, size_t& offset);
		CIndexHardwareBuffer*	LockIndexBuffer(size_t AllocSize,void** pBuffer, size_t& offset);
		void	UnLockVertex(size_t size = (size_t)(-1));
		void	UnLockIndex(size_t size = (size_t)(-1));
		void	ResetPool(void);
	protected:
		void	SetVertexBlockSize(size_t VertexBlockSize);
		void	SetIndexBlockSize(size_t IndexBlockSize);
		void	Destroy();

		typedef GVector<CVertexHardwareBuffer*>	VertexBufferPool;
		typedef GVector<CIndexHardwareBuffer*>	IndexBufferPool;
		
		void	OptimizeBuffer(void);
		void	CopyBuffData(CHardwareBuffer* from, CHardwareBuffer* dest, size_t offset, size_t length, LockOptions options);
		VertexBufferPool	m_VertexPool;
		ShadowBuffer*		m_VertexShadow;
		size_t				m_VertexLockStart;
		size_t				m_VertexLockSize;

		IndexBufferPool		m_IndexPool;
		ShadowBuffer*		m_IndexShadow;
		LockOptions			m_LockOp;
		size_t				m_IndexLockStart;
		size_t				m_IndexLockSize;

		size_t	m_VerCurNum;
		size_t  m_IdxCurNum;
		size_t	m_VerOffset;
		size_t	m_IdxOffset;
		size_t	m_VerBlockSize;
		size_t	m_IdxBlockSize;
	};
}