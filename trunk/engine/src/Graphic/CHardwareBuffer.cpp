#include "stdafx.h"
#include "CHardwareBuffer.h"
#include "CGraphicExp.h"
#include "ExpHelper.h"

CHardwareBuffer::CHardwareBuffer(Usage usage, bool systemMemory, bool useShadowBuffer)
:m_Usage(usage), m_IsLocked(false), m_SystemMemory(systemMemory),
m_UseShadowBuffer(useShadowBuffer), m_pShadowBuffer(NULL), m_ShadowUpdated(false)
{
	m_Size = 0;
	m_sLockStart = 0;
	m_sLockSize = 0;
}

CHardwareBuffer::~CHardwareBuffer()
{}

void* CHardwareBuffer::lock(size_t offset, size_t length, LockOptions options)
{
	Ast(!isLocked() && "Cannot lock this buffer, it is already locked!");
	Ast((offset + length)<=m_Size && "lock too long");


	if( options != HBL_READ_ONLY && (m_Usage&HBU_STATIC)!=0 )
	{
		Ast("HBU_STATIC Buffer必须在创建时传入数据，以后不能通过Lock进行修改");
		return NULL;
	}

	void* ret = 0;
	if (m_UseShadowBuffer)
	{
		if (options != HBL_READ_ONLY)
		{
			// 我们不得不呈现一个读/写锁所以我们应用'影射缓存'
			// 这个标记和unlock()的同步的
			m_ShadowUpdated = true;
		}

		ret = m_pShadowBuffer->lock(offset, length, options);
	}
	else
	{
		// 当我们没有应用'影射缓存'的时候我们就锁定真实的物理缓存
		ret = _lock(offset, length, options);
		m_IsLocked = true;
	}
	m_lockOp = options;
	m_sLockStart = offset;
	m_sLockSize = length;
	return ret;
}

//锁定缓存的全部区
void* CHardwareBuffer::lock( LockOptions options )
{
	return lock(0,m_Size,options);
}

//解锁缓存区域
void CHardwareBuffer::unlock(void)
{
	Ast(isLocked() && "Cannot unlock this buffer, it is not locked!");

	// 如果我们前面用了'影射缓存'
	if (m_UseShadowBuffer&&m_pShadowBuffer->isLocked())
	{
		m_pShadowBuffer->unlock();
		// 将影射缓存中的数据更新到实际的缓存中
		if (m_ShadowUpdated)
		{
			_updateFromShadow();
			m_ShadowUpdated = false;
		}
	}
	else
	{
		// 否则我们解开硬件缓存锁
		m_IsLocked = false;
		_unlock();
	}
	m_sLockStart = 0;
	m_sLockSize = m_Size;
}

void CHardwareBuffer::copy(CHardwareBuffer& srcBuffer, size_t srcOffset,
						   size_t dstOffset, size_t length, bool discardBuffer)
{
	const void *srcData = srcBuffer.lock(srcOffset, length, HBL_READ_ONLY);
	this->write(dstOffset, length, srcData, discardBuffer);
	srcBuffer.unlock();
}


size_t CHardwareBuffer::getSize(void)
{
	return m_Size;
}

///返回创建时的创建属性参照Usage
Usage CHardwareBuffer::getUsage(void)
{
	return m_Usage;
}

/// 返回这个缓存区表示的是否是系统内存
bool CHardwareBuffer::isSystemMemory(void)
{
	return m_SystemMemory;
}

/// 返回这个缓存区目前是否被锁定
bool CHardwareBuffer::isLocked(void)
{
	return m_IsLocked || (m_UseShadowBuffer&&m_pShadowBuffer->isLocked());
}

bool CHardwareBuffer::hasShadowBuffer(void)
{
	return m_UseShadowBuffer;
}

void CHardwareBuffer::updateRes()
{
	m_ShadowUpdated = true;
	_updateFromShadow();
}

void CHardwareBuffer::freeRes()
{
	return;
}

//------------------------------------------------------------------------
void CHardwareBuffer::_updateFromShadow(void)
{
	if (m_UseShadowBuffer && m_ShadowUpdated)
	{
		// 手动操作避免锁定时候遇到的问题
		const void *srcData = m_pShadowBuffer->_lock(
			m_sLockStart, m_sLockSize, HBL_READ_ONLY);
		// 如果锁定范围是全部的缓存区将使用HBL_DISCARD属性，否则应用HBL_NOMAL属性 -> 靠！D3D居然会给Warning
		Ast( (m_sLockStart + m_sLockSize)<=m_Size );
		void *destData = this->_lock(
			m_sLockStart, m_sLockSize, m_lockOp );//(0==m_sLockStart && m_sLockSize==m_Size)?HBL_DISCARD:HBL_NORMAL);
		// 将数据拷贝到真实物理缓存中
		if( NULL != destData && NULL!= srcData )
			memcpy(destData, srcData, m_sLockSize);

		this->_unlock();
		m_pShadowBuffer->_unlock();
		m_ShadowUpdated = false;
	}
}

bool	CHardwareBuffer::IsValid(void)
{
	return true;
}
