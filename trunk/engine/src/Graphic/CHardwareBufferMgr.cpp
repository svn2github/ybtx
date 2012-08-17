#include "stdafx.h"
#include "CHardwareBufferMgr.h"
#include "CShadowBuffer.h"
#include "TSqrAllocator.inl"
#include "CDataSources.h"

CHardwareBufferMgr::CHardwareBufferMgr()
: m_isNeedUpdate(false)
, m_sBufferSize(0)
{}

CHardwareBufferMgr::~CHardwareBufferMgr()
{
	Clear();
}

CHardwareBuffer* CHardwareBufferMgr::creatBuffer(size_t bufferSize)
{
	CHardwareBuffer* p = CDataSources::GetInst()->NewBuffer(bufferSize);
	if (NULL!=p)
		m_bufferlist.insert(BufferSet::value_type(p));
	return p;
}

bool CHardwareBufferMgr::DestroyBuffer(CHardwareBuffer* buffer)
{
	BufferSet::iterator it = m_bufferlist.find(buffer);
	if (it!=m_bufferlist.end())
	{
		m_sBufferSize -= buffer->getSize();
		delete *it;
		m_bufferlist.erase(it);
		return true;
	}
	return false;
}

void CHardwareBufferMgr::Clear()
{
	BufferSet::iterator it = m_bufferlist.begin();
	BufferSet::iterator ite = m_bufferlist.end();
	for (;it!=ite;it++)
		delete *it;
	m_bufferlist.clear();
	m_sBufferSize = 0;
}

void CHardwareBufferMgr::Release()
{
	BufferSet::iterator it = m_bufferlist.begin();
	BufferSet::iterator ite = m_bufferlist.end();
	for (;it!=ite;it++)
		(*it)->freeRes();
	m_isNeedUpdate = true;
}

void CHardwareBufferMgr::Update()
{
	if (m_isNeedUpdate)
	{
		BufferSet::iterator it = m_bufferlist.begin();
		BufferSet::iterator ite = m_bufferlist.end();
		for (;it!=ite;it++)
			(*it)->updateRes();
		m_isNeedUpdate = false;
	}
}

CVertexHardwareBuffer* CHardwareBufferMgr::createVertexBuffer(size_t vertexSize, size_t numVerts, Usage usage, 
										  void* pInitData, bool useSysMem , bool useShadowBuffer)
{
	CVertexHardwareBuffer *p = CDataSources::GetInst()->NewVertexBuffer( vertexSize,numVerts,usage,pInitData,useSysMem,useShadowBuffer);
	if (NULL!=p)
	{
		m_bufferlist.insert(BufferSet::value_type(p));
		m_sBufferSize += p->getSize();
	}
	return p;
}

CIndexHardwareBuffer* CHardwareBufferMgr::createIndexBuffer(size_t numIndexes, Usage usage,
										void* pInitData, bool useSysMem , bool useShadowBuffer)
{
	CIndexHardwareBuffer *p = CDataSources::GetInst()->NewIndexBuffer( numIndexes,usage,pInitData,useSysMem,useShadowBuffer);
	if (NULL!=p)
	{
		m_bufferlist.insert(BufferSet::value_type(p));
		m_sBufferSize += p->getSize();
	}
	return p;
}
