#include "stdafx.h"
#include "CDynamicPool.h"
#include "CGraphicExp.h"
#include "TSqrAllocator.inl"
#include "basemath.h"
#include "BaseHelper.h"
#include "GraphicBase.h"
#include "StringHelper.h"

CDynamicPool::CDynamicPool(size_t BlockSize,LockOptions LockOp)
: m_VerBlockSize(BlockSize)
, m_IdxBlockSize(BlockSize)
, m_VerCurNum(0)
, m_IdxCurNum(0)
, m_VerOffset(BlockSize)
, m_IdxOffset(BlockSize)
, m_VertexShadow(NULL)
, m_IndexShadow(NULL)
, m_LockOp(LockOp)
{

}

CDynamicPool::~CDynamicPool()
{
	Destroy();
}

void	CDynamicPool::Destroy()
{
	for( size_t i = 0; i<m_VertexPool.size(); ++i )
	{
		if( NULL != m_VertexPool[i] )
		{
			CHardwareBufferMgr::GetInst()->DestroyBuffer(m_VertexPool[i]);
			m_VertexPool[i] = NULL;
		}
	}
	GfkSafeDelete(m_VertexShadow);

	for( size_t i = 0; i<m_IndexPool.size(); ++i )
	{
		if( NULL != m_IndexPool[i] )
		{
			CHardwareBufferMgr::GetInst()->DestroyBuffer(m_IndexPool[i]);
			m_IndexPool[i] = NULL;
		}
	}
	GfkSafeDelete(m_IndexShadow);
}

void	CDynamicPool::SetVertexBlockSize(size_t VertexBlockSize)
{
	m_VerBlockSize = VertexBlockSize;
}

void	CDynamicPool::SetIndexBlockSize(size_t IndexBlockSize)
{
	m_IdxBlockSize = IndexBlockSize;
}

CVertexHardwareBuffer*	CDynamicPool::LockVertexBuffer(size_t AllocSize,void** pBuffer, size_t& offset)
{
	if(m_VerOffset + AllocSize > m_VerBlockSize)
	{
		m_VerOffset = 0;
		++m_VerCurNum;
		if(m_VerCurNum>m_VertexPool.size())
			m_VertexPool.push_back(NULL);
		if( m_VertexPool[m_VerCurNum - 1]  == NULL || m_VertexPool[m_VerCurNum - 1]->getSize()<AllocSize)
		{
			if(m_VertexPool[m_VerCurNum - 1])
				CHardwareBufferMgr::GetInst()->DestroyBuffer(m_VertexPool[m_VerCurNum - 1]);

			size_t VSize = sizeof(int16);
			size_t ASize = AllocSize>m_VerBlockSize?AllocSize:m_VerBlockSize;
			m_VertexPool[m_VerCurNum - 1] = CHardwareBufferMgr::GetInst()->createVertexBuffer(
				VSize , ASize / VSize , HBU_DYNAMIC_WRITE_ONLY,NULL, false, false );

			if (!m_VertexShadow || m_VertexShadow->getSize() < ASize)
			{
				GfkSafeDelete(m_VertexShadow);
				m_VertexShadow = new ShadowBuffer(ASize, HBU_DYNAMIC);
			}
		}
	}

	CVertexHardwareBuffer* pVerBuffer = m_VertexPool[m_VerCurNum - 1];
	if(pVerBuffer == NULL || pBuffer== NULL || m_VertexShadow == NULL)
		return NULL;
	offset = m_VerOffset;
	*pBuffer = m_VertexShadow->lock(m_VerOffset,AllocSize,HBL_NORMAL);
	m_VertexLockStart	= m_VerOffset;
	m_VertexLockSize	= AllocSize;

	m_VerOffset += AllocSize;
	return pVerBuffer;
}

CIndexHardwareBuffer*	CDynamicPool::LockIndexBuffer(size_t AllocSize,void** pBuffer, size_t& offset)
{
	if(m_IdxOffset + AllocSize > m_IdxBlockSize)
	{
		m_IdxOffset = 0;
		++m_IdxCurNum;
		if(m_IdxCurNum>m_IndexPool.size())
			m_IndexPool.push_back(NULL);
		if(m_IndexPool[m_IdxCurNum - 1] == NULL || m_IndexPool[m_IdxCurNum - 1]->getSize()<AllocSize )
		{
			if(m_IndexPool[m_IdxCurNum - 1])
				CHardwareBufferMgr::GetInst()->DestroyBuffer(m_IndexPool[m_IdxCurNum - 1]);

			size_t VSize = sizeof(int16);
			size_t ASize = AllocSize>m_IdxBlockSize?AllocSize:m_IdxBlockSize;
			m_IndexPool[m_IdxCurNum - 1] = CHardwareBufferMgr::GetInst()->createIndexBuffer(
				ASize / VSize , HBU_DYNAMIC_WRITE_ONLY, NULL, false, false );

			if (!m_IndexShadow || m_IndexShadow->getSize() < ASize)
			{
				GfkSafeDelete(m_IndexShadow);
				m_IndexShadow = new ShadowBuffer(ASize, HBU_DYNAMIC);
			}
		}
	}	

	CIndexHardwareBuffer* pIdxBuffer = m_IndexPool[m_IdxCurNum - 1];
	if(pIdxBuffer == NULL || pBuffer == NULL || m_IndexShadow == NULL)
		return NULL;
	offset = m_IdxOffset;
	if(pIdxBuffer->isLocked())
		pIdxBuffer->unlock();
	*pBuffer = m_IndexShadow->lock(m_IdxOffset,AllocSize,HBL_NORMAL);
	m_IndexLockStart	= m_IdxOffset;
	m_IndexLockSize	= AllocSize;

	m_IdxOffset += AllocSize;
	return pIdxBuffer;
}

void	CDynamicPool::UnLockVertex(size_t size)
{
	m_VertexShadow->unlock();
	CVertexHardwareBuffer* pBuffer = m_VertexPool[m_VerCurNum - 1];
	if(pBuffer == NULL)
		return;
	if (size !=0 )
		CopyBuffData(m_VertexShadow, pBuffer, m_VertexLockStart, 
			limit2( size, size_t(0), m_VertexLockSize ) , m_LockOp);
}

void	CDynamicPool::UnLockIndex(size_t size)
{
	m_IndexShadow->unlock();
	CIndexHardwareBuffer* pBuffer = m_IndexPool[m_IdxCurNum - 1];
	if(pBuffer == NULL )
		return;
	if (size != 0)
		CopyBuffData(m_IndexShadow, pBuffer, m_IndexLockStart, 
			limit2(size, size_t(0), m_IndexLockSize), m_LockOp);

}

void	CDynamicPool::ResetPool(void)
{
	OptimizeBuffer();
	m_VerCurNum = 0;
	m_IdxCurNum = 0;
	m_VerOffset = m_VerBlockSize;
	m_IdxOffset = m_IdxBlockSize;
}

void	CDynamicPool::OptimizeBuffer()
{
	size_t FreeBuffer = m_VertexPool.size() - m_VerCurNum;
	for( size_t vI = 1; vI < FreeBuffer; ++vI )
	{
		if( NULL != m_VertexPool[m_VerCurNum + vI] )
		{
			CHardwareBufferMgr::GetInst()->DestroyBuffer(m_VertexPool[m_VerCurNum + vI]);
			m_VertexPool[m_VerCurNum + vI] = NULL;
		}
	}

	FreeBuffer = m_IndexPool.size() - m_IdxCurNum;
	for( size_t vI = 1; vI < FreeBuffer; ++vI )
	{
		if( NULL != m_IndexPool[m_IdxCurNum + vI] )
		{
			CHardwareBufferMgr::GetInst()->DestroyBuffer(m_IndexPool[m_IdxCurNum + vI]);
			m_IndexPool[m_IdxCurNum + vI] = NULL;
		}
	}
}

void CDynamicPool::CopyBuffData(CHardwareBuffer* from, CHardwareBuffer* dest, size_t offset, size_t length, LockOptions options)
{
	if (from && dest )
	{
		Ast( (offset + length) <= from->getSize() && (offset + length) <= dest->getSize() );
		// 如果锁定范围是全部的缓存区将使用HBL_DISCARD属性，否则应用HBL_NOMAL属性 -> 靠！D3D居然会给Warning
		void *destData = dest->lock( offset, length, options );
		const void *srcData = from->lock( offset, length, HBL_READ_ONLY );

		// 将数据拷贝到真实物理缓存中
		if(destData&&srcData)
		{
			SQR_TRY{
				memcpy(destData, srcData, length);
			}
			SQR_CATCH(exp)
			{
				exp.AppendType("CDynamicPool::memcpy");
				string szErrMsg = format("render stack size:%u", length);
				exp.AppendMsg(szErrMsg);		
				GfkLogExpOnce(exp);
			}
			SQR_TRY_END;
		}
			

		from->unlock();
		dest->unlock();
	}
}
